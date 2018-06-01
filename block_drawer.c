// #include <stdio.h>

#define BLOCK_H (5)

int draw_blocks (int x1, int y1, int x2, int y2, float offset, int data_array[], int data_array_length) {
    int i, k, j;
    char empty_line[]= "        ";
    char full_line[] = "MMMMMMMM";
    char done_line[] = "........";

    int offset_line_number = (int)(BLOCK_H * offset);
    int current_block_data = 0;

    int draw_row_number = ((int)offset == 0 ? 4 : 5);

    for(i = 0; i < draw_row_number && i < data_array_length; i++) {
        current_block_data = data_array[i];
        for (k = 0; k < BLOCK_H - offset_line_number; k++) {
            for(j = 0; j < current_block_data-1; j++) {
                printf(empty_line);
            }
            printf(full_line);
            for(j = j+1; j < 4; j++) {
                printf(empty_line);
            }
            printf("\n");
        }
        offset_line_number = 0;
        printf("\n");
    }

    return draw_row_number;
}

void set_redraw_timeout() {

}


