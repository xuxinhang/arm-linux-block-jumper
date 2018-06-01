
#include "block_drawer.h"
#include "block_drawer.c"

#include "block_drawer_fb.c"


enum game_play_status {Waiting, Playing, Pause, Fail, Pass};
enum game_play_status play_page_status = Waiting;

void init_play_page();
void print_play_screen();
int play_page_event_handler(B_EVENT* event);
void play_page_start_playing();

int generate_block_data (int *container, int total);
