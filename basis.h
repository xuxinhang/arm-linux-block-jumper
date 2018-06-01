#include "basis.c"

/*** Common Data Strutures ***/

#define clear_screen() printf("\033[H\033[J")

enum event_type {Next, KeyDown, KeyUp };
enum page_type { Welcome, Play };

typedef struct Event_Item {
    enum event_type type;
    int value;
    struct Event_Item *_next;
} B_EVENT;



/**** Global Variable ****/

int g_end_main_loop = 0;
enum page_type g_active_page;



/**** Common Function ****/




