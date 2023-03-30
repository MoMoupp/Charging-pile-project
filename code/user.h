#ifndef _USER_
#define _USER_

#include "usrdata.h"
#include "touch.h"
#include "showpic.h"

bool user_add(Hnode_t *manger, user *data, LcdDevice *lcd);
user *logon(Hnode_t *manger, LcdDevice *lcd);
user *login(Hnode_t *manger, LcdDevice *lcd);
#endif