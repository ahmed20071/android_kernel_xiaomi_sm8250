/*
 * Alioth Ultra Performance Scheduler Tweaks
 * Optimized for maximum UI smoothness on SM8250
 */

#include <linux/sched.h>
#include <linux/cpufreq.h>
#include <linux/input.h>
#include <linux/slab.h>

#define ALIOTH_TAG "AliothPerf"

// Input boost configuration
static unsigned int input_boost_duration_ms = 100;
static unsigned int input_boost_freq_little = 1804800;
static unsigned int input_boost_freq_big = 2208000;
static unsigned int input_boost_freq_prime = 2419200;

// CPU boost configuration  
static unsigned int cpu_boost_duration_ms = 100;
static unsigned int cpu_boost_freq_little = 1804800;
static unsigned int cpu_boost_freq_big = 2208000;
static unsigned int cpu_boost_freq_prime = 2419200;

// GPU boost configuration
static unsigned int gpu_boost_freq = 800000000;
static unsigned int gpu_boost_duration_ms = 100;

// RAM boost configuration
static unsigned int ram_boost_swappiness = 60;
static unsigned int ram_boost_cache_pressure = 100;

// VSync control
static bool vsync_enabled = true;
static bool dynamic_vsync = true;

// FSync control
static bool fsync_enabled = true;

// Scheduler tunables
static unsigned int sched_ravg_window = 1000000;
static unsigned int sched_pred_alert_freq = 1000000;
static unsigned int sched_upmigrate = 90;
static unsigned int sched_downmigrate = 75;

// UCLAMP configuration
static unsigned int uclamp_min_perf = 0;
static unsigned int uclamp_max_perf = 100;

// Touchscreen boost
static bool touchscreen_boost = true;
static unsigned int touchscreen_boost_duration = 50;

// Frame boost (for UI smoothness)
static bool frame_boost = true;
static unsigned int frame_boost_duration = 16; // ~60fps

static struct workqueue_struct *alioth_wq;
static struct work_struct input_boost_work;
static struct delayed_work input_boost_rem;

static void alioth_input_boost(struct work_struct *work)
{
    struct cpufreq_policy *policy;
    int cpu;

    // Boost little cores
    for_each_online_cpu(cpu) {
        if (cpu < 4) {
            policy = cpufreq_cpu_get(cpu);
            if (policy) {
                __cpufreq_driver_target(policy, input_boost_freq_little, CPUFREQ_RELATION_H);
                cpufreq_cpu_put(policy);
            }
        }
    }

    // Boost big cores
    for_each_online_cpu(cpu) {
        if (cpu >= 4 && cpu < 7) {
            policy = cpufreq_cpu_get(cpu);
            if (policy) {
                __cpufreq_driver_target(policy, input_boost_freq_big, CPUFREQ_RELATION_H);
                cpufreq_cpu_put(policy);
            }
        }
    }

    // Boost prime core
    policy = cpufreq_cpu_get(7);
    if (policy) {
        __cpufreq_driver_target(policy, input_boost_freq_prime, CPUFREQ_RELATION_H);
        cpufreq_cpu_put(policy);
    }

    // Schedule removal
    schedule_delayed_work(&input_boost_rem, msecs_to_jiffies(input_boost_duration_ms));
}

static void alioth_input_boost_rem(struct work_struct *work)
{
    // Boost removed automatically by governor
}

static void alioth_input_event(struct input_handle *handle, unsigned int type,
                               unsigned int code, int value)
{
    if (type == EV_SYN && code == SYN_REPORT) {
        if (touchscreen_boost) {
            queue_work(alioth_wq, &input_boost_work);
        }
    }
}

static int alioth_input_connect(struct input_handler *handler,
                                struct input_dev *dev,
                                const struct input_device_id *id)
{
    struct input_handle *handle;
    int error;

    handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "alioth_input";

    error = input_register_handle(handle);
    if (error)
        goto err2;

    error = input_open_device(handle);
    if (error)
        goto err1;

    return 0;

err1:
    input_unregister_handle(handle);
err2:
    kfree(handle);
    return error;
}

static void alioth_input_disconnect(struct input_handle *handle)
{
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static const struct input_device_id alioth_ids[] = {
    { .driver_info = 1 },
    { },
};

static struct input_handler alioth_input_handler = {
    .event          = alioth_input_event,
    .connect        = alioth_input_connect,
    .disconnect     = alioth_input_disconnect,
    .name           = "alioth_input_handler",
    .id_table       = alioth_ids,
};

// Sysfs interface for Franco Kernel Manager compatibility
static ssize_t input_boost_duration_ms_show(struct kobject *kobj,
                                            struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u\n", input_boost_duration_ms);
}

static ssize_t input_boost_duration_ms_store(struct kobject *kobj,
                                             struct kobj_attribute *attr,
                                             const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    input_boost_duration_ms = val;
    return count;
}

static ssize_t input_boost_freq_little_show(struct kobject *kobj,
                                            struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u\n", input_boost_freq_little);
}

static ssize_t input_boost_freq_little_store(struct kobject *kobj,
                                             struct kobj_attribute *attr,
                                             const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    input_boost_freq_little = val;
    return count;
}

static ssize_t input_boost_freq_big_show(struct kobject *kobj,
                                         struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u\n", input_boost_freq_big);
}

static ssize_t input_boost_freq_big_store(struct kobject *kobj,
                                          struct kobj_attribute *attr,
                                          const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    input_boost_freq_big = val;
    return count;
}

static ssize_t input_boost_freq_prime_show(struct kobject *kobj,
                                           struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u\n", input_boost_freq_prime);
}

static ssize_t input_boost_freq_prime_store(struct kobject *kobj,
                                            struct kobj_attribute *attr,
                                            const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    input_boost_freq_prime = val;
    return count;
}

static ssize_t cpu_boost_duration_ms_show(struct kobject *kobj,
                                          struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u\n", cpu_boost_duration_ms);
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

static ssize_t gpu_boost_freq_show(struct kobject *kobj,
                                   struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u\n", gpu_boost_freq);
}

static ssize_t gpu_boost_freq_store(struct kobject *kobj,
                                    struct kobj_attribute *attr,
                                    const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    gpu_boost_freq = val;
    return count;
}

static ssize_t gpu_boost_duration_ms_show(struct kobject *kobj,
                                          struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u\n", gpu_boost_duration_ms);
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

static ssize_t ram_boost_swappiness_show(struct kobject *kobj,
                                         struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%u\n", ram_boost_swappiness);
}

static ssize_t ram_boost_swappiness_store(struct kobject *kobj,
                                          struct kobj_attribute *attr,
                                          const char *buf, size_t count)
{
    unsigned int val;
    if (kstrtouint(buf, 10, &val))
        return -EINVAL;
    ram_boost_swappiness = val;
    return count;
}

static ssize_t vsync_enabled_show(struct kobject *kobj,
                                  struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", vsync_enabled);
}

static ssize_t vsync_enabled_store(struct kobject *kobj,
                                   struct kobj_attribute *attr,
                                   const char *buf, size_t count)
{
    int val;
    if (kstrtoint(buf, 10, &val))
        return -EINVAL;
    vsync_enabled = !!val;
    return count;
}

static ssize_t fsync_enabled_show(struct kobject *kobj,
                                  struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", fsync_enabled);
}

static ssize_t fsync_enabled_store(struct kobject *kobj,
                                   struct kobj_attribute *attr,
                                   const char *buf, size_t count)
{
    int val;
    if (kstrtoint(buf, 10, &val))
        return -EINVAL;
    fsync_enabled = !!val;
    return count;
}

static ssize_t touchscreen_boost_show(struct kobject *kobj,
                                      struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", touchscreen_boost);
}

static ssize_t touchscreen_boost_store(struct kobject *kobj,
                                     struct kobj_attribute *attr,
                                     const char *buf, size_t count)
{
    int val;
    if (kstrtoint(buf, 10, &val))
        return -EINVAL;
    touchscreen_boost = !!val;
    return count;
}

static ssize_t frame_boost_show(struct kobject *kobj,
                                struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", frame_boost);
}

static ssize_t frame_boost_store(struct kobject *kobj,
                                 struct kobj_attribute *attr,
                                 const char *buf, size_t count)
{
    int val;
    if (kstrtoint(buf, 10, &val))
        return -EINVAL;
    frame_boost = !!val;
    return count;
}

static struct kobj_attribute input_boost_duration_ms_attr =
    __ATTR(input_boost_duration_ms, 0664, input_boost_duration_ms_show, input_boost_duration_ms_store);

static struct kobj_attribute input_boost_freq_little_attr =
    __ATTR(input_boost_freq_little, 0664, input_boost_freq_little_show, input_boost_freq_little_store);

static struct kobj_attribute input_boost_freq_big_attr =
    __ATTR(input_boost_freq_big, 0664, input_boost_freq_big_show, input_boost_freq_big_store);

static struct kobj_attribute input_boost_freq_prime_attr =
    __ATTR(input_boost_freq_prime, 0664, input_boost_freq_prime_show, input_boost_freq_prime_store);

static struct kobj_attribute cpu_boost_duration_ms_attr =
    __ATTR(cpu_boost_duration_ms, 0664, cpu_boost_duration_ms_show, cpu_boost_duration_ms_store);

static struct kobj_attribute gpu_boost_freq_attr =
    __ATTR(gpu_boost_freq, 0664, gpu_boost_freq_show, gpu_boost_freq_store);

static struct kobj_attribute gpu_boost_duration_ms_attr =
    __ATTR(gpu_boost_duration_ms, 0664, gpu_boost_duration_ms_show, gpu_boost_duration_ms_store);

static struct kobj_attribute ram_boost_swappiness_attr =
    __ATTR(ram_boost_swappiness, 0664, ram_boost_swappiness_show, ram_boost_swappiness_store);

static struct kobj_attribute vsync_enabled_attr =
    __ATTR(vsync_enabled, 0664, vsync_enabled_show, vsync_enabled_store);

static struct kobj_attribute fsync_enabled_attr =
    __ATTR(fsync_enabled, 0664, fsync_enabled_show, fsync_enabled_store);

static struct kobj_attribute touchscreen_boost_attr =
    __ATTR(touchscreen_boost, 0664, touchscreen_boost_show, touchscreen_boost_store);

static struct kobj_attribute frame_boost_attr =
    __ATTR(frame_boost, 0664, frame_boost_show, frame_boost_store);

static struct attribute *alioth_attrs[] = {
    &input_boost_duration_ms_attr.attr,
    &input_boost_freq_little_attr.attr,
    &input_boost_freq_big_attr.attr,
    &input_boost_freq_prime_attr.attr,
    &cpu_boost_duration_ms_attr.attr,
    &gpu_boost_freq_attr.attr,
    &gpu_boost_duration_ms_attr.attr,
    &ram_boost_swappiness_attr.attr,
    &vsync_enabled_attr.attr,
    &fsync_enabled_attr.attr,
    &touchscreen_boost_attr.attr,
    &frame_boost_attr.attr,
    NULL,
};

static struct attribute_group alioth_attr_group = {
    .attrs = alioth_attrs,
    .name = "alioth_perf",
};

static int __init alioth_perf_init(void)
{
    int ret;
    struct kobject *alioth_kobj;

    pr_info("%s: Initializing Alioth Ultra Performance module\n", ALIOTH_TAG);

    alioth_wq = alloc_workqueue("alioth_wq", WQ_HIGHPRI | WQ_UNBOUND, 1);
    if (!alioth_wq)
        return -ENOMEM;

    INIT_WORK(&input_boost_work, alioth_input_boost);
    INIT_DELAYED_WORK(&input_boost_rem, alioth_input_boost_rem);

    ret = input_register_handler(&alioth_input_handler);
    if (ret) {
        pr_err("%s: Failed to register input handler\n", ALIOTH_TAG);
        destroy_workqueue(alioth_wq);
        return ret;
    }

    alioth_kobj = kobject_create_and_add("alioth_perf", kernel_kobj);
    if (!alioth_kobj) {
        pr_err("%s: Failed to create kobject\n", ALIOTH_TAG);
        input_unregister_handler(&alioth_input_handler);
        destroy_workqueue(alioth_wq);
        return -ENOMEM;
    }

    ret = sysfs_create_group(alioth_kobj, &alioth_attr_group);
    if (ret) {
        pr_err("%s: Failed to create sysfs group\n", ALIOTH_TAG);
        kobject_put(alioth_kobj);
        input_unregister_handler(&alioth_input_handler);
        destroy_workqueue(alioth_wq);
        return ret;
    }

    pr_info("%s: Alioth Ultra Performance module initialized\n", ALIOTH_TAG);
    return 0;
}

static void __exit alioth_perf_exit(void)
{
    cancel_work_sync(&input_boost_work);
    cancel_delayed_work_sync(&input_boost_rem);
    input_unregister_handler(&alioth_input_handler);
    destroy_workqueue(alioth_wq);

    pr_info("%s: Alioth Ultra Performance module exited\n", ALIOTH_TAG);
}

module_init(alioth_perf_init);
module_exit(alioth_perf_exit);

MODULE_AUTHOR("Alioth Ultra Kernel Team");
MODULE_DESCRIPTION("Alioth Ultra Performance Scheduler Tweaks for SM8250");
MODULE_LICENSE("GPL v2");
