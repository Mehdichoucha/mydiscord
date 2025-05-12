#include <gtk/gtk.h>
#include "pages.h"
#include <libpq-fe.h>
#include <windows.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "connexion.h"
#include "interface_message.h"


enum { ENTRY_USERNAME, ENTRY_EMAIL, ENTRY_PASSWORD, NUM_ENTRIES };

void show_main_page(GtkWidget *button, gpointer user_data);
void show_log_form(GtkWidget *button, gpointer user_data);
void show_message_interface(GtkWidget *button, gpointer user_data) {
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(button)) != NULL) {
        gtk_widget_unparent(child);
    }

    GtkWidget *interface_msg = create_interface_message();
    gtk_box_append(GTK_BOX(button), interface_msg);
}
void on_message_button_clicked(GtkButton *button, gpointer user_data) {
    const char *username = (const char *)user_data;
    g_free((gpointer)user_data); 
}




static void on_register_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget **entries = (GtkWidget **)user_data;

    const gchar *username_tmp = gtk_editable_get_text(GTK_EDITABLE(entries[ENTRY_USERNAME]));
    const gchar *email = gtk_editable_get_text(GTK_EDITABLE(entries[ENTRY_EMAIL]));
    const gchar *password = gtk_editable_get_text(GTK_EDITABLE(entries[ENTRY_PASSWORD]));

    if (strlen(username_tmp) == 0 || strlen(email) == 0 || strlen(password) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new_with_markup(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "All champs must be full !");
        gtk_window_set_title(GTK_WINDOW(dialog), "Errof");
        gtk_window_present(GTK_WINDOW(dialog));
        return;
    }

    gchar *username = g_strdup(username_tmp);  

    PGconn *conn = PQconnectdb("dbname=mydiscord user=postgres password= host=localhost port=5432");

    if (PQstatus(conn) != CONNECTION_OK) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Error to connect with the database !");
        gtk_window_set_title(GTK_WINDOW(dialog), "Error");
        gtk_window_present(GTK_WINDOW(dialog));
        g_free(username);
        PQfinish(conn);
        return;
    }

    // Hash password with PBKDF2-HMAC-SHA256
    unsigned char salt[16];
    RAND_bytes(salt, sizeof(salt));

    unsigned char hash[32];
    PKCS5_PBKDF2_HMAC(password, strlen(password),
                      salt, sizeof(salt),
                      10000, EVP_sha256(),
                      sizeof(hash), hash);

    char hashed_password[16 * 2 + 1 + 32 * 2 + 1];
    for (int i = 0; i < sizeof(salt); i++) {
        sprintf(&hashed_password[i * 2], "%02x", salt[i]);
    }
    hashed_password[sizeof(salt) * 2] = '$';
    for (int i = 0; i < sizeof(hash); i++) {
        sprintf(&hashed_password[sizeof(salt) * 2 + 1 + i * 2], "%02x", hash[i]);
    }
    hashed_password[sizeof(hashed_password) - 1] = '\0';

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO users (username, email, password_hash) VALUES ('%s', '%s', '%s')",
             username, email, hashed_password);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Error with the reghistration : %s", PQerrorMessage(conn));
        gtk_window_set_title(GTK_WINDOW(dialog), "Error");
        gtk_window_present(GTK_WINDOW(dialog));
    } else {
        GtkWidget *parent_box = gtk_widget_get_parent(entries[ENTRY_USERNAME]);

        GtkWidget *child;
        while ((child = gtk_widget_get_first_child(parent_box)) != NULL) {
            gtk_widget_unparent(child);
        }

        // Connexion to serveur socket here
        if (!start_client_socket(username)) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Connexion with the server fail !");
            gtk_window_set_title(GTK_WINDOW(dialog), "Error");
            gtk_window_present(GTK_WINDOW(dialog));
            g_free(username);
            PQclear(res);
            PQfinish(conn);
            return;
        }

        gchar *message = g_strdup_printf("Welcome, %s !", username);
        GtkWidget *label = gtk_label_new(message);
        g_free(message);
        gtk_box_append(GTK_BOX(parent_box), label);

        GtkWidget *msg_button = gtk_button_new_with_label("Message");
        gtk_box_append(GTK_BOX(parent_box), msg_button);

        g_signal_connect(msg_button, "clicked", G_CALLBACK(on_message_button_clicked), g_strdup(username));
    }

    g_free(username);
    PQclear(res);
    PQfinish(conn);
}


void show_testformuulaire(GtkWidget *button, gpointer user_data) {
    GtkWidget *box = GTK_WIDGET(user_data);

    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(box)) != NULL) {
        gtk_widget_unparent(child);
    }

    GtkWidget *entry_username = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "User");
    gtk_box_append(GTK_BOX(box), entry_username);

    GtkWidget *entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Email");
    gtk_box_append(GTK_BOX(box), entry_email);

    GtkWidget *entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_box_append(GTK_BOX(box), entry_password);

    GtkWidget *btn_return = gtk_button_new_with_label("Return");
    g_signal_connect(btn_return, "clicked", G_CALLBACK(show_main_page), box);
    gtk_box_append(GTK_BOX(box), btn_return);

    GtkWidget **entries = g_new(GtkWidget *, NUM_ENTRIES);
    entries[ENTRY_USERNAME] = entry_username;
    entries[ENTRY_EMAIL] = entry_email;
    entries[ENTRY_PASSWORD] = entry_password;

    GtkWidget *btn_register = gtk_button_new_with_label("Register");
    g_signal_connect(btn_register, "clicked", G_CALLBACK(on_register_clicked), entries);
    gtk_box_append(GTK_BOX(box), btn_register);

    GtkWidget *btn_log = gtk_button_new_with_label("Log in");
    g_signal_connect(btn_log, "clicked", G_CALLBACK(show_log_form), box);
    gtk_box_append(GTK_BOX(box), btn_log);

    GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(button));
    gtk_window_present(window);
}