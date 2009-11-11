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
    struct linger linger;
    struct sockaddr_in addrin;
    socklen_t addrlen;
    char linebuf[80];

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
    tios.c_cflag &= ~(CSIZE | CSTOPB | PARENB | CRTSCTS 
#ifdef __MACH__
                        | CCTS_OFLOW | CRTS_IFLOW | MDMBUF
#endif
		     );
    tios.c_cflag |= CLOCAL | CS8 | CREAD;
    tios.c_iflag &= ~(IXON | IXOFF);

    if (-1 == tcsetattr(serial, TCSANOW, &tios))
    {
        perror("tcsetattr");
        close(serial);
        exit(1);
    }

    while (rc > 0)
    {
        rc = read(serial, linebuf, sizeof(linebuf));
        linebuf[rc] = 0;
        printf("discarded %s\n", linebuf);
    }

    s=socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1)
    {
        perror("open socket");
        close(serial);
        exit(1);
    }

    linger.l_onoff = 0;
    linger.l_linger = 0;
    if (setsockopt(s, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger))
        == -1)
    {
        perror("setsockopt");
        close(serial);
        close(s);
        exit(1);
    }

#ifdef __MACH__
    addrin.sin_len = sizeof(struct sockaddr_in);
#endif
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
    while ((cs = accept(s, (struct sockaddr *)&addrin, &addrlen)) != -1)
    {
        char cmdbuf[20], numbuf[60], reply[80];
        static const char *fmt = 
            "<reply><decimal>%s</decimal></reply>";
        printf("Got connection from %s\n",
            (char *)inet_ntoa(addrin));
        /* read the command */
        rc = recv(cs, cmdbuf, sizeof(cmdbuf), 0);
        if (rc > 0)
        {
            int jj;
            for (jj=0; jj < rc; ++jj)
            {
                if (tolower(cmdbuf[jj]) != 'q')
                {
                    printf("sending cmd %c\n", cmdbuf[jj]);
                    write(serial, cmdbuf, rc);
                }
            }
            write(serial, "Q", 1);
            usleep(1000000);
            write(serial, "Q", 1);
            while ((jj = read(serial, numbuf, sizeof(numbuf))) > 0) rc = jj;
            if (rc > 0)
            {
                int ii = 0, last_ii = 0;
                for (jj=0; jj < rc; ++jj)
                {
                    if (numbuf[jj] == '\r' || numbuf[jj] == '\n')
                    {
                        while (jj < rc && 
                            (numbuf[jj] == '\r' || numbuf[jj] == '\n'))
                                ++jj;
                        last_ii = ii;
                        ii = jj;
                    }
                }
                numbuf[rc--] = 0;
                while (rc >= 0 && (numbuf[rc] == '\r' || numbuf[rc] == '\n'))
                {
                    numbuf[rc--] = 0;
                }
                rc = snprintf(reply, sizeof(reply), fmt, &numbuf[last_ii]);
                printf("sending reply %s\n", reply);
                write(cs, reply, rc);
            }
        }
        close(cs);
        addrlen = sizeof(addrin);
    }
}
