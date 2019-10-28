#include "Meta.h"

bool read_meta(char* filename, std::string &metadata); //reads the meta-data file, checks for validity, save the meta-data statistics
bool read_config(char* filename, std::string& config, std::string &metadata, Config* confdata); //reads the config file, checks for validity, save the config variables
void time_log(std::vector<Meta> data, std::vector<int> calculations, PCB state, std::string logtype, char* logpath); //creates the timelog output
void* thread_delay(void* calc);

int main(int argc, char* argv[]) //main simply reads config & meta, which then creates an output configuration data file by carrying out the arithmetic from the variables from each file
{
  //VARIABLES:
  std::string config;
  std::string metadata;
  std::string temp;
  std::string num;
  int index=0;
  int cycle;
  char type;
  Config confdata;
  PCB state;
  std::vector<Meta> data;
  std::vector<int> calculations;

  //LOGIC & ARITHMETIC:
  if(argc==1)
  {
    std::cout << "ERROR: No file name specified." << std::endl;
  }
  if(argc>1)
  {
    if(!(read_config(argv[1], config, metadata, &confdata) && read_meta(confdata.conf_meta, metadata)))
    {
      std::cout<<"ERROR: Could not successfully scan "<<argv[1] << ". Possible typo?"<<std::endl;
      return 1;
    }

    //scanning through metadata string for stuff we actually care about
    while(metadata[index]!='.')
    {
      if(metadata[index]>='A' && metadata[index]<='Z' && metadata[index+1]=='{')
      {
        type=metadata[index];
        if(metadata[index+1]=='{')
        {
          //need to get string of task
          while(metadata[index] != '}')
          {
            temp+=metadata[index];
            index++;
          }
          index++;
          while(metadata[index]>='0' && metadata[index]<='9' && metadata[index]!=';')
          {
            num+=metadata[index];
            index++;
            cycle = atoi(num.c_str());
          }
        }
        //getting value associated with task
        temp.erase(0,2);
        temp.erase(temp.size(), temp.size()-1);
        //using paramaterized constructor and adding it to out vector
        Meta element(type, temp, cycle);
        data.push_back(element);
        //clearing strings after each iteration
        num.clear();
        temp.clear();
      }
      index++;
    }

    //we now do our calculations here, storing it into a vector
    for(int index=0;index<data.size();index++)
    {
      if(data[index].getTask()=="run" && data[index].getType()=='P')
      {
        calculations.push_back(data[index].getNum() * confdata.conf_processor);
      }
      if(data[index].getType()=='M')
      {
        calculations.push_back(data[index].getNum() * confdata.conf_memory);
      }
      if(data[index].getType()=='O')
      {
        if(data[index].getTask()=="projector")
        {
          calculations.push_back(data[index].getNum() * confdata.conf_projector);
        }
        if(data[index].getTask()=="monitor")
        {
          calculations.push_back(data[index].getNum() * confdata.conf_monitor);
        }
        if(data[index].getTask()=="hard drive")
        {
          calculations.push_back(data[index].getNum() * confdata.conf_harddrive);
        }
      }
      if(data[index].getType()=='I')
      {
        if(data[index].getTask()=="keyboard")
        {
          calculations.push_back(data[index].getNum() * confdata.conf_keyboard);
        }
        if(data[index].getTask()=="scanner")
        {
          calculations.push_back(data[index].getNum() * confdata.conf_scanner);
        }
        if(data[index].getTask()=="hard drive")
        {
          calculations.push_back(data[index].getNum() * confdata.conf_harddrive);
        }
      }
    }
  }
  //RESULTS:

  /*create output file, and write the calculated statistics to it*/
  // now we want to generate output, we'll simply write it out using ofstream

  time_log(data, calculations, state, confdata.conf_logtype, confdata.conf_logpath);

  return 0;
}

bool read_meta(char* filename, std::string &metadata) //reads the meta-data file, checks for validity, save the meta-data statistics
{
  std::string temp;
  std::ifstream fin;

  fin.open(filename);

  getline(fin, temp);

  if(temp != "Start Program Meta-Data Code:")
  {
    std::cout << "ERROR: Could not find file '" << filename << "'. Possible typo in filename or does not exist." << std::endl;
    return false;
  }

  while(getline(fin, metadata))
  {
    temp+=' ';
    temp+=metadata;
  }
  metadata=temp;
}

bool read_config(char* filename, std::string& config, std::string &metadata, Config* confdata) //reads the config file, checks for validity, save the config variables
{
  std::string output;
  std::string temp;
  std::string metafile;
  std::ifstream fin;
  bool file_check=false; //used to check if it's desirable to make a file

  fin.open(filename);
  getline(fin, temp);

  if(temp != "Start Simulator Configuration File") //if there is a typo in the filename, if there is a typo in the config file itself, it's missing a number, etc.
  {
    std::cout << "ERROR: '" << filename << "' is not in proper format. Possible typo in file or filename." << std::endl;
    return false;
  }

  int index=0;
  while(getline(fin, temp, ':')) //now we record data we care about
  {
    if(temp=="Version/Phase")
    {
      getline(fin, temp);
      temp.erase(0,1);
      confdata->conf_version=atof(temp.c_str());
    }
    else if(temp=="File Path")
    {
      getline(fin, temp);
      temp.erase(0,1);
      strcpy(confdata->conf_meta,temp.c_str());
    }
    else if(temp=="Monitor display time {msec}")
    {
      getline(fin, temp);
      temp.erase(0,1);
      confdata->conf_monitor=atoi(temp.c_str());
      if(confdata->conf_monitor==0)
      {
        return false;
      }
    }
    else if(temp=="Processor cycle time {msec}")
    {
      getline(fin, temp);
      temp.erase(0,1);
      confdata->conf_processor=atoi(temp.c_str());
      if(confdata->conf_processor==0)
      {
        return false;
      }
    }
    else if(temp=="Scanner cycle time {msec}")
    {
      getline(fin, temp);
      temp.erase(0,1);
      confdata->conf_scanner=atoi(temp.c_str());
      if(confdata->conf_scanner==0)
      {
        return false;
      }

    }
    else if(temp=="Hard drive cycle time {msec}")
    {
      getline(fin, temp);
      temp.erase(0,1);
      confdata->conf_harddrive=atoi(temp.c_str());
      if(confdata->conf_harddrive==0)
      {
        return false;
      }
    }
    else if(temp=="Keyboard cycle time {msec}")
    {
      getline(fin, temp);
      temp.erase(0,1);
      confdata->conf_keyboard=atoi(temp.c_str());
      if(confdata->conf_keyboard==0)
      {
        return false;
      }
    }
    else if(temp=="Memory cycle time {msec}")
    {
      getline(fin, temp);
      temp.erase(0,1);
      confdata->conf_memory=atoi(temp.c_str());
      if(confdata->conf_memory==0)
      {
        return false;
      }
    }
    else if(temp=="Projector cycle time {msec}")
    {
      getline(fin, temp);
      temp.erase(0,1);
      confdata->conf_projector=atoi(temp.c_str());
      if(confdata->conf_projector==0)
      {
        return false;
      }
    }
    else if(temp=="System memory {kbytes}")
    {
      getline(fin, temp);
      temp.erase(0,1);
      confdata->conf_sysmemory=atoi(temp.c_str());
      if(confdata->conf_sysmemory==0)
      {
        return false;
      }
    }
    else if(temp=="Log")
    {
      getline(fin,temp);
      temp.erase(0,1);
      confdata->conf_logtype=temp;
    }
    else if(temp=="Log File Path")
    {
      getline(fin, temp);
      temp.erase(0,1);
      strcpy(confdata->conf_logpath, temp.c_str());
    }
    else if(temp=="End Simulator Configuration File\n")
    {
      break;
    }
    else //There's a typo in config file; break program
    {
      return false;
    }
  }

  return true;
}

void time_log(std::vector<Meta> data, std::vector<int> calculations, PCB state, std::string logtype, char* logpath) //creates the timelog output
{
  paramStruct param;
  int processnum;
  unsigned int tempaddress;
  srand(time(NULL));
  clock_t timer;
  timer = clock();

  //generating output
  if(logtype == "Log to Both" || logtype == "Log to File")
  {
    std::ofstream fout;
    fout.open(logpath);
    for(int index=0;index<calculations.size();index++)
    {
      param.delay=calculations[index];
      if(data[index].getType()=='S' && data[index].getTask()=="start")
      {
        fout << (float)timer/CLOCKS_PER_SEC <<'-'<<" simulator program starting"<<std::endl;
      }
      else if(data[index].getType()=='A' && data[index].getTask()=="start")
      {
        timer = clock();
        processnum=data[index].getNum()+1;
        state.processState=0;
        fout << (float)timer/CLOCKS_PER_SEC << "- OS: preparing process " << processnum << std::endl;
        timer = clock();
        fout << (float)timer/CLOCKS_PER_SEC << "- OS: starting process " << processnum << std::endl;
      }
      else if(data[index].getType()=='P')
      {
        pthread_t pthread;
        timer=clock();
        fout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": start processing action" << std::endl;
        pthread_create(&pthread, NULL, thread_delay, (void*) &param);
        timer=clock();
        fout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": end processing action" << std::endl;
      }
      else if(data[index].getType()=='M')
      {
        if(data[index].getTask()=="allocate")
        {
          pthread_t pthread;
          timer=clock();
          fout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": allocating memory" << std::endl;
          pthread_create(&pthread, NULL, thread_delay, (void*) &param);
          tempaddress = rand();
          timer=clock();
          fout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": memory allocated at 0x" << std::hex << tempaddress << std::dec << std::endl;
        }
        else if(data[index].getTask()=="block")
        {
          pthread_t pthread;
          timer=clock();
          fout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": start memory blocking" << std::endl;
          pthread_create(&pthread, NULL, thread_delay, (void*) &param);
          timer=clock();
          fout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": end memory blocking" << std::endl;
        }
      }
      else if(data[index].getType()=='O')
      {
        pthread_t pthread;
        timer=clock();
        fout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": start " << data[index].getTask() << " output" << std::endl;
        pthread_create(&pthread, NULL, thread_delay, (void*) &param);
        timer=clock();
        // std::cout<<std::endl;
        fout << (float)timer/CLOCKS_PER_SEC + (param.delay/1000) << "- Process " << processnum << ": end " << data[index].getTask() << " output" << std::endl;
        // std::cout<<std::endl;
      }
      else if(data[index].getType()=='I')
      {
        pthread_t pthread;
        timer=clock();
        fout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": start " << data[index].getTask() << " input" << std::endl;
        pthread_create(&pthread, NULL, thread_delay, (void*) &param);
        timer=clock();
        fout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": end " << data[index].getTask() << " input" << std::endl;
      }
    }
    fout.close();
  }
  if(logtype == "Log to Both" || logtype == "Log to Monitor")
  {
    for(int index=0;index<calculations.size();index++)
    {
      param.delay=calculations[index];
      if(data[index].getType()=='S' && data[index].getTask()=="start")
      {
        std::cout << (float)timer/CLOCKS_PER_SEC <<'-'<<" simulator program starting"<<std::endl;
      }
      else if(data[index].getType()=='A' && data[index].getTask()=="start")
      {
        timer = clock();
        processnum=data[index].getNum()+1;
        state.processState=0;
        std::cout << (float)timer/CLOCKS_PER_SEC << "- OS: preparing process " << processnum << std::endl;
        timer = clock();
        std::cout << (float)timer/CLOCKS_PER_SEC << "- OS: starting process " << processnum << std::endl;
      }
      else if(data[index].getType()=='P')
      {
        pthread_t pthread;
        timer=clock();
        std::cout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": start processing action" << std::endl;
        pthread_create(&pthread, NULL, thread_delay, (void*) &param);
        timer=clock();
        std::cout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": end processing action" << std::endl;
      }
      else if(data[index].getType()=='M')
      {
        if(data[index].getTask()=="allocate")
        {
          pthread_t pthread;
          timer=clock();
          std::cout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": allocating memory" << std::endl;
          pthread_create(&pthread, NULL, thread_delay, (void*) &param);
          tempaddress = rand();
          timer=clock();
          std::cout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": memory allocated at 0x" << std::hex << tempaddress << std::dec << std::endl;
        }
        else if(data[index].getTask()=="block")
        {
          pthread_t pthread;
          timer=clock();
          std::cout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": start memory blocking" << std::endl;
          pthread_create(&pthread, NULL, thread_delay, (void*) &param);
          timer=clock();
          std::cout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": end memory blocking" << std::endl;
        }
      }
      else if(data[index].getType()=='O')
      {
        pthread_t pthread;
        timer=clock();
        pthread_create(&pthread, NULL, thread_delay, (void*) &param);
        std::cout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": start " << data[index].getTask() << " output" << std::endl;
        timer=clock();
        // std::cout<<std::endl;
        std::cout << (float)timer/CLOCKS_PER_SEC + (param.delay/1000) << "- Process " << processnum << ": end " << data[index].getTask() << " output" << std::endl;
        // std::cout<<std::endl;
      }
      else if(data[index].getType()=='I')
      {
        pthread_t pthread;
        timer=clock();
        std::cout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": start " << data[index].getTask() << " input" << std::endl;
        pthread_create(&pthread, NULL, thread_delay, (void*) &param);
        timer=clock();
        std::cout << (float)timer/CLOCKS_PER_SEC << "- Process " << processnum << ": end " << data[index].getTask() << " input" << std::endl;
      }
    }
  }
}

void* thread_delay(void* arg)
{
  paramStruct* calc = (paramStruct*) arg;
  clock_t test;
  clock_t elapsed;
  double timepassed;
  double wait = calc->delay / 1000;

  clock_t start = clock();
  while(true)
  {
    // std::cout<<calc->delay;
    test=clock();
    elapsed = test-start;
    timepassed=elapsed/(double)CLOCKS_PER_SEC;

    if(timepassed>=wait)
    {
      break;
    }
  }
  pthread_exit(NULL);
}
