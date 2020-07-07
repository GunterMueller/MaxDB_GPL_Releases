/*!**********************************************************************

  module: vwd41cpp.cpp

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Calling init-/exit- and service-function in C++ UserDll

  see also:

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end




************************************************************************/

#include <stdio.h>

#include "gwd00.h"
#include "heo670.h"
#include "hwd02waapi.h"
#include "hwd20control.h"
#include "hwd41cpp.h"

/* pointer to functions from user dll : */

typedef int (*CppInitOrExitFuncPtr)( sapdbwa_WebAgent &wa );

typedef int (*CppServiceFuncPtr)( sapdbwa_WebAgent     &wa,
                                  sapdbwa_HttpRequest  &req,
                                  sapdbwa_HttpReply    &rep );


typedef sapdbwa_Int2 (*InitOrExitFuncPtr)( sapdbwa_Handle wa );

typedef sapdbwa_Int2 (*ServiceFuncPtr)( sapdbwa_Handle       wa,
										sapdbwa_HttpRequestP req,
										sapdbwa_HttpReplyP   rep );


extern "C" void wd41_HandleException( const char  *exceptionString,
                                      const char  *func,
                                      const char  *name,
                                      twd25LogP    log );

extern "C" const char* wd41_GetExceptionStr( sapdbwa_UInt4 exception );


#define UNHANDLED_EXCEPTION_MSG "Unhandled exception."

#define TRY  try
#define CATCH_BLOCK \
    catch( sapdbwa_Int4 sysException ) { \
        wd41_HandleException( wd41_GetExceptionStr(sysException), \
                              funcType, userDllName, log ); \
        execOK = false; \
        \
        /* we only want to log the exception and disable service */ \
        /* let the web server handle it */ \
        /* PTS ER: 1118356 */ \
        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1118356 */ \
        /* throw; */ \
    }  \
    catch (...) { \
        wd41_HandleException( UNHANDLED_EXCEPTION_MSG, funcType,\
                              userDllName, log );\
        execOK = false; \
        \
        /* we only want to log the exception and disable service */ \
        /* let the web server handle it */ \
        /* PTS ER: 1118356 */ \
        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1118356 */ \
        /* throw; */ \
    }; 

//////////////////////
// c functions
//////////////////////


sapdbwa_Bool wd41CallExitFunc( const char  *userDllName,
                             twd25LogP    log,
                             sqlPROC      funcPtr,
                             sapdbwa_Handle     wa,
                             sapdbwa_Int2  *ret )
{
    const char        *funcType = "Exit";
    InitOrExitFuncPtr  func = (InitOrExitFuncPtr) funcPtr;
    sapdbwa_Bool         execOK = true;

    *ret = func( wa );

    return execOK;
}; // wd41CallExitFunc


sapdbwa_Bool wd41CallInitFunc( const char  *userDllName,
                             twd25LogP    log,
                             sqlPROC      funcPtr,
                             sapdbwa_Handle     wa,
                             sapdbwa_Int2  *ret )
{
    const char        *funcType = "Init";
    InitOrExitFuncPtr  func = (InitOrExitFuncPtr) funcPtr;
    sapdbwa_Bool         execOK = true;

    *ret = func( wa );

    return execOK;
}; // wd41CallInitFunc


sapdbwa_Bool wd41CallServiceFunc( const char     *userDllName,
                                twd25LogP       log,
                                sqlPROC         funcPtr,
                                sapdbwa_Handle        wa,
                                sapdbwa_HttpRequestP  req,
                                sapdbwa_HttpReplyP    rep,
                                sapdbwa_Int2     *ret )
{
    const char     *funcType = "Service";
    ServiceFuncPtr  func = (ServiceFuncPtr) funcPtr;
    sapdbwa_Bool      execOK = true;

    *ret = func( wa, req, rep );

    return execOK;
}; // wd41CallServiceFunc

    
sapdbwa_Bool wd41CallSessionDataDestr( const char *userDllName,
                                     twd25LogP   log,
                                     sqlPROC     funcPtr,
                                     void       *data,
                                     sapdbwa_Handle    wa )
{
    const char     *funcType = "Service";
    sapdbwa_CDestructor  func = (sapdbwa_CDestructor) funcPtr;
    sapdbwa_Bool      execOK = true;

    func( wa, data );

    return execOK;
}; // wd41CallSessionDataDestr

//////////////////
// C++ functions
//////////////////

sapdbwa_Bool wd41CallCppExitFunc( const char  *userDllName,
                                twd25LogP    log,
                                sqlPROC      funcPtr,
                                sapdbwa_Handle     wa,
                                sapdbwa_Int2  *ret )
{
    const char           *funcType = "Exit";
    CppInitOrExitFuncPtr  func = (CppInitOrExitFuncPtr) funcPtr;
    sapdbwa_Bool            execOK = true;
    sapdbwa_WebAgent  webAgent(wa);

    *ret = func( webAgent );

    return execOK;
}; // wd41CallCppExitFunc


sapdbwa_Bool wd41CallCppInitFunc( const char  *userDllName,
                                twd25LogP    log,
                                sqlPROC      funcPtr,
                                sapdbwa_Handle     wa,
                                sapdbwa_Int2  *ret )
{
    const char           *funcType = "Init";
    CppInitOrExitFuncPtr  func = (CppInitOrExitFuncPtr) funcPtr;
    sapdbwa_Bool            execOK = true;
    sapdbwa_WebAgent  webAgent(wa);

    *ret = func( webAgent );

    return execOK;
}; // wd41CallCppInitFunc


sapdbwa_Bool wd41CallCppServiceFunc( const char     *userDllName,
                                   twd25LogP       log,
                                   sqlPROC         funcPtr,
                                   sapdbwa_Handle        wa,
                                   sapdbwa_HttpRequestP  req,
                                   sapdbwa_HttpReplyP    rep,
                                   sapdbwa_Int2     *ret )
{
    const char        *funcType = "Service";
    CppServiceFuncPtr  func = (CppServiceFuncPtr) funcPtr;
    sapdbwa_Bool         execOK = true;
    sapdbwa_WebAgent        webAgent( wa );
    sapdbwa_HttpRequest     waReq( req );
    sapdbwa_HttpReply       waRep( rep );

    *ret = func( webAgent, waReq, waRep );

    return execOK;
}; // wd41CallCppServiceFunc

                                      
sapdbwa_Bool wd41CallCppSessionDataDestr( const char *userDllName,
                                        twd25LogP   log,
                                        sqlPROC     funcPtr,
                                        void       *data,
                                        sapdbwa_Handle    wa )
{
    const char           *funcType = "Destructor";
    sapdbwa_CppDestructor      func = (sapdbwa_CppDestructor) funcPtr;
    sapdbwa_Bool            execOK = true;
    sapdbwa_WebAgent        webAgent( wa );

    func( webAgent, data );

    return execOK;
}; // wd41CallCppSessionDataDestr                                      


/////////////////////
// exception handler
/////////////////////

void wd41_HandleException( const char  *exceptionString,
                           const char  *func,
                           const char  *name,
                           twd25LogP    log )
{
    char errMsg[1024];

	/* Flush exception data to log */
    sp77sprintf(errMsg, 1024, "Fatal exception in %s function of %s: %s.\n", func, name, exceptionString);
    wd25WriteLogMsg( log, errMsg );

    /* Disable service for next start */
    wd20ServiceStart( name, false );
    sp77sprintf(errMsg, 1024, "The service %s is disabled now.\n", name);
    wd25WriteLogMsg( log, errMsg );

}; // wd41_HandleException


const char* wd41_GetExceptionStr( sapdbwa_UInt4 exception )
{
	switch(exception)
	{
    case 0:
        return ("Non system exception");
#ifdef WIN32
	case EXCEPTION_ACCESS_VIOLATION:
		return ("EXCEPTION_ACCESS_VIOLATION");
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		return ("EXCEPTION_DATATYPE_MISALIGNMENT");
	case EXCEPTION_BREAKPOINT:
		return ("EXCEPTION_BREAKPOINT");
	case EXCEPTION_SINGLE_STEP:
		return ("EXCEPTION_SINGLE_STEP");
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		return ("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		return ("EXCEPTION_FLT_DENORMAL_OPERAND");
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        return ("EXCEPTION_FLT_DIVIDE_BY_ZERO");
	case EXCEPTION_FLT_INEXACT_RESULT:
        return ("EXCEPTION_FLT_INEXACT_RESULT");
	case EXCEPTION_FLT_INVALID_OPERATION:
		return ("EXCEPTION_FLT_INVALID_OPERATION");
	case EXCEPTION_FLT_OVERFLOW:
		return ("EXCEPTION_FLT_OVERFLOW");
	case EXCEPTION_FLT_STACK_CHECK:
		return ("EXCEPTION_FLT_STACK_CHECK");
	case EXCEPTION_FLT_UNDERFLOW:
		return ("EXCEPTION_FLT_UNDERFLOW");
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return ("EXCEPTION_INT_DIVIDE_BY_ZERO");
	case EXCEPTION_INT_OVERFLOW:
		return ("EXCEPTION_INT_OVERFLOW");
	case EXCEPTION_PRIV_INSTRUCTION:
		return ("EXCEPTION_PRIV_INSTRUCTION");
	case EXCEPTION_IN_PAGE_ERROR:
		return ("EXCEPTION_IN_PAGE_ERROR");
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return ("EXCEPTION_ILLEGAL_INSTRUCTION");
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		return ("EXCEPTION_NONCONTINUABLE_EXCEPTION");
	case EXCEPTION_STACK_OVERFLOW:
		return ("EXCEPTION_STACK_OVERFLOW");
	case EXCEPTION_INVALID_DISPOSITION:
		return ("EXCEPTION_INVALID_DISPOSITION");
	case EXCEPTION_GUARD_PAGE:
		return ("EXCEPTION_GUARD_PAGE");
	case EXCEPTION_INVALID_HANDLE:
		return ("EXCEPTION_INVALID_HANDLE");
#endif /* ifdef WIN32 */
    default:
        return ("UNKNOWN EXCEPTION");
	}; // switch
}; // GetExceptionStr

/*===========================================================================*/

