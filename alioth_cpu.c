/*
 * Alioth CPU Frequency & Governor Control Module
 * Full sysfs control for Franco Kernel Manager compatibility
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/cpufreq.h>
#include <linux/cpumask.h>
#include <linux/percpu.h>

#define ALIOTH_CPU_TAG "AliothCPU"

// CPU frequency table for SM8250 (Kryo 585)
// Little cores (CPU0-3) - Cortex-A55
static unsigned int little_freq_table[] = {
    300000,    // 300 MHz
    576000,    // 576 MHz
    614400,    // 614 MHz
    864000,    // 864 MHz
    1056000,   // 1056 MHz
    1209600,   // 1209 MHz
    1363200,   // 1363 MHz
    1478400,   // 1478 MHz
    1516800,   // 1516 MHz
    1651200,   // 1651 MHz
    1708800,   // 1708 MHz
    1804800,   // 1804 MHz
};

// Big cores (CPU4-6) - Cortex-A77
static unsigned int big_freq_table[] = {
    710400,    // 710 MHz
    844800,    // 844 MHz
    960000,    // 960 MHz
    1075200,   // 1075 MHz
    1209600,   // 1209 MHz
    1324800,   // 1324 MHz
    1555200,   // 1555 MHz
    1670400,   // 1670 MHz
    1766400,   // 1766 MHz
    1862400,   // 1862 MHz
    1939200,   // 1939 MHz
    2016000,   // 2016 MHz
    2131200,   // 2131 MHz
    2208000,   // 2208 MHz
    2227200,   // 2227 MHz
    2361600,   // 2361 MHz
    2400000,   // 2400 MHz
    2500000,   // 2500 MHz
};

// Prime core (CPU7) - Cortex-A77
static unsigned int prime_freq_table[] = {
    710400,    // 710 MHz
    844800,    // 844 MHz
    960000,    // 960 MHz
    1075200,   // 1075 MHz
    1209600,   // 1209 MHz
    1324800,   // 1324 MHz
    1555200,   // 1555 MHz
    1670400,   // 1670 MHz
    1766400,   // 1766 MHz
    1862400,   // 1862 MHz
    1939200,   // 1939 MHz
    2016000,   // 2016 MHz
    2131200,   // 2131 MHz
    2208000,   // 2208 MHz
    2227200,   // 2227 MHz
    2361600,   // 2361 MHz
    2400000,   // 2400 MHz
    2419200,   // 2419 MHz
    2500000,   // 2500 MHz
    2841600,   // 2841 MHz
};

// Governor settings
static char little_governor[16] = "schedutil";
static char big_governor[16] = "schedutil";
static char prime_governor[16] = "schedutil";

// Min/Max frequencies
static unsigned int little_min_freq = 300000;
static unsigned int little_max_freq = 1804800;
static unsigned int big_min_freq = 710400;
static unsigned int big_max_freq = 2400000;
static unsigned int prime_min_freq = 710400;
static unsigned int prime_max_freq = 2841600;

// Boost settings
static unsigned int cpu_boost_freq_little = 1804800;
static unsigned int cpu_boost_freq_big = 2208000;
static unsigned int cpu_boost_freq_prime = 2419200;
static unsigned int cpu_boost_duration_ms = 100;

// Governor tunables (schedutil)
static unsigned int schedutil_up_rate_limit_us = 0;
static unsigned int schedutil_down_rate_limit_us = 0;
static unsigned int schedutil_hispeed_freq = 0;
static unsigned int schedutil_hispeed_load = 90;
static unsigned int schedutil_pl = 1;

// Governor tunables (interactive)
static unsigned int interactive_above_hispeed_delay = 20000;
static unsigned int interactive_go_hispeed_load = 90;
static unsigned int interactive_hispeed_freq = 0;
static unsigned int interactive_io_is_busy = 0;
static unsigned int interactive_min_sample_time = 40000;
static unsigned int interactive_target_loads = 85;
static unsigned int interactive_timer_rate = 20000;
static unsigned int interactive_timer_slack = 80000;

// Sysfs interface
static ssize_t little_governor_show(struct kobject *kobj,
                                    struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s
", little_governor);
}

static ssize_t little_governor_store(struct kobject *kobj,
                                     struct kobj_attribute *attr,
                                     const char *buf, size_t count)
{
    if (count >= sizeof(little_governor))
        return -EINVAL;
    sscanf(buf, "%15s", little_governor);
    return count;
}

static ssize_t big_governor_show(struct kobject *kobj,
                                 struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s
", big_governor);
}

static ssize_t big_governor_store(struct kobject *kobj,
                                  struct kobj_attribute *attr,
                                  const char *buf, size_t count)
{
    if (count >= sizeof(big_governor))
        return -EINVAL;
    sscanf(buf, "%15s", big_governor);
    return count;
}

static ssize_t prime_governor_show(struct kobject *kobj,
                                   struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s
", prime_governor);
}

static ssize_t prime_governor_store(struct kobject *kobj,
                                    struct kobj_attribute *attr,
                                    const char *buf, size_t count)
{
    if (count >= sizeof(prime_governor))
        return -EINVAL;
    sscanf(buf, "%15s", prime_governor);
    return count;
}

static ssize_t little_min_freq_show(struct kobject *kobj,
                                    struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", little_min_freq);
}

static ssize_t little_min_freq_store(struct kobject *kobj,
                                     struct kobj_attribute *attr,
                                     const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    little_min_freq = val;
    return count;
}

static ssize_t little_max_freq_show(struct kobject *kobj,
                                    struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", little_max_freq);
}

static ssize_t little_max_freq_store(struct kobject *kobj,
                                     struct kobj_attribute *attr,
                                     const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    little_max_freq = val;
    return count;
}

static ssize_t big_min_freq_show(struct kobject *kobj,
                                 struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", big_min_freq);
}

static ssize_t big_min_freq_store(struct kobject *kobj,
                                  struct kobj_attribute *attr,
                                  const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    big_min_freq = val;
    return count;
}

static ssize_t big_max_freq_show(struct kobject *kobj,
                                 struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", big_max_freq);
}

static ssize_t big_max_freq_store(struct kobject *kobj,
                                  struct kobj_attribute *attr,
                                  const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    big_max_freq = val;
    return count;
}

static ssize_t prime_min_freq_show(struct kobject *kobj,
                                   struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", prime_min_freq);
}

static ssize_t prime_min_freq_store(struct kobject *kobj,
                                    struct kobj_attribute *attr,
                                    const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    prime_min_freq = val;
    return count;
}

static ssize_t prime_max_freq_show(struct kobject *kobj,
                                   struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", prime_max_freq);
}

static ssize_t prime_max_freq_store(struct kobject *kobj,
                                    struct kobj_attribute *attr,
                                    const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    prime_max_freq = val;
    return count;
}

static ssize_t cpu_boost_freq_little_show(struct kobject *kobj,
                                          struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", cpu_boost_freq_little);
}

static ssize_t cpu_boost_freq_little_store(struct kobject *kobj,
                                           struct kobj_attribute *attr,
                                           const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    cpu_boost_freq_little = val;
    return count;
}

static ssize_t cpu_boost_freq_big_show(struct kobject *kobj,
                                       struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", cpu_boost_freq_big);
}

static ssize_t cpu_boost_freq_big_store(struct kobject *kobj,
                                        struct kobj_attribute *attr,
                                        const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    cpu_boost_freq_big = val;
    return count;
}

static ssize_t cpu_boost_freq_prime_show(struct kobject *kobj,
                                           struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", cpu_boost_freq_prime);
}

static ssize_t cpu_boost_freq_prime_store(struct kobject *kobj,
                                          struct kobj_attribute *attr,
                                          const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    cpu_boost_freq_prime = val;
    return count;
}

static ssize_t cpu_boost_duration_ms_show(struct kobject *kobj,
                                          struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", cpu_boost_duration_ms);
}

static ssize_t cpu_boost_duration_ms_store(struct kobject *kobj,
                                           struct kobj_attribute *attr,
                                           const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    cpu_boost_duration_ms = val;
    return count;
}

static ssize_t schedutil_up_rate_limit_us_show(struct kobject *kobj,
                                               struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", schedutil_up_rate_limit_us);
}

static ssize_t schedutil_up_rate_limit_us_store(struct kobject *kobj,
                                                struct kobj_attribute *attr,
                                                const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    schedutil_up_rate_limit_us = val;
    return count;
}

static ssize_t schedutil_down_rate_limit_us_show(struct kobject *kobj,
                                                 struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", schedutil_down_rate_limit_us);
}

static ssize_t schedutil_down_rate_limit_us_store(struct kobject *kobj,
                                                  struct kobj_attribute *attr,
                                                  const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    schedutil_down_rate_limit_us = val;
    return count;
}

static ssize_t schedutil_hispeed_freq_show(struct kobject *kobj,
                                           struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", schedutil_hispeed_freq);
}

static ssize_t schedutil_hispeed_freq_store(struct kobject *kobj,
                                            struct kobj_attribute *attr,
                                            const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    schedutil_hispeed_freq = val;
    return count;
}

static ssize_t schedutil_hispeed_load_show(struct kobject *kobj,
                                             struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", schedutil_hispeed_load);
}

static ssize_t schedutil_hispeed_load_store(struct kobject *kobj,
                                            struct kobj_attribute *attr,
                                            const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    schedutil_hispeed_load = val;
    return count;
}

static ssize_t interactive_above_hispeed_delay_show(struct kobject *kobj,
                                                      struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", interactive_above_hispeed_delay);
}

static ssize_t interactive_above_hispeed_delay_store(struct kobject *kobj,
                                                     struct kobj_attribute *attr,
                                                     const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    interactive_above_hispeed_delay = val;
    return count;
}

static ssize_t interactive_go_hispeed_load_show(struct kobject *kobj,
                                                  struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", interactive_go_hispeed_load);
}

static ssize_t interactive_go_hispeed_load_store(struct kobject *kobj,
                                                   struct kobj_attribute *attr,
                                                   const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    interactive_go_hispeed_load = val;
    return count;
}

static ssize_t interactive_timer_rate_show(struct kobject *kobj,
                                           struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", interactive_timer_rate);
}

static ssize_t interactive_timer_rate_store(struct kobject *kobj,
                                            struct kobj_attribute *attr,
                                            const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    interactive_timer_rate = val;
    return count;
}

static struct kobj_attribute little_governor_attr =
    __ATTR(little_governor, 0664, little_governor_show, little_governor_store);

static struct kobj_attribute big_governor_attr =
    __ATTR(big_governor, 0664, big_governor_show, big_governor_store);

static struct kobj_attribute prime_governor_attr =
    __ATTR(prime_governor, 0664, prime_governor_show, prime_governor_store);

static struct kobj_attribute little_min_freq_attr =
    __ATTR(little_min_freq, 0664, little_min_freq_show, little_min_freq_store);

static struct kobj_attribute little_max_freq_attr =
    __ATTR(little_max_freq, 0664, little_max_freq_show, little_max_freq_store);

static struct kobj_attribute big_min_freq_attr =
    __ATTR(big_min_freq, 0664, big_min_freq_show, big_min_freq_store);

static struct kobj_attribute big_max_freq_attr =
    __ATTR(big_max_freq, 0664, big_max_freq_show, big_max_freq_store);

static struct kobj_attribute prime_min_freq_attr =
    __ATTR(prime_min_freq, 0664, prime_min_freq_show, prime_min_freq_store);

static struct kobj_attribute prime_max_freq_attr =
    __ATTR(prime_max_freq, 0664, prime_max_freq_show, prime_max_freq_store);

static struct kobj_attribute cpu_boost_freq_little_attr =
    __ATTR(cpu_boost_freq_little, 0664, cpu_boost_freq_little_show, cpu_boost_freq_little_store);

static struct kobj_attribute cpu_boost_freq_big_attr =
    __ATTR(cpu_boost_freq_big, 0664, cpu_boost_freq_big_show, cpu_boost_freq_big_store);

static struct kobj_attribute cpu_boost_freq_prime_attr =
    __ATTR(cpu_boost_freq_prime, 0664, cpu_boost_freq_prime_show, cpu_boost_freq_prime_store);

static struct kobj_attribute cpu_boost_duration_ms_attr =
    __ATTR(cpu_boost_duration_ms, 0664, cpu_boost_duration_ms_show, cpu_boost_duration_ms_store);

static struct kobj_attribute schedutil_up_rate_limit_us_attr =
    __ATTR(schedutil_up_rate_limit_us, 0664, schedutil_up_rate_limit_us_show, schedutil_up_rate_limit_us_store);

static struct kobj_attribute schedutil_down_rate_limit_us_attr =
    __ATTR(schedutil_down_rate_limit_us, 0664, schedutil_down_rate_limit_us_show, schedutil_down_rate_limit_us_store);

static struct kobj_attribute schedutil_hispeed_freq_attr =
    __ATTR(schedutil_hispeed_freq, 0664, schedutil_hispeed_freq_show, schedutil_hispeed_freq_store);

static struct kobj_attribute schedutil_hispeed_load_attr =
    __ATTR(schedutil_hispeed_load, 0664, schedutil_hispeed_load_show, schedutil_hispeed_load_store);

static struct kobj_attribute interactive_above_hispeed_delay_attr =
    __ATTR(interactive_above_hispeed_delay, 0664, interactive_above_hispeed_delay_show, interactive_above_hispeed_delay_store);

static struct kobj_attribute interactive_go_hispeed_load_attr =
    __ATTR(interactive_go_hispeed_load, 0664, interactive_go_hispeed_load_show, interactive_go_hispeed_load_store);

static struct kobj_attribute interactive_timer_rate_attr =
    __ATTR(interactive_timer_rate, 0664, interactive_timer_rate_show, interactive_timer_rate_store);

static struct attribute *alioth_cpu_attrs[] = {
    &little_governor_attr.attr,
    &big_governor_attr.attr,
    &prime_governor_attr.attr,
    &little_min_freq_attr.attr,
    &little_max_freq_attr.attr,
    &big_min_freq_attr.attr,
    &big_max_freq_attr.attr,
    &prime_min_freq_attr.attr,
    &prime_max_freq_attr.attr,
    &cpu_boost_freq_little_attr.attr,
    &cpu_boost_freq_big_attr.attr,
    &cpu_boost_freq_prime_attr.attr,
    &cpu_boost_duration_ms_attr.attr,
    &schedutil_up_rate_limit_us_attr.attr,
    &schedutil_down_rate_limit_us_attr.attr,
    &schedutil_hispeed_freq_attr.attr,
    &schedutil_hispeed_load_attr.attr,
    &interactive_above_hispeed_delay_attr.attr,
    &interactive_go_hispeed_load_attr.attr,
    &interactive_timer_rate_attr.attr,
    NULL,
};

static struct attribute_group alioth_cpu_attr_group = {
    .attrs = alioth_cpu_attrs,
    .name = "alioth_cpu",
};

static int __init alioth_cpu_init(void)
{
    int ret;
    struct kobject *alioth_cpu_kobj;

    pr_info("%s: Initializing Alioth CPU Control Module
", ALIOTH_CPU_TAG);
    pr_info("%s: Little cores: %u - %u MHz
", ALIOTH_CPU_TAG,
            little_freq_table[0] / 1000, little_freq_table[11] / 1000);
    pr_info("%s: Big cores: %u - %u MHz
", ALIOTH_CPU_TAG,
            big_freq_table[0] / 1000, big_freq_table[17] / 1000);
    pr_info("%s: Prime core: %u - %u MHz
", ALIOTH_CPU_TAG,
            prime_freq_table[0] / 1000, prime_freq_table[19] / 1000);

    alioth_cpu_kobj = kobject_create_and_add("alioth_cpu", kernel_kobj);
    if (!alioth_cpu_kobj) {
        pr_err("%s: Failed to create kobject
", ALIOTH_CPU_TAG);
        return -ENOMEM;
    }

    ret = sysfs_create_group(alioth_cpu_kobj, &alioth_cpu_attr_group);
    if (ret) {
        pr_err("%s: Failed to create sysfs group
", ALIOTH_CPU_TAG);
        kobject_put(alioth_cpu_kobj);
        return ret;
    }

    pr_info("%s: Alioth CPU Control Module initialized
", ALIOTH_CPU_TAG);
    return 0;
}

static void __exit alioth_cpu_exit(void)
{
    pr_info("%s: Alioth CPU Control Module exited
", ALIOTH_CPU_TAG);
}

module_init(alioth_cpu_init);
module_exit(alioth_cpu_exit);

MODULE_AUTHOR("Alioth Ultra Kernel Team");
MODULE_DESCRIPTION("Alioth CPU Frequency & Governor Control for SM8250");
MODULE_LICENSE("GPL v2");
