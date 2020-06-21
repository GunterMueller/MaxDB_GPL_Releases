
#ifndef MSSIM_TRACE_HPP
#define MSSIM_TRACE_HPP

#include "LVCSimulator/MultiSessionSimulator/MSSim_Coordinator.hpp"

#include <iostream>

using namespace std; 

enum MSSim_TraceEnum {
  TraceSimFunctionCall = 6,
  TraceSimDetail = 9 
};

#define MSSIM_TRACE( level, fmt ) 

#define MSSIM_TRACET( level, fmt ) \
{ \
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() );  \
  if( inst ) \
  { \
    cout << ( inst->taskCurrent() ) << "  " << fmt << endl;  \
  } \
}

#endif // #ifndef MSSim_TRACE_HPP