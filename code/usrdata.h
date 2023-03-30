#ifndef _USER_DATA_
#define _USER_DATA_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct user  //用户结构体
{
    char* phone;
    char* passwd;
    char* name;
    float money;
}user;

// 创建一个链表数据元素的结构体
typedef struct node
{
    user* Data;   // 用来存储链表中数据元素的有效数据  数据域
    struct node *Next; // 用来存储链表中数据元素的直接后继  指针域
} Node_t;

// 创建一个管理链表的结构体  头结点
typedef struct
{
    int NodeNum;  // 用来记录链表中的结点的数量
    Node_t *Head; // 用来记录链表中首结点的地址
    Node_t *Tail; // 用来记录链表中尾结点的地址
} Hnode_t;

Hnode_t *CircularLinkedList_Create(void);
void user_TailInsert(Hnode_t *manager, user *data);
bool user_find(Hnode_t *manager, char *f_Phone);
user *user_find_get(Hnode_t *manager, char *f_Phone);

#endif