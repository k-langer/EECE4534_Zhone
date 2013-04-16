#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include "xbee.h"

int main( void )
{ 
    int fd; /* File descriptor for the port */

    fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        /*
        * Could not open the port.
        */
        perror("open_port: Unable to open /dev/ttyACM0 - ");
    }
    system("stty -F /dev/ttyACM0 9600 ignbrk -brkint -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts");
    int ldisc = 17;
    ioctl(fd, TIOCSETD, &ldisc);
    fcntl(fd, F_SETFL, 0);

    struct termios options;

    tcgetattr(fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag |= (CLOCAL | CREAD | CS8);
    options.c_cflag &= ~(CSIZE | CSTOPB | PARENB);

    options.c_cflag |= CRTSCTS;

    tcsetattr(fd, TCSANOW, &options);

    Xbee_Init(fd);

    //unsigned char msg[] = {0x7e, 0x00, 0x0a, 0x81, 0x00, 0x02, 0x17, 0x00, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x71 };
    unsigned char msg[120] = {0};
    unsigned char length;
    int status = 0;
    
    while (1)
    {
        status = Xbee_GetMessage(msg, &length);
        printf("status: %d\n", status);

        unsigned char i;
        for (i = 0; i < length; i++)
            printf("%02x ", msg[i]);

        xbee_message_t *pXbee_msg = (xbee_message_t *)malloc(sizeof(xbee_message_t));
        status = Xbee_UnpackMessage(msg, 14, pXbee_msg);
        status = Xbee_PrintMessage(pXbee_msg);

        xbee_receive_message_t *pReceive_msg = (xbee_receive_message_t *)malloc(sizeof(xbee_receive_message_t));
        status = Xbee_UnpackReceiveMessage(pXbee_msg, pReceive_msg);
        status = Xbee_PrintReceiveMessage(pReceive_msg);
    }

    return 0 ;
}
