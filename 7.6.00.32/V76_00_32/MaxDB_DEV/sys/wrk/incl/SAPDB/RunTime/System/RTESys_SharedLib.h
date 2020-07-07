/*!---------------------------------------------------------------------
  @file           RTESys_SharedLib.h
  @author         JoergM
  @brief          System: Class for loading DLL during runtime
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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
---------------------------------------------------------------------*/



#ifndef RTESYS_SHAREDLIB_H
#define RTESYS_SHAREDLIB_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_CompilerFeatures.h"

#if defined (__cplusplus)
#  include "RunTime/System/RTESys_SharedLib.hpp"
#else
#  include "RunTime/RTE_Types.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(WIN32)
#  define SAPDB_DLL_EXTENSION ".dll"
#elif (defined HPUX && !defined HP_IA64)
#  define SAPDB_DLL_EXTENSION ".sl"
#else
#  define SAPDB_DLL_EXTENSION ".so"
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  PROTOTYPES                                                               *
 *===========================================================================*/

/* Since these function are used from C coding also, here is the 'basic' interface used in C++ implementation too... */
/*!--------------------------------------------------------------------
   @description    This function maps the specified .DLL file into the address space of the calling process.


  If the sharedLibName ends without an extension, the platform specifc extension is added
  automatically. If an extension is given, no other extension is added...
   @param          sharedLibName [in] Name of the DLL to be load 
                               Optionally without platform specific extension!
   @param          fullPath [out] completed path as it was used in loading call
   @param          loadMode [in] mode describing how to handle symbols
   @param          errorText [inout] Pointer to error text buffer
   @param          maxErrorTextLengthInBytes [in] maximum length of output including zero termination
   @param          sharedLibHandle [out] filled with handle to shared library on success
   @return value   true if DLL is loaded, false else (see errorText for details)

 --------------------------------------------------------------------*/

externC SAPDB_Bool RTESys_LoadSharedLibrary( SAPDB_Char const *sharedLibName, 
                                             RTE_LoadMode loadMode,
                                             SAPDB_Char *errorText,
                                             SAPDB_Int4  maxErrorTextLengthInBytes,
                                             RTE_Path VAR_ARRAY_REF fullPath, 
                                             RTE_SharedLibHandle VAR_VALUE_REF sharedLibHandle );

/*!--------------------------------------------------------------------
   @description    Unload a shared library loaded with a previous call to RTESys_LoadSharedLibrary
   @param          sharedLibHandle [in] handle to shared library
   @param          errorText [inout] Pointer to error text buffer
   @param          maxErrorTextLengthInBytes [in] maximum length of output including zero termination
   @return value   true if DLL was unloaded, false else (see errorText for details)

 --------------------------------------------------------------------*/

externC  SAPDB_Bool RTESys_UnloadSharedLibrary( RTE_SharedLibHandle sharedLibHandle,
                                                SAPDB_Char *errorText,
                                                SAPDB_Int4  maxErrorTextLengthInBytes );

/*!--------------------------------------------------------------------
   @description    Get a procedure address from the given shared library opened by a previous call to RTESys_LoadSharedLibrary
   @param          sharedLibHandle [in] handle to shared library
   @param          symbolicName [in] Pointer to the symbolic name of procedure address to search
   @param          errorText [inout] Pointer to error text buffer
   @param          maxErrorTextLengthInBytes [in] maximum length of output including zero termination
   @param          procAddress [out] filled with procedure address if symbolic name was resolved
   @return value   true if symbol was resolved, false else (see errorText for details)

 --------------------------------------------------------------------*/

externC  SAPDB_Bool RTESys_GetProcAddressFromSharedLibrary( RTE_SharedLibHandle sharedLibHandle,
                                                            SAPDB_Char const *symbolicName,
                                                            SAPDB_Char *errorText,
                                                            SAPDB_Int4  maxErrorTextLengthInBytes,
                                                            RTE_SharedLibProcAddress VAR_VALUE_REF procAddress );

#endif /* RTESYS_SHAREDLIB_H */

