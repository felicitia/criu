#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <linux/elf.h>
#include <compel/plugins/std/syscall-codes.h>
#include "common/page.h"
#include "uapi/compel/asm/infect-types.h"
#include "log.h"
#include "errno.h"
#include "infect.h"
#include "infect-priv.h"

unsigned __page_size = 0;
unsigned __page_shift = 0;

/*
 * Injected syscall instruction
 */
const char code_syscall[] = {
    0x73, 0x00, 0x00, 0x00, /* ecall */
    0x73, 0x00, 0x10, 0x00  /* ebreak */
};

static const int code_syscall_aligned = round_up(sizeof(code_syscall), sizeof(long));

static inline void __always_unused __check_code_syscall(void)
{
	pr_info("Executing function: %s in file: %s\n", __func__, __FILE__);

	BUILD_BUG_ON(code_syscall_aligned != BUILTIN_SYSCALL_SIZE);
	BUILD_BUG_ON(!is_log2(sizeof(code_syscall)));
}

/*
	prepare sigframe data structure that will be used to restore the user context after a signal handler returns.
	copying GP registers (user_regs_struct_t) and FP registers (user_fpregs_struct_t) to sigframe
	user_regs_struct_t and user_fpregs_struct_t are defined in compel/arch/riscv64/src/lib/include/uapi/asm/infect-types.h
*/
int sigreturn_prep_regs_plain(struct rt_sigframe *sigframe, user_regs_struct_t *regs, user_fpregs_struct_t *fpregs)
{
	pr_info("Executing function: %s in file: %s\n", __func__, __FILE__);

	// Copy the GP registers from user_regs_struct_t to rt_sigframe
	sigframe->uc.uc_mcontext.__gregs[0] = regs->pc;
    sigframe->uc.uc_mcontext.__gregs[1] = regs->ra;
    sigframe->uc.uc_mcontext.__gregs[2] = regs->sp;
    sigframe->uc.uc_mcontext.__gregs[3] = regs->gp;
    sigframe->uc.uc_mcontext.__gregs[4] = regs->tp;
    sigframe->uc.uc_mcontext.__gregs[5] = regs->t0;
    sigframe->uc.uc_mcontext.__gregs[6] = regs->t1;
    sigframe->uc.uc_mcontext.__gregs[7] = regs->t2;
    sigframe->uc.uc_mcontext.__gregs[8] = regs->s0;
    sigframe->uc.uc_mcontext.__gregs[9] = regs->s1;
    sigframe->uc.uc_mcontext.__gregs[10] = regs->a0;
    sigframe->uc.uc_mcontext.__gregs[11] = regs->a1;
    sigframe->uc.uc_mcontext.__gregs[12] = regs->a2;
    sigframe->uc.uc_mcontext.__gregs[13] = regs->a3;
    sigframe->uc.uc_mcontext.__gregs[14] = regs->a4;
    sigframe->uc.uc_mcontext.__gregs[15] = regs->a5;
    sigframe->uc.uc_mcontext.__gregs[16] = regs->a6;
    sigframe->uc.uc_mcontext.__gregs[17] = regs->a7;
    sigframe->uc.uc_mcontext.__gregs[18] = regs->s2;
    sigframe->uc.uc_mcontext.__gregs[19] = regs->s3;
    sigframe->uc.uc_mcontext.__gregs[20] = regs->s4;
    sigframe->uc.uc_mcontext.__gregs[21] = regs->s5;
    sigframe->uc.uc_mcontext.__gregs[22] = regs->s6;
    sigframe->uc.uc_mcontext.__gregs[23] = regs->s7;
    sigframe->uc.uc_mcontext.__gregs[24] = regs->s8;
    sigframe->uc.uc_mcontext.__gregs[25] = regs->s9;
    sigframe->uc.uc_mcontext.__gregs[26] = regs->s10;
    sigframe->uc.uc_mcontext.__gregs[27] = regs->s11;
    sigframe->uc.uc_mcontext.__gregs[28] = regs->t3;
    sigframe->uc.uc_mcontext.__gregs[29] = regs->t4;
    sigframe->uc.uc_mcontext.__gregs[30] = regs->t5;
    sigframe->uc.uc_mcontext.__gregs[31] = regs->t6;

	// Copy the FP registers from user_fpregs_struct_t structure to rt_sigframe
    memcpy(sigframe->uc.uc_mcontext.__fpregs.__d.__f, fpregs->d_regs, sizeof(fpregs->d_regs));
    sigframe->uc.uc_mcontext.__fpregs.__d.__fcsr = fpregs->fcsr;

	return 0;
}

int sigreturn_prep_fpu_frame_plain(struct rt_sigframe *sigframe, struct rt_sigframe *rsigframe)
{
	pr_info("Executing function: %s in file: %s\n", __func__, __FILE__);

	return 0;
}

/*
	fetch the general-purpose and floating-point registers of a given process. 
	save_regs_t function pointer is used to save the fetched registers.
*/
int compel_get_task_regs(pid_t pid, user_regs_struct_t *regs, user_fpregs_struct_t *ext_regs, save_regs_t save,
			 void *arg, __maybe_unused unsigned long flags)
{
	user_fpregs_struct_t tmp, *fpsimd = ext_regs ? ext_regs : &tmp;
	struct iovec iov; //iovec is io vector struct defined in Linux
	int ret;

	pr_info("Dumping GP/FPU registers for %d\n", pid);
	pr_info("Executing function: %s in file: %s\n", __func__, __FILE__);


	iov.iov_base = regs;
	iov.iov_len = sizeof(user_regs_struct_t);
	if ((ret = ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov))) {
		pr_perror("Failed to obtain CPU registers for %d", pid);
		goto err;
	}

	iov.iov_base = fpsimd;
	iov.iov_len = sizeof(*fpsimd);
	if ((ret = ptrace(PTRACE_GETREGSET, pid, NT_PRFPREG, &iov))) {
		pr_perror("Failed to obtain FPU registers for %d", pid);
		goto err;
	}

	ret = save(arg, regs, fpsimd);
err:
	return ret;
}

int compel_set_task_ext_regs(pid_t pid, user_fpregs_struct_t *ext_regs)
{
	struct iovec iov;

	pr_info("Restoring GP/FPU registers for %d\n", pid);
	pr_info("Executing function: %s in file: %s\n", __func__, __FILE__);


	iov.iov_base = ext_regs;
	iov.iov_len = sizeof(*ext_regs);
	if (ptrace(PTRACE_SETREGSET, pid, NT_PRFPREG, &iov)) {
		pr_perror("Failed to set FPU registers for %d", pid);
		return -1;
	}
	return 0;
}

int compel_syscall(struct parasite_ctl *ctl, int nr, long *ret, unsigned long arg1, unsigned long arg2,
		   unsigned long arg3, unsigned long arg4, unsigned long arg5, unsigned long arg6)
{
	user_regs_struct_t regs = ctl->orig.regs;
	int err;
	pr_info("Executing function: %s in file: %s\n", __func__, __FILE__);


	regs.a7 = (unsigned long)nr; //the system call number is typically passed in register a7
	regs.a0 = arg1;
	regs.a1 = arg2;
	regs.a2 = arg3;
	regs.a3 = arg4;
	regs.a4 = arg5;
	regs.a5 = arg6;
	
	err = compel_execute_syscall(ctl, &regs, code_syscall);

	*ret = regs.a0; // After executing the ECALL instruction, the return value from the system call is placed in register a0.
	return err;
}

/*
	calling the mmap system call in the context of the target (victim) process using the compel_syscall function. 
	used during the infection process to allocate memory for the parasite code.
*/
void *remote_mmap(struct parasite_ctl *ctl, void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	long map;
	int err;
	pr_info("Executing function: %s in file: %s\n", __func__, __FILE__);


	err = compel_syscall(ctl, __NR_mmap, &map, (unsigned long)addr, length, prot, flags, fd, offset);
	if (err < 0 || (long)map < 0)
		map = 0;

	return (void *)map;
}

void parasite_setup_regs(unsigned long new_ip, void *stack, user_regs_struct_t *regs)
{
	pr_info("Executing function: %s in file: %s\n", __func__, __FILE__);

	regs->pc = new_ip;
	if (stack)
		regs->sp = (unsigned long)stack;
}

bool arch_can_dump_task(struct parasite_ctl *ctl)
{
	/*
	 * TODO: Add proper check here
	 */
	return true;
}

int arch_fetch_sas(struct parasite_ctl *ctl, struct rt_sigframe *s)
{
	long ret;
	int err;
	pr_info("Executing function: %s in file: %s\n", __func__, __FILE__);


	err = compel_syscall(ctl, __NR_sigaltstack, &ret, 0, (unsigned long)&s->uc.uc_stack, 0, 0, 0, 0);
	return err ? err : ret;
}

/*
  	task size is the maximum virtual address space size that a process can occupy in the memory
	refer to linux kernal: arch/riscv/include/asm/pgtable.h
	Task size is 0x4000000000 for RV64 or 0x9fc00000 for RV32.
*/
#define TASK_SIZE 0x4000000000UL

unsigned long compel_task_size(void)
{
	return TASK_SIZE;
}