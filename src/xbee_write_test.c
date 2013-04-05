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
#include "inc/xbee.h"

int main( void )
{ 
    int fd; /* File descriptor for the port */

    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        /*
        * Could not open the port.
        */
        perror("open_port: Unable to open /dev/ttyUSB0 - ");
    }

    system("stty -F /dev/ttyUSB0 9600");
    int ldisc = 17;
    ioctl(fd, TIOCSETD, &ldisc);
    fcntl(fd, F_SETFL, FNDELAY);

    struct termios options;

    tcgetattr(fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag |= (CLOCAL | CREAD | CS8);
    options.c_cflag &= ~(CSIZE | CSTOPB | PARENB);

    options.c_cflag |= CRTSCTS;

    tcsetattr(fd, TCSANOW, &options);

    Xbee_Init(fd);

    unsigned char msg[5] = { 0x48, 0x65, 0x6c, 0x6c, 0x6f };
    ssize_t retVal;
    unsigned short to = 0x0001;

    int i = 0;
    Xbee_SendTransmitMessage(to, msg, 5);
    usleep(100000);
    printf("Sent: %d\n", i++);

    return 0 ;
}
