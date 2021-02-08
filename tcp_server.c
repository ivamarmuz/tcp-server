#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 7890
#define IP_BUFFER 20
#define BUFFER_SIZE 1024

int main(void)
{
    int fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t slen = sizeof(client_addr);
    char client_ip[IP_BUFFER];
    char buffer[BUFFER_SIZE];

    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1) {
        perror("Socket error");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if ((bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1) {
        perror("Bind error");
    }

    listen(fd, 5);
    printf("Listening on port %d...\n", PORT);

    while (1) {
        client_fd = accept(fd, (struct sockaddr *)&client_addr, &slen);
        
        if (fork() == 0) {
            close(fd);
            strcpy(client_ip, (char *)inet_ntoa((struct in_addr)client_addr.sin_addr));
            printf("Client connected: %s\n", client_ip);

            while (1) {
                memset(buffer, 0, BUFFER_SIZE);
                if ((read(client_fd, buffer,BUFFER_SIZE)) == 0) {
                    close(client_fd);
                    printf("Connection closed: %s\n", client_ip);
                    exit(0);
                }
                printf(" > %s", buffer);
            }
        }

        close(client_fd);

        while (wait4(-1, NULL, WNOHANG, NULL) > 0) {};
    }

    return 0;
}