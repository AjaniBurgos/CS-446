#ifndef META_H
#define META_H

#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<sstream>
#include<vector>
#include<sstream>
#include<stdlib.h>
#include<cstdio>
#include<pthread.h>
#include<unistd.h>
#include<typeinfo>
#include<math.h>

#include "Config.h"
#include "PCB.h"

class Meta
{
  public:
    Meta();
    Meta(char type, std::string input, int num);
    const char getType();
    const std::string getTask();
    const int getNum();
  private:
    char property;
    std::string task;
    int cycle;
};

#endif
