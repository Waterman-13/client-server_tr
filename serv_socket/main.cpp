//#include <QCoreApplication>
#include <QDebug>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


/*int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    return a.exec();
}*/

    // создание серверного сокета
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // определение адреса сервера
    struct sockaddr_in{
       int sin_family;
       int sin_port;
       int sin_addr.s_addr;
    }
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // привязка сокета к указанному IP-адресу и порту
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    // сколько подключений может одновременно ожидать этот сокет
        listen(server_socket, 5);

        int client_socket;

        client_socket = accept(server_socket, NULL, NULL);

        // отправление сообщения
        send(client_socket, server_message, sizeof(server_message), 0);

        // закрытие сокета
        close(server_socket);

        return 0;
    }

