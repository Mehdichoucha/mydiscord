#include <gtk/gtk.h>

// Structure pour stocker les widgets à utiliser dans le callback
typedef struct {
    GtkTextView *text_view;
    GtkEditable *entry;
} AppWidgets;

// Callback quand le bouton est cliqué
static void on_button_clicked(GtkButton *button, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    const gchar *message = gtk_editable_get_text(GTK_EDITABLE(widgets->entry));
    if (g_strcmp0(message, "") == 0) return; // Ne rien faire si vide

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(widgets->text_view);
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);

    // Ajouter le message + saut de ligne
    gtk_text_buffer_insert(buffer, &end, message, -1);
    gtk_text_buffer_insert(buffer, &end, "\n", -1);

    // Vider la zone de saisie
    gtk_editable_set_text(GTK_EDITABLE(widgets->entry), "");

    // Faire défiler automatiquement
    GtkAdjustment *adj = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(widgets->text_view));
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj));
}

// Fonction d'initialisation de l'interface
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "My Discord");
    gtk_window_set_default_size(GTK_WINDOW(window), 1400, 900);

    GtkWidget *fixed = gtk_fixed_new();
    gtk_window_set_child(GTK_WINDOW(window), fixed);

    // Créer la zone de défilement
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scrolled_window, 810, 200);
    gtk_fixed_put(GTK_FIXED(fixed), scrolled_window, 300, 80);

    // Créer la vue de texte (messages)
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_view);

    // Image du bouton
    GtkWidget *image = gtk_image_new_from_file("C:/Users/linda/OneDrive/Bureau/discord/images/send.png");
    gtk_image_set_pixel_size(GTK_IMAGE(image), 37);

    // Créer le bouton avec l'image
    GtkWidget *button = gtk_button_new();
    gtk_button_set_child(GTK_BUTTON(button), image);

    // Style CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "button {"
        "  background: none;"
        "  border: none;"
        "  box-shadow: none;"
        "  padding: 0;"
        "  outline: none;"
        "}"
        "button image {"
        "  margin: 0;"
        "}", -1);
    GtkStyleContext *context = gtk_widget_get_style_context(button);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_widget_set_focusable(button, FALSE);
    gtk_widget_set_hexpand(button, FALSE);
    gtk_widget_set_vexpand(button, FALSE);
    gtk_fixed_put(GTK_FIXED(fixed), button, 1070, 600);
    gtk_widget_set_size_request(button, 40, 40);

    // Zone de texte
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Envoie un message");
    gtk_fixed_put(GTK_FIXED(fixed), entry, 300, 600);
    gtk_widget_set_size_request(entry, 750, 40);

    // Structure pour passer les widgets au callback
    AppWidgets *widgets = g_new(AppWidgets, 1);
    widgets->text_view = GTK_TEXT_VIEW(text_view);
    widgets->entry = GTK_EDITABLE(entry);

    // Connecter le bouton
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), widgets);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.mydiscord.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
