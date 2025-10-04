#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mmu_context.h>
#include <linux/module.h>
#include <linux/smp.h>

#include "kernel_sym.h"

static int __init resched_ipi_init(void) {
  init_kernel_symbols();
  kernel_arch_smp_send_reschedule(3);
  return 0;
}

static void __exit resched_ipi_exit(void) {
  pr_info("Reschedule IPI module exit\n");
}

module_init(resched_ipi_init);
module_exit(resched_ipi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Kernel module to call arch_smp_send_reschedule to CPU 1");
