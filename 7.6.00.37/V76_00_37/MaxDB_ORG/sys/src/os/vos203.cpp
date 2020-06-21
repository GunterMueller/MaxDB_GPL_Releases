/*!
  @file           vos203.cpp
  @author         RaymondR
  @brief          Load DLL Functions
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
*/





/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#define NEEDED_WIN32_WINNT 0x0500

#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif

#include "gos203.h"
#include "geo200.h"
#include "geo007_1.h"
#include "geo60.h"
#include "geo001.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	__inline
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/


/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


/*---------------------------------------------------------------------------*/
/*--------------------------- class tos203_DynDLL ---------------------------*/
/*---------------------------------------------------------------------------*/

bool tos203_DynDLL::os203_LoadDynDLL( LPCTSTR                       DllName, 
                                      teo200_EventList *            pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos203_DynDLL::os203_LoadDynDLL"));

  return os203_LoadDynDLL( DllName, NULL, pEventList ); 
}

/*---------------------------------------------------------------------------*/

bool tos203_DynDLL::os203_LoadDynDLL( LPCTSTR                       DllName, 
                                      const HMODULE                 ModuleHdl,
                                      teo200_EventList *            pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos203_DynDLL::os203_LoadDynDLL"));
  
  HINSTANCE         hinst = NULL;
  CString           DllNameString;
  DWORD             Pos;
  bool              Ok;
  teo001_SystemRc   rc = NO_ERROR_EO001;

  CLEAR_EVENTLIST_EO200(pEventList);

  ASSERT(DllName != NULL);

  DllNameString = DllName;


  if ( DllNameString.Find( _T('\\') ) == -1 )
  {
    // --- If this 'ModuleHdl' is NULL, GetModuleFileName returns the path for the file 
    //     used to create the calling process. 
    if (GetModuleFileName(ModuleHdl, DllNameString.GetBuffer(_MAX_PATH), _MAX_PATH) != 0) 
    {
      DllNameString.ReleaseBuffer();

      Pos           = DllNameString.ReverseFind( _T('\\') ); 
      DllNameString = DllNameString.Left(Pos + 1) + DllName;


      hinst = LoadLibrary( DllNameString );
    }
    else
      DllNameString.ReleaseBuffer();
  }

  if ( hinst == NULL ) 
  {
    // --- use the filename only to load the DLL 
    DllNameString = DllName;

    hinst = LoadLibrary( DllNameString );
  }

  if ( hinst != NULL )
  {
    os203_DllName   = DllNameString;
    os203_hTCPIPDLL = hinst;
    Ok              = true;
  }
  else
  {
    rc = GetLastError();

    if ( rc == ERROR_MOD_NOT_FOUND )
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_DLL_NOT_FOUND, DllNameString );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    }
    else
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_LOADING_DLL, DllNameString, rc );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    }

    Ok = false;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

void*  tos203_DynDLL::os203_GetFuncAddr ( const LPSTR                   FuncName,
                                          teo200_EventList *            pEventList )
{ 
  ROUTINE_DBG_MEO60 (_T("tos203_DynDLL::os203_GetFuncAddr"));

  teo001_SystemRc   rc = NO_ERROR_EO001;

  ASSERT(os203_hTCPIPDLL != NULL);
  ASSERT(FuncName != NULL);

  CLEAR_EVENTLIST_EO200(pEventList);

  void* FuncAddr = GetProcAddress( os203_hTCPIPDLL, FuncName );

  if ( FuncAddr == NULL )
  {
    CString       FuncNameStr = FuncName; // - build a unicode string

    rc = GetLastError();

    if ( rc == ERROR_PROC_NOT_FOUND )
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                               ERR_DLL_FUNCTION_NOT_FOUND, FuncNameStr,
                               (LPCTSTR)os203_DllName  );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    }
    else
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                               ERR_GET_FUNCTION_ADDR_FAILED, FuncNameStr, rc,
                               (LPCTSTR)os203_DllName  );

      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    }
  }

  return  FuncAddr;
};

/*---------------------------------------------------------------------------*/

void tos203_DynDLL::os203_UnloadDynDLL()
{
  ROUTINE_DBG_MEO60 (_T("tos203_DynDLL::os203_UnloadDynDLL"));

  if ( os203_hTCPIPDLL != NULL )
  {
    FreeLibrary(  os203_hTCPIPDLL ); // unload DLL!!
    os203_hTCPIPDLL = NULL;
  }
}

/*---------------------------------------------------------------------------*/

tos203_DynDLL::~tos203_DynDLL()
{
  ROUTINE_DBG_MEO60 (_T("tos203_DynDLL::~tos203_DynDLL"));

  os203_UnloadDynDLL();
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/