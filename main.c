#include <stdio.h>
#include <stdlib.h>
#include <libio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>

#include "basis.h"
#include "welcome.h"
#include "play.h"

#include "welcome.c"
#include "play.c"


void set_crmode();
void basic_event_handler(B_EVENT* event);

/*** 程序主函数 ***/
int main() {

    int block_data[12];
    int block_data_length = 12;

    B_EVENT *event_list = (B_EVENT*)malloc(sizeof(B_EVENT));
    event_list->type = Next;
    
    set_crmode(); // 配置终端参数

    // Print the welcome Screen
    show_welcome();
    g_active_page = Welcome;

    /*
    * [!] 程序主循环
    * 用于处理消息机制
    */

    while (1) {
        // Read Stdin & push it to event_list
        B_EVENT* event_item;
        // Read Message & Do work
        B_EVENT* event_prev;
        event_item = event_list;

        char ch;
        while ((ch = getchar()) != EOF) {   /* read/print "abcde" from stdin */
            while(event_item->_next != NULL) {
                event_item = event_item->_next;
            }
            event_item = (event_item->_next = (B_EVENT*)malloc(sizeof(B_EVENT)));
            event_item->type = KeyDown;
            event_item->value = ch;
            event_item->_next = NULL;
            // printf("You input :: %c\n", event_item->value);
            if (ch > 10) {
                break;
            }
        }


        while (event_item != NULL) {

            // Print debug infomation
            switch (event_item->type) {
                case KeyDown:
                case KeyUp:
                    printf("A key event!\n");
                    break;
                case Next:
                    printf("Header or Skip\n");
                    break;
                default:
                    printf("Unknow Event\n");
                    break;
            }

            // Event Handler for basic application
            basic_event_handler(event_item);

            // Different event handlers for different paegs
            switch (g_active_page) {
                case Welcome:
                welcome_page_event_handler(event_item);
                break;
                case Play:
                play_page_event_handler(event_item);
                break;
            }
            
            // Clear event item had been processed
            if (event_item->type != Next) {
                event_prev = event_item;
                event_item = event_item->_next;
                free(event_prev);
            } else {
                event_item = event_item->_next;
            }
        }

        if (g_end_main_loop) break;

    }

    return 0;

}

void basic_event_handler(B_EVENT* event) {
    if (event) {

    }
}


void set_crmode(){
    //用来关闭终端缓冲，包含了好几个步骤
    //简单说，就是：读、改、更
    struct termios ttystate;
    tcgetattr(0,&ttystate);
    //关闭缓冲
    //&= 是让ICANON和他的相反位与肯定是0
    ttystate.c_lflag &= ~ICANON;
    //一次读入一个字符，立即反应
    ttystate.c_cc[VMIN] = 1;
    //更新设置
    tcsetattr(0,TCSANOW,&ttystate);
}

