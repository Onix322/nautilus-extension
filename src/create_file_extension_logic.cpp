////////////////////////////////////////////////////////
// Extension Logic (what it does)
///////////////////////////////////////////////////////
// Logic can_button
#include "glib-object.h"
#include "gtk/gtk.h"
#include "gtk/gtkshortcut.h"
#include <iostream>
#include <nautilus-extension.h>
#include <fstream>
#include <ranges>


// Logic acc_button
struct CurrentLocation {
  GtkWidget* entry;
  char* path;
};

struct DialogHandler {
  private:
    GtkWindow* dialog;

    bool is_set(){
      if(dialog == nullptr){
        std::cerr << "Dialog was not set!";
        return false;
      }
      return true;
    }

  public:
    bool open(){
      gtk_window_present(dialog);
      return is_active();
    };
    bool close(){
      gtk_window_destroy(dialog);
      return is_active();
    };
    bool is_active(){

      return gtk_window_is_active(dialog);
    };
    void set_dialog(GtkWindow* new_dialog_window){
      dialog = new_dialog_window;
    }
};

DialogHandler* dialog_handler = new DialogHandler();
// UTILS
std::string str_trim(std::string_view s) {
    auto is_space = [](unsigned char c) { return std::isspace(c); };
    
    // Elimină de la început și de la sfârșit folosind "views"
    auto trimmed = s 
        | std::views::drop_while(is_space) 
        | std::views::reverse 
        | std::views::drop_while(is_space) 
        | std::views::reverse;

    return {trimmed.begin(), trimmed.end()};
}

// BUTTONS Logic
void can_button_clicked(GtkButton* button, gpointer user_data) {
  // parameter "button" ignore, is only for jumping over the automatic insersion of GTK
  GtkWindow* window = GTK_WINDOW(user_data);
  dialog_handler->close();
}

void acc_button_clicked(GtkButton* button, gpointer user_data) {

  CurrentLocation* data = static_cast<CurrentLocation*>(user_data);
  GtkEntryBuffer* buffer = gtk_entry_get_buffer(GTK_ENTRY(data->entry));
  const char* input_literal = gtk_entry_buffer_get_text(buffer);

  if(sizeof(input_literal) == 0){
    delete input_literal;
    return;
  }


  GError* error = NULL;
  char* path_literal = g_filename_from_uri(data->path, NULL, &error);
  char* full_path = g_build_filename(path_literal, input_literal, NULL);

  std::ofstream outfile(full_path);
  outfile.open(full_path);

  if(outfile.is_open()){
    dialog_handler->close();
  } 

  outfile.close();  
}

// FACTORY 
GtkWindow* dialog_factory(GtkWindow* parent, char* path){

  // window
  GtkWidget* window = gtk_window_new();
  gtk_window_set_transient_for(GTK_WINDOW(window), parent);
  gtk_window_set_modal(GTK_WINDOW(window), true);
  gtk_window_set_resizable(GTK_WINDOW(window), false);
  gtk_window_set_title((GTK_WINDOW(window)), "New File");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
  //titlebar
  GtkWidget* titlebar = gtk_header_bar_new();
  gtk_header_bar_set_use_native_controls(GTK_HEADER_BAR(titlebar), true);
  // box
  GtkWidget* box_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  GtkWidget* box_entry = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  GtkWidget* box_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
  
  gtk_box_set_homogeneous(GTK_BOX(box_main), true);
  gtk_widget_set_margin_start(box_main, 15);
  gtk_widget_set_margin_end(box_main, 15);
  gtk_widget_set_margin_top(box_main, 15);
  gtk_widget_set_margin_bottom(box_main, 15);

  gtk_box_set_homogeneous(GTK_BOX(box_entry), true);
  gtk_box_set_homogeneous(GTK_BOX(box_buttons), true);
  gtk_box_set_spacing(GTK_BOX(box_main), 20);


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
  gtk_window_set_titlebar(GTK_WINDOW(window), titlebar);
  gtk_box_append(GTK_BOX(box_entry), entry);
  gtk_box_append(GTK_BOX(box_buttons), can_button);
  gtk_box_append(GTK_BOX(box_buttons), acc_button);
  gtk_window_set_default_widget(GTK_WINDOW(window), acc_button);
  gtk_box_append(GTK_BOX(box_main), box_entry);
  gtk_box_append(GTK_BOX(box_main), box_buttons);
  gtk_window_set_child(GTK_WINDOW(window), box_main);

  return GTK_WINDOW(window);
}

// What happens when you press the option
void on_click(NautilusMenuItem *item, gpointer user_data) {
  // 'item' is the button that was clicked
  // 'user_data' is whatever pointer you passed during registration (e.g., the folder path)    
  char* path = (char*) user_data;
  GtkWindow *parent_window = gtk_application_get_active_window(GTK_APPLICATION(g_application_get_default()));

  dialog_handler->set_dialog(dialog_factory(parent_window, path));
  dialog_handler->open();
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

