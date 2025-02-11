#include "nux.h"

static u32 sprites[] = {
    0, 16, 16, 16, 32, 16, 48, 16, // BUTTON_A
    0, 0,  16, 0,  32, 0,  48, 0,  // BUTTON_X
    0, 32, 16, 32, 32, 32, 48, 32, // BUTTON_Y
    0, 48, 16, 48, 32, 48, 48, 48, // BUTTON_B
};

static u32 spritemap[] = {};

void
start (void)
{
    trace("Hello World !");
}

void
update (void)
{
    for (int j = 0; j < 4; ++j)
    {
        button_t b = BUTTON_A << j;
        int      i = 0;
        if (button(0) & b)
        {
            i = 3;
        }
        int offset = j * 8;
        cursor(16, j * 16);
        blit(0,
             sprites[offset + i * 2 + 0],
             sprites[offset + i * 2 + 1],
             16,
             16);
    }
}
