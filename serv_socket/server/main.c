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
int i, j;
int yes = 1;
socklen_t len = 0;
pthread_mutex_t fds_lock = PTHREAD_MUTEX_INITIALIZER;

void *thread2(void *arg) {
  char buf[200]; //приёмный буфер
  int nbytes;
  int Listenfd = *(int *)arg;

  while (1) {
    pthread_mutex_lock(&fds_lock);
    read_fds = fds;
    pthread_mutex_unlock(&fds_lock);
    if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
      perror("select");
      exit(4);
    }
    for (i = 0; i <= fdmax; i++) {
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
            // кроме слушателя и себя
            printf("Принято сообщение: %s", buf);
            send(j, buf, nbytes, 0);
            bzero(&buf, sizeof buf);
            // if (j != Listenfd && j != i) {
            if (send(j, buf, nbytes, 0) == -1) {
              perror("send");
            }
            //}
          }
        }
      }
    }
  }
  close(Listenfd);
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
  FD_SET(Listenfd, &fds);
  fdmax = Listenfd; //сохранение наибольшего файлового дескриптора
  pthread_create(&threadID, NULL, (void *)thread2,
                 (void *)&Listenfd); // Создать поток

  while (1) {
    len = sizeof(Client);
    // используется для сохранения IP-адреса клиента
    char cli_ip[INET_ADDRSTRLEN] = "";
    bzero(&Client, len);
    for (i = 0; i <= fdmax; i++) {
      if (FD_ISSET(i, &read_fds)) {
        if (i == Listenfd) {
          Connfd = accept(Listenfd, (struct sockaddr *)&Client, &len);
          pthread_mutex_lock(&fds_lock);
          if (Connfd == -1) {
            perror("accept");
          } else {
            FD_SET(Connfd, &fds);
            if (Connfd > fdmax) {
              fdmax = Connfd;
            }
            inet_ntop(AF_INET, &Client.sin_addr, cli_ip, INET_ADDRSTRLEN);
            printf("----------------------------------------------\n");
            printf("client ip=%s,port=%d\n", cli_ip, ntohs(Client.sin_port));
          }
        }
      }
    }
    pthread_mutex_unlock(&fds_lock);
  }

  // close(Listenfd);
  return 0;
}
