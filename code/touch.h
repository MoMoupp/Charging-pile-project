#ifndef _TOUCH_
#define _TOUCH_
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/input.h>
//触摸，获得x，y坐标
void touch(int *ts_x, int *ts_y);
//键盘输入
char *key_input();
#endif