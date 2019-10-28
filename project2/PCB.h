#ifndef PCB_H
#define PCB_H

#include "Meta.h"

struct PCB
{
  int processState; //0=start | 1=ready | 2=running | 3=wait | 4= exit
};

struct paramStruct
{
  int delay;
  PCB state;
};


#endif
