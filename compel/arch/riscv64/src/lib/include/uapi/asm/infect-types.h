#ifndef UAPI_COMPEL_ASM_TYPES_H__
#define UAPI_COMPEL_ASM_TYPES_H__

#include <stdint.h>
#include <signal.h>
#include <sys/mman.h>
#include <asm/ptrace.h>

#define SIGMAX	   64
#define SIGMAX_OLD 31

/*
 * reusing user_regs_struct from the Linux kernel header arch/riscv/include/uapi/asm/ptrace.h 
 * general purpose registers
 */
typedef struct user_regs_struct user_regs_struct_t;

/*
	reusing __riscv_d_ext_state struct (double-precision floating-point state) 
	defined in Linux kernel header arch/riscv/include/uapi/asm/ptrace.h
	struct __riscv_d_ext_state {
	__u64 f[32];
	__u32 fcsr;
};
*/
typedef struct __riscv_d_ext_state user_fpregs_struct_t;

#define __compel_arch_fetch_thread_area(tid, th) 0
#define compel_arch_fetch_thread_area(tctl)	 0
#define compel_arch_get_tls_task(ctl, tls)
#define compel_arch_get_tls_thread(tctl, tls)

#define REG_RES(registers)	  ((uint64_t)(registers).a0)
#define REG_IP(registers)	  ((uint64_t)(registers).pc)
#define SET_REG_IP(registers, val) ((registers).pc = (val))

#define REG_GET_SP(registers)	  ((uint64_t)((registers).sp))

#define REG_SYSCALL_NR(registers) ((uint64_t)(registers).a7)

#define user_regs_native(pregs) true

#define ARCH_SI_TRAP TRAP_BRKPT

#define __NR(syscall, compat)   \
	({                      \
		(void)compat;   \
		__NR_##syscall; \
	})

#endif /* UAPI_COMPEL_ASM_TYPES_H__ */