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
#include "fc-reg-rd.h"



int main(int argc, char *argv[])
{
    int fd;
    fd = open("/dev/fcprobe", O_RDWR);
    if(fd < 0)
    {
        printf("fcprobe open failed\n");
        return 1;
    }

    printf("fcprobe open ok!\n");

    if(argc < 2)
    {
        goto close;
        return 1;
    }

    if(strcmp(argv[1], "REG_FCPROBE_HW_VERSION") == 0)
    {
        int ret;
        unsigned int reg = REG_FCPROBE_HW_VERSION;

        ioctl(fd, FCPROBE_HW_REGREAD, &reg);
        printf("%s : 0x%x\n", argv[1], reg);
    }
    if(strcmp(argv[1], "REG_FCPROBE_IMQ_PROD_INDEX") == 0)
    {
        unsigned int reg = REG_FCPROBE_IMQ_PROD_INDEX;
        ioctl(fd, FCPROBE_HW_REGREAD, &reg);
        printf("%s : 0x%x\n", argv[1], reg);
    }

close:
    close(fd);
    printf("fcprobe close ok!\n");
    return 0;
}



