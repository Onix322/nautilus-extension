#include "CreateFileExtension.hpp"

extern "C" {

  // Object creation
  struct _CreateFileExtension {
    GObject parent_instance;
  };

  struct _CreateFileExtensionClass {
    GObjectClass parent_class;
  };  

// G_DEFINE_DYNAMIC_TYPE(_CreateFileExtension, create_file_extension, G_TYPE_OBJECT);

  // 2. Object init (constructor)
  void create_file_extension_init (_CreateFileExtension *self){
    g_print("_CreateFileExtension init");
  }

  // 2.1. class init (singleton)
  void create_file_extension_class_init (_CreateFileExtensionClass *klass){
    g_print("_CreateFileExtension created");
  }

  // 4. Distroy / Class finalize (mandatory for DYNAMIC_TYPE)
  void create_file_extension_class_finalize (_CreateFileExtensionClass *klass){
    // Delete / Clean class_data
    g_print("_CreateFileExtension loaded");
  }

  GTypeInfo* create_file_extension_get_info() {
    GTypeInfo* info = new GTypeInfo {
      sizeof (_CreateFileExtensionClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) create_file_extension_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof (_CreateFileExtension),
      0,              /* n_preallocs */
      (GInstanceInitFunc) create_file_extension_init,
      NULL            /* value_table */
    };

    return info;
  }
}
