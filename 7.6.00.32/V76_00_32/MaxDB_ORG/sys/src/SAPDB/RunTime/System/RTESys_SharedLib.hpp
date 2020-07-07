/*!---------------------------------------------------------------------
  @file           RTESys_SharedLib.hpp
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



#ifndef RTESYS_SHAREDLIB_HPP
#define RTESYS_SHAREDLIB_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_CompilerFeatures.h"

#include "RunTime/RTE_MessageList.hpp"

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

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!--------------------------------------------------------------------
  @class          RTESys_SharedLib
   @description    This class loads and releases dynamic shared libraries (DLL's)


  This class is introduced to load system DLL's of NT such as KERNEL32.DLL during runtime..
   @note           A copy constructor is forbidden, as well as an assignment operation...

 --------------------------------------------------------------------*/

class RTESys_SharedLib 
{
public:
    /*!----------------------------------------------------------------
       @brief          Construct object handling loaded shared library
       @param          sharedLibName [in] Name of the DLL to be load 
                               Optionally without platform specific extension!
       @param          loadMode [in] mode describing how to handle symbols
       @param          ok [out] flag to indicate loading succeeded
       @param          errList [out] fill if ok is returned as false
       ??Prototypes<docTools.DocEntry.PrototypesEntry instance at 91e818>


       This function maps the specified .DLL file into the address space of the calling process.
       If the sharedLibName ends without an extension, the platform specifc extension is added
       automatically. If an extension is given, no other extension is added...

     ----------------------------------------------------------------*/

    RTESys_SharedLib ( SAPDB_Char const * sharedLibName, 
                       RTE_LoadMode loadMode,
                       SAPDB_Bool &ok,
                       SAPDBErr_MessageList &errList )
    : m_LibHandle(0)
    {
        ok = LoadSharedLib(sharedLibName, loadMode, errList);
    }
    
    RTESys_SharedLib ( SAPDB_Char const * sharedLibName, 
                       SAPDB_Bool &ok,
                       SAPDBErr_MessageList &errList)
    : m_LibHandle(0)
    {
        ok = LoadSharedLib(sharedLibName, RTE_LoadSymbolsLocal, errList);
    }

    /*!----------------------------------------------------------------
       @description    Unload the given DLL


  Since external reference counting would be needed, it is discouraged to destruct without exactly
  knowing and synchronizing all users of the DLL...
  Since it is a dtor errors are silently ignored. If you are intersted in reporting errors during unload operation,
  call the the Unload() member function before.

     ----------------------------------------------------------------*/

    ~RTESys_SharedLib()
    {
        (void)UnloadSharedLib((SAPDBErr_MessageList *)0);
    }

    /*!----------------------------------------------------------------
       @description    Unload the given DLL


  Since external reference counting would be needed, it is discouraged to destruct without exactly
  knowing and synchronizing all users of the DLL...
       @param          errList [out] fill if ok is returned as false
       @return value   True if unload without problems, false else (errList is filled)

     ----------------------------------------------------------------*/

    SAPDB_Bool Unload(SAPDBErr_MessageList & errList)
    {
        return UnloadSharedLib(&errList);
    }

    /*!----------------------------------------------------------------
       @description    Get a procedure calling address of the DLL


  The DLL symbols are not type save, so the returned pointer must be casted into a proper type or a union
  must be used if casting fails... Only the caller knows how to interpret correctly the address returned...
       @param          symbolicName [in] Name of procedure to search
       @param          procAddress [out] procedure address if found
       @param          errList [out] fill if ok is returned as false
       @return value   True if procedure found, false else

     ----------------------------------------------------------------*/

    SAPDB_Bool GetProcAddress( SAPDB_Char const * symbolicName,
                               RTE_SharedLibProcAddress & procAddress,
                               SAPDBErr_MessageList     & errList);

private:
    SAPDB_Bool LoadSharedLib( SAPDB_Char const *sharedLibName,
                              RTE_LoadMode loadMode,
                              SAPDBErr_MessageList &errList);

    /* forbidden, since it would have problems without reference counting... */
    RTESys_SharedLib( const RTESys_SharedLib & ) {}
    RTESys_SharedLib & operator=( const RTESys_SharedLib & ) { return *this; }

    SAPDB_Bool UnloadSharedLib(SAPDBErr_MessageList * pErrList); /* error list is optional since used in dtor too... */

    RTE_SharedLibHandle m_LibHandle;
    RTE_Path            m_FullSharedLibName;
}; 

#endif /* RTESYS_SHAREDLIB_HPP */

