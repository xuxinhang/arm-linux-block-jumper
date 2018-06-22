# Block-Jumper Game for the ARM Experiment-box

只在`中202`室的 “Linux 嵌入式软件开发” 的课程设计。

一个运行于ARM实验箱的“别踩白块”游戏。


## How to compile for an experiment box ?
```shell
armv4l-unknown-linux-gcc -o jump.arm -x c++  ./main.c -lm
./jump.arm
```

## Keywords
- UNIX signal 机制
- stdin / stdout 缓冲区的刷新
- 读写 FrameBuffer
- 类似订阅发布模式的主循环机制



