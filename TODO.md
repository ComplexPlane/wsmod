# TODO

## Flexible Sprite Drawing

Problem: existing sprite system has some limitations:

- Rotated sprites with off-center pivot round quad vertex positions to int, causing jittering. This
  seemingly isn't fixable without rewriting multiple functions
- Limited to 80 sprites (?)

A simpler, immediate mode-style sprite drawing API might be helpful here. However, it can't just
ignore the sprite system, because it must depth-sort among existing sprites.

### Option 1: Patch sprite system to avoid jittering

Add new Sprite flag that causes it to use a simpler render function which doesn't suffer from
jitter?

This _might_ work, but still require a decent amount of custom render code that replicates the
existing Sprite rendering code to some degree. It also doesn't remove any other Sprite limitations,
such as the annoyance of a retained-mode rendering API or the 80 sprite limit, which are annoying if
you want to use it in a higher level UI system.

### Option 2: Buffer custom sprite writes, interleave with Sprite depth sorting

This would require both buffering our custom sprites, as well as rewriting the depth-sort-then-draw
function for sprites, interleaving the two. This could work and uses an immediate mode API, but
requires more memory for storing texture draw parameters. It also doesn't generalize to depth
sorting text.

### Option 3: Buffer arbitary draw functions + depth, interleave with Sprite depth sorting

This allows arbitrary draw calls (textures, text, etc.) with minimal extra memory, but requires
using callback functions. API would be something like:

```cpp
constexpr f32 DEPTH = -0.25f;

void sprite_callback(void *ctx) {
    draw::texture(...);
    draw::text(...);
}

// ...
draw::sprite(DEPTH, nullptr, sprite_callback);
```

```cpp
constexpr f32 DEPTH = -0.25f;

void sprite_callback1(void *ctx) {
    draw::texture(...);
    draw::text(...);
}

// ...
draw::enqueue_sprite(DEPTH, nullptr, sprite_callback1);
```

Should the callback take a depth that it's required to pass down to draw calls? Or should it be
implicit in future draw calls?
