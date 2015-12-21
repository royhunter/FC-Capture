#include "fcprobe.h"



MODULE_LICENSE("Dual BSD/GPL");



DECLARE_TASKLET(FCProbe_Tasklet, FCProbe_Tasklet_Rx, 0);

fcprobe_imq_t *fcprobe_imq = NULL;
fcprobe_pmq_t *fcprobe_pmq = NULL;



void __iomem *ioaddr = NULL;
static unsigned int irq;
FCProbe_adapter *FCProbe = NULL;
void *buf_virts = NULL;
unsigned int usrcnt = 0;
static wait_queue_head_t fcpkt_rec;
static int fcpkt_flag = 0;
struct timeval time; //time.tv_sec: seconds since 1970



dev_t FC_dev;    //32 bits       12 bits:major   20 bits:minor


static struct file_operations FCProbe_fops={
    owner:    THIS_MODULE,
    open:     FCProbe_Open,
    unlocked_ioctl:    FCProbe_Ioctl,
//    mmap:     FCProbe_mmap,
    release:  FCProbe_Release,

};

static struct pci_device_id FCProbe_pci_tbl[]={
	{ FCHBA_VENDOR_ID, FCHBA_DEVICE_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 1},
	{ 0, },
};

static struct pci_driver FCProbe_Pci_Driver =
{
	.name=              DRV_NAME,
	.id_table=          FCProbe_pci_tbl,
	.probe=             FCProbe_Pci_Probe,
	.remove=            FCProbe_Pci_Remove,
};

static void FCProbe_Mem_Free(FCProbe_adapter *FCP)
{
    unsigned long virt_addr;

    for (virt_addr = (unsigned long)FCP->q_buf;
	     virt_addr < (unsigned long)FCP->q_buf + FCP->q_size;
	     virt_addr += PAGE_SIZE)
    {
	    // clear all pages
		ClearPageReserved(virt_to_page(virt_addr));
	}

    free_pages((unsigned long)FCP->q_buf, get_order(FCP->q_size));
}


static void __FCProbe_cleanup_dev(FCProbe_adapter *FCP)
{
	struct pci_dev *pdev;
	pdev = FCP->FC_pdev;

	if (FCP->mmio_addr)
		pci_iounmap (pdev, FCP->mmio_addr);

	pci_release_regions(pdev);
	pci_set_drvdata(pdev, NULL);
}

static void FCProbe_Interrupt_Release(FCProbe_adapter *FCP)
{
    free_irq(irq, (void *)FCP);
}


static void FCProbe_Pci_Remove(struct pci_dev *pci_dev)
{
    printk(KERN_INFO "FCProbe driver remove.....\n");
    IO_FCProbe_W32_F(REG_FCPROBE_LINK_ENABLE, 0x0);
    //IO_FCProbe_W32_F(REG_FCPROBE_ALL_RESET, 3);
    //IO_FCProbe_W32_F(REG_FCPROBE_ALL_RESET, 0);

    unregister_chrdev_region(FC_dev, 1);

    FCProbe_Interrupt_Release(FCProbe);

    /* it's ok to call this even if we have no regions to free */
	__FCProbe_cleanup_dev(FCProbe);

    pci_disable_device(pci_dev);

    FCProbe_Mem_Free(FCProbe);

    if(NULL != FCProbe)
    {
        kfree(FCProbe);
    }

    return;
}

void pmq_idx_r_update(uint32_t *idx)
{
    *idx = *idx + 1;
    if( *idx == FCPROBE_PMQ_ENTRY_NUM )
    {
	*idx = 0;
    }
    fcprobe_pmq->idx_r = *idx;
}

void imq_idx_r_update(uint64_t *idx)
{
    *idx = *idx + 1;
    if( *idx == FCPROBE_IMQ_ENTRY_NUM )
    {
        *idx = 0;
    }
    fcprobe_imq->idx_r = *idx;
}

void pmq_message_handle(unsigned long arg,int evth)
{
    int type;
    int channelnum;
    int cardnum;
    uint32_t *ptr;
    //printk("pmq message handle start------------------------------------\n");
#ifdef DEBUG_PRINTK
    printk(KERN_INFO "PMQ W index evth is %d, PMQ R index is %lld\n", evth, fcprobe_pmq->idx_r);
#endif
    //printk(KERN_INFO "PMQ cons is %d\n", IO_FCProbe_R32(REG_FCPROBE_PMQ_CONS_INDEX));
    //pmq_idx_r_update(&idx_r);
    type = fcprobe_pmq->spq_entry[evth].word0.s.type;
    channelnum = fcprobe_pmq->spq_entry[evth].word0.s.channel_num;
    cardnum = fcprobe_pmq->spq_entry[evth].word0.s.card_num;
#ifdef DEBUG_PRINTK
    printk(KERN_INFO "type is %d, channelnum is %d, cardnum is %d\n", type, channelnum, cardnum);
#endif
    ptr = (uint32_t *)&fcprobe_pmq->spq_entry[evth];
    //printk(KERN_INFO "prt is %p\n", (uint32_t *)(virt_to_phys(&fcprobe_pmq->spq_entry[evth])));
    /*printk(KERN_INFO "DWORD0 is 0x%x\n", *ptr);
        printk(KERN_INFO "DWORD1 is 0x%x\n", *(ptr+1));
        printk(KERN_INFO "DWORD2 is 0x%x\n", *(ptr+2));
        printk(KERN_INFO "DWORD3 is 0x%x\n", *(ptr+3));
        printk(KERN_INFO "DWORD4 is 0x%x\n", *(ptr+4));
        printk(KERN_INFO "DWORD5 is 0x%x\n", *(ptr+5));
        printk(KERN_INFO "DWORD6 is 0x%x\n", *(ptr+6));
        printk(KERN_INFO "DWORD7 is 0x%x\n", *(ptr+7));
        printk(KERN_INFO "DWORD8 is 0x%x\n", *(ptr+8));
        printk(KERN_INFO "DWORD9 is 0x%x\n", *(ptr+9));
    	*/
        /*
        printk(KERN_INFO "DWORD10 is 0x%x\n", *(ptr+10));
        printk(KERN_INFO "DWORD11 is 0x%x\n", *(ptr+11));
        printk(KERN_INFO "DWORD12 is 0x%x\n", *(ptr+12));
        printk(KERN_INFO "DWORD13 is 0x%x\n", *(ptr+13));
        printk(KERN_INFO "DWORD14 is 0x%x\n", *(ptr+14));
        printk(KERN_INFO "DWORD15 is 0x%x\n", *(ptr+15));
        printk(KERN_INFO "DWORD16 is 0x%x\n", *(ptr+16));
        printk(KERN_INFO "DWORD17 is 0x%x\n", *(ptr+17));
        printk(KERN_INFO "DWORD18 is 0x%x\n", *(ptr+18));
        printk(KERN_INFO "DWORD19 is 0x%x\n", *(ptr+19));

        printk(KERN_INFO "DWORD20 is 0x%x\n", *(ptr+20));
        printk(KERN_INFO "DWORD21 is 0x%x\n", *(ptr+21));
        printk(KERN_INFO "DWORD22 is 0x%x\n", *(ptr+22));
        printk(KERN_INFO "DWORD23 is 0x%x\n", *(ptr+23));
        printk(KERN_INFO "DWORD24 is 0x%x\n", *(ptr+24));
        printk(KERN_INFO "DWORD25 is 0x%x\n", *(ptr+25));
        printk(KERN_INFO "DWORD26 is 0x%x\n", *(ptr+26));
        printk(KERN_INFO "DWORD27 is 0x%x\n", *(ptr+27));
        printk(KERN_INFO "DWORD28 is 0x%x\n", *(ptr+28));
        printk(KERN_INFO "DWORD29 is 0x%x\n", *(ptr+29));

        printk(KERN_INFO "DWORD30 is 0x%x\n", *(ptr+30));
        printk(KERN_INFO "DWORD31 is 0x%x\n", *(ptr+31));
        printk(KERN_INFO "DWORD32 is 0x%x\n", *(ptr+32));
        printk(KERN_INFO "DWORD33 is 0x%x\n", *(ptr+33));
        printk(KERN_INFO "DWORD34 is 0x%x\n", *(ptr+34));
        printk(KERN_INFO "DWORD35 is 0x%x\n", *(ptr+35));
        printk(KERN_INFO "DWORD36 is 0x%x\n", *(ptr+36));
        printk(KERN_INFO "DWORD37 is 0x%x\n", *(ptr+37));
        printk(KERN_INFO "DWORD38 is 0x%x\n", *(ptr+38));
        printk(KERN_INFO "DWORD39 is 0x%x\n", *(ptr+39));

        printk(KERN_INFO "DWORD40 is 0x%x\n", *(ptr+40));
        printk(KERN_INFO "DWORD41 is 0x%x\n", *(ptr+41));
        printk(KERN_INFO "DWORD42 is 0x%x\n", *(ptr+42));
        printk(KERN_INFO "DWORD43 is 0x%x\n", *(ptr+43));
        printk(KERN_INFO "DWORD44 is 0x%x\n", *(ptr+44));
        printk(KERN_INFO "DWORD45 is 0x%x\n", *(ptr+45));
        printk(KERN_INFO "DWORD46 is 0x%x\n", *(ptr+46));
        printk(KERN_INFO "DWORD47 is 0x%x\n", *(ptr+47));
        printk(KERN_INFO "DWORD48 is 0x%x\n", *(ptr+48));
        printk(KERN_INFO "DWORD49 is 0x%x\n", *(ptr+49));
        */

    if(usrcnt)
    {
        if(copy_to_user((unsigned char *)arg, (unsigned char *)ptr, sizeof(fcprobe_lpq_entry_t)))
            return;
    #ifdef DEBUG_PRINTK
        printk("copy ok!\n");
    #endif
    }

    pmq_idx_r_update(&evth);
    IO_FCProbe_W32_F(REG_FCPROBE_PMQ_CONS_INDEX, fcprobe_pmq->idx_r);
#ifdef DEBUG_PRINTK
    printk("pmq message handle end------------------------------------\n");
#endif
    return;
}


void FCProbe_Tasklet_Rx(unsigned long unuesd)
{
   // int cmtype, evth;
    uint64_t idx_w = fcprobe_imq->idx_w;
    uint64_t idx_r = fcprobe_imq->idx_r;
    //fcprobe_imq_entry_t *imq_entry;
    //printk(KERN_INFO "TASKLET COMING\n");

    //IO_FCProbe_W8_F(REG_FCPROBE_INTERRUPT_ABLE, 0x0);

    //printk(KERN_INFO "IMQ W index is %lld, IMQ R index is %lld\n", idx_w, idx_r);
    //printk(KERN_INFO "IMQ con register is %d\n", IO_FCProbe_R32(REG_FCPROBE_IMQ_CONS_INDEX));

    if(idx_w != idx_r)
    {
        //printk(KERN_INFO "usrcnt is %d\n", usrcnt);
        if(usrcnt)
        {
            if(fcpkt_flag == 0)
            {
                fcpkt_flag = 1;
                wake_up_interruptible(&fcpkt_rec);
            #ifdef DEBUG_PRINTK
                printk(KERN_INFO "wakup usr\n");
            #endif
            }
            else
            {
     		    //printk(KERN_INFO "FCPKT_FLAG IS %d\n", fcpkt_flag);
            }
        }
/*
        else
        {
            imq_entry = &fcprobe_imq->imq_entry[idx_r];
            cmtype = imq_entry->word0.s.cm_type;
            evth = imq_entry->word0.s.event_handle;
        	if(IMQ_CMC_TYPE_PMQ == cmtype)
        	{
        	    printk(KERN_INFO "this is a pmq message\n");
        	    pmq_message_handle(0, evth);
        	}
        	else
            {
        	   printk(KERN_INFO "imq unknow message\n");
        	}

            imq_idx_r_update(&idx_r);
            IO_FCProbe_W32_F(REG_FCPROBE_IMQ_CONS_INDEX, fcprobe_imq->idx_r);
        }
*/
    }

    IO_FCProbe_W8_F(REG_FCPROBE_INTERRUPT_ABLE, FCPROBE_INTERRUPT_ENABLE);
}


static int FCProbe_register_chrdev(FCProbe_adapter *fcp)
{
	int result;
	result = alloc_chrdev_region(&FC_dev, 0, 1, DEVICE_NAME);
	if (0 != result)
  	{
  		printk(KERN_ERR "%s: cannot allocate chrdev region\n", __FUNCTION__);
        return result;
  	}

	printk("the major num is %d\n", MAJOR(FC_dev));
	printk("the minor num is %d\n", MINOR(FC_dev));

  	cdev_init(&fcp->FC_cdev, &FCProbe_fops);
  	fcp->FC_cdev.owner = THIS_MODULE;
 	fcp->FC_cdev.ops = &FCProbe_fops;

  	result = cdev_add(&fcp->FC_cdev, FC_dev, 1);
  	if(0 != result)
  	{
  		printk(KERN_NOTICE "Error %d adding FC_cdev", result);
		unregister_chrdev_region(FC_dev, 1);
		return result;
  	}

	return 0;
}

static irqreturn_t FCProbe_Interrupt_Handle (int irq, void *dev_instance)
{
    IO_FCProbe_W8_F(REG_FCPROBE_INTERRUPT_ABLE, FCPROBE_INTERRUPT_DISABLE);
    tasklet_schedule(&FCProbe_Tasklet);

    return IRQ_HANDLED;
}



static int FCProbe_Interrupt_Init(FCProbe_adapter *FCP)
{
	return request_irq(irq, (irq_handler_t)FCProbe_Interrupt_Handle, IRQF_SHARED, DEVICE_NAME, (void *)FCP);
}




static int FCProbe_Hw_Init(FCProbe_adapter *FCP)
{
    void *imq_base;
    unsigned int imq_base_low;
    unsigned int imq_base_high;
    void *imq_prod_idx_addr;
    unsigned int imq_prod_low;
    unsigned int imq_prod_high;
    void *pmq_base;
    unsigned int pmq_base_low;
    unsigned int pmq_base_high;
    void *pmq_prod_idx_addr;
    unsigned int pmq_prod_low;
    unsigned int pmq_prod_high;


    /*1: reset hw logic */

    /*2: IMQ Init*/
    imq_base = (void *)((char *)virt_to_phys(fcprobe_imq) + 2 * sizeof(uint64_t));
    imq_base_low = (unsigned long)imq_base & 0xffffffff;
    imq_base_high = ((unsigned long)imq_base >> 32) & 0xffffffff;
    printk("imq_base:  %p\n", imq_base);
    IO_FCProbe_W32_F(REG_FCPROBE_IMQ_BASE_HADDR, imq_base_high);
    IO_FCProbe_W32_F(REG_FCPROBE_IMQ_BASE_LADDR, imq_base_low);
    printk("imq entry num is %d\n", FCPROBE_IMQ_ENTRY_NUM);
    IO_FCProbe_W32_F(REG_FCPROBE_IMQ_LENGTH, FCPROBE_IMQ_ENTRY_NUM-1);


    imq_prod_idx_addr = (void *)((char *)virt_to_phys(fcprobe_imq) + sizeof(uint64_t));
    imq_prod_low = (unsigned long)imq_prod_idx_addr & 0xffffffff;
    imq_prod_high = ((unsigned long)imq_prod_idx_addr >> 32) & 0xffffffff;
    printk("imq_prod_idx_addr: %p\n", imq_prod_idx_addr);
    IO_FCProbe_W32_F(REG_FCPROBE_IMQ_PROD_INDEX_HADDR, imq_prod_high);
    IO_FCProbe_W32_F(REG_FCPROBE_IMQ_PROD_INDEX_LADDR, imq_prod_low);

    IO_FCProbe_W32_F(REG_FCPROBE_IMQ_CONS_INDEX, 0);


    /*3: PMQ Init*/
    pmq_base = (void *)((char *)virt_to_phys(fcprobe_pmq) + 2 * sizeof(uint64_t));
    pmq_base_low = (unsigned long)pmq_base & 0xffffffff;
    pmq_base_high = ((unsigned long)pmq_base >> 32) & 0xffffffff;
    IO_FCProbe_W32_F(REG_FCPROBE_PMQ_BASE_HADDR, pmq_base_high);
    IO_FCProbe_W32_F(REG_FCPROBE_PMQ_BASE_LADDR, pmq_base_low);
    printk("pmq_base: %p\n", pmq_base);
    printk("pmq entry num is %d\n", FCPROBE_PMQ_ENTRY_NUM);
    IO_FCProbe_W32_F(REG_FCPROBE_PMQ_LENGTH, FCPROBE_PMQ_ENTRY_NUM-1);

    pmq_prod_idx_addr = (void *)((char *)virt_to_phys(fcprobe_pmq) + sizeof(uint64_t));
    pmq_prod_low = (unsigned long)pmq_prod_idx_addr & 0xffffffff;
    pmq_prod_high = ((unsigned long)pmq_prod_idx_addr >> 32) & 0xffffffff;
    IO_FCProbe_W32_F(REG_FCPROBE_PMQ_PROD_INDEX_HADDR, pmq_prod_high);
    IO_FCProbe_W32_F(REG_FCPROBE_PMQ_PROD_INDEX_LADDR, pmq_prod_low);

    printk("pmq_prod_idx_addr is %p\n", pmq_prod_idx_addr);

    IO_FCProbe_W32_F(REG_FCPROBE_PMQ_CONS_INDEX, 0);

    printk("high addr is 0x%x\n", IO_FCProbe_R32(REG_FCPROBE_PMQ_BASE_HADDR));
    printk("low addr is 0x%x\n", IO_FCProbe_R32(REG_FCPROBE_PMQ_BASE_LADDR));

    IO_FCProbe_W32_F(REG_FCPROBE_SPEED_SEL, 1);// 2.15G speed


    /*5: Interrupt Enable*/
    IO_FCProbe_W8_F(REG_FCPROBE_INTERRUPT_ABLE, FCPROBE_INTERRUPT_ENABLE);

    /*4: Link Enable*/
    //IO_FCProbe_W32_F(REG_FCPROBE_LINK_ENABLE, 1);

    /*6: self test*/
    //IO_FCProbe_W32_F(REG_FCPROBE_LINK_ENABLE, 0x2);


    return 0;

}

static int FCProbe_Mem_Init(FCProbe_adapter *FCP)
{
    unsigned long virt_addr;
    void *buf = NULL;

    int mem_size = FCPROBE_IMQ_SIZE + FCPROBE_PMQ_SIZE;
    FCP->q_size = mem_size;

    buf = (void *)__get_free_pages(GFP_DMA, get_order(mem_size));
    if(NULL == buf)
    {
        return -ENOMEM;
    }

    FCP->q_buf = buf;
    buf_virts = (void *)buf;

    printk(KERN_INFO "imq/pmq base addr is %p\n",buf);

    memset(buf, 0, mem_size);

    fcprobe_imq = (fcprobe_imq_t *)buf;
    fcprobe_pmq = (fcprobe_pmq_t *)((uint8_t *)buf + FCPROBE_IMQ_SIZE);

    //mmap operation
	for (virt_addr = (unsigned long)buf;
	     virt_addr < (unsigned long)buf + mem_size;
	     virt_addr += PAGE_SIZE)
	{
		SetPageReserved(virt_to_page(virt_addr));
	}

    return 0;
}

void FCProbe_Pkt_Rx(unsigned long arg)
{
    int cmtype, evth;
    //uint64_t idx_w = fcprobe_imq->idx_w;
    uint64_t idx_r = fcprobe_imq->idx_r;

    fcprobe_imq_entry_t *imq_entry;
#ifdef DEBUG_PRINTK
    printk(KERN_ALERT "User coming to take packet\n");
    printk(KERN_ALERT "IMQ W index is %lld, IMQ R index is %lld\n", idx_w, idx_r);
#endif
    imq_entry = &fcprobe_imq->imq_entry[idx_r];
    cmtype = imq_entry->word0.s.cm_type;
    evth = imq_entry->word0.s.event_handle;
#ifdef DEBUG_PRINTK
    printk(KERN_ALERT "IMQ WORD0 0x%x\n", imq_entry->word0.u32);
    printk(KERN_ALERT "IMQ WORD1 0x%x\n", imq_entry->word1.u32);
    printk(KERN_ALERT "IMQ WORD2 0x%x\n", imq_entry->word2.u32);
    printk(KERN_ALERT "IMQ WORD3 0x%x\n", imq_entry->word3.u32);
#endif

	if(IMQ_CMC_TYPE_PMQ == cmtype)
	{
#ifdef DEBUG_PRINTK
	    printk(KERN_ALERT "this is a pmq message\n");
#endif
	    pmq_message_handle(arg, evth);
	}
	else
    {
#ifdef DEBUG_PRINTK
	   printk(KERN_ALERT "imq unknow message\n");
#endif
	}

    imq_idx_r_update(&idx_r);
#ifdef DEBUG_PRINTK
	printk(KERN_ALERT "==================>handler over imq r index is %lld\n", fcprobe_imq->idx_r);
#endif
    IO_FCProbe_W32_F(REG_FCPROBE_IMQ_CONS_INDEX, fcprobe_imq->idx_r);
}


long FCProbe_Ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int retval = 0;
    switch (cmd)
    {
        case FCPROBE_HW_VERSION:
        {
            unsigned int version = IO_FCProbe_R32(REG_FCPROBE_HW_VERSION);
            printk(KERN_INFO "verison is 0x%x\n", version);

            if(copy_to_user((unsigned int *)arg, &version, sizeof(unsigned int)))
				return -EFAULT;
            break;
        }
        case FCPROBE_HW_SELFTEST_ON:
        {
            unsigned int value = IO_FCProbe_R32(REG_FCPROBE_LINK_ENABLE);
            value= 2;
            IO_FCProbe_W32_F(REG_FCPROBE_LINK_ENABLE, value);
            break;
        }
    	case FCPROBE_HW_REAL_MODE_ON:
    	{
    	    unsigned int value = IO_FCProbe_R32(REG_FCPROBE_LINK_ENABLE);
    	    value = 1;
                IO_FCProbe_W32_F(REG_FCPROBE_LINK_ENABLE, value);
                break;
    	}
    	case FCPROBE_HW_SELFTEST_1_ON:
    	{
    	    unsigned int value = IO_FCProbe_R32(REG_FCPROBE_LINK_ENABLE);
    	    value = 4;
    	    IO_FCProbe_W32_F(REG_FCPROBE_LINK_ENABLE, value);
    	    break;
    	}
        case FCPROBE_HW_SELFTEST_OFF:
        case FCPROBE_HW_REAL_MODE_OFF:
    	case FCPROBE_HW_SELFTEST_1_OFF:
        {
            unsigned int value = IO_FCProbe_R32(REG_FCPROBE_LINK_ENABLE);
            value = 0;
            IO_FCProbe_W32_F(REG_FCPROBE_LINK_ENABLE, value);
            break;
        }
        case FCPROBE_HW_REGREAD:
        {
            unsigned int value = 0;
            unsigned int reg = 0;
            if (copy_from_user(&value, (int *)arg, sizeof(unsigned int)))
				return -EFAULT;
            reg = IO_FCProbe_R32(value);
            if(copy_to_user((unsigned int *)arg, &reg, sizeof(unsigned int)))
				return -EFAULT;
            break;
        }
        case FCPROBE_PKT_RECV:
        {
            uint64_t idx_w = fcprobe_imq->idx_w;
            uint64_t idx_r = fcprobe_imq->idx_r;
    #ifdef DEBUG_PRINTK
            printk(KERN_ALERT "User ioctl FCPROBE_PKT_RECV\n");
            printk(KERN_ALERT "idx_w is %lld, idx_r is %lld\n", idx_w, idx_r);
    #endif
            if(idx_w == idx_r)// empty
            {
	    wait:
                if(wait_event_interruptible(fcpkt_rec, fcpkt_flag!= 0))
    	        {
    		        return -ERESTARTSYS;
    	        }
    	        fcpkt_flag = 0;
#ifdef DEBUG_PRINTK
                printk(KERN_ALERT "User ioctl FCPROBE_PKT_RECV\n");
                printk(KERN_ALERT "idx_w is %lld, idx_r is %lld\n", idx_w, idx_r);
#endif
        		if(fcprobe_imq->idx_w == fcprobe_imq->idx_r)
        			goto wait;
        	}
#ifdef DEBUG_PRINTK
            printk(KERN_ALERT "BEGIN PKT RX\n");
#endif
            FCProbe_Pkt_Rx(arg);

            break;
        }
        default:
        {
            retval = -EINVAL;
            break;
        }
    }

    return retval;
}
#if 0
static int FCProbe_Mmap(struct file *filp,struct vm_area_struct *vma)
{
	int ret;
	ret = remap_pfn_range(vma,
               vma->vm_start,
               virt_to_phys((void*)((unsigned long)buf_virts)) >> PAGE_SHIFT,
               vma->vm_end - vma->vm_start,
               PAGE_SHARED);
	if(ret != 0) {
		return -EAGAIN;
	}
	return 0;
}
#endif

static int FCProbe_Open(struct inode *inode,struct file *filp)
{
   try_module_get(THIS_MODULE);
   printk(KERN_INFO "FCPROBE open success\n");
   usrcnt++;
   //IO_FCProbe_W32_F(REG_FCPROBE_LINK_ENABLE, 0x2);

   return 0;

}

static int FCProbe_Release(struct inode *inode,struct file *filp)
{
	module_put(THIS_MODULE);
	printk(KERN_INFO "FCPROBE close success\n");
    usrcnt = 0;
    	IO_FCProbe_W32_F(REG_FCPROBE_LINK_ENABLE, 0x0);
	return 0;
}

void FCProbe_TimeStamp_Init(void)
{
	do_gettimeofday(&time);

	printk(KERN_INFO "seconds since 1970 is %lld\n", (long long int)time.tv_sec);

	IO_FCProbe_W32_F(REG_FCPROBE_Current_HTimer, time.tv_sec & 0xffffffff);
    return;
}

static int FCProbe_Pci_Probe(struct pci_dev *pdev,
	const struct pci_device_id *pci_id)
{
    int rc = 0;
    unsigned int version;
    unsigned long mmio_start;
	unsigned long mmio_end;
	unsigned long mmio_flags;
	unsigned long mmio_len;

    printk(KERN_INFO "FCcard_pci_probe call\n");
    printk(KERN_INFO "Enable device...\n");

    if( NULL == pdev )
    {
        printk(KERN_ERR "pdev is null\n");
        return -1;
    }

    rc = pci_enable_device(pdev);
    if(0 != rc)
	{
  		printk(KERN_ERR "%s: cannot enable device\n", pci_name(pdev));
        return rc;
  	}
    printk(KERN_INFO "%s: enable device ok!\n", pci_name(pdev));

    if((FCProbe = (FCProbe_adapter *)kmalloc(sizeof(FCProbe_adapter), GFP_KERNEL)) == NULL)
  	{
		printk(KERN_ERR "FCcard_adapter: out of memory\n");
		return -ENOMEM;
  	}

    memset(FCProbe, 0, sizeof(FCProbe_adapter));

    FCProbe->FC_pdev = pdev;

    mmio_start = pci_resource_start(pdev, 0);      //get register resource
  	mmio_end = pci_resource_end(pdev, 0);
  	mmio_flags = pci_resource_flags(pdev, 0);
	mmio_len = pci_resource_len(pdev, 0);

    if(!(mmio_flags & IORESOURCE_MEM))        // make sure PCI base addr 0 is MMIO
  	{
  		printk(KERN_ERR "Cannot find proper PCI device base address, aborting.\n");
		rc = -ENODEV;
		goto err_out;
  	}
    printk(KERN_INFO "find mmio success\n");
    printk(KERN_INFO "MMIO_start is 0x%lx\n", mmio_start);

    rc = pci_request_regions(pdev, DEVICE_NAME);  //register the memory of pci device
 	if(0 != rc)
  		goto err_out;


    ioaddr = ioremap (mmio_start, mmio_len);    // ioremap MMIO region
  	if (NULL == ioaddr)
	{
		printk(KERN_ERR "%s: cannot remap MMIO, aborting\n", pci_name(pdev));
		rc = -EIO;
		goto err_out;
	}
	printk(KERN_INFO "ioaddr == %p\n", ioaddr);

    FCProbe->mmio_addr = ioaddr;
  	FCProbe->regs_len = mmio_len;
  	FCProbe->irq = pdev->irq;
	irq = pdev->irq;

	printk(KERN_INFO "irq is %d\n", pdev->irq);

    pci_set_master(pdev);

    if(0 != FCProbe_register_chrdev(FCProbe))          //register chrdev
	{
		printk(KERN_ERR "chrdev register fail\n");
		goto err_out;
	}

  	pci_set_drvdata(pdev, FCProbe);


    IO_FCProbe_W32_F(REG_FCPROBE_ALL_RESET, 3);
    IO_FCProbe_W32_F(REG_FCPROBE_ALL_RESET, 0);

    version = IO_FCProbe_R32(REG_FCPROBE_HW_VERSION);
    printk(KERN_INFO "FPGA bit file version is 0x%x\n", version);

    FCProbe_TimeStamp_Init();

    rc = FCProbe_Interrupt_Init(FCProbe);
    if(0 != rc)
    {
        printk(KERN_ERR "FCProbe_Interrupt_Init fail\n");
        goto err_out;
    }
    printk(KERN_INFO "FCProbe_Interrupt_Init ok\n");

    rc = FCProbe_Mem_Init(FCProbe);
	if(0 != rc)        //request irq and alloc memory
	{
	    FCProbe_Interrupt_Release(FCProbe);
		printk(KERN_ERR "FCProbe_init fail\n");
		goto err_out;
	}
    printk(KERN_INFO "FCProbe_Mem_Init ok\n");

	rc = FCProbe_Hw_Init(FCProbe);   //start hardware
    if(0 != rc)
    {
        FCProbe_Mem_Free(FCProbe);
        FCProbe_Interrupt_Release(FCProbe);
		printk(KERN_ERR "FCProbe_Hw_Init fail\n");
        goto err_out;
    }
    printk(KERN_INFO "FCProbe_Hw_Init ok\n");

    init_waitqueue_head(&fcpkt_rec);

    return 0;

err_out:                         //error process
    printk(KERN_ERR " probe error \n");
    __FCProbe_cleanup_dev(FCProbe);

    return rc;
}



static void __exit FCProbe_HBA_Module_Exit(void)
{
	pci_unregister_driver(&FCProbe_Pci_Driver);

	printk(KERN_INFO "FCProbe driver remove success\n");
}


static int __init FCProbe_HBA_Module_Init(void)
{
	printk(KERN_INFO FCHBA_DRIVER_NAME "\n");
	printk(KERN_INFO "FC HBA DRIVER built on %s, %s\n", __DATE__, __TIME__);

	return pci_register_driver(&FCProbe_Pci_Driver);   //register pci device,if success,return 0
}






MODULE_AUTHOR("LUOYE");
module_init(FCProbe_HBA_Module_Init);
module_exit(FCProbe_HBA_Module_Exit);


