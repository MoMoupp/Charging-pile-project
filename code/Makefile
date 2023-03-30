#万能makefile
CC:=arm-linux-gcc  #编译环境
TARGET:=Charging_pile  #变量TARGET 可执行文件名
DATAFILE:=userdata.txt #数据库文件名
FLAGS:=-I. -L. -ljpeg -lm #链接库
#SRCS= main.c  font.c  truetype.c  lcddevice.c
SRCS:=$(wildcard *.c)  #搜索当前目录下面的所有.c文件 （把所有.c的文件名称保存到SRCS变量中）
#OBJS=main.o  font.o  truetype.o  lcddevice.o
OBJS:=$(patsubst %.c,%.o,$(SRCS)) #把SRCS里面的.c 换为.o赋值给OBJS

$(TARGET):$(OBJS)  #使用变量
	$(CC) -o $@  $^   $(FLAGS)
	rm $(OBJS)

%.o:%.c 
	$(CC) -c $< -o $@  $(FLAGS)
clean:
	rm $(TARGET)