#include <linux/kprobes.h>
#include <linux/sched.h>

// Linux private headers
#include <kernel/sched/sched.h>

#include "logging.h"

#define SYMBOL_LIST                                                            \
  X(void, enqueue_task, (struct rq * rq, struct task_struct * p, int flags))   \
  X(bool, dequeue_task, (struct rq * rq, struct task_struct * p, int flags))   \
  X(void, init_cfs_rq, (struct cfs_rq * cfs_rq))                               \
  X(void, init_rt_rq, (struct rt_rq * rt_rq))                                  \
  X(void, init_dl_rq, (struct dl_rq * dl_rq))                                  \
  X(void, fair_server_init, (struct rq * rq))                                  \
  X(void, init_tg_cfs_entry,                                                   \
    (struct task_group * tg, struct cfs_rq * cfs_rq, struct sched_entity * se, \
     int cpu, struct sched_entity *parent))                                    \
  X(int, sched_fork, (unsigned long clone_flags, struct task_struct *p))       \
  X(int, init_rootdomain, (struct root_domain * rd))                           \
  X(void, rq_attach_root, (struct rq * rq, struct root_domain * rd))           \
  X(void, init_cfs_bandwidth,                                                  \
    (struct cfs_bandwidth * cfs_bandwidth,                                     \
     struct cfs_bandwidth * parent_cfs_bandwidth))

// Define function pointers
#define X(ret_type, name, args) ret_type(*kernel_##name) args;
SYMBOL_LIST
#undef X

static void *get_kallsyms_lookup_name(void) {
  struct kprobe kp = {.symbol_name = "kallsyms_lookup_name"};
  int ret = register_kprobe(&kp);
  if (ret < 0) {
    SCHED_WARN("Failed to register kprobe: %d. Using hardcoded address.\n",
               ret);
    // From `nm linux/current/linux | grep kallsyms_lookup_name`
    return (void *)0x600ad515;
  }
  unregister_kprobe(&kp);
  return (void *)kp.addr;
}

// Initialize function pointers
static void init_kernel_symbols(void) {
  void *(*kallsyms_lookup_name)(const char *name) = get_kallsyms_lookup_name();

#define X(ret_type, name, args)                                                \
  do {                                                                         \
    kernel_##name = kallsyms_lookup_name(#name);                               \
    if (kernel_##name == NULL) {                                               \
      pr_err("lookup_func_addr: %s not found\n", #name);                       \
    }                                                                          \
  } while (0);
  SYMBOL_LIST
#undef X
}
