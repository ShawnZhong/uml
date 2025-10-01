#include <linux/printk.h>
#include <linux/sched.h>

// Linux private headers
#include <kernel/sched/sched.h>

#define TERM_RESET "\x1b[0m"
#define TERM_RED "\x1b[31m"
#define TERM_GREEN "\x1b[32m"
#define TERM_YELLOW "\x1b[33m"
#define TERM_BLUE "\x1b[34m"
#define TERM_MAGENTA "\x1b[35m"
#define TERM_CYAN "\x1b[36m"
#define TERM_GRAY "\x1b[90m"

#define SCHED_INFO(fmt, ...) pr_info(TERM_GREEN fmt TERM_RESET, ##__VA_ARGS__)
#define SCHED_WARN(fmt, ...) pr_info(TERM_YELLOW fmt TERM_RESET, ##__VA_ARGS__)
#define SCHED_DEBUG(fmt, ...) pr_info(TERM_GRAY fmt TERM_RESET, ##__VA_ARGS__)

static void print_task(struct task_struct *task) {
  SCHED_INFO(" - pid=%d, comm=%s, state=%x", task->pid, task->comm,
             task->__state);
}

static void print_rq(struct rq *rq) {
  SCHED_INFO("cpu=%d, nr_running=%d, nr_queued=%u, clock=%llu", cpu_of(rq),
             rq->nr_running, rq->cfs.nr_queued, rq->clock);
  struct rb_node *node = rb_first_cached(&rq->cfs.tasks_timeline);
  while (node) {
    struct sched_entity *se = rb_entry(node, struct sched_entity, run_node);
    print_task(task_of(se));
    node = rb_next(&se->run_node);
  }
}
