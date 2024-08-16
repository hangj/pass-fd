#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char** argv, char** envp) {
    char* exe = argv[0];

    if (argc == 1) {
        struct sockaddr_in my_addr={0}, peer_addr={0};
        socklen_t peer_addr_size;
        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons(8666);
        //my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        int s = socket(AF_INET, SOCK_STREAM, 0);

        int reuseport = 1;
        if(setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport)) != 0) {
            fprintf(stderr, "set SO_REUSEPORT error: %d %s\n", errno, strerror(errno));
            return -1;
        }

        int nosigpipe = 1;
        if(setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, &nosigpipe, sizeof(nosigpipe)) !=0) {
            fprintf(stderr, "set SO_NOSIGPIPE error: %s\n", strerror(errno));
            return -1;
        }

        int nodelay = 1;
        if(setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)) != 0) {
            fprintf(stderr, "s set TCP_NODELAY error: %d %s\n", errno, strerror(errno));
            return -1;
        }

        if(bind(s, (struct sockaddr*)&my_addr, sizeof(struct sockaddr_in)) != 0){
            fprintf(stderr, "bind error: %d %s\n", errno, strerror(errno));
            return -1;
        }
        if(listen(s, 2) != 0){
            fprintf(stderr, "listen error: %d %s\n", errno, strerror(errno));
            return -1;
        }

        int client = accept(s, (struct sockaddr*)&peer_addr, &peer_addr_size);
        if (client < 0) {
            fprintf(stderr, "accept error: %s\n", strerror(errno));
            return -1;
        }
        printf("accepted: %d\n", client);


        int fd = fork();
        if (fd < 0){
            fprintf(stderr, "fork error: %s\n", strerror(errno));
            return -1;
        }
        if (fd > 0) {
            // parent
            send(client, "here's the parent\n", 18, 0);
            close(client);
            // should tell the child to close this fd, too.
            exit(0);
        }

        // child
        char buf[20] = {0};
        sprintf(buf, "%d %d", s, client);
        char* argv[] = {exe, buf, NULL};
        execve(argv[0], argv, envp);

        return 0;
    }


    {
        printf("child process...\n");

        int s = 0;
        int client = 0;
        sscanf(argv[1], "%d %d", &s, &client);

        if(s == 0 || client == 0) {
            fprintf(stderr, "invalid fd\n");
            return -1;
        }

        // pretend we got the notice that the parent closed this fd.
        close(client);

        int nodelay = 0;
        socklen_t option_len = sizeof(nodelay);
        if(getsockopt(s, IPPROTO_TCP, TCP_NODELAY, &nodelay, &option_len) != 0) {
            fprintf(stderr, "get TCP_NODELAY error: %d %s\n", errno, strerror(errno));
            return -1;
        }
        printf("TCP_NODELAY: %d\n", nodelay);

        int fd = accept(s, NULL, NULL);
        if (fd < 0) {
            fprintf(stderr, "accept error: %s\n", strerror(errno));
            return -1;
        }
        printf("accepted: %d\n", fd);
        send(fd, "here's the child\n", 17, 0);
        close(fd);
    }

    return 0;
}
