#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 12345
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

SOCKET clients[MAX_CLIENTS];
int client_count = 0;
CRITICAL_SECTION clients_mutex;

void add_client(SOCKET client_socket) {
    EnterCriticalSection(&clients_mutex);
    if (client_count < MAX_CLIENTS) {
        clients[client_count++] = client_socket;
    }
    LeaveCriticalSection(&clients_mutex);
}

void remove_client(SOCKET client_socket) {
    EnterCriticalSection(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i] == client_socket) {
            clients[i] = clients[--client_count];
            break;
        }
    }
    LeaveCriticalSection(&clients_mutex);
}

void broadcast_message(SOCKET sender_socket, const char *message) {
    EnterCriticalSection(&clients_mutex);
    for (int i = 0; i < client_count; ++i) {
        if (clients[i] != sender_socket) {
            send(clients[i], message, strlen(message), 0);
        }
    }
    LeaveCriticalSection(&clients_mutex);
}

DWORD WINAPI clientHandler(LPVOID client_socket_ptr) {
    SOCKET client_socket = *(SOCKET *)client_socket_ptr;
    free(client_socket_ptr);
    char buffer[BUFFER_SIZE];
    int valread;

    printf("Client conected (thread %lu).\n", GetCurrentThreadId());

    add_client(client_socket);

    char username[64] = "User";
    memset(buffer, 0, BUFFER_SIZE);
    valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (valread > 0) {
        sscanf(buffer, "%63[^\n]", username);  // accepte les espaces
        printf("Username : %s\n", username);
    }

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
            printf("Client %s disconnected (thread %lu).\n", username, GetCurrentThreadId());
            break;
        }

        e
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

        
        printf("[%s] (%s): %s", time_str, username, buffer);

        
        char response[BUFFER_SIZE + 128];
        snprintf(response, sizeof(response), "%s|%s", username, buffer);


        broadcast_message(client_socket, response);
    }

    remove_client(client_socket);
    closesocket(client_socket);
    return 0;
}


int main() {
    WSADATA wsaData;
    SOCKET server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    InitializeCriticalSection(&clients_mutex);

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("Error init with Winsock\n");
        return 1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        printf("Error create socket : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Error bind : %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        printf("Error de listen : %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server waiting for connections on port %d...\n", PORT);

    while (1) {
        SOCKET *new_socket = malloc(sizeof(SOCKET));
        *new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (*new_socket == INVALID_SOCKET) {
            printf("Error accept : %d\n", WSAGetLastError());
            free(new_socket);
            continue;
        }

        HANDLE thread = CreateThread(NULL, 0, clientHandler, new_socket, 0, NULL);
        if (thread == NULL) {
            printf("Error creating thread.\n");
            closesocket(*new_socket);
            free(new_socket);
        } else {
            CloseHandle(thread);
        }
    }

    DeleteCriticalSection(&clients_mutex);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
