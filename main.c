#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "pages.h"  

// Fonction qui affiche la page principale
void show_main_page(GtkWidget *button, gpointer user_data) {
    GtkWidget *box = GTK_WIDGET(user_data);

    // Supprimer les widgets enfants existants
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(box)) != NULL) {
        gtk_widget_unparent(child);
    }

    // Titre
    GtkWidget *label = gtk_label_new("Welcome to Discord!");
    gtk_widget_add_css_class(label, "main-label");
    gtk_box_append(GTK_BOX(box), label);

    // Bouton de connexion
    GtkWidget *login_button = gtk_button_new_with_label("Login !");
    g_signal_connect(login_button, "clicked", G_CALLBACK(show_testformuulaire), box);  
    gtk_box_append(GTK_BOX(box), login_button);

    // Logo Discord
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale("icons8-discorde-480.png", 400, 400, TRUE, NULL);
    if (pixbuf != NULL) {
        GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
        gtk_box_append(GTK_BOX(box), image);
        g_object_unref(pixbuf);
    } else {
        g_warning("Impossible de charger l'image : icons8-discorde-480.png");
    }
}

// Fonction appelée à l'activation de l'application
static void on_activate(GtkApplication *app, gpointer user_data) {
    // Charger le fichier CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "style.css");

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    // Créer la fenêtre principale
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Discord Client");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    // Créer une boîte pour les widgets
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(window), box);

    // Afficher la page principale
    show_main_page(NULL, box);
    gtk_window_present(GTK_WINDOW(window));
}

// Point d’entrée principal
int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.mydiscord", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);  

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
