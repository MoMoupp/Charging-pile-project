#include "user.h"
// 获得输入的账号 参数：账号字符串的地址，lcd结构体 ，隐藏键盘时需要的图片文件名
static void get_phnoe(char *ph_buf, LcdDevice *lcd, const char *hide_keyborad_name)
{
    show_keyboard(lcd, 600, 280);
    show_font(lcd, ph_buf, 506, 184, 264, 36, 26);
    while (1)
    {
        char key[2];
        int num = strlen(ph_buf);
        strcpy(key, key_input());
        if (strcmp(key,"C")==0)
        {
            ph_buf[0] = '\0';
            
        }
        else if (strcmp(key, "D") == 0)
        {
            if (num==0)
            {
                continue;
            }
            ph_buf[num - 1] = '\0';
        }
        else if (strcmp(key, "T") == 0 || strcmp(key, "Q") == 0)
        {
            hide_keyborad(lcd, 600, 280, hide_keyborad_name);
            return;
        }
        else if (strcmp(key, ".") == 0 || strcmp(key, "N") == 0)
        {
            continue;
        }
        else
        {
            strcat(ph_buf, key);
        }
        show_font(lcd, ph_buf, 506, 184, 264, 36, 26);
    }
    
}

// 获得输入的密码，参数：密码字符串的地址，lcd结构体 ，隐藏键盘时需要的图片文件名
static void get_passwdvoid(char *wd_buf, LcdDevice *lcd, const char *hide_keyborad_name)
{
    show_keyboard(lcd, 600, 280);
    show_font(lcd, wd_buf, 506, 244, 264, 36, 26);
    while (1)
    {
        char key[2];
        int num = strlen(wd_buf);
        strcpy(key, key_input());
        if (strcmp(key, "C") == 0)
        {
            wd_buf[0] = '\0';
        }
        else if (strcmp(key, "D") == 0)
        {
            if (num == 0)
            {
                continue;
            }
            wd_buf[num - 1] = '\0';
        }
        else if (strcmp(key, "T") == 0 || strcmp(key, "Q") == 0)
        {
            hide_keyborad(lcd, 600, 280, hide_keyborad_name);
            return;
        }
        else if (strcmp(key, ".") == 0 )
        {
            continue;
        }
        else
        {
            strcat(wd_buf, key);
        }
        show_font(lcd, wd_buf, 506, 244, 264, 36,26);
    }
}

// 添加用户 参数：链表管理结构体 需要添加的用户信息 lcd信息存储结构体,成功返回true，失败返回false
bool user_add(Hnode_t *manger, user *data, LcdDevice *lcd)
{

    bool permissions = true;
    // 判断账号是否存在
    if (user_find(manger, data->phone))
    {
        permissions = false;
    }

    if (permissions)
    {
        // 不存在则添加
        /*      char *name = (char *)malloc(70);
               strcat(name, "新用户");
               strcat(name, data->phone);
               data->name = name;
               data->money = 0.0;*/
        user_TailInsert(manger, data);
        return true;
    }
    else
    {
        show_font(lcd, "该账号已存在！", 200, 400, 250, 40,36);
        sleep(1);
        printf("该账号已存在！\n");
        return false;
    }
}

// 注册 参数：链表管理结构体 lcd信息存储结构体，成功返回用户结构体指针，失败返回NULL
user *logon(Hnode_t *manger, LcdDevice *lcd)
{
    char *phone = (char *)malloc(100);
    char *passwad = (char *)malloc(100);
logon:
    show_bmp(lcd, "pic/rsmain.bmp", 0, 0);
    //shuru
    phone[0] = '\0';
    passwad[0] = '\0';

    int x, y;
    while (1)
    {
        touch(&x, &y);
        if (y > 180 && y < 220 && x > 470 && x < 766)
        {
            get_phnoe(phone, lcd, "pic/h_keyboard_rs.bmp");
        }
        else if (y > 243 && y < 283 && x > 470 && x < 766)
        {
            get_passwdvoid(passwad, lcd, "pic/h_keyboard_rs.bmp");
        }
        else if (x > 0 && y > 0 && x < 50 && y < 45)
        {
            free(phone);
            free(passwad);
            return NULL;
        }
        else if (y > 316 && y < 352 && x > 466 && x < 755)
        {
            break;
        }
        else
        {
            hide_keyborad(lcd, 600, 280, "pic/h_keyboard_rs.bmp");
        }
    }
    //判断账号密码格式
    if (strlen(phone) != 11)
    {
        show_font(lcd, "电话号码不合法！", 200, 400, 280, 40, 36);
        sleep(1);
        printf("phone number is false\n");
        goto logon;
        /* free(phone);
        free(passwad); */
    }
    if (strlen(passwad) > 12)
    {
        printf("%d\n", strlen(passwad));
        show_font(lcd, "密码过长！", 200, 400, 180, 40, 36);
        sleep(1);
        printf("passwad longer\n");
        goto logon;
        /* free(phone);
        free(passwad); */
        //return NULL;
    }

    //创建新的堆内存
    user *new = (user *)malloc(sizeof(user));
    char *new_phone = (char *)malloc(12);
    char *new_passwad = (char *)malloc(13);
    char *name = (char *)malloc(70);
    //初始化数据
    strcpy(new_phone, phone);
    strcpy(new_passwad, passwad);
    new->phone = new_phone;
    new->passwd = new_passwad;
    //初始化用户名
    name[0] = '\0';
    strcat(name, "新用户");
    strcat(name, new->phone);
    new->name = name;
    //余额
    new->money = 0.0;

    //开始添加用户
    if (!user_add(manger,new,lcd))
    {   
        //添加失败
        printf("logon fail!\n");
        new->phone = NULL;
        new->passwd = NULL;
        // 释放输入缓冲区
        free(new->phone);
        free(new->passwd);
        free(new->name);
        free(new);
        goto logon;
    }
    printf("logon success!\n");
    // 释放输入缓冲区
    free(phone);
    free(passwad);
    return new;
}

// 登录 参数：链表管理结构体 lcd信息存储结构体，成功返回用户结构体指针，退出则放回NULL。
user *login(Hnode_t *manger, LcdDevice *lcd)
{
    char *ph = (char *)malloc(100);
    char *pswd = (char *)malloc(100);
login:    
    show_bmp(lcd, "pic/xsmain.bmp", 0, 0);
    ph[0] = '\0';
    pswd[0] = '\0';

    int x, y;
    while (1)
    {
        touch(&x, &y);        
        if (y > 180 && y < 220 && x > 470 && x < 766)
        {
            get_phnoe(ph, lcd, "pic/h_keyboard.bmp");
        }
        else if (y > 243 && y < 283 && x > 470 && x < 766)
        {
            get_passwdvoid(pswd, lcd, "pic/h_keyboard.bmp");
        }
        else if (y > 316 && y < 352 && x > 466 && x < 755)
        {
            break;
        }
        else if (x > 518 && y > 374 && x < 707 && y < 398)
        {
            user *logonData = logon(manger, lcd);
            if (logonData==NULL)
            {
                goto login;
            }
            return logonData;
        }
        //退出
        else if (x > 0 && y > 0 && x < 55 && y < 55)
        {
            free(ph);
            free(pswd);
            return NULL;
        }
        else
        {
            hide_keyborad(lcd, 600, 280, "pic/h_keyboard.bmp");
        }
        
    }
    
    // 判断账号密码格式
    if (strlen(ph) != 11)
    {
        show_font(lcd, "电话号码不合法！", 200, 400, 280, 40, 36);
        sleep(1);
        printf("phone number is false\n");
      /*   free(ph);
        free(pswd); */
        goto login;
    }
/*     if (strlen(pswd) > 12)
    {
        printf("passwad longer\n");
        free(ph);
        free(pswd);
        return NULL;
    } */

    //开始登录
    user *t_user = user_find_get(manger, ph);
    if (t_user == NULL)
    {   
        show_font(lcd, "用户不存在！", 200, 400, 220, 40, 36);
        sleep(2);
        printf("USER IS NOT exist\n");
        /* free(ph);
        free(pswd); */
        goto login;
    }
    if (strcmp(t_user->passwd, pswd) != 0)
    {
        show_font(lcd, "密码错误！", 200, 400, 200, 40, 36);
        sleep(1);
        printf("PASSWAD IS NOT ture\n");
        /* free(ph);
        free(pswd); */
        goto login;
    }
    printf("login success!\n");
    free(ph);
    free(pswd);
    return t_user;
}
