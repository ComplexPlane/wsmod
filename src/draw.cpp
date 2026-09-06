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

void draw_sorted_sprites(mkb::BOOL32 some_condition) {
    mkb::SpriteListNode* node = mkb::depth_sorted_sprites[0].prev;
    for (; node->sprite != (mkb::Sprite*)0x0; node = node->prev) {
        mkb::Sprite* sprite = node->sprite;
        mkb::textdraw_reset();
        mkb::draw_sprite(sprite);
        for (; sprite->next_sprite != nullptr; sprite = sprite->next_sprite) {
            mkb::draw_sprite(sprite->next_sprite);
        }
    }
    if (mkb::main_mode == mkb::MD_MINI) {
        mkb::g_md_mini_sprite_disp();
    }
    mkb::g_draw_playpoint_or_gift_sprites();
}

void sort_sprites(mkb::BOOL32 some_condition) {
    mkb::uint is_view_stage;
    u8* sprite_status;
    int node_pos;
    int sprite_idx;
    mkb::SpriteListNode* g_sss;
    mkb::SpriteListNode* pSVar3;

    is_view_stage = mkb::events[mkb::EVENT_VIEW].status != 0;
    node_pos = 2;
    mkb::depth_sorted_sprites[0].sprite = (mkb::Sprite*)0x0;
    mkb::depth_sorted_sprites[0].next = (mkb::SpriteListNode*)0x0;
    mkb::depth_sorted_sprites[0].prev = mkb::depth_sorted_sprites + 1;
    mkb::depth_sorted_sprites[1].sprite = (mkb::Sprite*)0x0;
    mkb::depth_sorted_sprites[1].next = mkb::depth_sorted_sprites;
    mkb::depth_sorted_sprites[1].prev = (mkb::SpriteListNode*)0x0;
    sprite_idx = 0;
    sprite_status = mkb::sprite_pool_info.status_list;
    do {
        pSVar3 = mkb::depth_sorted_sprites[0].prev;
        if ((int)mkb::sprite_pool_info.upper_bound <= sprite_idx) {
            return;
        }
        if (*sprite_status != '\0') {
            if (!is_view_stage || (mkb::sprites[sprite_idx].unique_id == 100)) {
                if (some_condition == 0) {
                    if ((mkb::sprites[sprite_idx].g_flags1 & 0x40000) != 0) {
                    LAB_8024883c:
                        if (mkb::sprites[sprite_idx].prev_sprite == (mkb::Sprite*)0x0) {
                            while ((pSVar3->sprite != (mkb::Sprite*)0x0 &&
                                    (mkb::sprites[sprite_idx].depth <= pSVar3->sprite->depth))) {
                                pSVar3 = pSVar3->prev;
                            }
                            g_sss = mkb::depth_sorted_sprites + node_pos;
                            g_sss->sprite = mkb::sprites + sprite_idx;
                            mkb::depth_sorted_sprites[node_pos].next = pSVar3->next;
                            mkb::depth_sorted_sprites[node_pos].prev = pSVar3;
                            pSVar3->next->prev = g_sss;
                            pSVar3->next = g_sss;
                            node_pos = node_pos + 1;
                        }
                    }
                } else if ((mkb::sprites[sprite_idx].g_flags1 & 0x40000) == 0)
                    goto LAB_8024883c;
            }
        }
        sprite_idx = sprite_idx + 1;
        sprite_status = sprite_status + 1;
    } while (true);
}

void sort_and_draw_sprites(mkb::BOOL32 some_condition) {
    sort_sprites(some_condition);
    draw_sorted_sprites(some_condition);
}

TRAMP(s_draw_sprites_tramp, mkb::sort_and_draw_sprites, sort_and_draw_sprites);

}  // namespace

void init() {
    HOOK_TRAMP(s_draw_sprites_tramp);
}

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
