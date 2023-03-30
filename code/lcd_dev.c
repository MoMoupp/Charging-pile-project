#include "lcd_dev.h"
#include <string.h>

static LcdDevice *lcd = NULL;

// 清屏
static void clear(unsigned int color)
{
    if (lcd == NULL)
        return;
    for (int i = 0; i < lcd->w * lcd->h; i++)
    {
        memcpy(lcd->mptr + i * lcd->pix, &color, lcd->pix);
    }
}
// 画正方形(null)
static void drawRect(int x, int y, int w, int h, unsigned int color) { return; }
// 填充
static void drawFill(int x, int y, int w, int h, unsigned int color)
{
    if (lcd == NULL)
        return;
    //越界处理
    if (x+w>lcd->w)
    {
        w = lcd->w - x;
    }
    if (y+h>lcd->h)
    {
        h = lcd->h - y;
    }
    //临时指针
    unsigned char *p = lcd->mptr;
    // 把指针p移动到要绘制的开始位置
    p = p + (x * lcd->pix + (y * lcd->w * lcd->pix));
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            memcpy(p + j * lcd->pix, &color, lcd->pix);
        }
        p += lcd->w * lcd->pix; // 移动到下一行
    }
}
// 画线(null)
static void drawLine(int x1, int y1, int x2, int y2) { return; }
// 画圆
static void drawCircle(int x, int y, int R, int color)
{
    if (lcd == NULL)
        return;
    char *t;
    for (int i = 0; i < lcd->w; i++)
    {
        for (int j = 0; j < lcd->h; j++)
        {
            int r1 = ((i - x) * (i - x)) + ((j - y) * (j - y)); // 圆函数
            if (r1 <= R * R)
            {
                t = lcd->mptr + 
                    ((j * lcd->w * lcd->pix) + (i * lcd->pix));
                memcpy(t, &color, lcd->pix);
            }
        }
    }
}
//获取lcd
LcdDevice *getLcd(const char *dev)
{
    if (lcd == NULL)
        return create_lcd(dev);
    else
        return lcd;
}
//创建lcd
LcdDevice *create_lcd(const char *dev)
{
    lcd = malloc(sizeof(LcdDevice));
    if (lcd == NULL)
        return NULL;

    // 初始化函数指针
    lcd->clear = clear;
    lcd->drawRect = drawRect;
    lcd->drawLine = drawLine;
    lcd->drawCircle = drawCircle;
    lcd->drawFill = drawFill;

    lcd->lcd_fd = open(dev, O_RDWR);
    if (lcd->lcd_fd < 0)
    {
        perror("lcd_open");
        goto err;
    }
    //获取参数
    struct fb_var_screeninfo *lcd_info = malloc(sizeof(struct fb_var_screeninfo));
    int ret = ioctl(lcd->lcd_fd, FBIOGET_VSCREENINFO, lcd_info);
    // lcd信息结构体地址
    lcd->lcd_vinfo = lcd_info;
    if (ret < 0)
    {   
        perror("ioctl");
        lcd->w = 800;
        lcd->h = 480;
        lcd->pix = 4;
    }
    else
    {
        #ifdef PC
        lcd->w = lcd_info.xres_virtual;
        lcd->h = lcd_info.yres_virtual;
        lcd->pix = lcd_info.bits_per_pixel / 8;

        #else
        lcd->w = lcd->lcd_vinfo->xres;
        lcd->h = lcd->lcd_vinfo->yres;
        lcd->pix = lcd_info->bits_per_pixel / 8;
        #endif
    }
    //lcd信息结构体地址
    lcd->lcd_vinfo = lcd_info;
    //初始化可见区域
    lcd->lcd_vinfo->xoffset = 0;
    lcd->lcd_vinfo->yoffset = 0;
    ioctl(lcd->lcd_fd, FBIOPAN_DISPLAY, lcd->lcd_vinfo);
    //映射
    int mp_size = lcd->w * lcd->h * lcd->pix;
    lcd->mptr = (unsigned char *)mmap(
                                    NULL, 
                                    mp_size*2, 
                                    PROT_READ | PROT_WRITE, 
                                    MAP_SHARED, 
                                    lcd->lcd_fd, 0);
    if (lcd->mptr==MAP_FAILED)
    {
        perror("map");
        goto err;
    }
    return lcd;

err:
    free(lcd);
    return NULL;
}
//释放lcd
bool destroy_lcd(LcdDevice *lcd)
{
    if (lcd != NULL)
    {
        if (lcd->mptr != MAP_FAILED)
        {
            int mp_size = lcd->w * lcd->h * lcd->pix;
            munmap(lcd->mptr, mp_size);
        }
        close(lcd->lcd_fd);
        free(lcd->lcd_vinfo);
        free(lcd);
        return true;
    }
    return false;
}