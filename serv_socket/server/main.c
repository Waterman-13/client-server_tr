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

//для списка клиентов
typedef struct _client {
  int Connfd;
  struct _client *prev;
  struct _client *next;
} CLIENT;
CLIENT *head = NULL, *tail = NULL;

CLIENT *clientDB = NULL;

void add_obj(CLIENT *list, CLIENT *client) {
  CLIENT *ptr = (CLIENT *)malloc(sizeof(CLIENT));
  ptr->prev = client;
  ptr->next = (client == NULL) ? NULL : client->next;
  if (client != NULL) {
    client->next = ptr;
    if (client->next != NULL) {
      client->next->prev = ptr;
    }
  }
  if (ptr->prev == NULL)
    head = ptr;
  if (ptr->next == NULL)
    tail = ptr;
}

void del_obj(CLIENT *client) {
  if (client == NULL)
    return;
  CLIENT *prev_obj = client->prev;
  CLIENT *next_obj = client->next;
  free(client);
  if (prev_obj != NULL)
    prev_obj->next = next_obj;
  else
    head = next_obj;

  if (next_obj != NULL)
    next_obj->prev = prev_obj;
  else
    tail = prev_obj;
}

void *connecion_process(void *arg) {
  int fd = *((int *)arg);

  return NULL;
}

void *TCPServer() {
  int quantClients;

  printf("Количество клиентов: ");
  scanf("%d", &quantClients);
  //считывание таблицы из текстового файла в буфер
  FILE *fp;
  struct Table {
    int column1;
    char column2[20];
    char column3[20];
  };

  struct Table buf[10];
  int i;
  fp = fopen("../testFile.txt", "r");
  for (i = 0; i < 3; i++) {
    fscanf(fp, "%d %s %s", &(buf[i].column1), &(buf[i].column2),
           &(buf[i].column3));
  }

  fclose(fp);

  pthread_t threadID = 0;
  struct sockaddr_in Server;
  struct sockaddr_in Client;
  int Listenfd = 0;
  int *pConnectfd = NULL;
  int j = 0;
  int yes = 0;
  // int index = 0;
  // int Connfd = 0;
  int ret = 0;
  socklen_t len = 0;

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  Listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (Listenfd < 0) {
    perror("socket");
    exit(1);
  }

  setsockopt(Listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  memset(&Server, 0, sizeof(Server));
  Server.sin_family = AF_INET;
  Server.sin_port = htons(8080);
  Server.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(Listenfd, (struct sockaddr *)&Server, sizeof(Server)) < 0) {
    perror("bind");
    exit(2);
  }

  if (listen(Listenfd, &quantClients) < 0) {
    perror("listen error");
    exit(3);
  }

  while (1) {
    FD_ZERO(&fds); //
    FD_SET(Listenfd, &fds);

    struct timeval timeout = {30, 0};

    switch (select(Listenfd + 1, &fds, NULL, NULL, &timeout)) {
    case -1:
      memset(buf, 0, 10);
      sprintf(buf, "main: create TCP server select fail(%d) %s.\n", errno,
              strerror(errno));

      return NULL;

    case 0:

      break;

    default: {
      if (FD_ISSET(Listenfd, &fds)) {
        len = sizeof(Client);
        bzero(&Client, len);

        int Connfd = accept(Listenfd, (struct sockaddr *)&Client, &len);

        if (Connfd != -1) {
          CLIENT *new_client = calloc(1, sizeof((*CLIENT)));
          new_client->Connfd = Connfd;
          add_obj(clientDB, new_client);
          FD_SET(new_client->Connfd, &fds);

          *pConnectfd = Connfd;
          send(Connfd, buf, sizeof(buf), 0);
          ret = recv(Connfd, buf, sizeof(buf), 0);

          if (ret < 0) {
            perror("recv error!\n");
            close(Listenfd);
            return -1;
          }

          printf("TCPServer:the connect fd is %d\n", Connfd);

          ret = pthread_create(&threadID, &attr, (void *)pConnectfd);
          if (0 != ret) {
            FD_CLR(Listenfd, &fds);
            return NULL;
          }
        }
      }
    }
    }
  }
  close(Listenfd);
  FD_CLR(Listenfd, &fds);
  pthread_attr_destroy(&attr);
  free(ptr);
  free(pConnectfd);
  return NULL;
}

fd_set fds;
pthread_mutex_t fds_lock;

void thread2() {
  while (1) {
    mutex_lock(fds_lock);

    select

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

  int printfEnable() {
    DEBUG = 1;
    return 0;
  }

  new_thread_for_processing_connected_clients

      while (1) {
    accept;
    if (connfd)

      add_new_client_to_databese_and_select_FDS

          CLIENT *new_client = calloc(1, sizeof((*CLIENT)));
    new_client->Connfd = Connfd;
    add_obj(clientDB, new_client);

    mutex_lock(fds_lock);
    FD_SET(new_client->Connfd, &fds);
    mutex_unlock(fds_lock);
  }

  printf("Main Init OK!\n");

  ret = pthread_create(&threadID, NULL, TCPServer, NULL);
  if (0 != ret) {
    printf("TCPServer: TCPServer build Fail!\n");
    return ERR;
  }
  return 0;
}
