// #include <stdio.h>

#define BLOCK_H (5)

#define BLOCK_STATE_WRONG (0x2 << 3)
#define BLOCK_STATE_RIGHT (0x1 << 3)


int draw_blocks (int x1, int y1, int x2, int y2, float offset, int data_array[], int data_array_length) {
    int i, k, j;
    char empty_line[]= "        ";
    char full_line[] = "\x16\x16\x16\x16\x16\x16\x16\x16";
    char done_line[] = "........";
    char wrong_line[]= "xxxxxxxx";

    int offset_line_number = (int)(BLOCK_H * offset);
    int current_block_data = 0;

    int draw_row_number = ((int)offset == 0 ? 4 : 5);

    for(i = 0; i < draw_row_number && i < data_array_length; i++) {
        current_block_data = data_array[i] & 0x07;
        for (k = 0; k < BLOCK_H - offset_line_number; k++) {
            for(j = 0; j < current_block_data-1; j++) {
                printf("%s", empty_line);
            }

            if ((data_array[i] & (0x3 << 3)) == BLOCK_STATE_WRONG) {
                printf("%s", wrong_line);
            } else if ((data_array[i] & (0x3 << 3)) == BLOCK_STATE_RIGHT) {
                printf("%s", done_line);
            } else {
                printf("%s", full_line);
            }

            for(j = j+1; j < 4; j++) {
                printf("%s", empty_line);
            }
            printf("\n");
        }
        offset_line_number = 0;
    }

    return draw_row_number;
}


void draw_block_prepare(){
    // 在画方块之前进行清屏等准备
    // 以后再写
}

void set_redraw_timeout() {

}


