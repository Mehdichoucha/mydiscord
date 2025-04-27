#include <gtk/gtk.h>
#include <pages.h>

enum { LOGIN_EMAIL, LOGIN_PASSWORD, NUM_LOGIN_ENTRIES };

void show_main_page(GtkWidget *button, gpointer user_data) ;



static void on_log_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget **entries = (GtkWidget **)user_data;

    const gchar *email = gtk_editable_get_text(GTK_EDITABLE(entries[LOGIN_EMAIL]));
    const gchar *password = gtk_editable_get_text(GTK_EDITABLE(entries[LOGIN_PASSWORD]));

    if (g_strcmp0(email, "") == 0 || g_strcmp0(password, "") == 0) {
        GtkWidget *dialog = gtk_message_dialog_new_with_markup(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Tous les champs doivent être remplis !");
        gtk_window_set_title(GTK_WINDOW(dialog), "Erreur");
        gtk_window_present(GTK_WINDOW(dialog));
        return;
    }

    g_print("Email: %s\n", email);
    g_print("Mot de passe: %s\n", password);

    GtkWidget *dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Connexion réussie !");
    gtk_window_set_title(GTK_WINDOW(dialog), "Succès");
    gtk_window_present(GTK_WINDOW(dialog));
}
static void on_window_close(GtkWindow *window) {
    gtk_window_destroy(window);
}

void show_log_form(GtkWidget *button, gpointer user_data) {
    GtkWidget *box = GTK_WIDGET(user_data);

    // Nettoyage du container
    GtkWidget *child;              
    while((child = gtk_widget_get_first_child(box)) !=NULL){
        gtk_widget_unparent(child);
    }

    
    GtkWidget *entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Email");
    gtk_box_append(GTK_BOX(box), entry_email);

    
    GtkWidget *entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Mot de passe");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_box_append(GTK_BOX(box), entry_password);

    
    GtkWidget **entries = g_new(GtkWidget *, NUM_LOGIN_ENTRIES);
    entries[LOGIN_EMAIL] = entry_email;
    entries[LOGIN_PASSWORD] = entry_password;

    
    GtkWidget *btn_login = gtk_button_new_with_label("Se connecter");
    g_signal_connect(btn_login, "clicked", G_CALLBACK(on_log_clicked), entries);
    gtk_box_append(GTK_BOX(box), btn_login);

    
    GtkWidget *btn_return = gtk_button_new_with_label("Retour");
    g_signal_connect(btn_return, "clicked", G_CALLBACK(show_main_page), box);
    gtk_box_append(GTK_BOX(box), btn_return);

    
    GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(button));
    gtk_window_present(window);
}
