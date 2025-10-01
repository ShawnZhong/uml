#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/mmu_context.h>
#include <linux/module.h>
#include <linux/sched.h>

// Linux private headers
#include <kernel/sched/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shawn Zhong");
MODULE_DESCRIPTION("Scheduler simulator");

#define TERM_RESET "\x1b[0m"
#define TERM_RED "\x1b[31m"
#define TERM_GREEN "\x1b[32m"
#define TERM_YELLOW "\x1b[33m"
#define TERM_BLUE "\x1b[34m"
#define TERM_MAGENTA "\x1b[35m"
#define TERM_CYAN "\x1b[36m"

// Global data structures
struct root_domain rd;
struct rq rq_array[NR_CPUS];
struct sched_entity se_array[NR_CPUS];
struct cfs_rq cfs_rq_array[NR_CPUS];
struct task_group tg = {
    .se = (struct sched_entity **)&se_array,
    .cfs_rq = (struct cfs_rq **)&cfs_rq_array,
    .shares = ROOT_TASK_GROUP_LOAD,
};

// Kernel function pointers
void (*kernel_enqueue_task)(struct rq *rq, struct task_struct *p, int flags);
bool (*kernel_dequeue_task)(struct rq *rq, struct task_struct *p, int flags);
void (*kernel_init_cfs_rq)(struct cfs_rq *cfs_rq);
void (*kernel_fair_server_init)(struct rq *rq);
void (*kernel_init_tg_cfs_entry)(struct task_group *tg, struct cfs_rq *cfs_rq,
                                 struct sched_entity *se, int cpu,
                                 struct sched_entity *parent);
int (*kernel_sched_fork)(unsigned long clone_flags, struct task_struct *p);
int (*kernel_init_rootdomain)(struct root_domain *rd);
void (*kernel_rq_attach_root)(struct rq *rq, struct root_domain *rd);

static void *get_kallsyms_lookup_name(void) {
  struct kprobe kp = {.symbol_name = "kallsyms_lookup_name"};
  int ret = register_kprobe(&kp);
  if (ret < 0) {
    pr_err("Failed to register kprobe: %d\n", ret);
    // From `nm linux` or `grep System.map`
    return (void *)0x6009fa1e;
  }
  unregister_kprobe(&kp);
  return (void *)kp.addr;
}

static void init_kernel_symbols(void) {
  void *(*kallsyms_lookup_name)(const char *name) = get_kallsyms_lookup_name();

#define INIT_SYMBOL(name)                                                      \
  do {                                                                         \
    kernel_##name = kallsyms_lookup_name(#name);                               \
    if (kernel_##name == NULL) {                                               \
      pr_err("lookup_func_addr: %s not found\n", #name);                       \
    }                                                                          \
  } while (0)

  INIT_SYMBOL(enqueue_task);
  INIT_SYMBOL(dequeue_task);
  INIT_SYMBOL(init_cfs_rq);
  INIT_SYMBOL(fair_server_init);
  INIT_SYMBOL(init_tg_cfs_entry);
  INIT_SYMBOL(sched_fork);
  INIT_SYMBOL(init_rootdomain);
  INIT_SYMBOL(rq_attach_root);
}

static void print_task(struct task_struct *task) {
  pr_info(TERM_GREEN " - pid=%d, comm=%s, state=%x" TERM_RESET "\n", task->pid,
          task->comm, task->__state);
}

static void print_rq(struct rq *rq) {
  pr_info(TERM_GREEN
          "cpu=%d, nr_running=%d, nr_queued=%u, clock=%llu" TERM_RESET "\n",
          cpu_of(rq), rq->nr_running, rq->cfs.nr_queued, rq->clock);
  struct rb_node *node = rb_first_cached(&rq->cfs.tasks_timeline);
  while (node) {
    struct sched_entity *se = rb_entry(node, struct sched_entity, run_node);
    print_task(task_of(se));
    node = rb_next(&se->run_node);
  }
}

struct task_spec {
  int pid;
  int cpu;
};

static struct task_struct *create_tasks(struct task_spec specs[], int n) {
  struct task_struct *tasks =
      kcalloc(n, sizeof(struct task_struct), GFP_KERNEL);
  for (int i = 0; i < n; i++) {
    kernel_sched_fork(0, &tasks[i]);
    tasks[i].pid = specs[i].pid;
    tasks[i].se.cfs_rq = &rq_array[specs[i].cpu].cfs;
    strcpy(tasks[i].comm, "test");
  }
  return tasks;
}

static void update_clock(struct rq *rq) {
  static int clock = 0;
  clock += 10000000;
  rq->clock = clock;
}

static int __init sched_sim(void) {
  init_kernel_symbols();

  pr_info("Scheduler initializing\n");

  // from sched_init
  kernel_init_rootdomain(&rd);

  int i;
  for_each_possible_cpu(i) {
    struct rq *rq = &rq_array[i];

    kernel_init_cfs_rq(&rq->cfs);
    INIT_LIST_HEAD(&rq->cfs.leaf_cfs_rq_list);
    kernel_init_tg_cfs_entry(&tg, &rq->cfs, &se_array[i], i, NULL);
    kernel_fair_server_init(rq);
    kernel_rq_attach_root(rq, &rd);
    rq->curr = &init_task;
    INIT_LIST_HEAD(&rq->cfs_tasks);
  }

  pr_info("Scheduler initialized\n");
  struct task_struct *tasks = create_tasks(
      (struct task_spec[]){
          {.pid = 10000, .cpu = 0},
          {.pid = 10001, .cpu = 0},
          {.pid = 10002, .cpu = 0},
      },
      3);

  {
    struct rq *rq = &rq_array[0];

    // Enqueue tasks
    for (int i = 0; i < 1; i++) {
      tasks[i].sched_class->enqueue_task(rq, &tasks[i], 0);
      update_clock(rq);
      pr_info("Enqueued task %d on cpu %d\n", tasks[i].pid, i);
      print_rq(rq);
    }

    // // Dequeue tasks
    // for (int i = 0; i < 1; i++) {
    //   tasks[i].sched_class->dequeue_task(rq, &tasks[i], 0);
    //   update_clock(rq);
    //   pr_info("Dequeued task %d on cpu %d\n", tasks[i].pid, i);
    //   print_rq(rq);
    // }
  }
  pr_info("Scheduler simulation complete\n");
  return 0;
}
module_init(sched_sim);
