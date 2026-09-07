#pragma once

#include "mkb/mkb.h"

namespace custompack::assets {

// Currently we assume there's only one custom models GMA, and one custom textures TPL.
// If this becomes undesirable, assets could instead only provide utility functions for
// loading/accessing assets, and the resources themselves could be stored in separate modules.

// Load all custom assets on gameheap reset
void gameheap_init();

// Get a model by name from the custom GMA
mkb::GmaModel *get_model(const char *model_name);

// Get a texture by index from the custom TPL
mkb::GXTexObj *get_texobj(u32 idx);

}  // namespace custompack::assets
