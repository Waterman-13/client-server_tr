/*
** client.c -- пример клиента потокового сокета
*/

/*#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT "3490" // порт для подключения клиентов
#define MAXDATASIZE 100 // максимальная длина принимаемых за раз данных
// получить sockaddr, IPv4 или IPv6:
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
 return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(int argc, char *argv[])
{
int sockfd, numbytes;
char buf[MAXDATASIZE];
struct addrinfo hints, *servinfo, *p;
int rv;
char s[INET6_ADDRSTRLEN];
 if (argc != 2) {
fprintf(stderr,"usage: client hostname\n");
exit(1);
}
 memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
return 1;
}
 // цикл по всем результатам и связывание с первым возможным
for(p = servinfo; p != NULL; p = p->ai_next) {
if ((sockfd = socket(p->ai_family, p->ai_socktype,
p->ai_protocol)) == -1) {
perror("client: socket");
continue;
}
 if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
close(sockfd);
perror("client: connect");
continue;
}
 break;
}
 if (p == NULL) {
fprintf(stderr, "client: failed to connect\n");
return 2;
}
 inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
s, sizeof s);
printf("client: connecting to %s\n", s);
 freeaddrinfo(servinfo); // с этой структурой закончили
 if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
perror("recv");
exit(1);
}
 buf[numbytes] = ’\0’;
 printf("client: received ’%s’\n",buf);
 close(sockfd);
 return 0;
}*/

//другой вариант клиентской части

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
        // create a socket
        int network_socket;
        network_socket = socket(AF_INET, SOCK_STREAM, 0);

        // specify an address for the socket
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(9002);
        server_address.sin_addr.s_addr = INADDR_ANY;

        int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
        // check for error with the connection
        if (connection_status == -1){
                printf("There was an error making a connection to the remote socket \n\n");
        }

        // receive data from the server
        char server_response[100][3];
        recv(network_socket, &server_response, sizeof(server_response), 0);

        // print out the server's response
        printf("The server sent the data: %s\n", server_response);

        // and then close the socket
        close(network_socket);

        return 0;
}
