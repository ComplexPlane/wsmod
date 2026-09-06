#include "draw.h"

#include <cstdarg>
#include "logging.h"
#include "mkb/mkb.h"

#include "assembly.h"
#include "containers.h"
#include "math_utils.h"
#include "patch.h"
#include "relutil.h"

namespace draw {

namespace {

struct SpriteRequest {
    void* draw_func;
    void* context;
    f32 depth;
};

constexpr u32 MAX_SPRITES = 64;
SpriteRequest s_sprite_requests_buf[MAX_SPRITES];
cnt::Vector<SpriteRequest> s_sprite_requests{s_sprite_requests_buf, LEN(s_sprite_requests_buf)};

}  // namespace

void texture(TextureRequest* req) {
    ASSERT(req->texobj != nullptr);

    // Compute vertex position transform
    mkb::mtxa_from_translate_xyz(req->pos.x, req->pos.y, req->depth);
    mkb::mtxa_rotate_z(-req->rot);
    mkb::mtxa_scale_xyz(req->size.x, req->size.y, 1);
    mkb::mtxa_translate_xyz(-req->pivot_uv.x, -req->pivot_uv.y, 0);

    // Compute vertex positions
    Vec pos_top_left = {0, 0, 0};
    Vec pos_top_right = {1, 0, 0};
    Vec pos_bottom_right = {1, 1, 0};
    Vec pos_bottom_left = {0, 1, 0};
    mkb::mtxa_tf_point(&pos_top_left, &pos_top_left);
    mkb::mtxa_tf_point(&pos_top_right, &pos_top_right);
    mkb::mtxa_tf_point(&pos_bottom_right, &pos_bottom_right);
    mkb::mtxa_tf_point(&pos_bottom_left, &pos_bottom_left);

    // Compute vertex UVs
    Vec2d uv_top_left = req->min_uv;
    Vec2d uv_top_right = {req->max_uv.x, req->min_uv.y};
    Vec2d uv_bottom_right = req->max_uv;
    Vec2d uv_bottom_left = {req->min_uv.x, req->max_uv.y};

    // Set GPU texture/widescreen scale/color
    mkb::GXLoadTexObj_cached(req->texobj, mkb::GX_TEXMAP0);
    mkb::set_ui_widescreen_scale_mtx(req->widescreen_x);
    mkb::GXSetTevColor(mkb::GX_TEVREG0, req->mul_color);
    mkb::GXSetTevColor(mkb::GX_TEVREG1, req->add_color);

    // Send vertex data
    auto write_vertex = [](Vec* pos, Vec2d* uv) {
        mkb::GXPosition3f32(pos->x, pos->y, pos->z);
        mkb::GXTexCoord2f32(uv->x, uv->y);
    };
    mkb::GXBegin(mkb::GX_QUADS, mkb::GX_VTXFMT7, 4);
    write_vertex(&pos_top_left, &uv_top_left);
    write_vertex(&pos_top_right, &uv_top_right);
    write_vertex(&pos_bottom_right, &uv_bottom_right);
    write_vertex(&pos_bottom_left, &uv_bottom_left);

    mkb::reset_ui_widescreen_scale_mtx();
}

void tick() {
    s_sprite_requests.reset();
}

void enqueue_sprite_internal(f32 depth, void* context, void* draw_func) {
    s_sprite_requests.push(SpriteRequest{
        .draw_func = draw_func,
        .context = context,
        .depth = depth,
    });
}

}  // namespace draw
