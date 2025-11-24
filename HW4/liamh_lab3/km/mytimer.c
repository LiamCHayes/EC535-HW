#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h> 
#include <linux/string.h> 
#include <linux/slab.h> 
#include <linux/fs.h> 
#include <linux/jiffies.h> 
#include <asm/uaccess.h> 
#include <linux/seq_file.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("Dual BSD/GPL");

// Proc file entry
static struct proc_dir_entry *proc_entry;

// Initialize character device functions
static int mytimer_fasync(int fd, struct file *filp, int mode);
static int mytimer_open(struct inode *inode, struct file *filp);
static int mytimer_release(struct inode *inode, struct file *filp);
static ssize_t mytimer_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static ssize_t mytimer_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static int mytimer_init(void);
static void mytimer_exit(void);
static void mytimer_handler(struct timer_list*);

struct file_operations mytimer_fops = {
write:
    mytimer_write,
read:
    mytimer_read,
open:
    mytimer_open,
release:
    mytimer_release,
fasync:
    mytimer_fasync
};

// Initialize procfs functions
static int mytimer_proc_open(struct inode*, struct file*);
static int mytimer_proc_show(struct seq_file*, void*);

static struct file_operations mytimer_pops = {
	.owner = THIS_MODULE,
	.open = mytimer_proc_open,
	.read = seq_read,
	.release = single_release,
};

// Declare init and exit
module_init(mytimer_init);
module_exit(mytimer_exit);

static int mytimer_major = 61; 
struct fasync_struct *async_queue; 

// Timer things
static struct timer_list * timer; 
char *message;
char *msg_status;

static void timer_handler(struct timer_list *data) {
    if (async_queue)
        kill_fasync(&async_queue, SIGIO, POLL_IN);

    del_timer(timer);
    kfree(message);
    message = NULL;
}

// Proc things
static unsigned long module_load_time;
static pid_t pid;
static char command_name[TASK_COMM_LEN];

static int mytimer_init(void) {
    int result;

    // Register device
    result = register_chrdev(mytimer_major, "mytimer", &mytimer_fops);
    if (result < 0)
    {
        printk(KERN_ALERT
               "mytimer: cannot obtain major number %d\n", mytimer_major);
        return result;
    }

    // Allocate space
    timer = (struct timer_list *) kmalloc(sizeof(struct timer_list), GFP_KERNEL);

    if (!timer)
    {
        printk(KERN_ALERT "Insufficient kernel memory\n");
        result = -ENOMEM;
        goto fail;
    } else {
		// Create procfs entry
		proc_entry = proc_create("mytimer", 0644, NULL, &mytimer_pops);
		if (proc_entry == NULL) {
			result = -ENOMEM;
			kfree(timer);
			printk(KERN_INFO "mytimer: Couldn't create proc entry\n");
		} 
	}

    module_load_time = jiffies;

    printk("mytimer loaded.\n");
    return 0;

fail:
    mytimer_exit();
    return result;
}

static void mytimer_exit(void) {
    unregister_chrdev(mytimer_major, "mytimer");
    remove_proc_entry("mytimer", NULL);
    if (timer)
        kfree(timer);

    printk(KERN_ALERT "Removing mytimer module\n");

}

static int mytimer_open(struct inode *inode, struct file *filp) {
    return 0;
}

static int mytimer_release(struct inode *inode, struct file *filp) {
    mytimer_fasync(-1, filp, 0);
    return 0;
}

static int mytimer_proc_show(struct seq_file *m, void *v) {
	char buf[256];
	unsigned long expires;
	unsigned long time_left_jiffies;
	struct timespec ts;
	long time_left_seconds;
	char sec[8];
	unsigned long elapsed_ms;
	
	// time since module loaded in ms
	elapsed_ms = jiffies_to_msecs(jiffies - module_load_time); 

	// time the timer has left
	expires = timer->expires;
	time_left_jiffies = expires - jiffies;		
	jiffies_to_timespec(time_left_jiffies, &ts);
	time_left_seconds = ts.tv_sec;
	scnprintf(sec, sizeof(sec), "%ld", time_left_seconds);
	
	// create string to print and store it in buf
	if (timer_pending(timer)) {
		snprintf(buf, sizeof(buf), "[mytimer]: %lu ms [PID]: %d [CMD]: %s [SEC]: %s",
				elapsed_ms, pid, command_name, sec);
	} else {
		snprintf(buf, sizeof(buf), "[mytimer]: %lu ms", elapsed_ms);
	}

	seq_printf(m, "%s\n", buf);
	return 0;
}

static int mytimer_proc_open(struct inode *inode, struct file *file) {
    return single_open(file, mytimer_proc_show, NULL);
}

static ssize_t mytimer_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
	if (timer_pending(timer)) {
		// Check how many seconds are left
		unsigned long expires = timer->expires;
		unsigned long time_left_jiffies = expires - jiffies;		
		struct timespec ts;
		jiffies_to_timespec(time_left_jiffies, &ts);
		long time_left_seconds = ts.tv_sec;
		char sec[8];
		scnprintf(sec, sizeof(sec), "%ld", time_left_seconds);

		// Get message
		char line[256];
		int len;
		len = snprintf(line, sizeof(line), "%s %s\n", message, sec);

		// Print line
		if (*f_pos >= strlen(line)) {
			return 0; 
		}
		if (count > strlen(line) - *f_pos) {
			count = strlen(line) - *f_pos;
		}

		if (raw_copy_to_user(buf, line + *f_pos, count)) {
			return -EFAULT;
		}
		*f_pos += count;
	}
	return count;
}

static ssize_t mytimer_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
	// Read in len of time or message from the userspace 
	char kbuf[64];
	if (count > sizeof(kbuf) - 1) 
		return -EINVAL;

	if (raw_copy_from_user(kbuf, buf, count)) 
		return -EFAULT;
	kbuf[count] = '\0';

	char *str = kbuf;
	char *token;
	char *new_message;
	token = strsep(&str, " ");

	if (strcmp(token, "-s") == 0) {
		// Get timer len
		token = strsep(&str, " ");
		long value;
		int ret;
		ret = kstrtol(token, 10, &value);
		if (ret != 0) {
			printk(KERN_ERR "Conversion to int failed\n");
		}
		size_t timer_len = (size_t)value * 1000;		

		// Check if timer already exists and has the same message
		if (strcmp(msg_status, "update") == 0) {
			// Update timer
			mod_timer(timer, jiffies + msecs_to_jiffies(timer_len));
			return -EIO;
		} else if (strcmp(msg_status, "new timer") == 0) {
			// Start timer and store message
			timer_setup(timer, timer_handler, 0);
			mod_timer(timer, jiffies + msecs_to_jiffies(timer_len));
		} else {
			// Cannot add another timer
			return -EINVAL;
		}
	} else if (strcmp(token, "-w") == 0) {
		// Get new message and check for the conditions
		new_message = strsep(&str, " ");
		if (message && strcmp(new_message, message) == 0) {
			msg_status = "update";
		} else if (message == NULL) {
			msg_status = "new timer";
			message = kstrdup(new_message, GFP_KERNEL);
		} else {
			msg_status = "error";
		}
		
		// Get the pid and command name of user program for proc
		pid = pid_nr(filp->f_owner.pid);
		struct task_struct *owner_task = pid_task(find_vpid(pid), PIDTYPE_PID);
		if (owner_task) {
			strncpy(command_name, owner_task->comm, TASK_COMM_LEN);
			command_name[TASK_COMM_LEN - 1] = '\0';
		} else {
			printk(KERN_INFO "[INFO] No owner task found for PID %d\n", pid);
		}
	} else if (strcmp(token, "-r") == 0) {
		// Remove timer
        if (timer_pending(timer)) {
            struct siginfo info;
            struct task_struct *task = pid_task(find_vpid(pid), PIDTYPE_PID);

            memset(&info, 0, sizeof(struct siginfo));
            info.si_signo = SIGIO;     
            info.si_code = SI_QUEUE;  
            info.si_int = 1; 

            send_sig_info(SIGIO, &info, task);

            del_timer_sync(timer);
            kfree(message);
            message = NULL;
        }
	}
	
	return count;
}

static int mytimer_fasync(int fd, struct file *filp, int mode) {
    return fasync_helper(fd, filp, mode, &async_queue);
}
