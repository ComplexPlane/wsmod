#include "custompack/assets.h"

#include "logging.h"
#include "patch.h"

#include "mkb/mkb.h"

namespace custompack::assets {

namespace {

// Load and initialize a TPL for direct use (aka without a model)
mkb::TplBuffer* load_standalone_tpl(const char* path) {
    mkb::TplBuffer* tpl = mkb::g_load_tpl((char*)path);
    ASSERT(tpl != nullptr);

    tpl->texobjs = (mkb::GXTexObj*)mkb::OSAllocFromHeap(mkb::main_heap,
                                                        sizeof(mkb::GXTexObj) * tpl->texture_count);
    ASSERT(tpl->texobjs != nullptr);

    // Initialize texobjs
    for (u32 i = 0; i < (u32)tpl->texture_count; i++) {
        mkb::TplTextureHeader* tex = &tpl->texture_headers[i];
        if (tex->width != 0 && tex->height != 0) {
            mkb::GXTexObj* texobj = &tpl->texobjs[i];
            void* data = (void*)((u32)tpl + (u32)tex->data_offset);
            mkb::GXInitTexObj(texobj, data, tex->width, tex->height, tex->format & 0x1F,
                              mkb::GX_CLAMP, mkb::GX_CLAMP, mkb::GX_FALSE);
        }
    }

    return tpl;
}

}  // namespace

mkb::GmaBuffer* s_gma;
mkb::TplBuffer* s_tpl;

void gameheap_init() {
    // NOTE: replace with your custom model paths, such as "/init/custom_models.gma"
    mkb::TplBuffer* tpl = mkb::g_load_tpl("/init/common.tpl");
    ASSERT(tpl != nullptr);
    s_gma = mkb::g_load_gma("/init/common.gma", tpl);
    ASSERT(s_gma != nullptr);

    // NOTE: replace with your custom TPL filepath, such as "/init/custom_hud.tpl"
    s_tpl = load_standalone_tpl("/init/common.tpl");
}

mkb::GmaModel* get_model(const char* name) {
    ASSERT(s_gma != nullptr);
    if (s_gma == nullptr) return nullptr;
    for (int i = 0; i < s_gma->model_count; i++) {
        const char* curr_name = s_gma->model_entries[i].name;
        if (mkb::strcmp(const_cast<char*>(curr_name), const_cast<char*>(name)) == 0) {
            return s_gma->model_entries[i].model;
        }
    }
    ABORT_MSG("Model '%s' not found", name);
}

mkb::GXTexObj* get_texobj(u32 idx) {
    ASSERT(s_tpl != nullptr);
    ASSERT(s_tpl->texobjs != nullptr);
    ASSERT((s32)idx < s_tpl->texture_count);
    return &s_tpl->texobjs[idx];
}

}  // namespace custompack::assets
