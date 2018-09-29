#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/stat.h>
#include "gapbuffer.h"

#define KEY_ESCAPE 27

void usage(char* invocation) {
    fprintf(stderr, "Usage: %s file\n", invocation);
    exit(1);
}

void display_buffer(struct GapBuffer* buffer) {
    erase();
    // display the buffer contents
    int row = 0;
    int col = 0;
    for (int i = 0; i < gb_charc(buffer); i++) {
        char ch = gb_get(buffer, i);
        if (ch == '\n') {
            row++;
            col = 0;
        } else {
            mvaddch(row, col, ch);
            col++;
        }
    }

    refresh();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        usage(argv[0]);
    }
    char* path = argv[1];

    struct GapBuffer buffer;
    struct stat path_stat;
    stat(path, &path_stat);
    if (!S_ISREG(path_stat.st_mode)) {
        gb_init(&buffer, 256);
    } else {
        gb_init(&buffer, path_stat.st_size + 256);
        char* fileContents = malloc(sizeof(path_stat.st_size));
        FILE* file = fopen(path, "r");
        fread(fileContents, 1, path_stat.st_size, file);
        fclose(file);
        gb_insert_chars(&buffer, fileContents, path_stat.st_size);
    }

    initscr();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    
    bool shouldExit = FALSE;
    int index = 0;
    int row = 0;
    int col = 0;
    int c;
    while (!shouldExit) {

        display_buffer(&buffer);
        move(row, col);
        gb_move_gap(&buffer, index);

        switch (c = getch()) {
            case KEY_ESCAPE: {
                FILE* file = fopen(path, "w+");
                gb_fprint(&buffer, file);
                fclose(file);
                shouldExit = TRUE;
            }  break;
            case KEY_BACKSPACE: {
                gb_delete_backward(&buffer, 1);
            }

            case KEY_LEFT: {
                if (index > 0) {
                    if (col > 0) {
                        col--;
                    } else {
                        // move up a row
                        row--;

                        int i;
                        for (i = index - 2; i > -1; i--) {
                            if (gb_get(&buffer, i) == '\n') {
                                break;
                            }
                        }

                        col = index - (i + 2);
                    }

                    index--;
                }
            } break;

            case KEY_RIGHT: {
                if (index < gb_charc(&buffer)) {
                    if (gb_get(&buffer, index) == '\n') {
                        // move to start of next line
                        col = 0;
                        row++;
                    } else {
                        col++;
                    }
                    index++;
                }
            } break;
            
            case KEY_UP: {

            } break;
            
            case KEY_DOWN: {

            } break;

            default: {
                gb_insert(&buffer, (char) c);
                if (c == '\n') {
                    row++;
                    col = 0;
                } else {
                    col++;
                }
                index++;
            }
        }
    }

    endwin();

    return 0;
}
