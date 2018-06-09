#define BLOCK_1_KEY '7'
#define BLOCK_2_KEY '8'
#define BLOCK_3_KEY '9'
#define BLOCK_4_KEY '0'
#define PLAY_PAUSE_KEY ' '


int* block_data = NULL;
int block_data_length = 40;

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

void print_fail_screen () {
    printf("~ Good luck next time ~");
}

void print_pass_screen () {
    clear_screen();
    printf("What a talent you are !");
}


float draw_offset = 0.0;
int* block_data_pointer = NULL;
int current_block_type = -1;
int is_selected_block_type_correct = Right;

int play_page_event_handler (B_EVENT* event) {

    if (block_data_pointer == NULL) {
        block_data_pointer = block_data;
    }

    if (event->type == KeyDown || event->type == KeyUp) {
        if (play_page_status == Playing) {
            switch(event->value) {
                case BLOCK_1_KEY:
                if (is_selected_block_type_correct == Unknown) {
                    is_selected_block_type_correct = current_block_type == 1 ? Right : Wrong;
                }
                break;

                case BLOCK_2_KEY:
                if (is_selected_block_type_correct == Unknown) {
                    is_selected_block_type_correct = current_block_type == 2 ? Right : Wrong;
                }
                break;

                case BLOCK_3_KEY:
                if (is_selected_block_type_correct == Unknown) {
                    is_selected_block_type_correct = current_block_type == 3 ? Right : Wrong;
                }
                break;

                case BLOCK_4_KEY:
                if (is_selected_block_type_correct == Unknown) {
                    is_selected_block_type_correct = current_block_type == 4 ? Right : Wrong;
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
        } else if (play_page_status == Fail) {
            switch (event->value) {
                default:
                play_page_status = Waiting;
                print_play_screen();
            }
        } else if (play_page_status = Pass) {
            switch (event->value) {
                default:
                play_page_status = Waiting;
                print_play_screen();
            }
        }
    } else if (event->type == Timeout && play_page_status == Playing) {
        // printf("Timeout in Play screen");
        if (play_page_status == Playing && event->value == FrameRefresh) { // [TODO]
            current_block_type = *block_data_pointer;

            // 剩余块的数量（包括当前块在内）
            int block_remained_number = block_data + block_data_length - block_data_pointer;

            if (is_selected_block_type_correct == Wrong) {
                printf("{{ You Wrong }}");
                play_page_status = Fail;
            }

            if (draw_offset == 0.0 || draw_offset < 0.001) {
                if (is_selected_block_type_correct == Unknown) {
                    play_page_status = Fail;
                    block_data_pointer--;
                    block_remained_number++;
                    draw_offset = 0.7;
                    is_selected_block_type_correct = Wrong;
                } else if (is_selected_block_type_correct == Right && block_remained_number == 0) {
                    play_page_status = Pass;
                } else {
                    is_selected_block_type_correct = Unknown;
                }
            }

            // Set special block effect
            if (is_selected_block_type_correct == Right) {
                (*block_data_pointer) |= BLOCK_STATE_RIGHT;
            } else if (is_selected_block_type_correct == Wrong) {
                (*block_data_pointer) |= BLOCK_STATE_WRONG;
            }

            refresh_frame(draw_offset, block_data_pointer, block_remained_number > 5 ? 5 : block_remained_number);
            
            draw_offset += 0.2;
            if (draw_offset >= 0.999) {
                block_data_pointer++;
                draw_offset = 0.0;
            }

            // Print other screens
            if (play_page_status == Pass) {
                print_pass_screen();
            } else if (play_page_status == Fail) {
                print_fail_screen();
            }
        }
    }

    return 0;
}

void play_page_start_playing () {
    // Screen Output
    clear_screen();
    printf("= = = = = = = = = = =\n= Play Start, Now! =\n");

    // Genrate block series
    if (block_data != NULL) {
        free(block_data);
    }
    block_data = (int*)malloc(block_data_length * sizeof(int));
    generate_block_data(block_data, block_data_length);

    // Prepare to print game image
    clear_screen();
    draw_block_prepare();

    // Reset variables
    draw_offset = 0.0;
    block_data_pointer = NULL;
    current_block_type = -1;
    is_selected_block_type_correct = Right;

    play_page_status = Playing;
}


int generate_block_data (int *container, int total) {
    int i;
    srand((int) time(0));
    for (i = 0; i < total; i++) {
        container[i] = rand() % 4 + 1;
    }
    return i + 1;
}

void refresh_frame (float offset, int *block_data, int block_data_length) {
    static float prev_offset = 0.0;

    clear_screen();
    draw_blocks(0, 0, 0, 0, offset, block_data, block_data_length);
    #ifndef _NO_FRAMEBUFFER
    draw_blocks_fb(50, 100, 450, 410, offset, block_data, block_data_length);
    #endif
}


