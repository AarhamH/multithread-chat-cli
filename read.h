#ifndef _READ_INPUT_H
#define _READ_INPUT_H

#include "list.h"

// Reads input from the keyboard
// Starts a new pthread

void readerInit(List* l);
void readerShutdown();
void readerCancel();

#endif