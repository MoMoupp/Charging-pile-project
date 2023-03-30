#ifndef _show_PIC_
#define _show_PIC_

#include "lcd_dev.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>
#include "jpeglib.h" //双引号指的是先从当前路径查找头文件 使用libjpeg库，必须包含
#include "font.h"

// BMP——————————————————————————————————————————————————
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
#pragma pack(1) // 设置1字节对齐
typedef struct BitFileHeader
{
    uint16 bfType; // 位图类别，根据不同的操作系统而不同，在Windows中，此字段的值总为‘BM’BMP
    uint32 bfSize; // BMP图像文件的大小
    uint16 bfReserved1;
    uint16 bfReserved2;
    uint32 bfOffBits; // BMP图像数据的地址
    uint32 biSize;    // 表示本结构的大小
    uint32 biWidth;   // 位图的宽度
    uint32 biHeight;  // 位图的高度
    uint16 biPlanes;
    uint16 biBitCount;      // BMP图像的色深，即一个像素用多少位表示，常见有1、4、8、16、24和32，分别对应单色、16色、256色、16位高彩色、24位真彩色和32位增强型真彩色
    uint32 biCompression;   // 压缩方式，0表示不压缩，1表示RLE8压缩，2表示RLE4压缩，3表示每个像素值由指定的掩码决定
    uint32 biSizeImage;     // BMP图像数据大小，必须是4的倍数，图像数据大小不是4的倍数时用0填充补足
    uint32 biXPelsPerMeter; // 水平分辨率，单位像素/m
    uint32 biYPelsPerMeter; // 垂直分辨率，单位像素/m
    uint32 biClrUsed;       // BMP图像使用的颜色，0表示使用全部颜色，对于256色位图来说，此值为100h=256
    uint32 biClrImportant;  // 重要的颜色数，此值为0时所有颜色都重要，对于使用调色板的BMP图像来说，当显卡不能够显示所有颜色时，此值将辅助驱动程序显示颜色
} BitHeader;
#pragma pack(0) // 设置为默认字节对齐

//bmp结构体，里面存储图片像素数据，高，色深，宽
typedef struct Bmp
{
    int w, h, pix;       // 保存宽高，一个像素占用的字节数
    unsigned char *data; // 保存rgb数据
    BitHeader header;    // 保存bmp头
} BMP;
// 创建bmp对象
struct Bmp *create_bmp(const char *bmpfile);
// 按照给的尺寸缩放，新的宽，新的高
void zoom_bmp(struct Bmp *mp, int new_w, int new_h);
// 销毁BMP对象
bool destroy_bmp(struct Bmp *mp);
// BMP——————————————————————————————————————————————————

// 任意位置显示jpeg图片
void show_jpeg(LcdDevice *lcd, char *filename, int x, int y);
// 任意位置显示jpeg_gif图片
void show_gif_jpeg(LcdDevice *lcd, char *filename, int x, int y);
// 任意位置显示bmp图片
void show_bmp(LcdDevice *lcd, char *filename, int x, int y);
// 在lcd的x,y位置显示汉字图片 ，x，y位置, 宽，高，size字体大小
void show_font(LcdDevice *lcd, char *s_font, int x, int y, int width, int height,int size);
//开机动画
void open_animation(LcdDevice *lcd);
// 关机动画
void shutdown_a(LcdDevice *lcd);
//  显示键盘，参数：lcd结构体
void show_keyboard(LcdDevice *lcd, int x, int y);
// 隐藏键盘，hide_keyborad_name每个界面隐藏键盘的相应图片文件名
void hide_keyborad(LcdDevice *lcd, int x, int y, const char *hide_keyborad_name);
#endif