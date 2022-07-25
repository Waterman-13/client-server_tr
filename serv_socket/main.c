#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main() {
    FILE *fp;
    char server_message[100][3];
    if ((fp=fopen("test", "r"))==NULL) {
    printf ("Невозможно открыть файл.\n");
    exit(1);
    }
    if(fread(server_message, sizeof(char), 300, fp)!=300) {
    if(feof(fp)) printf("Premature end of file.");
    else printf("File read error.");
    };
    fclose(fp);

    //char server_message[256] = "путь/к/текстовому/файлу";
    int quantClients;

    printf("Количество клиентов: ");
    scanf("%d",&quantClients);

    // создание серверного сокета
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // определение адреса сервера
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // привязка сокета к указанному IP и порту
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    // сколько подключений может ожидать этот сокет одновременно
    listen(server_socket, quantClients);

    int client_socket;

    client_socket = accept(server_socket, NULL, NULL);

    // отправление сообщения
    send(client_socket, server_message, sizeof(server_message), 0);

    // закрытие сокета
    close(server_socket);

        return 0;
}

