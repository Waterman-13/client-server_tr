#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define PORT "8080"

// получить sockaddr, IPv4 или // IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void) {
  //задаём количество клиентов
  int quantClient;
  printf("Введите количество клиентов: ");
  scanf("%d", &quantClient);

  fd_set master; // главный список файловых дескрипторов
  fd_set read_fds; // временный список файловых дескрипторов для select()
  int fdmax; // максимальный номер файлового дескриптора

  int listener; // дескриптор слушаемого сокета

  int newfd; // новопринятый дескриптор сокета

  struct sockaddr_storage remoteaddr; // адрес клиента
  socklen_t addrlen;
  char buf[200];
  /*char buff[200];
  FILE *fpf;

  int k = 0;
  fpf = fopen("testFile.txt", "r");
  while (fscanf(fpf, "%s", &buff[k]) != EOF){

  k++;
}

  // fgets(buf, 200, fp);
  fclose(fpf);*/
  int nbytes;
  // буфер для данных клиента
  char remoteIP[INET6_ADDRSTRLEN];
  int yes = 1;
  int i, j, rv;
  // для setsockopt() SO_REUSEADDR, ниже

  struct addrinfo hints, *ai, *p;
  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  // очистка главного и временного массивов
  // получить сокет и связать
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
    fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
    exit(1);
  }
  for (p = ai; p != NULL; p = p->ai_next) {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
      continue;
    }
    // убрать мерзкое сообщение "address already in use"
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
      close(listener);
      continue;
    }
    break;
  }

  // если мы здесь, значит не связались
  if (p == NULL) {
    fprintf(stderr, "selectserver: failed to bind\n");
    exit(2);
  }
  freeaddrinfo(ai);
  // с этим закончили
  // слушаем
  listen(listener, &quantClient);
  if (listen(listener, &quantClient) == -1) {
    perror("listen");
    exit(3);
  }
  // добавить слушателя в главный массив
  FD_SET(listener, &master);
  // сохранить наибольший файловый дескриптор
  fdmax = listener;
  // вот он
  // главный цикл
  for (;;) {
    read_fds = master;
    // копируем
    if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
      perror("select");
      exit(4);
    }
    // ищем готовые для чтения данные в существующих подключениях
    for (i = 0; i <= fdmax; i++) {

      if (FD_ISSET(i, &read_fds)) {
        if (i == listener) {
          // Есть!!
          // обрабатываем новые подключения
          addrlen = sizeof remoteaddr;
          newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
          if (newfd == -1) {
            perror("accept");
          } else {
            FD_SET(newfd, &master);
            // добавить в главный массив
            if (newfd > fdmax) {
              // отслеживаем максимальный номер
              fdmax = newfd;
            }
            printf("selectserver: new connection from %s on "
                   "socket %d\n",
                   inet_ntop(remoteaddr.ss_family,
                             get_in_addr((struct sockaddr *)&remoteaddr),
                             remoteIP, INET6_ADDRSTRLEN),
                   newfd);
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
            close(i);
            // Пока!
            FD_CLR(i, &master);
            // удалить из главного массива
          } else {
            // от клиента что-то получили
            for (j = 0; j <= fdmax; j++) {
              // посылаем всем!
              if (FD_ISSET(j, &master)) {

                // if (j == listener && j == i) {
                send(j, buf, nbytes, 0);
                if (send(j, buf, nbytes, 0) == -1) {
                  perror("send");
                }
                //}
              }
            }
          }
        } // END обработка данных от клиента
      }   // END есть новое входящее подключение
    }     // END цикл по файловым дескрипторам
  }       // END for(;;)
  return 0;
}
