#ifndef UAPI_COMPEL_ASM_TYPES_H__
#define UAPI_COMPEL_ASM_TYPES_H__

#include <stdint.h>
#include <signal.h>
#include <sys/mman.h>
#include <asm/ptrace.h>

#define SIGMAX	   64
#define SIGMAX_OLD 31

/*
 * Copied from the Linux kernel header arch/riscv/include/uapi/asm/ptrace.h but modified to 64bit
 */

typedef struct {
	uint64_t pc;
	uint64_t ra;
	uint64_t sp;
	uint64_t gp;
	uint64_t tp;
	uint64_t t0;
	uint64_t t1;
	uint64_t t2;
	uint64_t s0;
	uint64_t s1;
	uint64_t a0;
	uint64_t a1;
	uint64_t a2;
	uint64_t a3;
	uint64_t a4;
	uint64_t a5;
	uint64_t a6;
	uint64_t a7;
	uint64_t s2;
	uint64_t s3;
	uint64_t s4;
	uint64_t s5;
	uint64_t s6;
	uint64_t s7;
	uint64_t s8;
	uint64_t s9;
	uint64_t s10;
	uint64_t s11;
	uint64_t t3;
	uint64_t t4;
	uint64_t t5;
	uint64_t t6;
} user_regs_struct_t;

// reusing __riscv_d_ext_state struct (double-precision floating-point state) defined in Linux kernel header arch/riscv/include/uapi/asm/ptrace.h
typedef struct __riscv_d_ext_state user_fpregs_struct_t;

#define __compel_arch_fetch_thread_area(tid, th) 0
#define compel_arch_fetch_thread_area(tctl)	 0
#define compel_arch_get_tls_task(ctl, tls)
#define compel_arch_get_tls_thread(tctl, tls)

#define REG_RES(registers)	  ((uint64_t)(registers).a0)
#define REG_IP(registers)	  ((uint64_t)(registers).pc)
#define SET_REG_IP(registers, val) ((registers).pc = (val))

// #ifndef REG_SP
#define REG_GET_SP(registers)	  ((uint64_t)((registers).sp))
// #endif

#define REG_SYSCALL_NR(registers) ((uint64_t)(registers).a7)

#define user_regs_native(pregs) true

#define ARCH_SI_TRAP TRAP_BRKPT

#define __NR(syscall, compat)   \
	({                      \
		(void)compat;   \
		__NR_##syscall; \
	})

#endif /* UAPI_COMPEL_ASM_TYPES_H__ */