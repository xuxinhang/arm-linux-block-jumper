
#include "block_drawer.h"
#include "block_drawer.c"

#ifndef _NO_FRAMEBUFFER
#include "block_drawer_fb.c"
#endif


#define BLOCK_STATE_WRONG (0x2 << 3)
#define BLOCK_STATE_RIGHT (0x1 << 3)


enum game_play_status {Waiting, Playing, Pause, Fail, Pass};
enum select_block_type_result { Unknown, Right, Wrong };

enum game_play_status play_page_status = Waiting;

void init_play_page();
void print_play_screen();
int play_page_event_handler(B_EVENT* event);
void play_page_start_playing();

int generate_block_data (int *container, int total);
void refresh_frame (float offset, int *block_data, int block_data_length);