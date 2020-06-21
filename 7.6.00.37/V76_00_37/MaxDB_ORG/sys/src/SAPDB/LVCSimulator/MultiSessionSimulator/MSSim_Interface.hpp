#ifndef MSSIM_INTERFACE_HPP
#define MSSIM_INTERFACE_HPP

#ifdef WIN32
#define MSSIMAPI _declspec(dllimport)
#else
#define MSSIMAPI
#endif

void MSSIMAPI MSSim_Activate(); 
void MSSIMAPI MSSim_Suspend(); 
void MSSIMAPI MSSim_Resume(); 
void MSSIMAPI MSSim_Reset(); 


class MSSim_SuspendScope
{
public: 

  MSSim_SuspendScope() 
  {
    MSSim_Suspend(); 
  }

  ~MSSim_SuspendScope() 
  {
    MSSim_Resume(); 
  }
};

#endif // #ifndef MSSIM_INTERFACE_HPP