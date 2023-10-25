#ifndef TRANSMIT
#define TRANSMIT

void SetupTransmit(char* HostArg, char* PortArg, List* ListArg);
void CancelTransmit();
void CloseTransmit();
void SignalTransmit();

#endif