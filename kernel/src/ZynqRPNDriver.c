#include <linux/module.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>

/////////////////////////////////////////////////////////////////////////
//
// Defined behavior:   
// 
// 
// 123u456ua means "push 123, push 456, add". Numbers in Decimal. 
// Doing a read will return the stack0 value (or an error code).  
//
// Be sure to install the overlay first (Run the python/RunSanityCheck script).
//
// xilinx% sudo -s 
// xilinx% RunRPNSanity.py 
// xilinx% install ZynqRPNDriver.ko
// xilinx% mknod /dev/zynqrpn0 c 229 0  
//
//  Then you should be able to run  
// xilinx% echo "r1234u2345ua" > /dev/zynqrpn0    
// xilinx% cat /dev/zynqrpn0  
// should return V3579  
//

#define RESET 'r'
#define PUSH 'u'
#define POP  'o'
#define ADD  'a'
#define SUB  's'
#define MUL  'm'

#define MAJOR_DEV_NUM      (229)
#define MINOR_DEV_MAX_NUM    (1)
#define MAX_MESSAGE_LEN    (256) 

//
// Contains information about all of the "devices" we will use this driver to represent. 
// 
static struct cdev my_dev;

typedef struct {
   char Message[MAX_MESSAGE_LEN]; 
   int MessageLen; 
}  MyMinorDevContext_t;

typedef struct {
   MyMinorDevContext_t Contexts[MINOR_DEV_MAX_NUM]; 
}  MyGlobalData_t; 


static MyGlobalData_t MyGlobalData; 

typedef struct { 
  int minorDevice;
  int byteStartIndex;  
  unsigned long int RunningNumberDecimal; 
  unsigned int errorcode; 
} MyFHPrivateData_t; 



static int zynqRPNDriverFileOpen(struct inode *inode, struct file *filp)
{

  MyFHPrivateData_t * myFHPrivateData;

  pr_info("zynqRPNDriverFileOpen() called Maj %d Min %d filp %p.\n",imajor(inode),iminor(inode),filp);

  myFHPrivateData = (MyFHPrivateData_t *) kmalloc(sizeof(MyFHPrivateData_t),GFP_KERNEL); 
  filp->private_data = (void *) myFHPrivateData;

  myFHPrivateData->minorDevice = iminor(inode); 
  myFHPrivateData->byteStartIndex = 0; 
  myFHPrivateData->RunningNumberDecimal = 0;
  
  return 0;

}

static int zynqRPNDriverFileClose(struct inode *inode, struct file *filp)
{
  pr_info("zynqRPNDriverFileClose() called Maj %d Min %d filp %p.\n",imajor(inode),iminor(inode),filp);
  kfree(filp->private_data); 
  return 0;
}




ssize_t zynqRPNDriverFileRead (struct file *filp, char * buf, size_t byteCount, loff_t * fileOffset)
{

    char stack0valueorerror[10]; 
    char c; 
    int startByte,bytesRead,messageLen; 
    MyFHPrivateData_t * myFHPrivateData = filp->private_data; 
    unsigned long int register2value;
    void __iomem *regs;

    pr_info("zynqRPNDriverFileRead()\n"); 

    // Read Register 8, which contains our stack value. 
    regs = ioremap(0x40000008, 4);

    register2value = readl(regs);
    sprintf(stack0valueorerror,"V%8.8lu",register2value);  
    messageLen = 9; 
    iounmap(regs);
    pr_info("Read function sending %s\n",stack0valueorerror); 
   
    bytesRead = 0;
    startByte = myFHPrivateData->byteStartIndex;  

    while ((bytesRead < byteCount) && ((startByte + bytesRead) < messageLen))
    { 
      c=stack0valueorerror[bytesRead]; 
      put_user(c,buf+bytesRead);
      bytesRead ++; 
    }  
    startByte += bytesRead; 
    myFHPrivateData->byteStartIndex = startByte; 
    return bytesRead; 

}

// 
// Called whenever someone does a write() on a file they just opened. 
// 
//
ssize_t zynqRPNDriverFileWrite (struct file *filp, 
                                const char * buf, 
                                size_t byteCount, 
                                loff_t * byteOffset)
{

    MyFHPrivateData_t * myFHPrivateData;
    char v; 
    ssize_t bytesWritten = 0; 

    void __iomem *stackpushregister;
    void __iomem *commandregister;

    pr_info("zynqRPNDriverFileWrite()"); 
    
    myFHPrivateData = filp->private_data;

    stackpushregister = ioremap(0x40000000, 4);
    commandregister   = ioremap(0x40000004, 4);
   

    while ((bytesWritten < byteCount) && (bytesWritten < MAX_MESSAGE_LEN))
    { 
      get_user(v,buf+bytesWritten); 
      pr_info("got character %c",v); 
      if (v>='0' && v <= '9') 
      {
         myFHPrivateData->RunningNumberDecimal *= 10;
         myFHPrivateData->RunningNumberDecimal += v-'0';
      }
      else 
      { 
        switch (v) 
        { 
          case RESET: writel(0x1,commandregister); break;
          case PUSH : 
                     pr_info("Pushing %lu", 
                            myFHPrivateData->RunningNumberDecimal);
                     writel(myFHPrivateData->RunningNumberDecimal,
                                                stackpushregister); 
                     writel(0x2,commandregister); 
                     myFHPrivateData->RunningNumberDecimal = 0;
                     break; 
          case POP : writel(0x4,commandregister); break;
          case ADD : writel(0x8,commandregister); break;
          case SUB : writel(0x10,commandregister); break;
          case MUL : writel(0x20,commandregister); break;
          default  : break;
        } 
      } 
      bytesWritten++;  
    }  
    
    iounmap(stackpushregister);
    iounmap(commandregister);

    MyGlobalData.Contexts[myFHPrivateData->minorDevice].MessageLen = bytesWritten;
    return bytesWritten; 

}

// Declare File operations structure. 
static const struct file_operations my_dev_fops = {
	.owner          = THIS_MODULE,
        .read           = zynqRPNDriverFileRead,  
        .write          = zynqRPNDriverFileWrite,  
	.open           = zynqRPNDriverFileOpen,
	.release        = zynqRPNDriverFileClose,

};

static int __init zynqRPNDriverInit(void)
{

   int minorDevice; 
   int j;
   int ret;

   dev_t dev;
  
   dev = MKDEV(MAJOR_DEV_NUM, 0);
   pr_info("Char Driver initialized for Major Device %d\n",MAJOR_DEV_NUM);

   // Allocate Device Numbers 
   ret = register_chrdev_region(dev, 1, "simpleCharacterDriver");
   if (ret < 0) {
     pr_info("Unable to allocate Major Device %d\n", MAJOR_DEV_NUM);
     return ret;
   }

   //
   // Initialize the cdev structure and add it to the kernel space 
   //

   cdev_init(&my_dev, &my_dev_fops);
   for (minorDevice = 0; minorDevice < MINOR_DEV_MAX_NUM; minorDevice++)
   { 
      pr_info("  Minor Driver %d\n",minorDevice);
      dev = MKDEV(MAJOR_DEV_NUM, minorDevice);
      ret = cdev_add(&my_dev, dev, 1);
      if (ret < 0) 
      {
        unregister_chrdev_region(dev, 1);
        pr_info("Unable to add cdev\n");
        return ret;
      }
      for (j = 0; j < MAX_MESSAGE_LEN; j++)
      {
        MyGlobalData.Contexts[minorDevice].Message[j] = (char) 0;    
        MyGlobalData.Contexts[minorDevice].MessageLen = 0;    
      }
  }


  return 0;
}

static void __exit zynqRPNDriverExit(void)
{
   pr_info("Roger's Char Driver exit\n");
   cdev_del(&my_dev);
   unregister_chrdev_region(MKDEV(MAJOR_DEV_NUM, 0), 1);
}

module_init(zynqRPNDriverInit);
module_exit(zynqRPNDriverExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roger Pease");
MODULE_DESCRIPTION("This is a simple character driver which takes in an up-to-255 byte message and outputs it to readers upon request.");
