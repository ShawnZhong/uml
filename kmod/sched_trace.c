#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shawn Zhong");
MODULE_DESCRIPTION("Scheduler tracer");

static int pre_handler(struct kprobe *p, struct pt_regs *regs) {
  pr_info("kernel_clone\n");
  return 0;
}

static struct kprobe kp = {
    .symbol_name = "kernel_clone",
    .pre_handler = (kprobe_pre_handler_t)pre_handler,
};

static int __init sched_trace_init(void) {
  int ret = register_kprobe(&kp);
  if (ret < 0) {
    pr_err("Failed to register kprobe: %d\n", ret);
    return ret;
  }
  return 0;
}
static void __exit sched_trace_exit(void) { unregister_kprobe(&kp); }

module_init(sched_trace_init);
module_exit(sched_trace_exit);
