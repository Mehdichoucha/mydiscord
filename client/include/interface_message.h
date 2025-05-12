#ifndef INTERFACE_MESSAGE_H
#define INTERFACE_MESSAGE_H

#include <gtk/gtk.h>

// Structure à utiliser pour passer les données à show_message_interface
typedef struct {
    char *username;
    GtkWidget *box;
} MessageInterfaceData;

// Fonction pour créer l'interface de messagerie

GtkWidget *create_interface_message(void);

#endif
