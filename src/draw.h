#pragma once

#include "mkb/mkb.h"

namespace draw {

inline constexpr u32 SCREEN_WIDTH = 640;
inline constexpr u32 SCREEN_HEIGHT = 480;

inline constexpr GXColor WHITE = {0xff, 0xff, 0xff, 0xff};
inline constexpr GXColor BLACK = {0x00, 0x00, 0x00, 0xff};
inline constexpr GXColor RED = {0xfd, 0x68, 0x75, 0xff};
inline constexpr GXColor ORANGE = {0xfd, 0xac, 0x68, 0xff};
inline constexpr GXColor BLUE = {0x9d, 0xe3, 0xff, 0xff};
inline constexpr GXColor PINK = {0xdf, 0x7f, 0xfa, 0xff};
inline constexpr GXColor PURPLE = {0xb1, 0x5a, 0xff, 0xff};
inline constexpr GXColor GREEN = {0x00, 0xff, 0x00, 0xff};

// Parameters for directly drawing a textured quad to the screen.
//
// Coordinates are in f32 screen space pixels:
// X: [0.f, 640.f] (left to right)
// Y: [0.f, 480.f] (top to bottom)
// The coordinate system applies in both 4:3 and 16:9, though in 16:9 textures are "unstretched"
// about `widescreen_x`.
struct TextureRequest {
    // Texture to draw, obtainable from e.g. `mkb::TplBuffer.texobjs`
    mkb::GXTexObj *texobj;
    // Position in screen coordinates (pixels). Rotation is applied in screen space about this
    // point
    Vec2d pos;
    // Rotation (about `pos` in screen space)
    s16 rot;
    // Width and height in screen space pixels
    Vec2d size;
    // Z position, used for stack order.
    // Positive depth is away from camera.
    f32 depth = 0.025;
    // Screen space X position about which to unstretch texture in widescreen.
    // Another way to think about it: whichever part of your texture is at this X position will not
    // not move
    f32 widescreen_x = (f32)SCREEN_WIDTH / 2.f;

    // Pivot point UV coordinate. This point in the texture is positioned at `pos`
    Vec2d pivot_uv = {0.5f, 0.5f};
    // If you want to draw a subregion of a texture, you can override the texture UV coordinate
    // range
    Vec2d min_uv = {0.f, 0.f};
    Vec2d max_uv = {1.f, 1.f};

    // Multiplied to each pixel's color. Specify alpha here as well
    GXColor mul_color = {0xff, 0xff, 0xff, 0xff};
    // Added to each pixel's color
    GXColor add_color = {0x00, 0x00, 0x00, 0x00};
};

void init();
void tick();

/*
 * Functions which draw immediately
 */

void texture(TextureRequest *req);

/*
 * Functions which draw later
 */

void enqueue_draw_request_internal(f32 depth, void *context, void *draw_func);

// Requests arbitrary 2D rendering to occur at the specified depth. It is depth-sorted alongside the
// game's sprites. Positive depth is away from the viewer. Depth should be >0 to draw under the
// pause menu.
template <typename T>
void defer(f32 depth, T *context, void (*draw_func)(T *context)) {
    enqueue_draw_request_internal(depth, (void *)context, (void *)draw_func);
}

}  // namespace draw
