#include "hud.h"

#include "assets.h"
#include "draw.h"
#include "math_utils.h"
#include "mkb/mkb.h"
#include "relutil.h"
#include "slider.h"

using namespace mathutils;

namespace custompack::hud {

namespace {

constexpr f32 DEPTH = 0.1;

// Texture indices in HUD TPL
enum class HudTex {
    Foo = 0,
    Bar = 1,
    Bas = 2,
};

u32 s_timer;

void draw_callback(void* ctx) {
    draw::TextureRequest req = {
        .texobj = assets::get_texobj((u32)HudTex::Foo),
        .pos = {500, 300},
        .size = {50, 50},
        .depth = DEPTH,
        .widescreen_x = 500,
        // Add some blue
        .add_color = {.r = 0, .g = 0, .b = 0x80, .a = 0},
    };
    draw::texture(&req);

    req = {
        .texobj = assets::get_texobj((u32)HudTex::Bar),
        .pos = {500, 350},
        .rot = deg_to_s16(s_timer),
        .size = {75, 75},
        .depth = DEPTH,
        .widescreen_x = 500,
        .pivot_uv = {1, 1},  // Rotate about the bottom right corner
        .mul_color = {.r = 0xff, .g = 0xff, .b = 0xff, .a = 0x80},  // 50% transparency

    };
    draw::texture(&req);
}

}  // namespace

void stobj_tick() {
    s_timer++;
}

void draw_2d() {
    if (mkb::main_mode != mkb::MD_GAME) return;

    draw::defer(DEPTH, (void*)nullptr, draw_callback);
}

}  // namespace custompack::hud
