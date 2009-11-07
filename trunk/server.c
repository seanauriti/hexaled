#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <termios.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int serial = 0;
    int s = 0;
    struct termios tios;
    int rc;
    int cs;
    struct sockaddr_in addrin;
    socklen_t addrlen;

    memset(&tios, 0, sizeof(tios));
    memset(&addrin, 0, sizeof(addrin));

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s serial_port\n", argv[0]);
        exit(1);
    }
    serial = open(argv[1], O_RDWR | O_NONBLOCK);
    if (serial == -1)
    {
        perror("Open serial port");
        exit(1);
    }
    if (-1 == tcgetattr(serial, &tios))
    {
        perror("tcgetattr");
        close(serial);
        exit(1);
    }
    cfsetspeed(&tios, 19200);
    tios.c_cflag &= ~(CSIZE | CSTOPB | PARENB 
                        | CCTS_OFLOW | CRTS_IFLOW | CRTSCTS | MDMBUF);
    tios.c_cflag |= CLOCAL | CS8 | CREAD;
    tios.c_iflag &= ~(IXON | IXOFF);

    if (-1 == tcsetattr(serial, TCSANOW, &tios))
    {
        perror("tcsetattr");
        close(serial);
        exit(1);
    }

    s=socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1)
    {
        perror("open socket");
        close(serial);
        exit(1);
    }

    addrin.sin_len = sizeof(struct sockaddr_in);
    addrin.sin_family = AF_INET;
    addrin.sin_port = htons(11111);
    rc = bind(s, (struct sockaddr *)&addrin, sizeof(addrin));
    if (rc == -1)
    {
        perror("bind socket");
        close(s);
        close(serial);
        exit(1);
    }
    rc = listen(s, 10);
    if (rc == -1)
    {
        perror("listen");
        close(s);
        close(serial);
        exit(1);
    }
    addrlen = sizeof(addrin);
    while ((cs = accept(s, (struct sockaddr *)&addrin, &addrlen)) == -1)
    {
        char cmdbuf[20], numbuf[20], reply[80];
        static const char *fmt = 
            "<reply><decimal>%s</decimal></reply>";
        printf("Got connection from %s\n",
            inet_ntoa(addrin));
        /* read the command */
        rc = recv(cs, cmdbuf, sizeof(cmdbuf), 0);
        if (rc > 0)
        {
            cmdbuf[rc] = 0;
            printf("command %s\n", cmdbuf);
            write(serial, cmdbuf, rc);
            write(serial, "Q", 1);
            rc = read(serial, numbuf, sizeof(numbuf));
            if (rc > 0)
            {
                numbuf[rc] = 0;
                printf("read %s\n", numbuf);
                rc = snprintf(reply, sizeof(reply), fmt, numbuf);
                write(cs, reply, rc);
            }
        }
        close(cs);
        addrlen = sizeof(addrin);
    }
}
