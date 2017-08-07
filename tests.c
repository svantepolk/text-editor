#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "gapbuffer.h"

int main(void) {
    struct GapBuffer gb;
    gb_init(&gb, 10);
    assert(gb.bufferSize == 10 && gb.gapSize == 10);
    gb_insert(&gb, 'a');
    assert(gb_get(&gb, 0) == 'a');
    assert(gb.gapSize == gb.bufferSize - 1);
    gb_insert_chars(&gb, "bcd", 3);
    assert(gb_charc(&gb) == 4);
    assert(gb_get(&gb, 2) == 'c');
    gb_move_gap(&gb, 0);
    assert(gb_get(&gb, 2) == 'c');
    gb_insert(&gb, 'x');
    assert(gb_get(&gb, 0) == 'x');
    assert(gb_get(&gb, 3) == 'c');
}