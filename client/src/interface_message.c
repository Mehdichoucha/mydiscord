#include <gtk/gtk.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "connexion.h"
#include "salon.h"
#include "global.h"

typedef struct {
    GtkTextView *text_view;
    GtkEditable *entry;
} AppWidgets;

static AppWidgets *global_widgets = NULL;
static SalonType salon_actuel = SALON_GENERAL;

// Show all messages from active room
void rafraichir_messages() {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_widgets->text_view));
    gtk_text_buffer_set_text(buffer, "", -1);

    int count = get_number_messages(salon_actuel);
    for (int i = 0; i < count; i++) {
        const char *msg = get_message(salon_actuel, i);
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(buffer, &iter);
        gtk_text_buffer_insert(buffer, &iter, msg, -1);
        gtk_text_buffer_insert(buffer, &iter, "\n", -1);
    }

    GtkAdjustment *adj = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(global_widgets->text_view));
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj));
}

// Show the message in the room
gboolean afficher_message_dans_textview(gpointer data) {
    const char *message = (const char *)data;
    char *sep = strchr(message, '|');

    if (sep) {
        *sep = '\0';
        const char *username = message;
        const char *content = sep + 1;

        char formatted[1024];
        snprintf(formatted, sizeof(formatted), "[%s] %s", username, content);
        add_message(salon_actuel, formatted);

        if (global_widgets != NULL) {
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_widgets->text_view));
            GtkTextIter iter;
            gtk_text_buffer_get_end_iter(buffer, &iter);
            gtk_text_buffer_insert(buffer, &iter, formatted, -1);
            gtk_text_buffer_insert(buffer, &iter, "\n", -1);

            GtkAdjustment *adj = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(global_widgets->text_view));
            gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj));
        }
    } else {

        add_message(salon_actuel, message);
    }

    free(data);
    return FALSE;
}


// Thread for receive messages 
void *receive_thread_func(void *arg) {
    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = receive_message(buffer, sizeof(buffer));
        if (bytes_received <= 0) break;

        buffer[bytes_received] = '\0';  

        printf("Reçu du serveur : %s\n", buffer);  

        char *formatted_message = strdup(buffer); 
        g_idle_add((GSourceFunc)afficher_message_dans_textview, formatted_message);
    }
    return NULL;
}


// Send a message from user entry
void on_button_clicked(GtkButton *button, gpointer user_data) {
    const char *message = gtk_editable_get_text(GTK_EDITABLE(global_widgets->entry));

    if (message && strlen(message) > 0) {
        char message_complet[1024];
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        
        snprintf(message_complet, sizeof(message_complet), "[%02d:%02d:%02d] %s : %s", tm.tm_hour, tm.tm_min, tm.tm_sec, current_username, message);

        
        send_message(message_complet);
        add_message(salon_actuel, message_complet);

        gtk_editable_set_text(GTK_EDITABLE(global_widgets->entry), "");
        rafraichir_messages();
    }
}

// Change the room 
static void on_left_button_clicked(GtkButton *button, gpointer user_data) {
    const char *label = gtk_button_get_label(button);

    if (g_strcmp0(label, "Room 1") == 0)
        salon_actuel = SALON_GENERAL;
    else if (g_strcmp0(label, "Room 2") == 0)
        salon_actuel = SALON_DEV;
    else
        salon_actuel = SALON_RANDOM;

    rafraichir_messages();
}

// Creation of the main user interface
GtkWidget* create_interface_message(void) {
    GtkWidget *fixed = gtk_fixed_new();


    GtkWidget *room1 = gtk_button_new_with_label("Room 1");
    gtk_fixed_put(GTK_FIXED(fixed), room1, 30, 40);
    gtk_widget_set_size_request(room1, 150, 40);
    g_signal_connect(room1, "clicked", G_CALLBACK(on_left_button_clicked), NULL);

    GtkWidget *room2 = gtk_button_new_with_label("Room 2");
    gtk_fixed_put(GTK_FIXED(fixed), room2, 30, 90);
    gtk_widget_set_size_request(room2, 150, 40);
    g_signal_connect(room2, "clicked", G_CALLBACK(on_left_button_clicked), NULL);

    GtkWidget *room3 = gtk_button_new_with_label("Room 3");
    gtk_fixed_put(GTK_FIXED(fixed), room3, 30, 140);
    gtk_widget_set_size_request(room3, 150, 40);
    g_signal_connect(room3, "clicked", G_CALLBACK(on_left_button_clicked), NULL);

    // text with scroll
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scrolled_window, 810, 200);
    gtk_fixed_put(GTK_FIXED(fixed), scrolled_window, 300, 80);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_view);

   
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Send a message");
    gtk_widget_set_size_request(entry, 750, 40);
    gtk_fixed_put(GTK_FIXED(fixed), entry, 300, 600);

    // Bouton send
    GtkWidget *image = gtk_image_new_from_file("send.png");
    gtk_image_set_pixel_size(GTK_IMAGE(image), 37);
    GtkWidget *button = gtk_button_new();
    gtk_button_set_child(GTK_BUTTON(button), image);
    gtk_widget_set_size_request(button, 40, 40);
    gtk_fixed_put(GTK_FIXED(fixed), button, 1070, 600);

    // Recuperation widgets
    AppWidgets *widgets = g_new(AppWidgets, 1);
    widgets->text_view = GTK_TEXT_VIEW(text_view);
    widgets->entry = GTK_EDITABLE(entry);
    global_widgets = widgets;

    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), widgets);

    init_salons();
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receive_thread_func, NULL);

    return fixed;
}
