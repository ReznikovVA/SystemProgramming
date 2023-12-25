#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rv");

#define DEVICE_NAME "watchdog"

static int timeout_seconds = 60;
module_param(timeout_seconds, int, 0644);
static int signal_number = SIGTERM;
module_param(signal_number, int, 0644);

static int major_number;
static struct proc_dir_entry *proc_entry;
static struct timer_list watchdog_timer;
static struct list_head process_list;
static spinlock_t lock;

struct monitored_process {
    struct list_head list;
    pid_t pid;
    unsigned long last_interaction_time;
};

static void timer_callback(struct timer_list *timer) {
    struct monitored_process *proc_entry;
    struct list_head *pos, *q;
    unsigned long current_time = jiffies;

    spin_lock(&lock);
    list_for_each_safe(pos, q, &process_list){
        proc_entry = list_entry(pos, struct monitored_process, list);
        if (current_time - proc_entry->last_interaction_time > timeout_seconds * HZ) {
            send_sig(signal_number, pid_task(find_vpid(proc_entry->pid), PIDTYPE_PID), 1);
        }
    }
    spin_unlock(&lock);

    mod_timer(timer, jiffies + timeout_seconds * HZ);
}

static ssize_t watchdog_read(struct file *file, char __user *buffer, size_t count, loff_t *offset) {
    struct monitored_process *proc_entry;
    struct list_head *pos;
    unsigned long current_time = jiffies;

    spin_lock(&lock);
    list_for_each(pos, &process_list) {
        proc_entry = list_entry(pos, struct monitored_process, list);
        if (proc_entry->pid == current->pid) {
            proc_entry->last_interaction_time = current_time;
            break;
        }
    }
    spin_unlock(&lock);

    return 0;
}

static int watchdog_open(struct inode *inode, struct file *file) {
    struct monitored_process *new_proc;
    new_proc = kmalloc(sizeof(struct monitored_process), GFP_KERNEL);
    if (!new_proc)
        return -ENOMEM;

    new_proc->pid = current->pid;
    new_proc->last_interaction_time = jiffies;
    spin_lock(&lock);
    list_add(&new_proc->list, &process_list);
    spin_unlock(&lock);

    return 0;
}

static int watchdog_release(struct inode *inode, struct file *file) {
    struct monitored_process *proc_entry, *next;

    spin_lock(&lock);
    list_for_each_entry_safe(proc_entry, next, &process_list, list) {
        if (proc_entry->pid == current->pid) {
            list_del(&proc_entry->list);
            kfree(proc_entry);
            break;
        }
    }
    spin_unlock(&lock);

    return 0;
}

static const struct file_operations fops = {
    .open = watchdog_open,
    .release = watchdog_release,
    .read = watchdog_read,
};

static const struct proc_ops watchdog_proc_ops = {
    .proc_open = watchdog_open,
    .proc_release = watchdog_release,
    .proc_read = watchdog_read,
};

static int __init watchdog_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Watchdog device registration failed\n");
        return major_number;
    }

    INIT_LIST_HEAD(&process_list);
    timer_setup(&watchdog_timer, timer_callback, 0);
    mod_timer(&watchdog_timer, jiffies + timeout_seconds * HZ);

    spin_lock_init(&lock);

    proc_entry = proc_create("watchdog_info", 0, NULL, &watchdog_proc_ops);
    if (!proc_entry) {
        printk(KERN_ALERT "Failed to create proc entry\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "Watchdog device registered with major number: %d\n", major_number);
    return 0;
}

static void __exit watchdog_exit(void) {
    struct monitored_process *proc_entry, *next;

    spin_lock(&lock);
    list_for_each_entry_safe(proc_entry, next, &process_list, list) {
        list_del(&proc_entry->list);
        kfree(proc_entry);
    }
    spin_unlock(&lock);
del_timer(&watchdog_timer);
    remove_proc_entry("watchdog_info", NULL);
    unregister_chrdev(major_number, DEVICE_NAME);

    printk(KERN_INFO "Watchdog device unregistered\n");
}

module_init(watchdog_init);
module_exit(watchdog_exit);
