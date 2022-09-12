//[04.08.2022 16:32]
//Вариант 1.
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
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

struct sockaddr_in Server;
struct sockaddr_in Client;
fd_set fds; //главный список файловых дескрипторов
fd_set read_fds; // временный список файловых дескрипторов для select()
int fdmax;
int yes = 1;
socklen_t len = 0;
pthread_mutex_t fds_lock = PTHREAD_MUTEX_INITIALIZER;
typedef struct {
  // struct sockaddr_in address;
  int fsocket;
  int faccept;
} client_t;

void *thread2(void *arg) {
  char buf[200]; //приёмный буфер
  int nbytes;
  int i, j;
  client_t *cli_info = (client_t *)arg;

  FD_SET(cli_info->fsocket,&fds);
  fdmax = cli_info->fsocket; //сохранение наибольшего файлового дескриптора
  while (1) {
    pthread_mutex_lock(&fds_lock);
    read_fds = fds;
    if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
      perror("select");
      exit(4);
    }
    // ищем готовые для чтения данные в существующих подключениях
    for (i = 0; i <= fdmax; i++) {
      if (FD_ISSET(i, &read_fds)) {
        if (i == cli_info->fsocket) {
          if (cli_info->faccept == -1) {
            perror("accept");
          } else {
            FD_SET(cli_info->faccept, &fds);
            if (cli_info->faccept > fdmax) {
              fdmax = cli_info->faccept;
            }
          }
        } else {
          // обработка данных от клиента
          if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
            // ошибка или соединение закрыто клиентом
            if (nbytes == 0) {
              // соединение закрыто
              printf("selectserver: socket %d hung up\n", i);
            } else {
              perror("recv");
            }
            close(i);        // Пока!
            FD_CLR(i, &fds); // удалить из главного массива
          } else {
            // от клиента что-то получили
            for (j = 0; j <= fdmax; j++) {
              // посылаем всем!
              if (FD_ISSET(j, &fds)) {
                send(j, buf, nbytes, 0);
                //if (j != cli_info->fsocket && j != i) {
                  if (send(j, buf, nbytes, 0) == -1) {
                    perror("send");
                  }
                //}
              }
            }
          }
        }
      }
    }
    pthread_mutex_unlock(&fds_lock);
  }
  free(cli_info);
  close(cli_info->fsocket);
}

/*void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}*/

int main() {
  int Connfd = 0;

  // struct sockaddr_storage remoteaddr; // адрес клиента
  // socklen_t addrlen;

  //задаём количество клиентов
  int quantClient;
  printf("Введите количество клиентов: ");
  scanf("%d", &quantClient);
  // char buf[200];
  // int ret = 0;
  int Listenfd = 0;
  // int *pConnectfd = NULL;
  pthread_t threadID = 0;

  // clientDB = calloc(1, sizeof((*clientDB)));
  // clientDB->Connfd = -1;

  // new_thread_for_processing_connected_clients;
  pthread_mutex_init(&fds_lock, NULL);

  Listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (Listenfd < 0) {
    perror("socket error");
    exit(1);
  }
  printf("TCPServer:create socket success\n");
  FD_ZERO(&fds);
  FD_ZERO(&read_fds);

  bzero(&Server, sizeof(Server));
  Server.sin_family = AF_INET;
  Server.sin_port = htons(8080);
  Server.sin_addr.s_addr = htonl(INADDR_ANY);
  // Server.sin_addr.s_addr = inet_addr("127.0.0.1");
  setsockopt(Listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if (bind(Listenfd, (struct sockaddr *)&Server, sizeof(Server)) < 0) {
    perror("bind");
    exit(2);
  }
  if (listen(Listenfd, quantClient) < 0) {
    perror("listen error");
    exit(3);
  }

  while (1) {
    pthread_mutex_lock(&fds_lock);
    // FD_ZERO(&fds);
    // FD_SET(Listenfd, &fds);
    // if (FD_ISSET(Listenfd, &fds)) {
    len = sizeof(Client);
    char cli_ip[INET_ADDRSTRLEN] = ""; // используется для сохранения IP-адреса клиента
    bzero(&Client, len);
    Connfd = accept(Listenfd, (struct sockaddr *)&Client, &len);
    /*if (Connfd != -1) {
      pConnectfd = (int *)malloc(sizeof(int));
      if (NULL == pConnectfd) {
        printf("TCPServer: pConnectfd malloc Failed.\n");
        break;
      }

      memset(pConnectfd, 0, sizeof(int));
      *pConnectfd = Connfd;

      printf("TCPServer:get a new request!\n");
      printf("TCPServer:the connect fd is %d\n", Connfd);
    }*/
    // распечатать ip и порт клиента
    inet_ntop(AF_INET, &Client.sin_addr, cli_ip, INET_ADDRSTRLEN);
    printf("----------------------------------------------\n");
    printf("client ip=%s,port=%d\n", cli_ip, ntohs(Client.sin_port));

    pthread_mutex_unlock(&fds_lock);
    //}
    //      pthread_create(&threadID, NULL, (void *)thread2,
    //                     (void *)&Listenfd); // Создать поток
  }
  client_t *cli_info = (client_t *)malloc(sizeof(client_t));
  cli_info->fsocket = Listenfd;
  cli_info->faccept = Connfd;
  pthread_create(&threadID, NULL, (void *)thread2,
                 (void *)cli_info); // Создать поток

  // close(Listenfd);
  return 0;
}
