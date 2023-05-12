#ifndef __CR_ATOMIC_H__
#define __CR_ATOMIC_H__

typedef struct {
	int counter;
} atomic_t;


#define smp_mb() asm volatile("fence rw,rw" : : : "memory")

/* Copied from the Linux kernel header arch/riscv64/include/asm/atomic.h */

static inline int atomic_read(const atomic_t *v)
{
	return (*(volatile int *)&(v)->counter);
}

static inline void atomic_set(atomic_t *v, int i)
{
	v->counter = i;
}

// #define atomic_get atomic_read

static inline int atomic_add_return(int i, atomic_t *v)
{
	unsigned long tmp;
	int result;

	asm volatile(
		"1: lr.w %0, %2\n"
		" add %0, %0, %3\n"
		" sc.w %1, %0, %2\n"
		" bnez %1, 1b"
		: "=&r"(result), "=&r"(tmp), "+A"(v->counter)
		: "Ir"(i)
		: "memory");

	smp_mb();
	return result;
}

static inline int atomic_sub_return(int i, atomic_t *v)
{
	unsigned long tmp;
	int result;

	asm volatile(
		"1: lr.w %0, %2\n"
		" sub %0, %0, %3\n"
		" sc.w %1, %0, %2\n"
		" bnez %1, 1b"
		: "=&r"(result), "=&r"(tmp), "+A"(v->counter)
		: "Ir"(i)
		: "memory");

	smp_mb();
	return result;
}

static inline int atomic_inc(atomic_t *v)
{
	return atomic_add_return(1, v) - 1;
}

static inline int atomic_add(int val, atomic_t *v)
{
	return atomic_add_return(val, v) - val;
}

static inline int atomic_dec(atomic_t *v)
{
	return atomic_sub_return(1, v) + 1;
}

//  atomically compare and exchange a value in a memory location
static inline int atomic_cmpxchg(atomic_t *ptr, int old, int new)
{
	unsigned long tmp;
	int oldval;

	smp_mb();

	asm volatile(
		"1: lr.w %0, %2\n"
		" bne %0, %3, 2f\n"
		" sc.w %1, %4, %2\n"
		" bnez %1, 1b\n"
		"2:"
		: "=&r"(oldval), "=&r"(tmp), "+A"(ptr->counter)
		: "r"(old), "r"(new)
		: "memory");

	smp_mb();
	return oldval;
}

/* true if the result is 0, or false for all other cases. */
#define atomic_dec_and_test(v) (atomic_sub_return(1, v) == 0)
#define atomic_dec_return(v)   (atomic_sub_return(1, v))

#define atomic_inc_return(v) (atomic_add_return(1, v))

#endif /* __CR_ATOMIC_H__ */
