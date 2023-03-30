#include "touch.h"  //输入子系统需要使用


/*
    对于触摸屏而言，属于字符设备，所以在开发板的Linux系统已经
    默认把触摸屏的驱动程序安装在Linux内核中,当触摸屏的驱动安装
    完成后，系统内核会自动在开发板的Linux系统的/dev目录生成对
    应的设备文件，对于触摸屏而言输入输入设备，所以触摸屏的设备
    文件的路径：/dev/input/event0

*/
void touch(int *ts_x, int *ts_y)
{
    //1.打开触摸屏，触摸屏文件 "/dev/input/event0"
    int ts_fd = open("/dev/input/event0",O_RDWR);
    if(-1 == ts_fd)
    {
        perror("open touch screen error");
        exit(1);
    }
    *ts_x = -1;
    *ts_y = -1;
    int f_x = -1, f_y = -1;
    //2.定义一个input_event结构体类型变量来保存read的数据
    struct input_event ts_event;

/*     while (1)
    {
        read(ts_fd, &ts_event, sizeof(ts_event));
        if (ts_event.type == EV_ABS)
        {
            // 如果输入设备是触摸屏，则判断X轴以及Y轴
            if (ts_event.code == ABS_X)
            {
                *ts_x = ts_event.value * 800 / 1024;
            }
            if (ts_event.code == ABS_Y)
            {
                *ts_y = ts_event.value * 480 / 600;
            }
        }
        if (*ts_x!=-1&&*ts_y!=-1)
        {
            f_x = *ts_x;
            f_y = *ts_y;
            break;
        }
        
    } */
    
    while(1)
    {
        //3.分析读取的数据并进行输出，先判断输入设备的类型(EV_ABS表示触摸屏)
        read(ts_fd,&ts_event,sizeof(ts_event));
        if(ts_event.type == EV_ABS)
        {
            //如果输入设备是触摸屏，则判断X轴以及Y轴
            if (ts_event.code == ABS_X)
            {
                *ts_x = ts_event.value * 800 / 1024;
            }
            if (ts_event.code == ABS_Y)
            {
                *ts_y = ts_event.value * 480 / 600;
            }
            
        }       
        // 表示松开
        if (ts_event.type == EV_KEY && ts_event.code == BTN_TOUCH && ts_event.value == 0)
        {
            printf("x:%d y:%d\n", *ts_x, *ts_y);
            break;
        }
    }
    if (*ts_x>f_x)
    {
        //printf("右滑\n");
    }
    else
    {
        //printf("左滑\n");
    }
    //4.关闭触摸屏
    close(ts_fd);

}

char *key_input()
{

    int x, y;
    touch(&x, &y);
    if (x > 620 && y > 300 && x < 650 && y < 330)
    {
        return "1";
    }
    else if (y > 300 && y < 330 && x > 660 && x < 690)
    {
        return "2";
    }
    else if (y > 300 && y < 330 && x > 700 && x < 730)
    {
        return "3";
    }
    else if (y > 343 && y < 375 && x > 620 && x < 650)
    {
        return "4";
    }
    else if (y > 343 && y < 375 && x > 660 && x < 690)
    {
        return "5";
    }
    else if (y > 343 && y < 375 && x > 700 && x < 730)
    {
        return "6";
    }
    else if (y > 385 && y < 416 && x > 620 && x < 650)
    {
        return "7";
    }
    else if (y > 385 && y < 416 && x > 660 && x < 690)
    {
        return "8";
    }
    else if (y > 385 && y < 416 && x > 700 && x < 730)
    {
        return "9";
    }
    else if (y > 426 && y < 475 && x > 620 && x < 650)
    {
        return "0";
    }
    else if (y > 343 && y < 375 && x > 740 && x < 785)
    {
        return "D";
    }
    else if (y > 385 && y < 416 && x > 740 && x < 785)
    {
        return "C";
    }
    else if (y > 426 && y < 475 && x > 700 && x < 785)
    {
        return "T";
    }
    else if (y > 426 && y < 475 && x > 660 && x < 700)
    {
        return ".";
    }
    else if (y > 300 && y < 330 && x > 740 && x < 785)
    {
        return "Q";
    }
    else
    {
        return "Q";
    }
}