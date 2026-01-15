////////////////////////////////////////////////////////
// Extension Logic (what it does)
///////////////////////////////////////////////////////
// Logic can_button
#include "glib-object.h"
#include "gtk/gtk.h"
#include <nautilus-extension.h>
#include <fstream>
void can_button_clicked(GtkButton* button, gpointer user_data) {
  // parameter "button" ignore, is only for jumping over the automatic insersion of GTK
  GtkWindow* window = GTK_WINDOW(user_data);
  gtk_window_destroy(window);
}

// Logic acc_button
struct CurrentLocation {
  GtkWidget* entry;
  char* path;
};

void acc_button_clicked(GtkButton* button, gpointer user_data) {

  CurrentLocation* data = static_cast<CurrentLocation*>(user_data);
  GtkEntryBuffer* buffer = gtk_entry_get_buffer(GTK_ENTRY(data->entry));
  const char* input_literal = gtk_entry_buffer_get_text(buffer);
  if(sizeof(input_literal) == 0) return;

  GError* error = NULL;
  char* path_literal = g_filename_from_uri(data->path, NULL, &error);
  char* full_path = g_build_filename(path_literal, input_literal, NULL);

  std::ofstream outfile(full_path);
  outfile.close(); 

  g_print("%s", full_path);
}

// Logic dialog
void dialog(GtkWindow* parent, char* path){

  // window
  GtkWidget* window = gtk_window_new();
  gtk_window_set_transient_for(GTK_WINDOW(window), parent);
  gtk_window_set_modal(GTK_WINDOW(window), true);
  gtk_window_set_resizable(GTK_WINDOW(window), false);

  // box
  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);

  // entry 
  GtkEntryBuffer* buffer = gtk_entry_buffer_new(NULL, -1);
  GtkWidget* entry = gtk_entry_new_with_buffer(buffer);

  // buttons
  GtkWidget* acc_button = gtk_button_new_with_label("Create");
  GtkWidget* can_button = gtk_button_new_with_label("Cancel");

  // BUTTONS WIRERING
  // Details: 
  // on_can_button_clicked gets 2 params 
  // 1st param button -> represent by first param in g_signal_connect
  // 2nd param window -> represented by the last param in g_signal_connect
  g_signal_connect(
      can_button,
      "clicked",
      G_CALLBACK(can_button_clicked),
      window
      );

  CurrentLocation* data = new CurrentLocation(entry, path);

  g_signal_connect(
      acc_button,
      "clicked",
      G_CALLBACK(acc_button_clicked),
      data
      );

  // appending
  gtk_box_append(GTK_BOX(box), entry);
  gtk_box_append(GTK_BOX(box), can_button);
  gtk_box_append(GTK_BOX(box), acc_button);
  gtk_window_set_child(GTK_WINDOW(window), box);


  // output
  gtk_window_present((GTK_WINDOW(window)));
}

void on_click(NautilusMenuItem *item, gpointer user_data) {
  // 'item' is the button that was clicked
  // 'user_data' is whatever pointer you passed during registration (e.g., the folder path)    
  char* path = (char*) user_data;
  GtkWindow *window = gtk_application_get_active_window(GTK_APPLICATION(g_application_get_default()));
  dialog(window, path);
} 

extern "C" GList* get_background_items(NautilusMenuProvider *provider, NautilusFileInfo *info) {

  // Create your item
  NautilusMenuItem *item = nautilus_menu_item_new(
      "CreateFileExtension::NewFileAction",
      "Create File",
      "Create a new file",
      "document-new"
      );
  char *uri = nautilus_file_info_get_uri(info);
  // activate item
  g_signal_connect_data(item, 
      "activate", 
      G_CALLBACK(on_click), 
      uri,             
      (GClosureNotify)g_free,
      (GConnectFlags)0);

  return g_list_append(NULL, item);
}

