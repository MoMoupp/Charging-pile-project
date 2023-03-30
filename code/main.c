#include "user.h"
#include "showpic.h"

// 从文本读取数据 参数:数据文件名，链表管理结构体，lcd结构体
static void user_data_read(char *file, Hnode_t *manger, LcdDevice *lcd);
// 保存数据到文本，参数:数据文件名，链表管理结构体
static void usre_data_save(char *file, Hnode_t *manger);
//主界面
static void dl_main(LcdDevice *lcd, Hnode_t *manger);
// 选择充电方式界面，参数:lcd结构体,当前登录用户结构体地址
static void cd_main(LcdDevice *lcd, user *now_user);
// 支付选择界面，参数:lcd结构体,当前登录用户结构体地址
static void c_pay(LcdDevice *lcd, user *now_user);
// 扫码支付界面，参数:lcd结构体,当前登录用户结构体地址
static void SM_pay(LcdDevice *lcd, user *now_user);
// 余额判断，参数:lcd结构体,当前登录用户结构体地址
static void YR_pay(LcdDevice *lcd, user *now_user);
// 余额不足界面，参数:lcd结构体,当前登录用户结构体地址
static void YR_pay_f(LcdDevice *lcd, user *now_user);
// 余额足够界面，参数:lcd结构体,当前登录用户结构体地址
static void YR_pay_t(LcdDevice *lcd, user *now_user);
// 正在充电界面，参数:lcd结构体,当前登录用户结构体地址
static void now_cd(LcdDevice *lcd);
//获得充值金额
static void get_money(LcdDevice *lcd, user *now_user, char *money);

// 主函数入口
int main(int argc, char **argv)
{   
    //创建链表
    Hnode_t *manger = CircularLinkedList_Create();
    //获得lcd参数
    LcdDevice *lcd = create_lcd("/dev/fb0");
    // 开机动画
    open_animation(lcd);
    
    //读取数据库数据
    user_data_read("userdata.txt", manger, lcd);
    //主界面
    dl_main(lcd, manger);
    //关机
    shutdown_a(lcd);
    //保存用户数据到数据库
    usre_data_save("userdata.txt", manger);
    lcd->clear(0x00000000);
    //lcd解除关联
    destroy_lcd(lcd);
    return 0;
}



//从文本读取数据
static void user_data_read(char *file, Hnode_t *manger, LcdDevice *lcd)
{
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        perror("无初始信息");
        return;
    }
    while (1)
    {
        char all_data[200];
        if (fgets(all_data, 200, fp) == NULL)
        {
            perror("fgts:");
            break;
        }
        user *data = malloc(sizeof(user));
        data->name = malloc(20);
        data->passwd = malloc(12);
        data->phone = malloc(12);
        //分段读取
        sscanf(all_data, "%[^@]@%[^@]@%[^@]@%f[^/n]", data->phone, 
                                                    data->passwd, 
                                                    data->name, 
                                                    &(data->money));
        printf("%s:%s:%s:%.2f\n", data->phone,data->passwd,data->name,data->money);
        user_add(manger, data, lcd);
        
    }
    printf("read over\n");
    fclose(fp);
}
//保存数据到文本
static void usre_data_save(char *file, Hnode_t *manger)
{
    FILE *fp = fopen(file, "w+");
    if (fp == NULL)
    {
        perror("fopen");
        return;
    }
    Node_t *Phead = manger->Head;
    while (Phead!=NULL)
    {
        fprintf(fp, "%s@%s@%s@%.2f\n", Phead->Data->phone,
                Phead->Data->passwd,
                Phead->Data->name, 
                Phead->Data->money);
        Phead = Phead->Next;
        if (Phead==manger->Head)
        {
            printf("save over\n");
            break;
        }
    }
}

static void dl_main(LcdDevice *lcd,Hnode_t* manger)
{
    while (1)
    {
        show_bmp(lcd, "pic/dlmain.bmp", 0, 0);
        int x, y;
        touch(&x, &y);
        if (x > 600 && y > 95 && x < 690 && y < 180)
        {
            // 退出
            return;
        }
        else if (x > 281 && y > 105 && x < 360 && y < 185)
        {
            user *now_user;
            while (1)
            {
                now_user = login(manger, lcd);
                // 退出
                if (now_user == NULL)
                {
                    break;
                }
                cd_main(lcd, now_user);
            }
        }
        else
        {
            continue;
        }
    }
}

// 选择充电方式界面
static void cd_main(LcdDevice *lcd, user *now_user)
{ 
    // 显示用户数据
    char *show_data = malloc(60);
    show_data[0] = '\0';
    
keep:
    sprintf(show_data, "%s 余额:%.2f", now_user->name, now_user->money);
    show_bmp(lcd, "pic/cd.bmp", 0, 0);
    show_font(lcd, show_data, 0, 430, 380, 36, 26);
    while (1)
    {
        int x, y;
        touch(&x, &y);
        if (x > 0 && y > 0 && x < 70 && y < 65)
        {   
            //退出登录
            free(show_data);
            return;
        }
        else if (x > 127 && y > 152 && x < 256 && y < 327)
        {   
            //选择支付方式
            c_pay(lcd, now_user);
            goto keep;
        }
        else if (x > 546 && y > 157 && x < 675 && y < 330)
        { 
            // 选择支付方式
            c_pay(lcd, now_user);
            goto keep;
        }
        else
        {
            continue;
        }
    }
}

static void c_pay(LcdDevice *lcd, user *now_user)
{   
    //显示用户数据
    char *show_data = malloc(60);
    show_data[0] = '\0';
    sprintf(show_data, "%s 余额:%.2f", now_user->name, now_user->money);
    show_bmp(lcd, "pic/c_pay.bmp", 0, 0);
    show_font(lcd, show_data, 0, 430, 380, 36, 26);
    while (1)
    {
        int x, y;
        touch(&x, &y);
        if (x > 705 && y > 0 && x < 800 && y < 90)
        {
            // 返回
            free(show_data);
            return;
        }
        else if (x > 53 && y > 160 && x < 222 && y < 311)
        {   
            //余额支付
            YR_pay(lcd, now_user);
            free(show_data);
            return;
        }
        else if (x > 370 && y > 155 && x < 500 && y < 328)
        {   
            //扫码支付
            SM_pay(lcd, now_user);
            free(show_data);
            return;
        }
        else
        {
            continue;
        }
    }
}

static void YR_pay(LcdDevice *lcd, user *now_user)
{   
    //简单判断
    if (now_user->money >= 8)
    {
        
        YR_pay_t(lcd, now_user);
    }
    else
    {
        YR_pay_f(lcd, now_user);
    }
}

static void YR_pay_f(LcdDevice *lcd, user *now_user)
{
    // 显示用户数据
    char *show_data = malloc(60);
    show_data[0] = '\0';
    sprintf(show_data, "%s 余额:%.2f", now_user->name, now_user->money);
    show_bmp(lcd, "pic/YR_false.bmp", 0, 0);
    show_font(lcd, show_data, 0, 80, 380, 36, 26);

    while (1)
    {
        int x, y;
        touch(&x, &y);
        if (x > 705 && y > 0 && x < 800 && y < 90)
        {
            // 返回
            free(show_data);
            return;
        }
        else if (x > 440 && y > 350 && x < 592 && y < 392)
        {
            //充值余额
            SM_pay(lcd, now_user);
            free(show_data);
            return;
        }
        else
        {
            continue;
        }
    }
}

static void YR_pay_t(LcdDevice *lcd, user *now_user)
{
    // 显示用户数据
    char *show_data = malloc(60);
    show_data[0] = '\0';
    sprintf(show_data, "%s 余额:%.2f", now_user->name, now_user->money);
    show_bmp(lcd, "pic/YR_ture.bmp", 0, 0);
    show_font(lcd, show_data, 0, 80, 380, 36, 26);

    while (1)
    {
        int x, y;
        touch(&x, &y);
        if (x > 305 && y > 310 && x < 434 && y < 363)
        {
            //结算，开始充电
            now_user->money -= 8;
            now_cd(lcd);
            free(show_data);
            return;
        }
        else
        {
            continue;
        }
    }
}

static void SM_pay(LcdDevice *lcd, user *now_user)
{
    char money[20] = "100";
    // 显示用户数据
    char *show_data = malloc(60);
    show_data[0] = '\0';
    sprintf(show_data, "%s 余额:%.2f", now_user->name, now_user->money);
    show_bmp(lcd, "pic/sm_pay.bmp", 0, 0);
    show_font(lcd, show_data, 0, 80, 380, 36, 26);
    while (1)
    {
        int x, y;
        touch(&x, &y);
        if (x > 705 && y > 0 && x < 800 && y < 90)
        {
            // 返回
            free(show_data);
            return;
        }
        else if (y > 252 && y < 322 && x > 54 && x < 215)
        {   
            //修改充值金额
            get_money(lcd, now_user,money);
        }
        else if (x > 280 && y > 364 && x < 527 && y < 410)
        {   
            //结算,开始充电
            float money_f;
            money_f = atof(money);
            now_user->money += money_f;
            if (now_user->money>=8)
            {
                now_user->money -= 8;
                now_cd(lcd);
                free(show_data);
                return;
            }
            show_font(lcd, "余额不足！", 200, 400, 200, 40, 36);
            sleep(1);
            free(show_data);
            return;
        }
        else
        {
            hide_keyborad(lcd, 600, 280, "pic/k_sm_pay.bmp");
            continue;
        }
    }
}

//显示充电中
static void now_cd(LcdDevice *lcd)
{
    show_bmp(lcd, "pic/cd_now.bmp", 0, 0);
    while (1)
    {
        int x, y;
        touch(&x, &y);
        if (x > 478 && y > 209 && x < 657 && y < 244)
        {
            
            return;
        }
        else
        {
            continue;
        }
    }
    
    
}

static void get_money(LcdDevice *lcd, user *now_user, char *money)
{
    //money[0]='\0';
    show_keyboard(lcd, 600, 280);
    show_font(lcd, money, 83, 266, 150, 36, 26);
    while (1)
    {
        char key[2];
        int num = strlen(money);
        
        strcpy(key, key_input());
        if (strcmp(key, "C") == 0)
        {
            money[0] = '\0';
        }
        else if (strcmp(key, "D") == 0)
        {
            if (num == 0)
            {
                continue;
            }
            money[num - 1] = '\0';
        }
        else if (strcmp(key, "T") == 0 || strcmp(key, "Q") == 0)
        {
            hide_keyborad(lcd, 600, 280, "pic/k_sm_pay.bmp");
            return;
        }
        else if (strcmp(key, "N") == 0)
        {
            continue;
        }
        else
        {
            strcat(money, key);
        }
        show_font(lcd, money, 83, 266, 150, 36, 26);
    }
    if (money[0]=='.')
    {
        money[0] = '0';
        money[1] = '\0';
    }
    show_font(lcd, money, 83, 266, 150, 36, 26);
}