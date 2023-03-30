#ifndef _LCDDEVICE_H_
#define _LCDDEVICE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <linux/fb.h>
// lcd结构体，里面存储lcd文件标识符，宽，高，色深，lcd映射地址，lcd可变设备信息
typedef struct LcdDevice
{
    int lcd_fd; //lcd文件标识符
    int w, h, pix;//w：宽，h：高，pix：色深
    unsigned char *mptr; //lcd映射地址
    struct fb_var_screeninfo* lcd_vinfo; //lcd可变设备信息
    void (*clear)(unsigned int color);
    void (*drawRect)(int x, int y, int w, int h, unsigned int color);
    void (*drawFill)(int x, int y, int w, int h, unsigned int color);
    void (*drawLine)(int x1, int y1, int x2, int y2);
    void (*drawCircle)(int x, int y, int R, int color);
} LcdDevice;
LcdDevice *create_lcd(const char *dev);
bool destroy_lcd(LcdDevice *lcd);
#endif