#include "gio/gio.h"
#include <cstddef>
#include <fstream>
#include <nautilus-extension.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h> 
#include <string.h>
extern "C" {

  //////////////////////////////////////////////////
  /// Create, init and distroy of your Extension instance (Mandatory)
  //////////////////////////////////////////////////
  /// 1. create
  struct _CreateFileExtension {
    GObject parent_instance;
  };
  struct _CreateFileExtensionClass {
    GObjectClass parent_class;
  };

  // 2. Object init (constructor)
  static void
    create_file_extension_init (_CreateFileExtension *self)
    {
      g_print("_CreateFileExtension init");
    }

  // 2.1. class init (singleton)
  static void
    create_file_extension_class_init (_CreateFileExtensionClass *klass)
    {
      g_print("_CreateFileExtension created");
    }

  // 4. Distroy / Class finalize (mandatory for DYNAMIC_TYPE)
  static void
    create_file_extension_class_finalize (_CreateFileExtensionClass *klass)
    {
      // Delete / Clean class_data
      g_print("_CreateFileExtension loaded");
    }

  //4. Macro for dym class - will create everything needed to create a dynamic class
  G_DEFINE_DYNAMIC_TYPE(_CreateFileExtension, create_file_extension, G_TYPE_OBJECT);

  ////////////////////////////////////////////////////////
  // Extension Logic (what it does)
  ///////////////////////////////////////////////////////
  // Logic can_button
  static void can_button_clicked(GtkButton* button, gpointer user_data) {
    // parameter "button" ignore, is only for jumping over the automatic insersion of GTK
    GtkWindow* window = GTK_WINDOW(user_data);
    gtk_window_destroy(window);
  }

  // Logic acc_button
  struct CurrentLocation {
    GtkWidget* entry;
    char* path;
  };

  static void acc_button_clicked(GtkButton* button, gpointer user_data) {
   
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
  static void dialog(GtkWindow* parent, char* path){

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

  static void on_click(NautilusMenuItem *item, gpointer user_data) {
    // 'item' is the button that was clicked
    // 'user_data' is whatever pointer you passed during registration (e.g., the folder path)    
    char* path = (char*) user_data;
    GtkWindow *window = gtk_application_get_active_window(GTK_APPLICATION(g_application_get_default()));
    dialog(window, path);
    // PSEUDO
    // 1. open dialog
    // 2. after writting 2 buttons cancel create
    // 3. with c++ create a file with that name
    
   } 

  static GList* get_background_items(NautilusMenuProvider *provider, NautilusFileInfo *info) {

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

  /////////////////////////////////////////////////////////////
  /// initialization of Interface you are going to work with
  /////////////////////////////////////////////////////////////
  //
  //
  // 1. implementing the interface
  // Mandatory for Nautilus Context menu
  static void menu_provider_iface_init(NautilusMenuProviderInterface *iface) {
    iface->get_background_items = get_background_items;
  }

  ///////////////////////////////////////////////////////////////
  // Mandatory for Nautilus to register the extension
  // Without those, nautilus is not going register the extension
  ///////////////////////////////////////////////////////////////
  //
  // 
  // 0. info about the type
  // -- in C everything needs to be described manually
  static const GTypeInfo create_file_extension_info = {
    sizeof (_CreateFileExtensionClass),                  // Class size
    NULL,                                                // base_init
    NULL,                                                // base_finalize
    (GClassInitFunc) create_file_extension_class_init,   // Class init func (VTable)
    NULL,                                                // class_finalize
    NULL,                                                // class_data
    sizeof (_CreateFileExtension),                       // Instance size (object)
    0,                                                   // n_preallocs
    (GInstanceInitFunc) create_file_extension_init,      // Init function of instance (constructor)
  };

  static GType type_id = 0;

  // 1. Registration of type
  void nautilus_module_initialize(GTypeModule *module) {

    type_id = g_type_module_register_type(
        module, 
        G_TYPE_OBJECT, 
        "CreateFileExtension",
        &create_file_extension_info,
        (GTypeFlags)0
        );

    static const GInterfaceInfo menu_info = {
      (GInterfaceInitFunc) menu_provider_iface_init, NULL, NULL
    };
    g_type_module_add_interface (module, type_id, NAUTILUS_TYPE_MENU_PROVIDER, &menu_info);

    g_print("libnautilus_ext: initialized\n");
    (void)module;
  }


  // 2. What the type does 
  void nautilus_module_list_types(const GType **types, int *num_types) {
    static GType type_list[1];
    type_list[0] = type_id; // type_id-ul salvat in initialize

    *types = type_list;
    *num_types = 1;

    g_print("libnautilus_ext: types listed\n");
    g_print("libnautilus_ext: loaded\n");
  }


  // 3. Delete / clean of type
  void nautilus_module_shutdown(void) {
    g_print("libnautilus_ext: shutdown\n");
  }

} // extern "C"

