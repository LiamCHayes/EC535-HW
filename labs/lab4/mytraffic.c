#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/jiffies.h> 
#include <linux/timer.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

MODULE_LICENSE("Dual BSD/GPL");

// TODO find a way to listen for button press and set appropriate mode

// declare device file functions
static int mytraffic_open(struct inode *inode, struct file *filp);
static int mytraffic_release(struct inode *inode, struct file *filp);
static ssize_t mytraffic_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static ssize_t mytraffic_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static int mytraffic_init(void);
static void mytraffic_exit(void);
static void mytraffic_handler(struct timer_list*);

struct file_operations mytraffic_fops = {
write:
    mytraffic_write,
read:
    mytraffic_read,
open:
    mytraffic_open,
release:
    mytraffic_release,
};

module_init(mytraffic_init);
module_exit(mytraffic_exit);

static int mytraffic_major = 61; 

// Traffic light
enum Mode {NORMAL, FLASHING_RED, FLASHING_YELLOW};
enum LightColor {RED, YELLOW, GREEN, PEDESTRIAN, OFF};
int cycle_rate_seconds = 1;
int pedestrian = 0;

static enum Mode mode = NORMAL;
static enum LightColor light_color = GREEN;

static int btn0_gpio = 26;
static int btn1_gpio = 46;
static int btn0_irq;
static int btn1_irq;
static irqreturn_t button_isr(int irq, void *data) {
    if (irq == btn0_irq) {
        switch (mode) {
            case NORMAL:
                mode = FLASHING_RED;
                break;
            case FLASHING_RED:
                mode = FLASHING_YELLOW;
                break;
            case FLASHING_YELLOW:
                mode = NORMAL;
                break;
        }
    } else if (irq == btn1_irq) {
        if (mode == NORMAL) {
            pedestrian = 1;
        }
    }

    return IRQ_HANDLED;
}

static int red_gpio = 67;
static int yellow_gpio = 68;
static int green_gpio = 44;
static void set_light(enum LightColor color) {
    int ret;
    int gpios[] = {red_gpio, yellow_gpio, green_gpio};
    int i;

    // Set all GPIOs to 0 at first
    for (i=0; i<3; i++) {
        ret = gpio_request(gpios[i], "my_gpio");
        if (ret) {
            pr_err("Failed to request GPIO %d\n", gpios[i]);
            goto free_gpios;
        }
        ret = gpio_direction_output(gpios[i], 0);
        if (ret) {
            pr_err("Failed to set GPIO direction %d\n", gpios[i]);
            goto free_gpios;
        }
        gpio_set_value(gpios[i], 0);
    }

    switch (color) {
        case RED:
            gpio_set_value(red_gpio, 1);
            break;
        case YELLOW:
            gpio_set_value(yellow_gpio, 1);
            break;
        case GREEN:
            gpio_set_value(green_gpio, 1);
            break;
        case PEDESTRIAN:
            gpio_set_value(red_gpio, 1);
            gpio_set_value(yellow_gpio, 1);
            break;
        case OFF:
            // everything is already off
            break;
    }

    return 0;

free_gpios:
    // Free all requested GPIOs on error
    for (i = 0; i < 3; i++) {
        gpio_free(gpios[i]);
    }
    return ret;
}

static struct timer_list * timer; 

static void timer_handler(struct timer_list *data) {
    // timer logic
    if (mode == NORMAL) {
        switch (light_color) {
            case GREEN:
                light_color = YELLOW;
                set_light(light_color);
                mod_timer(timer, jiffies + msecs_to_jiffies(cycle_rate_seconds * 1000 * 1));
                break;
            case YELLOW:
                if (pedestrian == 1) {
                    light_color = PEDESTRIAN;
                    set_light(light_color);
                    mod_timer(timer, jiffies + msecs_to_jiffies(cycle_rate_seconds * 1000 * 5));
                } else if (pedestrian == 0) {
                    light_color = RED;
                    set_light(light_color);
                    mod_timer(timer, jiffies + msecs_to_jiffies(cycle_rate_seconds * 1000 * 2));
                }
                break;
            case RED:
                light_color = GREEN;
                set_light(light_color);
                mod_timer(timer, jiffies + msecs_to_jiffies(cycle_rate_seconds * 1000 * 3));
                break;
            case PEDESTRIAN:
                pedestrian = 0;
                light_color = GREEN;
                set_light(light_color);
                mod_timer(timer, jiffies + msecs_to_jiffies(cycle_rate_seconds * 1000 * 3));
                break;
        }
    } else if (mode == FLASHING_RED) {
        switch (light_color) {
            case RED:
                light_color = OFF;
                set_light(light_color);
                mod_timer(timer, jiffies + msecs_to_jiffies(cycle_rate_seconds * 1000 * 1));
                break;
            case OFF:
                light_color = RED;
                set_light(light_color);
                mod_timer(timer, jiffies + msecs_to_jiffies(cycle_rate_seconds * 1000 * 1));
                break;
            default:
                light_color = RED;
                set_light(light_color);
                mod_timer(timer, jiffies + msecs_to_jiffies(cycle_rate_seconds * 1000 * 1));
                break;
        }
    } else if (mode == FLASHING_YELLOW) {
        switch (light_color) {
            case YELLOW:
                light_color = OFF;
                set_light(light_color);
                mod_timer(timer, jiffies + msecs_to_jiffies(cycle_rate_seconds * 1000 * 1));
                break;
            case OFF:
                light_color = YELLOW;
                set_light(light_color);
                mod_timer(timer, jiffies + msecs_to_jiffies(cycle_rate_seconds * 1000 * 1));
                break;
            default:
                light_color = YELLOW;
                set_light(light_color);
                mod_timer(timer, jiffies + msecs_to_jiffies(cycle_rate_seconds * 1000 * 1));
                break;
        }
    }
}

// ---------------------
// Device file functions
// ---------------------
static int mytraffic_init(void) {
    int result;

    // Register device
    result = register_chrdev(mytraffic_major, "mytraffic", &mytraffic_fops);
    if (result < 0)
    {
        printk(KERN_ALERT
               "mytraffic: cannot obtain major number %d\n", mytraffic_major);
        return result;
    }

    // Allocate space
    timer = (struct timer_list *) kmalloc(sizeof(struct timer_list), GFP_KERNEL);

    if (!timer)
    {
        printk(KERN_ALERT "Insufficient kernel memory\n");
        result = -ENOMEM;
        goto fail;
    }

    // initialize timer in green, normal mode
    timer_setup(timer, timer_handler, 0);
    mod_timer(timer, jiffies + msecs_to_jiffies(cycle_rate_seconds * 1000 * 3));

    // Set up button interrupts
    int ret;
    ret = gpio_request(btn0_gpio, "BTN0");
    if (ret) return ret;
    gpio_direction_input(btn0_gpio);

    ret = gpio_request(btn1_gpio, "BTN1");
    if (ret) return ret;
    gpio_direction_input(btn1_gpio);

    // Map GPIOs to IRQ numbers
    btn0_irq = gpio_to_irq(btn0_gpio);
    btn1_irq = gpio_to_irq(btn1_gpio);

    // Request IRQs on rising or falling edges
    ret = request_irq(btn0_irq, button_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "btn0_irq", NULL);
    if (ret) pr_err("Failed to request IRQ for BTN0\n");

    ret = request_irq(btn1_irq, button_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "btn1_irq", NULL);
    if (ret) pr_err("Failed to request IRQ for BTN1\n");

    printk(KERN_ALERT "mytraffic loaded.\n");
    return 0;

fail:
    printk(KERN_ALERT "failed to load mytraffic.\n");
    mytraffic_exit();
    return result;
}

static void mytraffic_exit(void) {
    unregister_chrdev(mytraffic_major, "mytraffic");
    del_timer(timer);
    if (timer)
        kfree(timer);

    // set gpio to 0 and release
    int ret;
    int gpios[] = {red_gpio, yellow_gpio, green_gpio};
    int i;

    for (i=0; i<3; i++) {
        gpio_set_value(gpios[i], 0);
        gpio_free(gpios[i]);
    }

    // free irqs and button gpio
    free_irq(btn0_irq, NULL);
    free_irq(btn1_irq, NULL);
    gpio_free(btn0_gpio);
    gpio_free(btn1_gpio);

    printk(KERN_ALERT "Removing mytraffic module\n");

free_gpios:
    // Free all requested GPIOs on error
    for (i = 0; i < 3; i++) {
        gpio_free(gpios[i]);
    }
    return ret;
}

static int mytraffic_open(struct inode *inode, struct file *filp) {
    return 0;
}

static int mytraffic_release(struct inode *inode, struct file *filp) {
    return 0;
}

static ssize_t mytraffic_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
    // TODO print information:
    //      current operational mode
    //      current cycle rate
    //      current status of lights
    //      current pedestrian status

    return count;
}

static ssize_t mytraffic_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
    // TODO alter cycle rate
    return count;
}
