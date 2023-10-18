#ifndef TRANSMIT_H
#define TRANSMIT_H

void senderInit(char* HostnameArg, char* PortArg, List* ListArg);
void senderCancel();
void senderShutdown();
void senderSignaller();


#endif

