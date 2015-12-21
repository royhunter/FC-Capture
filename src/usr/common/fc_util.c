#include "fc_util.h"




int fcprobe_open()
{
    int fd;
    fd = open("/dev/fcprobe", O_RDWR);
    if(fd < 0)
    {
        printf("fcprobe open failed\n");
        return 1;
    }
}

/*
 * buf size must large than 4*64 bytes
*/
int fcprobe_recv(void *buf)
{
    return ioctl(fd, FCPROBE_PKT_RECV, buf);
}



void fcprobe_close(int fd)
{
    close(fd);
}
