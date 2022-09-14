#include <linux/init.h> /* Needed for the macros */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/module.h> /* Needed by all modules */

// Temporary license; will decide later
MODULE_LICENSE("Dual MIT/GPL");

MODULE_AUTHOR("Asriel Camora <asriel@camora.dev>");

MODULE_DESCRIPTION("Mount L4 archives natively");

// TODO: Add version macro with cmake
MODULE_VERSION("0.1");

/*
#include "LibInterface.h"
static int __init hello_start(void)
{
    pr_info("Loading module %s...\n", GetDriverName());
    printk(KERN_INFO "Hello world\n");
    return 0;
}

static void __exit hello_end(void)
{
    printk(KERN_INFO "Goodbye princess.\n");
    pr_info("Unloading module %s...\n", GetDriverName());
}*/