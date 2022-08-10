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

fd_set fds;
struct sockaddr_in Server;
struct sockaddr_in Client;
int yes = 0;
int Connfd = 0;
int Listenfd = 0;
socklen_t len = 0;
pthread_mutex_t fds_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct Table {
  int column1;
  char column2[20];
  char column3[20];
} Table;

/*Table sort(){
//сортируем buf
};*/
void *thread2(void *arg) {
  //считывание таблицы из текстового файла в буфер
  FILE *fp;

  Table buf[10];
  int i = 0;
  fp = fopen("home/vadim/client_server/testFile.txt", "r");

  while (fscanf(fp, "%d %s %s", &(buf[i].column1), &(buf[i].column2),
                &(buf[i].column3)) != EOF) {
    i++;
  }

  fclose(fp);
  // Table server_message = sort(buf);

  // int recv_len = 0;
  // char recv_buf[1024] = ""; // приемный буфер
  int Connfd = *(int *)arg; // переданный подключенный сокет
  pthread_mutex_unlock(&fds_lock);

  while (1) {
    pthread_mutex_lock(&fds_lock);
    FD_ZERO(&fds);
    FD_SET(Listenfd, &fds);
    struct timeval timeout = {30, 0};
    if (select(Listenfd + 1, &fds, NULL, NULL, &timeout) == -1) {
      memset(buf, 0, 10);
      sprintf(buf, "main: create TCP server select fail(%d) %s.\n", errno,
              strerror(errno));
      // VOS_PRINTF_LOG(logMessage);
      return NULL;
    }

    else if (select(Listenfd + 1, &fds, NULL, NULL, &timeout) == 0) {
      printf("TCPServer:TCP receiving nothing......\n");
      break;
    }

    /*default: {
      if (FD_ISSET(Listenfd, &fds)) {
        len = sizeof(Client);
        bzero(&Client, len);

        Connfd = accept(Listenfd, (struct sockaddr *)&Client, &len);

        if (Connfd != -1) {
          pConnectfd = (int *)malloc(sizeof(int));
          if (NULL == pConnectfd) {
            VOS_PRINTF_LOG("TCPServer: pConnectfd malloc Failed.\n");
            break;
          }

          memset(pConnectfd, 0, sizeof(int));
          *pConnectfd = Connfd;

          // VOS_PRINTF_LOG("TCPServer:get a new request!\n");
          printf("TCPServer:the connect fd is %d\n", Connfd);

          ret = pthread_create(&threadID, &attr, TCP_Analyzer,
                               (void *)pConnectfd);
          if (0 != ret) {
            // VOS_PRINTF_LOG("TCPServer: TCP_Analyzer build Fail!\n");
            FD_CLR(Listenfd, &fds); //
            free(pConnectfd);
            return NULL;
          }
        }
      }
    }*/
  }

  pthread_mutex_unlock(&fds_lock);

  if (Connfd != -1) {
    send(Connfd, buf, sizeof(buf), 0);
  }
}

int main() {
  //задаём количество клиентов
  int quantClient;
  printf("Введите количество клиентов: ");
  scanf("%d", &quantClient);

  int ret = 0;
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

  setsockopt(Listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  bzero(&Server, sizeof(Server));
  Server.sin_family = AF_INET;
  Server.sin_port = htons(8080);
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
    pthread_mutex_lock(&fds_lock);
    Connfd = accept(Listenfd, (struct sockaddr *)&Client, &len);
    if (Connfd == -1) {
      // pConnectfd = (int *)malloc(sizeof(int));
      // if (NULL == pConnectfd) {
      perror("accept error!\n");
      break;
    }

    else if (Connfd != -1)

    {

      // Параметры, переданные в функцию обратного вызова, & Connfd, адрес
      // передан

      pthread_create(&threadID, NULL, (void *)thread2,
                     (void *)&Connfd); // Создать поток

      // pthread_detach(&threadID); // отсоединяем поток
    }
    pthread_mutex_unlock(&fds_lock);
  }
}
