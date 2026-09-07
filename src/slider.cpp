#include "slider.h"
#include "containers.h"
#include "draw.h"
#include "math_utils.h"
#include "mkb/mkb.h"
#include "pad.h"

namespace slider {

namespace {

constexpr bool SHOW_SLIDERS = true;
constexpr f32 DEPTH = 0;

struct Slider {
    const char *name;
    f32 value;
    f32 inc;
};

Slider s_slider_buf[16];
cnt::Vector<Slider> s_sliders{s_slider_buf, LEN(s_slider_buf)};
u32 s_selected_slider = 0;

void draw_callback(void *ctx) {
    if (SHOW_SLIDERS) {
        for (u32 i = 0; i < s_sliders.count(); i++) {
            Slider *slider = &s_sliders[i];
            const char *prefix = i == s_selected_slider ? "->" : "  ";

            mkb::textdraw_reset();
            mkb::textdraw_set_font(mkb::FONT_ASC_12x12);
            mkb::textdraw_set_pos(400, 10 + i * 15);
            mkb::textdraw_set_mul_color(RGBA(0, 0xff, 0, 0xff));
            mkb::textdraw_set_depth(DEPTH);
            mkb::textdraw_printf("%s %s = %.2f", prefix, slider->name, slider->value);
        }
    }
}

}  // namespace

f32 get(const char *name, f32 initial_value, f32 increment) {
    Slider *slider = nullptr;
    for (u32 i = 0; i < s_sliders.count(); i++) {
        if (mkb::strcmp((char *)s_sliders[i].name, (char *)name) == 0) {
            slider = &s_sliders[i];
        }
    }
    if (slider == nullptr) {
        slider = s_sliders.push_zeroed();
        slider->name = name;
        slider->value = initial_value;
        slider->inc = increment;
    }

    return slider->value;
}

void tick() {
    if (SHOW_SLIDERS) {
        if (s_sliders.count() == 0) {
            return;
        }
        if (pad::button_pressed(mkb::PAD_BUTTON_UP)) {
            s_selected_slider =
                mathutils::clamp((s32)s_selected_slider - 1, (s32)0, (s32)(s_sliders.count() - 1));
        }
        if (pad::button_pressed(mkb::PAD_BUTTON_DOWN)) {
            s_selected_slider =
                mathutils::clamp((s32)s_selected_slider + 1, (s32)0, (s32)(s_sliders.count() - 1));
        }
        if (pad::button_repeated(mkb::PAD_BUTTON_LEFT)) {
            Slider *slider = &s_sliders[s_selected_slider];
            slider->value -= slider->inc;
        }
        if (pad::button_repeated(mkb::PAD_BUTTON_RIGHT)) {
            Slider *slider = &s_sliders[s_selected_slider];
            slider->value += slider->inc;
        }
    }
}

void draw_2d() {
    draw::defer(DEPTH, (void *)nullptr, draw_callback);
}

}  // namespace slider
