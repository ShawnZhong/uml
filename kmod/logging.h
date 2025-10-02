#include <linux/printk.h>

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
