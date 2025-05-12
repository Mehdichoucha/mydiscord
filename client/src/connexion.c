#include "connexion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 12345


static SOCKET sock = INVALID_SOCKET;
extern char current_username[64];

int init_connexion(const char *username) {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("Error WSAStartup\n");
        return 0;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Error for create a socket : %d\n", WSAGetLastError());
        return 0;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Fail connexion\n");
        closesocket(sock);
        return 0;
    }

    
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%s\n", username);
    send(sock, buffer, strlen(buffer), 0);

    return 1;
}

int send_message(const char *message) {
    if (sock == INVALID_SOCKET) return -1;

    char buffer[1024];
    char timestamp[20];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm_info);

    
    snprintf(buffer, sizeof(buffer), "[%s] %s : %s", timestamp, current_username, message);

    int len = strlen(buffer);
    return send(sock, buffer, len, 0);
}


int receive_message(char *buffer, int taille) {
    if (sock == INVALID_SOCKET) return 0;
    memset(buffer, 0, taille);
    return recv(sock, buffer, taille - 1, 0);
}

int start_client_socket(const char *username) {
    return init_connexion(username);
}

void close_connexion() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
    WSACleanup();
}

