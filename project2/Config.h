#ifndef CONFIG_H
#define CONFIG_H

struct Config
{
  float conf_version;
  char conf_meta[255];
  int conf_monitor;
  int conf_processor;
  int conf_scanner;
  int conf_harddrive;
  int conf_keyboard;
  int conf_memory;
  int conf_projector;
  std::string conf_logtype;
  char conf_logpath[255];
  int conf_sysmemory;
};

#endif
