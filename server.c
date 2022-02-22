#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 4981
#define MAXLINE 1024
int *ptr;

void doprocessing(int sock);

void *UDP(int sock);

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n, pid;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = PORT;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    /* Now start listening for the clients, here
       * process will go in sleep mode and will wait
       * for the incoming connection
    */

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        printf("acepting\n");
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        /* Create child process */
        pid = fork();

        if (pid < 0) {
            perror("ERROR on fork");
            exit(1);
        }

        if (pid == 0) {
            /* This is the client process */
            close(sockfd);

            doprocessing(newsockfd);
            exit(0);
        } else {
            printf("close\n");
            // UDP(newsockfd);
            close(newsockfd);
        }
        printf("while\n");
    } /* end of while */
}


void doprocessing(int sock) {
    int N = 2;
    ptr = mmap(NULL, N * sizeof(int),
               PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    ssize_t n;
    char buffer[256];
    printf("welcome\n");

    bzero(buffer, 256);
    n = read(sock, buffer, 255);

    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    printf("TCP message: %s\n", buffer);
    if ((strncmp(buffer, "exit", 4)) == 0) {
        write(sock, "exit", 4);
        return;
    }
    n = write(sock, "I got your message", 18);
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }

    pthread_t th;
    if (pthread_create(&th, NULL, &UDP, NULL) != 0) {
        perror("Failed to create thread");
    }

//    if (pthread_join(th, NULL) != 0) {
//        perror("Failed to join thread");
//    }

    while (1) {
        bzero(buffer, 256);
        n = read(sock, buffer, 255);

        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }

        printf("TCP loop message: %s\n", buffer);
        if ((strncmp(buffer, "exit", 4)) == 0) {
            ptr[0] = 1;

            if (pthread_join(th, NULL) != 0) {
                perror("Failed to join thread");
            }

            write(sock, &ptr[1], 2);
            printf("TCP exit %d\n", ptr[1]);
            break;
        }

        n = write(sock, "I got your message TCP", 18);

        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
    }
    printf("bye from TCP\n");
}


void *UDP(int sock) {
    int counter = 0;
    printf("helooooooooooooooooooUDP\n");
    sock = PORT;
    uint16_t port = (uint16_t) sock;
    printf("port: %hu\n", port);

    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    //servaddr.sin_addr.s_addr = INADDR_ANY;

    inet_pton(AF_INET, "192.168.0.18", &servaddr.sin_addr);

    servaddr.sin_port = htons(port);


    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *) &servaddr,
             sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    ssize_t len, n;

    len = sizeof(cliaddr);


    int flags = fcntl(sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, flags);

    while (ptr[0] == 0) {
        n = recvfrom(sockfd, (char *) buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *) &cliaddr,
                     (socklen_t *) &len);
        if (n != -1) {
            buffer[n] = '\0';
            printf("Client UDP: %s fuel: %d\n", buffer, ptr[0]);
            counter++;
            sleep(1);
        }
        //  printf("UDP \n");
    }
    //  ptr[1] = counter;
    printf("UDP exit and counter: %d\n", counter);
    return NULL;
}
