#include "hud.h"

#include "assets.h"
#include "draw.h"
#include "logging.h"
#include "math_utils.h"
#include "mkb/mkb.h"
#include "slider.h"

using namespace mathutils;

namespace custompack::hud {

namespace {

constexpr f32 DEPTH = -0.25;

// Texture indices in HUD TPL
enum class HudTex {
    Foo = 0,
    Bar = 1,
    Bas = 2,
};

void draw_callback(void* ctx) {
    draw::TextureRequest req = {
        .texobj = assets::get_texobj((u32)HudTex::Foo),
        .pos = {slider::get("tex x", 500), slider::get("tex y", 300)},
        .size = {50, 50},
        .depth = DEPTH,
        .widescreen_x = 200,
        // Add some blue
        .add_color = {.r = 0, .g = 0, .b = 0x40, .a = 0},
    };
    draw::texture(&req);

    req = {
        .texobj = assets::get_texobj((u32)HudTex::Bar),
        .pos = {400, 200},
        .rot = deg_to_s16(90),
        .size = {125, 75},
        .depth = DEPTH,
        .widescreen_x = 125,
        .pivot_uv = {1, 1},
        .mul_color = {.a = 0x80},  // 50% transparent
    };
    draw::texture(&req);
}

}  // namespace

void draw_2d() {
    if (mkb::main_mode != mkb::MD_GAME) return;

    draw::sprite_sorted(DEPTH, (void*)nullptr, draw_callback);
}

}  // namespace custompack::hud
