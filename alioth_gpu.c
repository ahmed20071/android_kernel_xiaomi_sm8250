/*
 * Alioth GPU Boost & Control Module
 * Maximum GPU performance with sysfs tuning
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/platform_device.h>
#include <linux/devfreq.h>
#include <linux/msm_kgsl.h>

#define ALIOTH_GPU_TAG "AliothGPU"

// GPU frequency table for Adreno 650 (SM8250)
static unsigned int gpu_freq_table[] = {
    220000000,   // 220 MHz
    305000000,   // 305 MHz
    428000000,   // 428 MHz
    514000000,   // 514 MHz
    596000000,   // 596 MHz
    670000000,   // 670 MHz
    725000000,   // 725 MHz
    778000000,   // 778 MHz
    810000000,   // 810 MHz
    835000000,   // 835 MHz
    865000000,   // 865 MHz
    890000000,   // 890 MHz
    900000000,   // 900 MHz
};

#define GPU_FREQ_TABLE_SIZE (sizeof(gpu_freq_table) / sizeof(gpu_freq_table[0]))

// GPU boost settings
static unsigned int gpu_boost_freq = 900000000;
static unsigned int gpu_boost_duration_ms = 100;
static unsigned int gpu_min_freq = 220000000;
static unsigned int gpu_max_freq = 900000000;
static unsigned int gpu_default_pwrlevel = 5;
static unsigned int gpu_throttle_enable = 1;
static unsigned int gpu_bus_split = 1;
static unsigned int gpu_clk_always_on = 0;
static unsigned int gpu_force_clk_on = 0;
static unsigned int gpu_force_bus_on = 0;
static unsigned int gpu_force_rail_on = 0;
static unsigned int gpu_idle_timer = 80;
static unsigned int gpu_strtstp_sleepwake = 1;
static unsigned int gpu_llc_slice_enable = 1;
static unsigned int gpu_llc_slice_size = 0;
static unsigned int gpu_preemption_preemptlevel = 1;
static unsigned int gpu_preemption_timeslice = 5000;
static unsigned int gpu_preemption_cntl = 0x3;

// Governor settings
static char gpu_governor[16] = "msm-adreno-tz";
static unsigned int gpu_governor_msm_adreno_tz = 1;
static unsigned int gpu_governor_msm_adreno_ondemand = 0;
static unsigned int gpu_governor_msm_adreno_performance = 0;
static unsigned int gpu_governor_msm_adreno_powersave = 0;

// Sysfs interface
static ssize_t gpu_boost_freq_show(struct kobject *kobj,
                                   struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", gpu_boost_freq);
}

static ssize_t gpu_boost_freq_store(struct kobject *kobj,
                                    struct kobj_attribute *attr,
                                    const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    if (val < gpu_min_freq || val > gpu_max_freq)
        return -EINVAL;
    gpu_boost_freq = val;
    return count;
}

static ssize_t gpu_boost_duration_ms_show(struct kobject *kobj,
                                          struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", gpu_boost_duration_ms);
}

static ssize_t gpu_boost_duration_ms_store(struct kobject *kobj,
                                           struct kobj_attribute *attr,
                                           const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    gpu_boost_duration_ms = val;
    return count;
}

static ssize_t gpu_min_freq_show(struct kobject *kobj,
                                 struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", gpu_min_freq);
}

static ssize_t gpu_min_freq_store(struct kobject *kobj,
                                  struct kobj_attribute *attr,
                                  const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    gpu_min_freq = val;
    return count;
}

static ssize_t gpu_max_freq_show(struct kobject *kobj,
                                 struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", gpu_max_freq);
}

static ssize_t gpu_max_freq_store(struct kobject *kobj,
                                  struct kobj_attribute *attr,
                                  const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    gpu_max_freq = val;
    return count;
}

static ssize_t gpu_freq_table_show(struct kobject *kobj,
                                   struct kobj_attribute *attr, char *buf)
{
    int i, len = 0;
    for (i = 0; i < GPU_FREQ_TABLE_SIZE; i++) {
        len += sprintf(buf + len, "%u ", gpu_freq_table[i]);
    }
    len += sprintf(buf + len, "
");
    return len;
}

static ssize_t gpu_default_pwrlevel_show(struct kobject *kobj,
                                         struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", gpu_default_pwrlevel);
}

static ssize_t gpu_default_pwrlevel_store(struct kobject *kobj,
                                          struct kobj_attribute *attr,
                                          const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    gpu_default_pwrlevel = val;
    return count;
}

static ssize_t gpu_throttle_enable_show(struct kobject *kobj,
                                        struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", gpu_throttle_enable);
}

static ssize_t gpu_throttle_enable_store(struct kobject *kobj,
                                         struct kobj_attribute *attr,
                                         const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    gpu_throttle_enable = !!val;
    return count;
}

static ssize_t gpu_bus_split_show(struct kobject *kobj,
                                  struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", gpu_bus_split);
}

static ssize_t gpu_bus_split_store(struct kobject *kobj,
                                   struct kobj_attribute *attr,
                                   const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    gpu_bus_split = !!val;
    return count;
}

static ssize_t gpu_idle_timer_show(struct kobject *kobj,
                                   struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", gpu_idle_timer);
}

static ssize_t gpu_idle_timer_store(struct kobject *kobj,
                                    struct kobj_attribute *attr,
                                    const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    gpu_idle_timer = val;
    return count;
}

static ssize_t gpu_governor_show(struct kobject *kobj,
                                 struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s
", gpu_governor);
}

static ssize_t gpu_governor_store(struct kobject *kobj,
                                  struct kobj_attribute *attr,
                                  const char *buf, size_t count)
{
    if (count >= sizeof(gpu_governor))
        return -EINVAL;
    sscanf(buf, "%15s", gpu_governor);
    return count;
}

static ssize_t gpu_preemption_preemptlevel_show(struct kobject *kobj,
                                                struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u
", gpu_preemption_preemptlevel);
}

static ssize_t gpu_preemption_preemptlevel_store(struct kobject *kobj,
                                                 struct kobj_attribute *attr,
                                                 const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    gpu_preemption_preemptlevel = val;
    return count;
}

static struct kobj_attribute gpu_boost_freq_attr =
    __ATTR(gpu_boost_freq, 0664, gpu_boost_freq_show, gpu_boost_freq_store);

static struct kobj_attribute gpu_boost_duration_ms_attr =
    __ATTR(gpu_boost_duration_ms, 0664, gpu_boost_duration_ms_show, gpu_boost_duration_ms_store);

static struct kobj_attribute gpu_min_freq_attr =
    __ATTR(gpu_min_freq, 0664, gpu_min_freq_show, gpu_min_freq_store);

static struct kobj_attribute gpu_max_freq_attr =
    __ATTR(gpu_max_freq, 0664, gpu_max_freq_show, gpu_max_freq_store);

static struct kobj_attribute gpu_freq_table_attr =
    __ATTR(gpu_freq_table, 0444, gpu_freq_table_show, NULL);

static struct kobj_attribute gpu_default_pwrlevel_attr =
    __ATTR(gpu_default_pwrlevel, 0664, gpu_default_pwrlevel_show, gpu_default_pwrlevel_store);

static struct kobj_attribute gpu_throttle_enable_attr =
    __ATTR(gpu_throttle_enable, 0664, gpu_throttle_enable_show, gpu_throttle_enable_store);

static struct kobj_attribute gpu_bus_split_attr =
    __ATTR(gpu_bus_split, 0664, gpu_bus_split_show, gpu_bus_split_store);

static struct kobj_attribute gpu_idle_timer_attr =
    __ATTR(gpu_idle_timer, 0664, gpu_idle_timer_show, gpu_idle_timer_store);

static struct kobj_attribute gpu_governor_attr =
    __ATTR(gpu_governor, 0664, gpu_governor_show, gpu_governor_store);

static struct kobj_attribute gpu_preemption_preemptlevel_attr =
    __ATTR(gpu_preemption_preemptlevel, 0664, gpu_preemption_preemptlevel_show, gpu_preemption_preemptlevel_store);

static struct attribute *alioth_gpu_attrs[] = {
    &gpu_boost_freq_attr.attr,
    &gpu_boost_duration_ms_attr.attr,
    &gpu_min_freq_attr.attr,
    &gpu_max_freq_attr.attr,
    &gpu_freq_table_attr.attr,
    &gpu_default_pwrlevel_attr.attr,
    &gpu_throttle_enable_attr.attr,
    &gpu_bus_split_attr.attr,
    &gpu_idle_timer_attr.attr,
    &gpu_governor_attr.attr,
    &gpu_preemption_preemptlevel_attr.attr,
    NULL,
};

static struct attribute_group alioth_gpu_attr_group = {
    .attrs = alioth_gpu_attrs,
    .name = "alioth_gpu",
};

static int __init alioth_gpu_init(void)
{
    int ret;
    struct kobject *alioth_gpu_kobj;

    pr_info("%s: Initializing Alioth GPU Boost Module
", ALIOTH_GPU_TAG);

    alioth_gpu_kobj = kobject_create_and_add("alioth_gpu", kernel_kobj);
    if (!alioth_gpu_kobj) {
        pr_err("%s: Failed to create kobject
", ALIOTH_GPU_TAG);
        return -ENOMEM;
    }

    ret = sysfs_create_group(alioth_gpu_kobj, &alioth_gpu_attr_group);
    if (ret) {
        pr_err("%s: Failed to create sysfs group
", ALIOTH_GPU_TAG);
        kobject_put(alioth_gpu_kobj);
        return ret;
    }

    pr_info("%s: Alioth GPU Boost Module initialized
", ALIOTH_GPU_TAG);
    pr_info("%s: GPU Frequency Table: %u - %u MHz
", ALIOTH_GPU_TAG,
            gpu_freq_table[0] / 1000000, gpu_freq_table[GPU_FREQ_TABLE_SIZE - 1] / 1000000);
    return 0;
}

static void __exit alioth_gpu_exit(void)
{
    pr_info("%s: Alioth GPU Boost Module exited
", ALIOTH_GPU_TAG);
}

module_init(alioth_gpu_init);
module_exit(alioth_gpu_exit);

MODULE_AUTHOR("Alioth Ultra Kernel Team");
MODULE_DESCRIPTION("Alioth GPU Boost & Control for Adreno 650");
MODULE_LICENSE("GPL v2");
