#ifndef WRITE
#define WRITE

void SignalWriter();
void SetupWriter(char* HostArg, List* l);
void CancelWriter();
void CloseWriter();

#endif