#ifndef META_CPP
#define META_CPP

#include "Meta.h"

Meta::Meta()
{
  property='\0';
  cycle=0;
}
Meta::Meta(char type, std::string input, int num)
{
  property=type;
  task=input;
  cycle=num;
  // std::cout<<property<<'{'<<task<<'}'<<cycle<<std::endl;
}
const char Meta::getType()
{
  return property;
}
const std::string Meta::getTask()
{
  return task;
}
const int Meta::getNum()
{
  return cycle;
}
#endif
