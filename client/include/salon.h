#ifndef SALON_H
#define SALON_H

#define MAX_SALONS 3
#define MAX_MESSAGES 100
#define MAX_MESSAGE_LENGTH 512

typedef enum {
    SALON_GENERAL = 0,
    SALON_DEV,
    SALON_RANDOM
} SalonType;

typedef struct {
    char messages[MAX_MESSAGES][MAX_MESSAGE_LENGTH];
    int message_count;
} Salon;

void init_salons();
void add_message(SalonType salon, const char *message);
const char* get_message(SalonType salon, int index);
int get_number_messages(SalonType salon);

#endif
