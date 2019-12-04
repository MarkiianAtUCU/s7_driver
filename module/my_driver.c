#include <linux/init.h>
#include <linux/module.h>
#define ISDIGIT(c)  ((c) >= '0' && (c) <= '9')

#include "md.h"
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Markiian Matsiuk <matsiuk@ucu.edu.ua>");
MODULE_DESCRIPTION("A simple Linux driver for 7 segment display");
MODULE_VERSION("0.1");

static unsigned int currentNumber = 0;           ///< Default GPIO for the LED is 49
module_param(currentNumber, uint, 0660);          ///< Param desc. S_IRUGO can be read/not changed
MODULE_PARM_DESC(currentNumber, " Current number on display (default=0)");

//           4
//      +========+
//      |        |
//   27 |        | 17
//      |   22   |
//      +========+
//      |        |
//   26 |        | 6
//      |   13   |
//      +========+  O- 5

int pins[] = {5,22,27,26,13,6,17,4};

char numbers[] = {
        0b11111100,
        0b01100000,
        0b11011010,
        0b11110010,
        0b01100110,
        0b10110110,
        0b10111110,
        0b11100000,
        0b11111110,
        0b11110110
};

char data;
int i;

void displayNumber(char number) {
    i =0;
    for (i = 0; i < 8; ++i) {
        if (number & 1) {
            operate_GPIO(pins[i], high);
        } else {
            operate_GPIO(pins[i], low);
        }
        number >>= 1;
    }
}


static ssize_t num_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
    return sprintf(buf, "%i\n", currentNumber);       // Display the state -- simplistic approach
}

static ssize_t num_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
    if (strlen(buf) == 2 && ISDIGIT(buf[0])) {
        currentNumber = buf[0] - '0';
        displayNumber(numbers[currentNumber]);

    } else {
        printk(KERN_ERR "[segment_display] - Invalid input value\n");
    }
    return count;
}



static struct kobj_attribute num_attr = __ATTR(currentNumber, 0660, num_show, num_store);

static struct attribute *segment_display_attrs[] = {
        &num_attr.attr,
        NULL,
};

static struct attribute_group attr_group = {
        .name  = "s7_display",
        .attrs = segment_display_attrs,
};

static struct kobject *segment_display_kobj;            /// The pointer to the kobject


static int __init md_init(void)
{
    int result = 0;
    printk(KERN_INFO "[segment_display] - Initializing segment_display driver\n");

    segment_display_kobj = kobject_create_and_add("segment_display", kernel_kobj->parent); // kernel_kobj points to /sys/kernel
    if(!segment_display_kobj){
        printk(KERN_ALERT "[segment_display] - failed to create kobject\n");
        return -ENOMEM;
    }
    // add the attributes to /sys/ebb/ -- for example, /sys/ebb/led49/ledOn
    result = sysfs_create_group(segment_display_kobj, &attr_group);
    if(result) {
        printk(KERN_ALERT "[segment_display] - failed to create sysfs group\n");
        kobject_put(segment_display_kobj);                // clean up -- remove the kobject sysfs entry
        return result;
    }

    displayNumber(numbers[currentNumber]);

//    printk("Executed extern command: %i\n", operate_GPIO(6, high));
    return 0;
}

static void __exit md_exit(void)
{
    kobject_put(segment_display_kobj);
    displayNumber(0b00000000);
    printk("segment_display driver unloaded!\n");
}

module_init(md_init);
module_exit(md_exit);