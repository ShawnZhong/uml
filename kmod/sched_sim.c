#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
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

struct rq rq_array[NR_CPUS];
struct sched_entity se_array[NR_CPUS];
struct cfs_rq cfs_rq_array[NR_CPUS];
struct task_group tg = {
    .se = (struct sched_entity **)&se_array,
    .cfs_rq = (struct cfs_rq **)&cfs_rq_array,
    .shares = ROOT_TASK_GROUP_LOAD,
};

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
    tasks[i].pid = specs[i].pid;
    tasks[i].__state = TASK_NEW;
    tasks[i].policy = SCHED_NORMAL;
    tasks[i].se.cfs_rq = &rq_array[specs[i].cpu].cfs;
    strcpy(tasks[i].comm, "test");
  }
  return tasks;
}

static int __init sched_sim(void) {
  // From `nm linux` or `grep System.map`
  const struct sched_class *fair_sched_class = (void *)0x60472db0;
  void (*fair_server_init)(struct rq *rq) = (void *)0x6006d58c;
  void (*init_tg_cfs_entry)(struct task_group *tg, struct cfs_rq *cfs_rq,
                            struct sched_entity *se, int cpu,
                            struct sched_entity *parent) = (void *)0x6006d855;

  pr_info("Scheduler initializing\n");

  // from sched_init
  for (int i = 0; i < NR_CPUS; i++) {
    struct rq *rq = &rq_array[i];

    INIT_LIST_HEAD(&rq->cfs.leaf_cfs_rq_list);
    init_tg_cfs_entry(&tg, &rq->cfs, &se_array[i], i, NULL);
    fair_server_init(rq);
    rq->curr = &init_task;
  }

  pr_info("Scheduler initialized\n");
  struct task_struct *tasks = create_tasks(
      (struct task_spec[]){
          {.pid = 0, .cpu = 0},
          {.pid = 1, .cpu = 0},
          {.pid = 2, .cpu = 0},
      },
      3);

  struct rq *rq = &rq_array[0];

  // Enqueue tasks
  for (int i = 0; i < 3; i++) {
    fair_sched_class->enqueue_task(rq, &tasks[i], 0);
    pr_info("Enqueued task %d on cpu %d\n", tasks[i].pid, i);
    print_rq(rq);
  }

  // Dequeue tasks
  for (int i = 0; i < 3; i++) {
    fair_sched_class->dequeue_task(rq, &tasks[i], 0);
    pr_info("Dequeued task %d on cpu %d\n", tasks[i].pid, i);
    print_rq(rq);
  }

  pr_info("Scheduler simulation complete\n");
  return 0;
}
module_init(sched_sim);
