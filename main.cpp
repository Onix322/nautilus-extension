///////////////////////////////////////////////////////////////
// Mandatory for Nautilus:
// 1. register the custom extension
// 2. implement the interface you are working with
// Without those, nautilus is not going register the extension
///////////////////////////////////////////////////////////////
#include <cstddef>
#include <create_file_extension_logic.hpp>
#include <nautilus-extension.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h> 
#include <string.h>
#include <create_file_extension_logic.hpp>
#include <CreateFileExtension.hpp>
extern "C" {
  /////////////////////////////////////////////////////////////
  // Initialization of Interface you are going to work with
  /////////////////////////////////////////////////////////////

  // 1. implementing the interface
  // in this case the backgound context menu (right click on nautilus's backgound)
  static void menu_provider_iface_init(NautilusMenuProviderInterface *iface) {
    iface->get_background_items = get_background_items;
  }

  static GType type_id = 0;

  /////////////////////////////////////////////////////////////
  // Registration of your extension 
  /////////////////////////////////////////////////////////////

  void nautilus_module_initialize(GTypeModule *module) {

    type_id = g_type_module_register_type(
        module, 
        G_TYPE_OBJECT, 
        "CreateFileExtension",
        &*create_file_extension_get_info(),
        (GTypeFlags)0
        );

    static const GInterfaceInfo menu_info = {
      (GInterfaceInitFunc) menu_provider_iface_init, NULL, NULL
    };
    g_type_module_add_interface (module, type_id, NAUTILUS_TYPE_MENU_PROVIDER, &menu_info);

    g_print("libnautilus_ext: initialized\n");
    (void)module;
  }

  void nautilus_module_list_types(const GType **types, int *num_types) {
    static GType type_list[1];
    type_list[0] = type_id;

    *types = type_list;
    *num_types = 1;

    g_print("libnautilus_ext: types listed\n");
    g_print("libnautilus_ext: loaded\n");
  }

  void nautilus_module_shutdown(void) {
    g_print("libnautilus_ext: shutdown\n");
  }

}

