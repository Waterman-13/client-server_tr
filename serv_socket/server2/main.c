#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLIENTS 10
#define INFO "/home/vadim/client_server/server/testFile.txt"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // определить мьютекс
typedef struct {
  struct sockaddr_in address;
  int sockfd;
} client_t;
client_t *clients[MAX_CLIENTS];

/************************************************************************

Имя функции: void * client_process (void * arg)

Функция: функция потока, обработка информации о клиенте
************************************************************************/

void *client_process(void *arg) {
  int recv_len = 0;
  char recv_buf[1024] = ""; // приемный буфер

  client_t *cli_info = (client_t *)arg;
  //запись файла в массив на передачу
  FILE *fp;
  char send_buf[500];
  pthread_mutex_unlock(&mutex);
  // получить данные

  // if ((recv_len = recv(cli_info->sockfd, recv_buf, sizeof(recv_buf), 0)) > 0)
  // {
  while ((recv_len = recv(cli_info->sockfd, recv_buf, sizeof(recv_buf), 0)) > 0) {

      printf(" от %s:%d : %s \n", inet_ntoa(cli_info->address.sin_addr),
             cli_info->address.sin_port, recv_buf); // распечатать данные
      fp = fopen(INFO, "r");
      while (!feof(fp)) {
        fgets(send_buf, 500, fp);
        send(cli_info->sockfd, send_buf, strlen(send_buf), 0);
      }
      fclose(fp);
      // send(cli_info->sockfd, send_buf, sizeof(send_buf),0); // вернуть данные
      // клиенту
  }
  printf("client %s:%d closed! \n", inet_ntoa(cli_info->address.sin_addr),
         cli_info->address.sin_port);
  close(cli_info->sockfd); // Закрыть подключенный сокет
  free(cli_info);
  return NULL;
}

//===============================================================
// Формат синтаксиса: void main (void)
// Реализация функции: основная функция, установить параллельный сервер TCP
// Параметры ввода: Нет
// Выходные параметры: Нет
//===============================================================

int main() {
  //задаём количество клиентов
  int quantClient;
  int sockfd = 0; // сокет
  int connfd = 0;
  int err_log = 0;
  struct sockaddr_in my_addr; // структура адреса сервера
  unsigned short port = 8080; // порт прослушивания
  pthread_t thread_id;

  printf("Введите количество клиентов: ");
  scanf("%d", &quantClient);
  // инициализировать мьютекс, мьютекс включен по умолчанию
  pthread_mutex_init(&mutex, NULL);
  printf("TCP Server Started at port %d!\n", port);

  sockfd = socket(AF_INET, SOCK_STREAM, 0); // создать TCP-сокет
  if (sockfd < 0) {
    perror("socket error");
    exit(-1);
  }

  bzero(&my_addr, sizeof(my_addr)); // Инициализировать адрес сервера
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  printf("Binding server to port %d\n", port);
  // связать
  err_log = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
  if (err_log != 0) {
    perror("bind");
    close(sockfd);
    exit(-1);
  }

  //  /*Prevent putting socket to quarantine after unxpected closing*/
  //  int flags = fcntl(sockfd, F_GETFL, 0);
  //  if (flags == -1)
  //  {
  //    perror("Cannot read listening socket fd");
  //  }
  //  flags |= O_NONBLOCK;
  //  if (fcntl(sockfd, F_SETFL, flags) != 0)
  //  {
  //    perror("Cannot write listening socket fd");
  //  }
  //  /*******************************************************************/

  // слушаем, сокет становится пассивным
  err_log = listen(sockfd, quantClient);
  if (err_log != 0) {
    perror("listen");
    close(sockfd);
    exit(-1);
  }

  if (quantClient > 3) {
    perror("listen");
    close(sockfd);
  }

  printf("Waiting client...\n");
  while (1) {
    char cli_ip[INET_ADDRSTRLEN] = ""; // используется для сохранения IP-адреса клиента
    // используется для сохранения адреса клиента
    struct sockaddr_in client_addr;
    socklen_t cliaddr_len = sizeof(client_addr); // необходимо инициализировать !!!
    // блокировка, pthread_mutex_lock ()
    pthread_mutex_lock(&mutex);
    // Получить установленное соединение
    connfd = accept(sockfd, (struct sockaddr *)&client_addr, &cliaddr_len);
    if (connfd < 0) {
      perror("accept this time");
      continue;
    }
    // распечатать ip и порт клиента
    inet_ntop(AF_INET, &client_addr.sin_addr, cli_ip, INET_ADDRSTRLEN);
    printf("----------------------------------------------\n");
    printf("client ip=%s,port=%d\n", cli_ip, ntohs(client_addr.sin_port));

    client_t *cli_info = (client_t *)malloc(sizeof(client_t));
    cli_info->address = client_addr;
    cli_info->sockfd = connfd;
    if (connfd > 0) {
      // Параметры, переданные в функцию обратного вызова, & connfd, адрес
      // передан
      pthread_create(&thread_id, NULL, (void *)client_process,
                     (void *)cli_info); // Создать поток
      pthread_detach(thread_id); // отсоединяем поток, автоматически
      // перезаписываем ресурсы в конце
    }
  }
  close(sockfd);
  return 0;
}
