/*!
  @file           heo02x.h
  @author         BurkhardD
  @special area   shared library handling
  @brief          Interface for handle shared library functions
  @see            Copyright (c) 1998-2005 SAP AG

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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



\endif
*/

#ifdef __cplusplus
    #define externC      extern "C"
#else
    #define externC
#endif

#ifndef HEO02X_H
#define HEO02X_H
#include "gsp00.h"

typedef void  (*sqlPROC)();
#ifndef WIN32
#ifndef __HANDLE_T__
#define __HANDLE_T__ __HANDLE_T__
typedef void* HANDLE;
#endif
#else
#include <windows.h>
#endif
/*!
   @brief          loads a shared library
   @param          szPath [IN] - The name of the library to load. The name can                               include the path but it should not include the                              fileextension.
   @param          cbLenErrText [IN] - Lenght in Bytes of buffer ErrText.
   @param          ErrText [OUT] - The errormessage  text if an error has occured. return - handle for the loaded library. If the function fails the           return handle will be NULL and the errortext returns the           errormessage.

 */


externC HANDLE sqlLoadLibrary(char *szPath, char *ErrText, int cbLenErrText);

/*!
   @brief          loads a shared library
   @param          szPath [IN] - The name of the library to load. The name can                               include the path but it should not include the                              fileextension.
   @param          Mode [IN] - RTLD_GLOBAL_EO02 or RTLD_LOCAL_EO02                              load with 'global' or 'local' scope of symbol-resolution
   @param          cbLenErrText [IN] - Lenght in Bytes of buffer ErrText.
   @param          ErrText [OUT] - The errormessage  text if an error has occured. return - handle for the loaded library. If the function fails the           return handle will be NULL and the errortext returns the           errormessage.

 */


#define RTLD_GLOBAL_EO02 0x00000001
#define RTLD_LOCAL_EO02  0x00000002

externC HANDLE sqlLoadLibraryEx(char *szPath, int Mode, char *ErrText, int cbLenErrText);

/*!
   @brief          determines the address of a funtcion from a shared library
   @param          hLib [IN] - Handle to the shared library.
   @param          szSqlFunction [IN] - Name of the function.
   @param          cbLenErrText [IN] - Lenght in Bytes of buffer ErrText.
   @param          ErrText [OUT] - The errormessage  text if an error has occured. return - the address to the funtion of a shared library. If the function          fails the return adress will be NULL and the errortext returns the           errormessage.

 */


externC sqlPROC sqlGetProcAddress(HANDLE hLib, const char *szSqlFunction, 
                                  char *ErrText, int cbLenErrText);

/*!
   @brief          unloads a shared library
   @param          hLib [IN] - Handle to the shared library.
   @param          cbLenErrText [IN] - Lenght in Bytes of buffer ErrText.
   @param          ErrText [OUT] - The errormessage  text if an error has occured. return - TRUE if success or FALSE if the function fails. If the function           fails the errtext parameter will show the errormessage.

 */


externC boolean sqlFreeLibrary(HANDLE hLib, char *ErrText, int cbLenErrText);
 
#endif /*HEO02_H*/
