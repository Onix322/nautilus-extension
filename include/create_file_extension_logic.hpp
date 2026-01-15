#pragma once
#include "glib.h"
#include <nautilus-extension.h>

extern "C" GList* get_background_items(NautilusMenuProvider *provider, NautilusFileInfo *info);
