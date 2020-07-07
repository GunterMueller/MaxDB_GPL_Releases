/*!
  @file           gos203.h
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



#ifndef GOS203_H
#define GOS203_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined ( UNICODE )
# define  _UNICODE 
#endif

#include <afx.h>
#include "geo200.h"
#include "geo60.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

// -- Dummy modifier for MK_FUNCPTR_OS203, MK_PTYPE_OR_FUNCPTR_OS203 ...
//
#define DMOD_OS203 

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

# define MK_FUNCPTR_OS203(_retval, _modifier, _func, _param)            \
           typedef _retval (_modifier *t_##_func) _param;               \
           t_##_func _func;

# define LOAD_FUNC_OS203(_func)                                          \
           (((_func = (t_##_func)os203_GetFuncAddr(#_func)) == NULL) ? false : true)

# define LOAD_FUNC_EX_OS203(_func, _err)                                 \
           (((_func = (t_##_func)os203_GetFuncAddr(#_func,_err)) == NULL) ? false : true)

//
//--- make prototype or function pointer
//
#if defined CREATE_DLL_LOADER

#define  MK_CLASS(_class )              class _class  : public tos203_DynDLL

//--- function pointer
# define MK_PTYPE_OR_FUNCPTR_OS203(_retval, _modifier, _func, _param)   \
           typedef _retval (_modifier *t_##_func) _param;               \
           t_##_func _func;
#else 

# define  MK_CLASS(_class )              class _class

//--- prototype 
# define MK_PTYPE_OR_FUNCPTR_OS203(_retval, _modifier, _func, _param)  \
           _retval _modifier class##_func _param
#endif

# define MK_C_DLL_ENTRY_POINT_OS203(_retval, _modifier, _func, _param, _class, _object, _callparam)  \
           extern "C" _retval _modifier _func _param {  return _object.class##_func _callparam; }    \
           _retval _modifier _class::class##_func _param



/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*---------------------------------------------------------------------------*/
/*--------------------------- class tos203_DynDLL ---------------------------*/
/*---------------------------------------------------------------------------*/

class tos203_DynDLL
{
public:
  inline bool          os203_DLLLoaded();

protected:
  tos203_DynDLL                          ();
  ~tos203_DynDLL                         ();

  void*                os203_GetFuncAddr ( const LPSTR                   FuncName,
                                           teo200_EventList *            pEventList = NULL );
  bool                 os203_LoadDynDLL  ( LPCTSTR                       DllName,
                                           teo200_EventList *            pEventList = NULL );
  bool                 os203_LoadDynDLL  ( LPCTSTR                       DllName,
                                           const HMODULE                 ModuleHdl,
                                           teo200_EventList *            pEventList = NULL );
  void                 os203_UnloadDynDLL();

private:
  CString          os203_DllName;
  HINSTANCE        os203_hTCPIPDLL;
};

/*---------------------------------------------------------------------------*/

inline tos203_DynDLL::tos203_DynDLL ()
{ 
  ROUTINE_DBG_MEO60 (_T("tos203_DynDLL::tos203_DynDLL"));

  os203_DllName   = "";
  os203_hTCPIPDLL = NULL;
};

/*---------------------------------------------------------------------------*/

inline bool tos203_DynDLL::os203_DLLLoaded ()
{ 
  ROUTINE_DBG_MEO60 (_T("tos203_DynDLL::os203_DLLLoaded"));
                           
  return (os203_hTCPIPDLL != NULL); 
};


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

#endif  /* GOS203_H */
