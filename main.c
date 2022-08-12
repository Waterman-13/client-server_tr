#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

//для сортировки принятой таблицы
void qs_string(char items[200], int left, int right) {
  register int i, j;
  char *x;
  char temp[200];
  i = left;
  j = right;
  x = items[(left + right) / 2];
  do {
    while ((strcmp(items[i], x) < 0) && (i < right))
      i++;
    while ((strcmp(items[j], x) > 0) && (j > left))
      j--;
    if (i <= j) {
      strcpy(temp, items[i]);
      strcpy(items[i], items[j]);
      strcpy(items[j], temp);
      i++;
      j--;
    }
  } while (i <= j);
  if (left < j)
    qs_string(items, left, j);
  if (i < right)
    qs_string(items, i, right);
}
void quick_string(char items[200], int count) {
  qs_string(items, 0, count - 1);
}

//главная функция
int main(int argc, char *argv[]) {
  int connfd = 0;
  // int cLen = 0;
  struct sockaddr_in client;
  if (argc < 2) {
    printf("Usage: clientent[server IP address]\n");
    return -1;
  }
  client.sin_family = AF_INET;
  client.sin_port = htons(8080);
  client.sin_addr.s_addr = inet_addr(argv[1]);
  connfd = socket(AF_INET, SOCK_STREAM, 0);
  if (connfd < 0) {
    perror("socket");
    return -1;
  }
  if (connect(connfd, (struct sockaddr *)&client, sizeof(client)) < 0) {
    perror("connect");
    return -1;
  }

  char buf[200];
  send(connfd,buf,200,0);
  bzero(buf, sizeof(buf));
  recv(connfd, buf, 200, 0);
  //сортировка buf
  int count = sizeof(buf) / sizeof(buf[0]);
  quick_string(buf, count);
  for (int i = 0; i < count; i++) {
    printf("recv: %s\n", buf[i]);
  }

  // bzero(buffer, sizeof(buffer));
  // strcpy(buffer, "this is a client!\n");
  // send(connfd, buffer, 1024, 0);
  /*while(1)
  {
      bzero(buffer, sizeof(buffer));
      scanf("%s", buffer);
      int p = strlen(buffer);
      buffer[p] = '\0';
      send(connfd, buffer, 1024, 0);
      printf("i have send buffer");
  }*/
  close(connfd);
  return 0;
}
