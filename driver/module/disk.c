#include <linux/blkdev.h>
#include <linux/fs.h>

#define MY_BLOCK_MAJOR 240
#define MY_BLKDEV_NAME "mybdev"

static int __init my_block_init(void)
{
    int status;

    status = register_blkdev(MY_BLOCK_MAJOR, MY_BLKDEV_NAME);
    if (status < 0)
    {
        pr_err("unable to register mybdev block device\n");
        return -EBUSY;
    }
    //...
    return 0;
}

static void __exit my_block_exit(void)
{
    //...
    unregister_blkdev(MY_BLOCK_MAJOR, MY_BLKDEV_NAME);
}

module_init(my_block_init);

module_exit(my_block_exit);