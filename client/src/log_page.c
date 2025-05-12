#include <gtk/gtk.h>
#include <pages.h>
#include <libpq-fe.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "connexion.h"



enum { LOGIN_EMAIL, LOGIN_PASSWORD, NUM_LOGIN_ENTRIES };

void show_main_page(GtkWidget *button, gpointer user_data);

static void on_log_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget **entries = (GtkWidget **)user_data;

    const gchar *email = gtk_editable_get_text(GTK_EDITABLE(entries[LOGIN_EMAIL]));
    const gchar *password = gtk_editable_get_text(GTK_EDITABLE(entries[LOGIN_PASSWORD]));

    if (g_strcmp0(email, "") == 0 || g_strcmp0(password, "") == 0) {
        GtkWidget *dialog = gtk_message_dialog_new_with_markup(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "All champs must be full !");
        gtk_window_set_title(GTK_WINDOW(dialog), "Error");
        gtk_window_present(GTK_WINDOW(dialog));
        return;
    }

    PGconn *conn = PQconnectdb("dbname=mydiscord user=postgres password= host=localhost port=5432");

    if (PQstatus(conn) != CONNECTION_OK) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Error to connect with database !");
        gtk_window_set_title(GTK_WINDOW(dialog), "Error");
        gtk_window_present(GTK_WINDOW(dialog));
        PQfinish(conn);
        return;
    }

    //   recup username and hash
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT username, password_hash FROM users WHERE email='%s'", email);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "User or password wrong!");
        gtk_window_set_title(GTK_WINDOW(dialog), "Error");
        gtk_window_present(GTK_WINDOW(dialog));
        PQclear(res);
        PQfinish(conn);
        return;
    }

    const char *username = PQgetvalue(res, 0, 0);
    const char *stored_hash = PQgetvalue(res, 0, 1);

    // split salt et hash
    char salt_hex[33], hash_hex[65];
    if (sscanf(stored_hash, "%32[^$]$%64s", salt_hex, hash_hex) != 2) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Invalid format for the password !");
        gtk_window_set_title(GTK_WINDOW(dialog), "Error");
        gtk_window_present(GTK_WINDOW(dialog));
        PQclear(res);
        PQfinish(conn);
        return;
    }

    //  convert salt
    unsigned char salt[16];
    for (int i = 0; i < 16; i++) {
        sscanf(&salt_hex[i * 2], "%2hhx", &salt[i]);
    }

    //  rehash password
    unsigned char hash[32];
    PKCS5_PBKDF2_HMAC(password, strlen(password),
                      salt, sizeof(salt),
                      10000, EVP_sha256(),
                      sizeof(hash), hash);

    //  convert  hash in hexa
    char hash_check[65];
    for (int i = 0; i < 32; i++) {
        sprintf(&hash_check[i * 2], "%02x", hash[i]);
    }
    hash_check[64] = '\0';

    //  compare
    if (strcmp(hash_check, hash_hex) != 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "User or password are wrong !");
        gtk_window_set_title(GTK_WINDOW(dialog), "Error");
        gtk_window_present(GTK_WINDOW(dialog));
    } else {
        //  Connect socket here
        if (!start_client_socket(username)) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Fail to connect with the server !");
            gtk_window_set_title(GTK_WINDOW(dialog), "Error");
            gtk_window_present(GTK_WINDOW(dialog));
            PQclear(res);
            PQfinish(conn);
            return;
        }

        GtkWidget *parent_box = gtk_widget_get_parent(entries[LOGIN_EMAIL]);

        GtkWidget *child;
        while ((child = gtk_widget_get_first_child(parent_box)) != NULL) {
            gtk_widget_unparent(child);
        }

        gchar *message = g_strdup_printf("Welcome, %s !", username);
        GtkWidget *label = gtk_label_new(message);
        g_free(message);
        gtk_box_append(GTK_BOX(parent_box), label);

        GtkWidget *msg_button = gtk_button_new_with_label("Message");
        gtk_box_append(GTK_BOX(parent_box), msg_button);
        g_signal_connect_swapped(msg_button, "clicked", G_CALLBACK(show_message_interface), parent_box);
    }

    PQclear(res);
    PQfinish(conn);
}
void show_log_form(GtkWidget *button, gpointer user_data) {
    GtkWidget *box = GTK_WIDGET(user_data);

    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(box)) != NULL) {
        gtk_widget_unparent(child);
    }

    GtkWidget *entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Email");
    gtk_box_append(GTK_BOX(box), entry_email);

    GtkWidget *entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_box_append(GTK_BOX(box), entry_password);

    GtkWidget **entries = g_new(GtkWidget *, NUM_LOGIN_ENTRIES);
    entries[LOGIN_EMAIL] = entry_email;
    entries[LOGIN_PASSWORD] = entry_password;

    GtkWidget *btn_log = gtk_button_new_with_label("Log in");
    g_signal_connect(btn_log, "clicked", G_CALLBACK(on_log_clicked), entries);
    gtk_box_append(GTK_BOX(box), btn_log);

    GtkWidget *btn_return = gtk_button_new_with_label("Return");
    g_signal_connect(btn_return, "clicked", G_CALLBACK(show_main_page), box);
    gtk_box_append(GTK_BOX(box), btn_return);
}
