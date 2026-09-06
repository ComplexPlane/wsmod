#include "custompack.h"
#include "custompack/assets.h"
#include "custompack/hud.h"
#include "custompack/things.h"
#include "load_stageconf.h"

namespace custompack {

void mainloop_init() {
}

void maingame_init() {
}

void mainloop_tick() {
    things::tick();
}

void draw_stage() {
    things::draw_stage();
}

void draw_view_stage() {
    things::draw_view_stage();
}

void stobj_init() {
    things::stobj_init();
}

void stobj_tick() {
    things::stobj_tick();
}

void preanim_tick() {
}

void stage_init() {
    stageconf::stage_init();
    things::stage_init();
}

void gameheap_init() {
    assets::gameheap_init();
}

void draw_2d() {
    hud::draw_2d();
}

}  // namespace custompack
