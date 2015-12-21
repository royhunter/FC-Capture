#ifndef __FCPROBE_H__
#define __FCPROBE_H__




#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/compiler.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <asm/page.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <linux/semaphore.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>



#include "fcprobe_io.h"
#include "fcprobe_imq.h"
#include "fcprobe_pmq.h"



#define DEVICE_NAME         "FC-PROBE"
#define DRV_NAME            "FC-PROBE-DRIVER"
#define DRV_VERSION         "1.0"
#define FCHBA_DRIVER_NAME   DRV_NAME DRV_VERSION

#define FCHBA_VENDOR_ID     0x10EE
#define FCHBA_DEVICE_ID     0x0105



typedef struct {
    struct pci_dev *FC_pdev;
    struct cdev FC_cdev;
    void __iomem *mmio_addr;
    unsigned int irq;
    unsigned int regs_len;
    unsigned int q_size;
    void *q_buf;
}FCProbe_adapter;




#define FCPROBE_IOC_MAGIC 'f'


#define FCPROBE_HW_VERSION        _IOWR(FCPROBE_IOC_MAGIC, 1, unsigned int *)
#define FCPROBE_HW_SELFTEST_ON    _IOWR(FCPROBE_IOC_MAGIC, 2, unsigned int *)
#define FCPROBE_HW_SELFTEST_OFF   _IOWR(FCPROBE_IOC_MAGIC, 3, unsigned int *)
#define FCPROBE_HW_REGREAD        _IOWR(FCPROBE_IOC_MAGIC, 4, unsigned int *)
#define FCPROBE_PKT_RECV          _IOWR(FCPROBE_IOC_MAGIC, 5, unsigned int *)
#define FCPROBE_HW_REAL_MODE_ON   _IOWR(FCPROBE_IOC_MAGIC, 6, unsigned int *)
#define FCPROBE_HW_REAL_MODE_OFF   _IOWR(FCPROBE_IOC_MAGIC, 7, unsigned int *)
#define FCPROBE_HW_SELFTEST_1_ON   _IOWR(FCPROBE_IOC_MAGIC, 8, unsigned int *)
#define FCPROBE_HW_SELFTEST_1_OFF  _IOWR(FCPROBE_IOC_MAGIC, 9, unsigned int *)






void FCProbe_Tasklet_Rx(unsigned long unused);
static int FCProbe_Open(struct inode *inode,struct file *filp);
static int FCProbe_Release(struct inode *inode,struct file *filp);
#if 0
static int FCProbe_Mmap(struct file *filp,struct vm_area_struct *vma);
#endif
long FCProbe_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static int FCProbe_Pci_Probe(struct pci_dev *pdev, const struct pci_device_id *pci_id);
static void FCProbe_Pci_Remove(struct pci_dev *pci_dev);



#endif
