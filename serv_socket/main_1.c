//[04.08.2022 16:32]
//Вариант 1.
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

fd_set fds;
int quantClient;
printf("Введите количество клиентов: ");
scanf("%d", &quantClient);

struct sockaddr_in Server;
struct sockaddr_in Client;
int yes = 0;
int Connfd = 0;
int Listenfd = 0;
socklen_t len = 0;

void thread2() {

  pthread_mutex_t fds_lock;

  setsockopt(Listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  bzero(&Server, sizeof(Server));
  Server.sin_family = AF_INET;
  Server.sin_port = htons(PORT);
  Server.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(Listenfd, (struct sockaddr *)&Server, sizeof(Server)) < 0) {
    perror("bind");
    exit(2);
  }
  if (listen(Listenfd, &quantClient) < 0) {
    perror("listen error");
    exit(3);
  }

  while (1) {
    FD_ZERO(&fds);
    FD_SET(Listenfd, &fds);
    mutex_lock(fds_lock);
    struct timeval timeout = {30, 0};
    switch (select(Listenfd + 1, &fds, NULL, NULL, &timeout))

      mutex_unlock(fds_lock);

    if (have_request)
      send_response
  }
}

int main() {

  int ret = 0;
  pthread_t threadID = 0;

  clientDB = calloc(1, sizeof((*clientDB)));
  clientDB->Connfd = -1;

  new_thread_for_processing_connected_clients;
  ///////////////////////////////////////////////////////////////////
  int Listenfd = 0;

  Listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (Listenfd < 0) {
    perror("socket");
    exit(1);
  }
  printf("TCPServer:create socket success\n");

  setsockopt(Listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  bzero(&Server, sizeof(Server));
  Server.sin_family = AF_INET;
  Server.sin_port = htons(PORT);
  Server.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(Listenfd, (struct sockaddr *)&Server, sizeof(Server)) < 0) {
    perror("bind");
    exit(2);
  }
  if (listen(Listenfd, &quantClient) < 0) {
    perror("listen error");
    exit(3);
  }

  while (1) {
    /*accept;
    if (connfd)

      add_new_client_to_databese_and_select_FDS

          // CLIENT *new_client = calloc(1, sizeof((*CLIENT)));
          new_client->Connfd = Connfd;
    add_obj(clientDB, new_client);

    mutex_lock(fds_lock);
    FD_SET(new_client->Connfd, &fds);
    mutex_unlock(fds_lock);*/
    Connfd = accept(Listenfd, (struct sockaddr *)&Client, &len);
    if (Connfd != -1) {
      pConnectfd = (int *)malloc(sizeof(int));
      if (NULL == pConnectfd) {
        printf("TCPServer: pConnectfd malloc Failed.\n");
        break;
      }
    }
  }
