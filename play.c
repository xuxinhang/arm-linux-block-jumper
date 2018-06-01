#define BLOCK_1_KEY '7'
#define BLOCK_2_KEY '8'
#define BLOCK_3_KEY '9'
#define BLOCK_4_KEY '0'
#define PLAY_PAUSE_KEY ' ' 


int block_data[40];

int waiting_block_data;

void init_play_page () {
    g_active_page = Play;
    // 屏幕输出
    print_play_screen();
    // 初始化状态标志
    play_page_status = Waiting;
}

void print_play_screen() {
    // Clean Screen
    clear_screen();
    printf("Now Just Play\n");
    printf("Press [E] to play\n");
    printf("[B] Return to welcome screen\n");
}

int play_page_event_handler (B_EVENT* event) {
    
    if (event->type == KeyDown || event->type == KeyUp) {
        if (play_page_status == Playing) {
            switch(event->value) {
                case BLOCK_1_KEY:
                if (waiting_block_data == 1) {

                } else {
                    
                }
                break;
            }
        } else if (play_page_status == Waiting) {
            switch(event->value) {
                case 'E': case 'e':
                play_page_start_playing();
                break;

                case 'B': case 'b':
                show_welcome();
                break; 
            }
        }
    }

    return 0;
}

void play_page_start_playing () {
    play_page_status = Playing;
    // Screen Output
    clear_screen();
    printf("= = = = = = = = = = =\n= Play Start, Now! =\n");
    
    // Genrate
    generate_block_data(block_data, 40);

    // Print Game Image
    clear_screen();
    draw_block_prepare();
    clear_screen();
    draw_blocks(0, 0, 0, 0, 0.3, block_data, 40);
    draw_blocks_fb(50, 100, 450, 410, 0.3, block_data, 40);

    // Start Second
}


int generate_block_data (int *container, int total) {
    int i;
    srand((int) time(0));
    for (i = 0; i < total; i++) {
        container[i] = rand() % 4 + 1;
    }
    return i + 1;
}




