#include <stdio.h>
#include <string.h>
#include "salon.h"

static Salon salons[MAX_SALONS];

void init_salons() {
    for (int i = 0; i < MAX_SALONS; i++) {
        salons[i].message_count = 0;
    }
}

void add_message(SalonType salon, const char *message) {
    if (salon < 0 || salon >= MAX_SALONS) return;

    if (salons[salon].message_count < MAX_MESSAGES) {
        strncpy(salons[salon].messages[salons[salon].message_count], message, MAX_MESSAGE_LENGTH - 1);
        salons[salon].messages[salons[salon].message_count][MAX_MESSAGE_LENGTH - 1] = '\0'; 
        salons[salon].message_count++;
    }
}

const char* get_message(SalonType salon, int index) {
    if (salon < 0 || salon >= MAX_SALONS) return NULL;
    if (index < 0 || index >= salons[salon].message_count) return NULL;

    return salons[salon].messages[index];
}

int get_number_messages(SalonType salon) {
    if (salon < 0 || salon >= MAX_SALONS) return 0;
    return salons[salon].message_count;
}
