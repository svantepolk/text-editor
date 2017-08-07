struct GapBuffer {
    int lineCount;
    int gapStart;
    int gapSize;
    char* buffer;
    int bufferSize;
};

void gb_init(struct GapBuffer*, int);
void gb_init_with_text(struct GapBuffer*, int, char*, int);

void gb_fprint(struct GapBuffer*, FILE*);

int gb_charc(struct GapBuffer*);
char gb_get(struct GapBuffer*, int);

void gb_move_gap(struct GapBuffer*, int);
void gb_grow_gap(struct GapBuffer*, int);
void gb_shrink_gap(struct GapBuffer*, int);

void gb_insert(struct GapBuffer*, char);
void gb_insert_chars(struct GapBuffer*, char*, int);
void gb_delete_forward(struct GapBuffer*, int);
void gb_delete_backward(struct GapBuffer*, int);

int gb_line_index(struct GapBuffer*, int);
int gb_line_length(struct GapBuffer*, int);
