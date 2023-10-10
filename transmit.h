#ifndef TRANSMIT_H
#define TRANSMIT_H

#include "list.h"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// Functions
struct addrinfo SetUpHints(struct addrinfo HintsArg);
int SetUpSocket(int SocketArg);
void SetupUDPSender(char* hostname, char* port, List* list);
void CloseUDPSender();

#endif

