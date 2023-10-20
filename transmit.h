#ifndef _SEND_UDP_H
#define _SEND_UDP_H

#include "list.h"

void senderInit(char* hostnm, char* p, List* l);
void senderCancel();
void senderShutdown();
void senderSignaller();

#endif