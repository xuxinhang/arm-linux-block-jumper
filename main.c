#include <stdio.h>
#include <stdlib.h>
// #include <libio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <sys/time.h>
#include <signal.h>

#include "basis.h"
#include "welcome.h"
#include "play.h"

#include "welcome.c"
#include "play.c"


void set_crmode();
void basic_event_handler(B_EVENT* event);
void signal_main_handler();
void signal_timeout_handler(int type);
void create_signal_timeout(time_t v_s, suseconds_t v_ms, time_t i_s, suseconds_t i_ms, int flag);
void common_event_passer (B_EVENT* event_item);

// Event 列表
B_EVENT *event_list = (B_EVENT*)NULL;
// 定时器队列
B_TIMEOUT_QUEUE *timeout_queue = (B_TIMEOUT_QUEUE*)NULL;

/*** 程序主函数 ***/
int main() {

    /* Common Variables */
    event_list = (B_EVENT*)malloc(sizeof(B_EVENT));
    event_list->_next = NULL;

    /* Useless Variables */
    int block_data[12];
    int block_data_length = 12;

    event_list->type = Next;

    set_crmode(); // 配置终端参数

    // Print the welcome Screen
    show_welcome();
    g_active_page = Welcome;


    /*** 信号机制处理***/
    signal(SIGALRM, signal_timeout_handler);

    /*
    * [!] 程序主循环
    * 用于处理消息机制
    */

    create_signal_timeout(1,100,0,770000, FrameRefresh);
    // 1, 100 , 0 , 330000

    while (1) {
        // Read Stdin & push it to event_list
        B_EVENT* event_item;
        // Read Message & Do work
        B_EVENT* event_prev;
        event_item = event_list;
        
        // Force to refresh stdout cache
        fflush(stdout);

        // hbhit
        fd_set rfds;
        struct timeval tv;
        int retval;
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        retval = select(1, &rfds, NULL, NULL, &tv);

        char ch;
        while (retval > 0 && (ch = getchar()) != EOF ) {
        // while ((ch = getchar()) != EOF) {   // read/print "abcde" from stdin
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

        event_item = event_list->_next;

        while (event_item != NULL) {

            // Clear event item had been processed
            // if (event_item->type != Next) {
                common_event_passer(event_item);
                event_prev = event_item;
                event_item = event_item->_next;
                free(event_prev);
            // } else {
            //     event_item = event_item->_next;
            // }
        }

        event_list->_next = NULL;

        if (g_end_main_loop) break;

    }

    return 0;

}

void common_event_passer (B_EVENT* event_item) {

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
}

void basic_event_handler(B_EVENT* event) {
    if (event) {

    }
}

void signal_main_handler() {
}

/** 定时信号处理器 **/
void signal_timeout_handler (int type) {
    // printf("SIG TIME OUT");
    B_EVENT* event_item;
    B_EVENT* event_prev;
    event_item = event_list;
    int flag;

    // Remove timeout item from Timeout-Queue
    B_TIMEOUT_QUEUE *timeout_item = timeout_queue;
    if(timeout_queue == NULL) return;

    // Add to event list.
    while(event_item->_next != NULL) {
        event_item = event_item->_next;
    }
    event_item = (event_item->_next = (B_EVENT*)malloc(sizeof(B_EVENT)));
    event_item->type = Timeout;
    event_item->value = flag = timeout_queue->flag;
    event_item->_next = NULL;

    // Delete Timeout item.
    timeout_item = timeout_queue->_next;
    free(timeout_queue);
    timeout_queue = timeout_item;

    // Add timeout item for next tigger
    if (timeout_queue == NULL) {
        timeout_queue = (B_TIMEOUT_QUEUE*)malloc(sizeof(B_TIMEOUT_QUEUE));
        timeout_item = timeout_queue;
    } else {
        timeout_item = timeout_queue;
        while(timeout_item->_next != NULL) timeout_item = timeout_item->_next;
        timeout_item->_next = (B_TIMEOUT_QUEUE*)malloc(sizeof(B_TIMEOUT_QUEUE));
        timeout_item = timeout_item->_next;
    }
    timeout_item->flag = flag;
    timeout_item->signal = ITIMER_REAL;
    timeout_item->_next = (B_TIMEOUT_QUEUE*)NULL;
}

/** 发起定时器 **/
void create_signal_timeout(time_t v_s, suseconds_t v_ms, time_t i_s, suseconds_t i_ms, int flag) {
    // Register the timer in OS
    struct itimerval new_value, old_value;
    new_value.it_value.tv_sec = v_s;
    new_value.it_value.tv_usec = v_ms;
    new_value.it_interval.tv_sec = i_s;
    new_value.it_interval.tv_usec = i_ms;
    setitimer(ITIMER_REAL, &new_value, &old_value);

    // Add to the queue
    B_TIMEOUT_QUEUE *timeout_item;
    if (timeout_queue == NULL) {
        timeout_queue = (B_TIMEOUT_QUEUE*)malloc(sizeof(B_TIMEOUT_QUEUE));
        timeout_item = timeout_queue;
    } else {
        timeout_item = timeout_queue;
        while(timeout_item->_next != NULL) timeout_item = timeout_item->_next;
        timeout_item->_next = (B_TIMEOUT_QUEUE*)malloc(sizeof(B_TIMEOUT_QUEUE));
        timeout_item = timeout_item->_next;
    }
    timeout_item->flag = flag;
    timeout_item->signal = ITIMER_REAL;
    timeout_item->_next = (B_TIMEOUT_QUEUE*)NULL;
}


/** 关闭终端输入缓冲区 **/
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

