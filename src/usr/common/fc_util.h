#ifndef __FC_UTIL_H__
#define __FC_UTIL_H__
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

#include <linux/ioctl.h>

#include "fc_util.h"

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










#endif
