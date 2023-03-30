#include "usrdata.h"

// 创建
Hnode_t * CircularLinkedList_Create(void)
{
    // 1.为头结点申请一块堆内存，并对头结点中得成员进行初始化
    Hnode_t *manager = (Hnode_t *)malloc(sizeof(Hnode_t));

    // 2.判断堆内存是否申请成功
    if (NULL != manager)
    {
        manager->NodeNum = 0; // 由于现在的链表中没有任何元素，所以初始化结点数量为0
        manager->Head = NULL;
        manager->Tail = NULL;
    }
    else
    {
        perror("Malloc Memory For Manager is Error!");
        return NULL; // C语言中的内存分布，如果使用的是32位系统，则虚拟内存是4G，从内存地址开始的一部分是保留区，NULL就指向这块内存空间，这块内存用户没有访问权限
    }
    // 3.把manager变量进行返回
    return manager;
}

// 创建一个新结点 ---> 设计程序的时候需要遵循一个原则“高内聚，低耦合” --->内聚指的是一个模块内部的紧密关系  耦合指的是两个模块之间的紧密关系
static Node_t *CircularLinkedList_CreateNode(user *data)
{
    // 1.需要为新结点申请一块堆内存
    Node_t *newnode = (Node_t *)malloc(sizeof(Node_t));

    // 2.判断堆内存是否申请成功，如果申请成功，则对结点进行初始化
    if (NULL != newnode)
    {
        newnode->Data = data; // 把数据存储到结点的数据域中
        newnode->Next = NULL; // 由于此时并没有其他结点，所以指针域指向NULL
    }
    else
    {
        perror("Malloc Memory For NewNode is Error!");
        return NULL; // C语言中的内存分布，如果使用的是32位系统，则虚拟内存是4G，从内存地址开始的一部分是保留区，NULL就指向这块内存空间，这块内存用户没有访问权限
    }

    return newnode;
}

// 判断链表中是否存在结点
static bool CircularLinkedList_IsEmpty(Hnode_t *manager)
{
    return (manager->Head == NULL);
}

// 利用尾部插入的方式实现结点的录入
void user_TailInsert(Hnode_t *manager, user *data)
{
    Hnode_t *pmanager = manager; // 防止头结点的地址丢失，所以进行地址备份，使用备份地址

    // 1.创建一个新结点
    Node_t *newnode = CircularLinkedList_CreateNode(data);

    // 2.判断链表中此时是否存在结点，如果此时链表中没有结点存在，则新结点就作为链表的首结点，如果此时链表中已经存在部分结点，则新结点就作为链表的尾结点
    if (CircularLinkedList_IsEmpty(manager))
    {
        pmanager->Head = newnode; // 把新结点的地址作为首结点的地址进行记录
        pmanager->Tail = newnode; // 由于该结点作为链表中得第一个结点，所以此时把新结点的地址作为尾结点的地址进行记录
    }
    else
    {
        pmanager->Tail->Next = newnode; // 为了把新结点加入到链表中连接起来
        newnode->Next = pmanager->Head; // 由于设计的是循环链表，所以需要把新的尾结点的指针域指向链表的首结点
    }

    // 3.由于新增了一个结点，所以链表头结点的结点数量+1
    pmanager->NodeNum++;

    // 4.由于尾结点的地址更新了，所以需要修改头结点中存储尾结点地址的成员
    pmanager->Tail = newnode;
}

// 遍历链表的所有元素，按电话号码 查找相应数据存在则返回ture，否则返回false
bool user_find(Hnode_t *manager,char *f_Phone)
{

    // 为了防止首结点的地址丢失，需要对首结点地址进行备份，使用备份地址
    Node_t *Phead = manager->Head;

    //循环的从链表的首结点向后遍历，然后输出每个结点的数据域
    while (1)
    {
        if (Phead == NULL)
        {
            break;
        }
        //利用strcmp函数比较字符串
        if (strcmp(f_Phone, Phead->Data->phone)==0)
        {
            return true;
        }
        
        Phead = Phead->Next; // 把直接后继的元素地址进行更改

        // 由于是循环链表，所以当向后遍历的过程中临时指针变量存储的地址又再次等于首地址，则说明遍历了一遍，此时while()退出
        if (manager->Head == Phead)
        {
            break;
        }
    }
    //遍历完没有找到
    return false;
}

// 遍历链表的所有元素 , 按电话号码查找相应数据存在则返回该user地址，否则返回NULL
user* user_find_get(Hnode_t *manager, char *f_Phone)
{

    // 为了防止首结点的地址丢失，需要对首结点地址进行备份，使用备份地址
    Node_t *Phead = manager->Head;

    // 循环的从链表的首结点向后遍历，然后输出每个结点的数据域
    while (1)
    {
        if (Phead == NULL)
        {
            break;
        }
        // 利用strcmp函数比较字符串
        if (strcmp(f_Phone, Phead->Data->phone) == 0)
        {
            return Phead->Data;
        }

        Phead = Phead->Next; // 把直接后继的元素地址进行更改

        // 由于是循环链表，所以当向后遍历的过程中临时指针变量存储的地址又再次等于首地址，则说明遍历了一遍，此时while()退出
        if (manager->Head == Phead)
        {
            break;
        }
    }
    // 遍历完没有找到
    return NULL;
}