
int welcome_page_event_handler(B_EVENT* event) {
    if (event->type == KeyDown || event->type == KeyUp) {
        switch (event->value) {
            case 'Q': case 'q':
            g_end_main_loop = 1;
            break;

            case 'S': case 's':
            init_play_page();
            break;
        }
    }

    return 0;
}


void show_welcome() {
    g_active_page = Welcome;
    // Print tip text to stdout
    clear_screen();
    printf("\n");
    printf("=========== Block Jump! ============\n");
    printf("**                                **\n");
    printf("**        [S] = Start Game        **\n");
    // printf("**        [Q] = Start Game        **\n");
    // printf("**        [S] = Start Game        **\n");
    printf("**        [C] = Change Options    **\n");
    printf("**        [Q] = Quit              **\n");
    printf("**                                **\n");
    printf("========== by: Bob Green ===========\n");
    printf("\n");
    printf("Your choice:\t");
    // generate_block_data(block_data, block_data_length);
}
