#include <linux/init.h> /* Needed for the macros */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/module.h> /* Needed by all modules */

///< The license type -- this affects runtime behavior
MODULE_LICENSE("GPL");

///< The author -- visible when you use modinfo
MODULE_AUTHOR("Bob");

///< The description -- see modinfo
MODULE_DESCRIPTION("A simple Hello world LKM!");

///< The version of the module
MODULE_VERSION("0.1");

#include "nothing/nothing.h"

static int __init hello_start(void)
{
    printk(KERN_INFO
           "Loading hello module...\n");
    printk(KERN_INFO
           "Hello world\n");
    do_literally_nothing(23);
    return 0;
}

static void __exit hello_end(void)
{
    printk(KERN_INFO
           "Goodbye princess.\n");
}

module_init(hello_start);

module_exit(hello_end);
