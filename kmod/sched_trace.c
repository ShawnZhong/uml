#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/mmu_context.h>
#include <linux/module.h>
#include <linux/sched.h>

// Linux private headers
#define __BPF_HELPERS__
#include <kernel/sched/sched.h>
#include <tools/lib/bpf/bpf_tracing.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shawn Zhong");
MODULE_DESCRIPTION("Scheduler tracer");

static int enqueue_task_fair_handler(struct kprobe *kp, struct pt_regs *regs) {
  struct rq *rq = (void *)PT_REGS_PARM1(regs);
  struct task_struct *p = (void *)PT_REGS_PARM2(regs);
  int flags = (int)PT_REGS_PARM3(regs);

  pr_info("enqueue_task_fair: cpu=%d, p=%s, flags=%o\n", rq->cpu, p->comm,
          flags);
  return 0;
}

static struct kprobe kp = {
    .symbol_name = "enqueue_task_fair",
    .pre_handler = enqueue_task_fair_handler,
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
