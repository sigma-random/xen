#ifndef __I386_SCHED_H
#define __I386_SCHED_H

#include <linux/config.h>
#include <asm/desc.h>
#include <asm/atomic.h>
#include <asm/pgalloc.h>
#include <asm/tlbflush.h>

/*
 * Used for LDT copy/destruction.
 */
int init_new_context(struct task_struct *tsk, struct mm_struct *mm);
void destroy_context(struct mm_struct *mm);


static inline void enter_lazy_tlb(struct mm_struct *mm, struct task_struct *tsk)
{
#if 0 /* XEN */
	unsigned cpu = smp_processor_id();
	if (per_cpu(cpu_tlbstate, cpu).state == TLBSTATE_OK)
		per_cpu(cpu_tlbstate, cpu).state = TLBSTATE_LAZY;
#endif
}

#define prepare_arch_switch(rq,next)	__prepare_arch_switch()
#define finish_arch_switch(rq, next)	spin_unlock_irq(&(rq)->lock)
#define task_running(rq, p)		((rq)->curr == (p))

static inline void __prepare_arch_switch(void)
{
	/*
	 * Save away %fs and %gs. No need to save %es and %ds, as those
	 * are always kernel segments while inside the kernel. Must
	 * happen before reload of cr3/ldt (i.e., not in __switch_to).
	 */
	__asm__ __volatile__ ( "movl %%fs,%0 ; movl %%gs,%1"
		: "=m" (*(int *)&current->thread.fs),
		  "=m" (*(int *)&current->thread.gs));
	__asm__ __volatile__ ( "movl %0,%%fs ; movl %0,%%gs"
		: : "r" (0) );
}

static inline void switch_mm(struct mm_struct *prev,
			     struct mm_struct *next,
			     struct task_struct *tsk)
{
	int cpu = smp_processor_id();

	if (likely(prev != next)) {
		/* stop flush ipis for the previous mm */
		cpu_clear(cpu, prev->cpu_vm_mask);
#if 0 /* XEN */
		per_cpu(cpu_tlbstate, cpu).state = TLBSTATE_OK;
		per_cpu(cpu_tlbstate, cpu).active_mm = next;
#endif
		cpu_set(cpu, next->cpu_vm_mask);

		/* Re-load page tables */
		load_cr3(next->pgd);

		/*
		 * load the LDT, if the LDT is different:
		 */
		if (unlikely(prev->context.ldt != next->context.ldt))
			load_LDT_nolock(&next->context, cpu);
	}
#if 0 /* XEN */
	else {
		per_cpu(cpu_tlbstate, cpu).state = TLBSTATE_OK;
		BUG_ON(per_cpu(cpu_tlbstate, cpu).active_mm != next);

		if (!cpu_test_and_set(cpu, next->cpu_vm_mask)) {
			/* We were in lazy tlb mode and leave_mm disabled 
			 * tlb flush IPI delivery. We must reload %cr3.
			 */
			load_cr3(next->pgd);
			load_LDT_nolock(&next->context, cpu);
		}
	}
#endif
}

#define deactivate_mm(tsk, mm) \
	asm("movl %0,%%fs ; movl %0,%%gs": :"r" (0))

#define activate_mm(prev, next) do {		\
	switch_mm((prev),(next),NULL);		\
} while (0)

#endif
