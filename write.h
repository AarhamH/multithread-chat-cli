#ifndef WRITE
#define WRITE

void SignalWriter(); 
void SetupWriter(char* HostArg, List* ListArg);
void CancelWriter();
void CloseWriter();

#endif