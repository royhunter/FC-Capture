#include "fc_util.h"


int main()
{
    int fd;
    unsigned int version;
    unsigned int value;
    char buf[1024];
    fd = open("/dev/fcprobe", O_RDWR);
    if(fd < 0)
    {
        printf("fcprobe open failed\n");
        return 1;
    }

    printf("fcprobe open ok!\n");

    ioctl(fd, FCPROBE_HW_VERSION, &version);

    printf("fcprobe version is 0x%x\n", version);


    int ret;
    int i;
    int *ptr = (int *)buf;
    ioctl(fd, FCPROBE_HW_SELFTEST_ON, NULL);
    printf("begin rx:\n");
    for(i = 0; ; i++)
    {
        //usleep(500000);
	    memset(buf, 0, sizeof(buf));
        ret = ioctl(fd, FCPROBE_PKT_RECV, buf);
        printf("rx done ret is %d\n", ret);
        printf("word0 is 0x%x\n", *ptr);
        printf("word1 is 0x%x\n", *(ptr+1));
        printf("word2 is 0x%x\n", *(ptr+2));
        printf("word3 is 0x%x\n", *(ptr+3));

    }
    ioctl(fd, FCPROBE_HW_SELFTEST_OFF, NULL);



    close(fd);

    return 0;
}




