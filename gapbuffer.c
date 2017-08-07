/*
 * reference: https://www.finseth.com/craft/
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "gapbuffer.h"

/*
 * Initialize a GapBuffer with a gap encompassing the whole buffer
 */
void gb_init(struct GapBuffer* gb, int bufferSize) {
    gb->gapStart = 0;
    gb->buffer = malloc(sizeof(char) * bufferSize);
    gb->bufferSize = bufferSize;
    gb->gapSize = bufferSize;
}

/*
 * Initialize a GapBuffer with text. The gap will start at index 0.
 */
void gb_init_with_text(struct GapBuffer* gb, int gapSize, char* text, int textLength) {
    int bufferSize = textLength + gapSize;

    gb->buffer = malloc(bufferSize * sizeof(char));
    gb->bufferSize = bufferSize;
    
    memcpy(gb->buffer + gapSize, text, textLength);
    
    gb->gapStart = 0;
    gb->gapSize = gapSize;
}

/*
 * Print the text parts of a GapBuffer to a FILE*.
 */
void gb_fprint(struct GapBuffer* gb, FILE* stream) {
    int charc = gb_charc(gb);
    // print the part before the gap
    fprintf(stream, "%*s", gb->gapStart, gb->buffer); 
    charc -= gb->gapStart;
    // print the part after the gap
    char* afterGap = gb->buffer + gb->gapStart + gb->gapSize - 1;
    fprintf(stream, "%*s", charc, afterGap);
}

void gb_move_gap(struct GapBuffer* gb, int index) {
    if (index == gb->gapStart) {
        return;
    }
    
    if (index > gb->gapStart) {
        while (gb->gapStart < index) {
            int gapStart = gb->gapStart;
            gb->buffer[gapStart] = gb->buffer[gapStart+gb->gapSize];
            gb->gapStart++;
        }
    } else if (index < gb->gapStart) {
        while (gb->gapStart > index) {
            int gapStart = gb->gapStart;
            gb->buffer[gapStart+gb->gapSize-1] = gb->buffer[gapStart-1];
            gb->gapStart--;
        }
    }
}

/*
 * Insert a single character.
 */
void gb_insert(struct GapBuffer* gb, char c) {
    // make more space if needed
    if (gb->gapSize == 0) {
        gb_grow_gap(gb, 1 + gb->bufferSize * 1.6);
    }

    gb->buffer[gb->gapStart] = c;
    gb->gapStart++;
    gb->gapSize--;
}

/*
 * Insert a string of characters.
 */
void gb_insert_chars(struct GapBuffer* gb, char* chars, int charc) {
    // make more space if needed
    if (gb->gapSize < charc) {
        gb_grow_gap(gb, charc - gb->gapSize + gb->bufferSize * 1.6);
    }

    char* gapStart = gb->buffer + gb->gapStart;
    // copy the chars-to-insert to the start of the gap
    memcpy(gapStart, chars, charc * sizeof(char));
    gb->gapStart += charc;
    gb->gapSize -= charc;
}

void gb_delete_forward(struct GapBuffer* gb, int charc) {
    int newGapSize = gb->gapSize + charc;
    if (gb->gapStart + newGapSize <= gb->bufferSize)
    {
        gb->gapSize = newGapSize;
    } else {
        gb->gapSize = gb->bufferSize - gb->gapStart;
    }
}

void gb_delete_backward(struct GapBuffer* gb, int charc) {
    if (gb->gapStart - charc >= 0) {
        gb->gapStart -= charc;
        gb->gapSize += charc;
    } else {
        gb->gapSize += gb->gapStart;
        gb->gapStart = 0;
    }
}

/*
 * Returns the number of chars in the buffer.
 */
int gb_charc(struct GapBuffer* gb) {
    return gb->bufferSize - gb->gapSize;
}

char gb_get(struct GapBuffer* gb, int index) {
    // ensure index refers to a valid char
    assert(index < gb->bufferSize - gb->gapSize);
    
    if (index < gb->gapStart) {
        return gb->buffer[index];
    } else {
        return gb->buffer[index + gb->gapSize]; 
    }
}

int gb_line_index(struct GapBuffer* gb, int lineNumber) {
    // keep the last lookup since most are going to be nearby
    static int indexCache = 0;
    static int lineCache = 0;

    if (lineCache == lineNumber) {
        return indexCache;
    }

    int charc = gb_charc(gb);

    while (lineCache < lineNumber) {
        while (gb_get(gb, indexCache) != '\n' && indexCache < charc) {
            indexCache++;
        }

        if (gb_get(gb, indexCache) == '\n') {
            indexCache++;
            lineCache++;
        } else {
            // tried to get an out-of-bounds line
            indexCache = 0;
            lineCache = 0;
            return -1;
        }
    }
    if (lineCache > lineNumber) {
        indexCache -= 2; // when going backwards, need to skip over the newline char
        if (indexCache < 0) {
            indexCache = 0;
            lineCache = 0;
            return 0;
        }
        while (lineCache > lineNumber) {
            while (gb_get(gb, indexCache) != '\n' && indexCache > 0) {
                indexCache--;
            }

            if (gb_get(gb, indexCache) == '\n') {
                indexCache++;
                lineCache--;
            } else {
                // beginning of buffer
                lineCache--;
            }
        }
    }

    return indexCache;
}

/*
 * Returns the length of a given line, not including the newline character
 */
int gb_line_length(struct GapBuffer* gb, int lineNumber) {
    int lineIndex = gb_line_index(gb, lineNumber);
    int index = lineIndex;
    int charc = gb_charc(gb);
    while (gb_get(gb, index) != '\n' && index < charc) {
        index++;
    }
    
    return index - lineIndex;
}

/*
 * Increases the gap and underlying buffer by n bytes.
 */
void gb_grow_gap(struct GapBuffer* gb, int n) {
    int newGapSize = gb->gapSize + n;
    int newBufferSize = gb->bufferSize + n;
    char* newBuffer = malloc(newBufferSize * sizeof(char));
    
    // copy from beginning of buffer to start of gap
    int bytesToCopy = gb->gapStart;
    memcpy(newBuffer, gb->buffer, bytesToCopy);
    // copy from end of gap to end of buffer
    int newGapEnd = gb->gapStart + newGapSize;
    bytesToCopy = (gb->bufferSize) - gb->gapSize - gb->gapStart;
    memcpy(newBuffer + newGapEnd,
           gb->buffer + gb->gapStart + gb->gapSize,
           bytesToCopy);
    
    free(gb->buffer);
    gb->buffer = newBuffer;
    gb->bufferSize = newBufferSize;
    gb->gapSize = newGapSize;
}
