#ifndef CONNEXION_H
#define CONNEXION_H

int init_connexion(const char *username);  
int send_message(const char *message);
int receive_message(char *buffer, int buffer_size);
void close_connexion(void);
int start_client_socket(const char *username);


#endif
