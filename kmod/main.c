#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>

// Private headers
#include <kernel/sched/sched.h>

struct rq rq_array[NR_CPUS];
struct sched_entity se_array[NR_CPUS];
struct cfs_rq cfs_rq_array[NR_CPUS];
struct task_group tg = {
    .se = (struct sched_entity **)&se_array,
    .cfs_rq = (struct cfs_rq **)&cfs_rq_array,
    .shares = ROOT_TASK_GROUP_LOAD,
};

static void print_task(struct task_struct *task) {
  pr_info(" - pid=%d, comm=%s, state=%x\n", task->pid, task->comm,
          task->__state);
}

static void print_rq(struct rq *rq) {
  pr_info("nr_running=%d, nr_queued=%u\n", rq->nr_running, rq->cfs.nr_queued);
  struct rb_node *node = rb_first_cached(&rq->cfs.tasks_timeline);
  while (node) {
    struct sched_entity *se = rb_entry(node, struct sched_entity, run_node);
    print_task(task_of(se));
    node = rb_next(&se->run_node);
  }
}

static struct task_struct *create_task(int pid, int cpu) {
  struct task_struct *p;
  p = kcalloc(1, sizeof(struct task_struct), GFP_KERNEL);
  p->pid = pid;
  strcpy(p->comm, "test");
  p->__state = TASK_NEW;
  p->policy = SCHED_NORMAL;
  p->se.cfs_rq = &rq_array[cpu].cfs;
  return p;
}

static int __init sched_test(void) {
  // From `nm linux` or `grep System.map`
  const struct sched_class *fair_sched_class = (void *)0x60472d30;
  void (*fair_server_init)(struct rq *rq) = (void *)0x6006d58c;
  void (*init_tg_cfs_entry)(struct task_group *tg, struct cfs_rq *cfs_rq,
                            struct sched_entity *se, int cpu,
                            struct sched_entity *parent) = (void *)0x6006d855;

  pr_info("Initializing scheduler\n");

  // from sched_init
  for (int i = 0; i < NR_CPUS; i++) {
    struct rq *rq = &rq_array[i];

    INIT_LIST_HEAD(&rq->cfs.leaf_cfs_rq_list);
    init_tg_cfs_entry(&tg, &rq->cfs, &se_array[i], i, NULL);
    fair_server_init(rq);
    rq->curr = &init_task;
  }

  pr_info("Scheduler initialized\n");

  struct task_struct *p1 = create_task(42, 0);
  struct task_struct *p2 = create_task(43, 0);
  struct task_struct *p3 = create_task(44, 0);

  struct rq *rq = &rq_array[0];
  print_rq(rq);
  fair_sched_class->enqueue_task(rq, p1, 0);
  print_rq(rq);
  fair_sched_class->enqueue_task(rq, p2, 0);
  print_rq(rq);
  fair_sched_class->enqueue_task(rq, p3, 0);
  print_rq(rq);
  fair_sched_class->dequeue_task(rq, p3, 0);
  print_rq(rq);
  fair_sched_class->dequeue_task(rq, p2, 0);
  print_rq(rq);
  fair_sched_class->dequeue_task(rq, p1, 0);
  print_rq(rq);

  pr_info("Scheduler test complete\n");
  return 0;
}
module_init(sched_test);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shawn Zhong");
MODULE_DESCRIPTION("Scheduler tester");
