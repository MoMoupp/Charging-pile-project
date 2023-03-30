#include "showpic.h"

// BMP——————————————————————————————————————————————————
//  创建Bmp图片
struct Bmp *create_bmp(const char *bmpfile)
{
    struct Bmp *bmp = malloc(sizeof(struct Bmp));
    if (bmp == NULL)
    {
        perror("bmp_malloc");
        return NULL;
    }
    // 打开bmpfile文件
    FILE *file = fopen(bmpfile, "r");
    if (file == NULL)
    {
        perror(bmpfile);
        free(bmp);
        return NULL;
    }
    // 读取bmp头
    size_t size = fread(&bmp->header, 1, 54, file);
    if (size < 0)
    {
        perror(bmpfile);
        fclose(file);
        free(bmp);
        return NULL;
    }
    // 计算像素占用的内存空间大小
    bmp->w = bmp->header.biWidth;
    bmp->h = bmp->header.biHeight;
    bmp->pix = bmp->header.biBitCount / 8;
    unsigned int rgbsize = bmp->w * bmp->h * bmp->pix;

    // 申请保存像素数据的空间
    bmp->data = (unsigned char *)malloc(rgbsize);

    // 计算填充的字节数
    int h_bit = (4 - ((bmp->w * bmp->pix) % 4)) % 4;

    int rd_num = 0; // 记录读取次数
    while (1)
    {
        // 一次读取一行
        size_t f = fread(bmp->data + (rd_num * bmp->w * bmp->pix), 1, bmp->w * bmp->pix, file);
        // 跳过填充的字节
        fseek(file, h_bit, SEEK_CUR);
        if (f == 0)
        {
            // perror("bmp_over");
            break;
        }
        rd_num++;
    }
    // 返回
    fclose(file);
    return bmp;
}
// 按照给的尺寸缩放
void zoom_bmp(struct Bmp *mp, int new_w, int new_h)
{
    if (mp == NULL)
    {
        return;
    }
    // 申请一个保存目标图片的空间
    unsigned char *destmap = malloc(new_w * new_h * mp->pix);

    // 从原图中拷贝数据到目标图中
    for (int i = 0; i < new_h; i++)
    {
        // 拷贝一行
        for (int j = 0; j < new_w; j++)
        {
            // 拷贝一个像素
            destmap[j * 3 + 0 + i * new_w * 3] = mp->data[(j * mp->w / new_w) * 3 + 0 + (i * mp->h / new_h) * mp->w * 3];
            destmap[j * 3 + 1 + i * new_w * 3] = mp->data[(j * mp->w / new_w) * 3 + 1 + (i * mp->h / new_h) * mp->w * 3];
            destmap[j * 3 + 2 + i * new_w * 3] = mp->data[(j * mp->w / new_w) * 3 + 2 + (i * mp->h / new_h) * mp->w * 3];
        }
    }
    mp->w = new_w;
    mp->h = new_h;
    free(mp->data);
    mp->data = destmap;
}
// 销毁bmp
bool destroy_bmp(struct Bmp *mp)
{
    if (mp == NULL)
        return false;
    free(mp->data);
    free(mp);
    return true;
}
//BMP——————————————————————————————————————————————————


// 任意位置显示bmp图片
void show_bmp(LcdDevice *lcd, char * filename, int x, int y)
{
    struct Bmp *mp = create_bmp(filename);

    if (lcd == NULL||mp==NULL)
    {
        return;
    }
    int dw = mp->w; // 绘制的宽
    int dh = mp->h; // 绘制的高
    // 边界处理
    if (x + mp->w > lcd->w)
    {
        dw = lcd->w - x;
    }
    if (y + mp->h > lcd->h)
    {
        dh = lcd->h - y;
    }
    int d_pix = mp->pix;
    // 比较图片和屏幕的色深大小
    if (mp->pix > lcd->pix)
    {
        d_pix = lcd->pix;
    }
    // 倒转
    char *t_bmp = mp->data + mp->w * (mp->h - 1) * mp->pix;

    // 临时指针
    unsigned char *p = lcd->mptr;
    // 多缓冲显示
    int show_num = 0;
    if (lcd->lcd_vinfo->yoffset == 0)
    {   
        p += lcd->w * lcd->h * lcd->pix; // 移到到下一块屏幕，800*480*4
        show_num = 1;
    }
    else if (lcd->lcd_vinfo->yoffset == lcd->h)
    {
        p = lcd->mptr;
        show_num = 0;
    }
    printf("P:%d\n", show_num);
    // 把指针p移动到要绘制的开始位置
    p = p + (x * lcd->pix + (y * lcd->w * lcd->pix));
    // 绘制bmp图片
    for (int i = 0; i < dh; i++)
    {
        for (int j = 0; j < dw; j++)
        {
            memcpy(p + j * lcd->pix, t_bmp + j * mp->pix, d_pix);
        }
        p += lcd->w * lcd->pix;
        t_bmp -= mp->w * mp->pix;
    }
    // 可见区与虚拟区的偏移量
    lcd->lcd_vinfo->yoffset = show_num * lcd->h;
    lcd->lcd_vinfo->xoffset = 0;
    // 写入设备信息
    ioctl(lcd->lcd_fd, FBIOPAN_DISPLAY, lcd->lcd_vinfo);

    destroy_bmp(mp);
}

// 任意位置显示jpeg_gif图片
void show_jpeg(LcdDevice *lcd, char *filename, int x, int y)
{

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // 1.定义解码对象以及定义错误处理对象，并且进行初始化
    cinfo.err = jpeg_std_error(&jerr); // 把错误处理对象以及解码对象进行关联
    jpeg_create_decompress(&cinfo);

    // 2.指定需要解码的文件，必须以二进制方式打开
    FILE *infile;

    if ((infile = fopen(filename, "rb")) == NULL)
    {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }

    jpeg_stdio_src(&cinfo, infile); // 把解码文件和定义了解码对象进行关联

    // 3.读取要解码的JPEG图片的文件头
    jpeg_read_header(&cinfo, TRUE);

    // 4.设置解码参数，如果不设置则使用默认值即可 （可选）

    // 5开始解码
    jpeg_start_decompress(&cinfo);

    int dw = cinfo.output_width;  // 要绘制的宽
    int dh = cinfo.output_height; // 要绘制的高
    // 边界处理
    if (x + cinfo.output_width > lcd->w)
    {
        dw = lcd->w - x;
    }
    if (y + cinfo.output_height > lcd->h)
    {
        dh = lcd->h - y;
    }

    // 申请了一块内存，内存的大小就是JPEG图片一行的数据大小 = 宽像素 * 色深
    int row_stride;
    row_stride = dw * cinfo.output_components;
    // 申请一行大小的堆内存，用户来存储读取的一行数据
    unsigned char *buffer = (unsigned char *)malloc(row_stride);
    // 防止地址丢失
    unsigned char *p = lcd->mptr;
    // 多缓冲显示gif
    int show_num = 0;
    if (lcd->lcd_vinfo->yoffset == 0)
    {
        p += lcd->w * lcd->h * lcd->pix; // 移到到下一块屏幕，800*480*4
        show_num = 1;
    }
    else if (lcd->lcd_vinfo->yoffset == lcd->h)
    {
        p = lcd->mptr;
        show_num = 0;
    }

    // 把指针p移动到要绘制的开始位置
    p = p + (x * lcd->pix + (y * lcd->w * lcd->pix));
    // 6.循环读取JPEG图片的每一行的数据
    while (cinfo.output_scanline < dh)
    {
        // 每次只扫描一行
        jpeg_read_scanlines(&cinfo, &buffer, 1);

        // 扫描一行数据，并写入到LCD，需要用户自行修改  JPEG图片的像素的存储顺序 RGB
        int i = 0;
        for (i = 0; i < dw; i++)
        {
            p[i * 4] = buffer[i * 3 + 2];     // B
            p[i * 4 + 1] = buffer[i * 3 + 1]; // G
            p[i * 4 + 2] = buffer[i * 3];     // R
        }
        p += (lcd->w) * (lcd->pix); // lcd映射移到到下一行
    }
    // 可见区与虚拟区的偏移量
    lcd->lcd_vinfo->yoffset = show_num * lcd->h;
    lcd->lcd_vinfo->xoffset = 0;
    // 写入设备信息
    ioctl(lcd->lcd_fd, FBIOPAN_DISPLAY, lcd->lcd_vinfo);

    // 7.解码完成
    jpeg_finish_decompress(&cinfo);
    // 8.释放对象
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    free(buffer);
}

// 任意位置显示jpeg_gif图片
void show_gif_jpeg(LcdDevice *lcd, char *filename, int x, int y)
{

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // 1.定义解码对象以及定义错误处理对象，并且进行初始化
    cinfo.err = jpeg_std_error(&jerr); // 把错误处理对象以及解码对象进行关联
    jpeg_create_decompress(&cinfo);

    // 2.指定需要解码的文件，必须以二进制方式打开
    FILE *infile;

    if ((infile = fopen(filename, "rb")) == NULL)
    {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }

    jpeg_stdio_src(&cinfo, infile); // 把解码文件和定义了解码对象进行关联

    // 3.读取要解码的JPEG图片的文件头
    jpeg_read_header(&cinfo, TRUE);

    // 4.设置解码参数，如果不设置则使用默认值即可 （可选）

    // 5开始解码
    jpeg_start_decompress(&cinfo);

    int dw = cinfo.output_width;  // 要绘制的宽
    int dh = cinfo.output_height; // 要绘制的高
    // 边界处理
    if (x + cinfo.output_width > lcd->w)
    {
        dw = lcd->w - x;
    }
    if (y + cinfo.output_height > lcd->h)
    {
        dh = lcd->h - y;
    }

    // 申请了一块内存，内存的大小就是JPEG图片一行的数据大小 = 宽像素 * 色深
    int row_stride;
    row_stride = dw * cinfo.output_components;
    // 申请一行大小的堆内存，用户来存储读取的一行数据
    unsigned char *buffer = (unsigned char *)malloc(row_stride);
    // 防止地址丢失
    unsigned char *p = lcd->mptr;

    // 把指针p移动到要绘制的开始位置
    p = p + (x * lcd->pix + (y * lcd->w * lcd->pix));

    clock_t start, end;
    start = clock();

    // 6.循环读取JPEG图片的每一行的数据
    while (cinfo.output_scanline < dh)
    {
        // 每次只扫描一行
        jpeg_read_scanlines(&cinfo, &buffer, 1);

        // 扫描一行数据，并写入到LCD，需要用户自行修改  JPEG图片的像素的存储顺序 RGB
        int i = 0;
        for (i = 0; i < dw; i++)
        {
            p[i * 4] = buffer[i * 3 + 2];     // B
            p[i * 4 + 1] = buffer[i * 3 + 1]; // G
            p[i * 4 + 2] = buffer[i * 3];     // R
        }
        p += (lcd->w) * (lcd->pix); // lcd映射移到到下一行
    }

    end = clock();
    double secnods = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time is %.10f\n", secnods);

    // 7.解码完成
    jpeg_finish_decompress(&cinfo);
    // 8.释放对象
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    free(buffer);
}
//开机动画
void open_animation(LcdDevice *lcd)
{
    char ck[20];
    char *ck_name[260];

    for (int i = 0, j = 0; i < 260; i++)
    {
        if (i>=200)
        {
            sprintf(ck, "oa2/%d.jpg", j++);
            ck_name[i] = (char *)malloc(sizeof(ck));
            strcpy(ck_name[i], ck);
            continue;
        }
        sprintf(ck, "oa/%d.jpg", i);
        ck_name[i] = (char *)malloc(sizeof(ck));
        strcpy(ck_name[i], ck);
    }
    printf("create over\n");

    lcd->lcd_vinfo->yoffset = 0;
    lcd->lcd_vinfo->xoffset = 0;
    ioctl(lcd->lcd_fd, FBIOPAN_DISPLAY, lcd->lcd_vinfo);

    for (int i = 0; i < 260; i++)
    {
        show_gif_jpeg(lcd, ck_name[i], 0, 0);
        printf(ck_name[i]);
        printf("\n");
    }
    for (int i = 0; i < 260; i++)
    {
        free(ck_name[i]);
    }
}
//关机动画
void shutdown_a(LcdDevice *lcd)
{
    char ck[20];
    char *ck_name[60];

    for (int i = 0; i < 60; i++)
    {
        sprintf(ck, "oa2/%d.jpg", i);
        ck_name[i] = (char *)malloc(sizeof(ck));
        strcpy(ck_name[i], ck);
    }
    printf("create over\n");

    lcd->lcd_vinfo->yoffset = 0;
    lcd->lcd_vinfo->xoffset = 0;
    ioctl(lcd->lcd_fd, FBIOPAN_DISPLAY, lcd->lcd_vinfo);

    for (int i = 0; i < 60; i++)
    {
        show_gif_jpeg(lcd, ck_name[i], 0, 0);
        printf(ck_name[i]);
        printf("\n");
    }
    for (int i = 0; i < 60; i++)
    {
        free(ck_name[i]);
    }
}
// 在lcd的x,y位置显示汉字图片
static void show_f(LcdDevice *lcd, int x, int y, bitmap *mp);

void show_font(LcdDevice *lcd, char *s_font, int x, int y,int width,int height,int size)
{
    // 创建字库对象
    font *f = fontLoad("./simfang.ttf");
    if (f == NULL)
    {
        printf("fontload error\n");
        return;
    }

    // 设置字体大小
    fontSetSize(f, size);

    // screen-绘制文字区域 （800x80x4）
    bitmap *screen = createBitmapWithInit(width, height, 4, 0xFFFFFF00); // 创建用显示汉字的像素空间（800*80*4）
    // 把汉字绘制到文件区域
    fontPrint(f, screen, 0, 5, s_font, 0x00000000, 0); // RGBA
    // 把文件绘制区域显示在lcd上
    // 显示
    show_f(lcd, x, y, screen);

    // 销毁汉字显示区域
    destroyBitmap(screen);
    // 销毁字库
    fontUnload(f);
}

static void show_f(LcdDevice *lcd, int x, int y, bitmap *mp)
{
    // 根据lcd，bmp尺寸和x，y位置计算要绘制图片的宽dw，高dh
    int dw, dh;
    // 保存lcd绘制开始位置(x,y)指针
    unsigned char *p = lcd->mptr;
    // 保存bmp图片数据的开始位置p指针
    unsigned char *px = NULL;
    //判断可见区
    if (lcd->lcd_vinfo->yoffset == 0)
    {
        p = lcd->mptr;
    }
    else if (lcd->lcd_vinfo->yoffset == lcd->h)
    {
        p += lcd->w * lcd->h * lcd->pix; // 移到到下一块屏幕，800*480*4
        
    }
    //边界处理
    if (x >= 0 && y >= 0)
    {
        // 根据lcd，bmp尺寸和x，y位置计算要绘制图片的宽dw，高dh
        dw = ((x + mp->width) > lcd->w) ? (lcd->w - x) : mp->width;
        dh = ((y + mp->height) > lcd->h) ? (lcd->h - y) : mp->height;

        // 把lcd绘图指针移动x，y位置
        p +=  (x + y * lcd->w) * 4;
        // 定义一个指针指向图片像素首地址
        px = mp->map;
    }
    // 绘制
    for (int i = 0; i < dh; i++)
    {
        for (int j = 0; j < dw; j++)
        {
            // 从px开始位置拷贝3字节给到p开始位置,之后p和px 往后偏移j个像素
            memcpy(p + j * 4, px + j * mp->byteperpixel, mp->byteperpixel);
        }
        p += lcd->w * 4;                    // lcd绘制指针移动到下一行
        px += mp->width * mp->byteperpixel; // bmp图片指针移动到下一行
    }
}
 
void show_keyboard(LcdDevice *lcd, int x, int y)
{
    struct Bmp *mp = create_bmp("pic/keyboard.bmp");

    if (lcd == NULL||mp == NULL)
    {
        return;
    }
    int dw = mp->w; // 绘制的宽
    int dh = mp->h; // 绘制的高
    // 边界处理
    if (x + mp->w > lcd->w)
    {
        dw = lcd->w - x;
    }
    if (y + mp->h > lcd->h)
    {
        dh = lcd->h - y;
    }
    int d_pix = mp->pix;
    // 比较图片和屏幕的色深大小
    if (mp->pix > lcd->pix)
    {
        d_pix = lcd->pix;
    }
    // 倒转
    char *t_bmp = mp->data + mp->w * (mp->h - 1) * mp->pix;

    // 临时指针
    unsigned char *p = lcd->mptr;
    // 可见去判断
    if (lcd->lcd_vinfo->yoffset == 0)
    {
        p = lcd->mptr; 
    }
    else if (lcd->lcd_vinfo->yoffset == lcd->h)
    {
        p += lcd->w * lcd->h * lcd->pix;
    }

    // 把指针p移动到要绘制的开始位置
    p = p + (x * lcd->pix + (y * lcd->w * lcd->pix));
    // 绘制bmp图片
    for (int i = 0; i < dh; i++)
    {
        for (int j = 0; j < dw; j++)
        {
            memcpy(p + j * lcd->pix, t_bmp + j * mp->pix, d_pix);
        }
        p += lcd->w * lcd->pix;
        t_bmp -= mp->w * mp->pix;
    }
    destroy_bmp(mp);
}

void hide_keyborad(LcdDevice *lcd, int x, int y, const char *hide_keyborad_name)
{
    struct Bmp *mp = create_bmp(hide_keyborad_name);

    if (lcd == NULL || mp == NULL)
    {
        return;
    }
    int dw = mp->w; // 绘制的宽
    int dh = mp->h; // 绘制的高
    // 边界处理
    if (x + mp->w > lcd->w)
    {
        dw = lcd->w - x;
    }
    if (y + mp->h > lcd->h)
    {
        dh = lcd->h - y;
    }
    int d_pix = mp->pix;
    // 比较图片和屏幕的色深大小
    if (mp->pix > lcd->pix)
    {
        d_pix = lcd->pix;
    }
    // 倒转
    char *t_bmp = mp->data + mp->w * (mp->h - 1) * mp->pix;

    // 临时指针
    unsigned char *p = lcd->mptr;
    // 可见去判断
    if (lcd->lcd_vinfo->yoffset == 0)
    {
        p = lcd->mptr;
    }
    else if (lcd->lcd_vinfo->yoffset == lcd->h)
    {
        p += lcd->w * lcd->h * lcd->pix;
    }

    // 把指针p移动到要绘制的开始位置
    p = p + (x * lcd->pix + (y * lcd->w * lcd->pix));
    // 绘制bmp图片
    for (int i = 0; i < dh; i++)
    {
        for (int j = 0; j < dw; j++)
        {
            memcpy(p + j * lcd->pix, t_bmp + j * mp->pix, d_pix);
        }
        p += lcd->w * lcd->pix;
        t_bmp -= mp->w * mp->pix;
    }
    destroy_bmp(mp);
}

void hide_keyborad_rs(LcdDevice *lcd, int x, int y)
{
    struct Bmp *mp = create_bmp("pic/h_keyboard_rs.bmp");

    if (lcd == NULL || mp == NULL)
    {
        return;
    }
    int dw = mp->w; // 绘制的宽
    int dh = mp->h; // 绘制的高
    // 边界处理
    if (x + mp->w > lcd->w)
    {
        dw = lcd->w - x;
    }
    if (y + mp->h > lcd->h)
    {
        dh = lcd->h - y;
    }
    int d_pix = mp->pix;
    // 比较图片和屏幕的色深大小
    if (mp->pix > lcd->pix)
    {
        d_pix = lcd->pix;
    }
    // 倒转
    char *t_bmp = mp->data + mp->w * (mp->h - 1) * mp->pix;

    // 临时指针
    unsigned char *p = lcd->mptr;
    // 可见去判断
    if (lcd->lcd_vinfo->yoffset == 0)
    {
        p = lcd->mptr;
    }
    else if (lcd->lcd_vinfo->yoffset == lcd->h)
    {
        p += lcd->w * lcd->h * lcd->pix;
    }

    // 把指针p移动到要绘制的开始位置
    p = p + (x * lcd->pix + (y * lcd->w * lcd->pix));
    // 绘制bmp图片
    for (int i = 0; i < dh; i++)
    {
        for (int j = 0; j < dw; j++)
        {
            memcpy(p + j * lcd->pix, t_bmp + j * mp->pix, d_pix);
        }
        p += lcd->w * lcd->pix;
        t_bmp -= mp->w * mp->pix;
    }
    destroy_bmp(mp);
}