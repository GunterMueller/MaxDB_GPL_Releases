/*!
  @file           gos205.h
  @author         RaymondR
  @brief          Microsoft Cluster Server - The PropertyList class.
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



#ifndef GOS205_H
#define GOS205_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <afx.h>
#include <clusapi.h>
#include <resapi.h>
#include "geo200.h"
#include "geo60.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class tos205_PropertyList
{
public:
  tos205_PropertyList( bool AllowInserts = true );
  ~tos205_PropertyList();

  inline operator  bool() const;

protected:
  bool                                   os205_AllowInserts;
  CLUSPROP_BUFFER_HELPER                 os205_PropList;
  CLUSPROP_BUFFER_HELPER                 os205_PropCurrent;
  DWORD                                  os205_BufferSize;
  DWORD                                  os205_DataSize;

public:
  inline CLUSPROP_BUFFER_HELPER const *  os205_PropertyListPtr()  const; 
  DWORD                                  os205_PropertyListSize() const;



  bool                   os205_FillPropertyList   ( CLUSPROP_BUFFER_HELPER  PropList,
                                                    const DWORD             DataSize,
                                                    teo200_EventList *      pEventList = NULL );

  bool                   os205_InsertProperty     ( LPCWSTR                 Name,
                                                    const CString &         Value,
                                                    teo200_EventList *      pEventList = NULL );

  bool                   os205_InsertProperty     ( LPCWSTR                 Name,
                                                    const DWORD             Value,
                                                    teo200_EventList *      pEventList = NULL );

  bool                   os205_InsertProperty     ( LPCWSTR                 Name,
                                                    const PBYTE             Value,
                                                    const DWORD             ValueSize,
                                                    teo200_EventList *      pEventList = NULL );


  void                   os205_DeleteProperty     ( LPCWSTR                 Name );

  void                   os205_ClearList          ();

  bool                   os205_GetPropertyValue   ( LPCWSTR                 Name,
                                                    WORD                    &Format,
                                                    DWORD                   &ValueLen,
                                                    void**                  Value );

// --- Overrides
protected:
  bool                   os205_AllocPropList      ( DWORD                  Minimum,
                                                    teo200_EventList *     pEventList = NULL );

  void                   os205_DelPropEntry       ( CLUSPROP_BUFFER_HELPER PropEntry );

  CLUSPROP_BUFFER_HELPER os205_FindPropName       ( LPCWSTR                Name );

  bool                   os205_AddPropName        ( LPCWSTR                Name,
                                                    teo200_EventList *     pEventList = NULL );

  bool                   os205_AddPropValue       ( CLUSTER_PROPERTY_TYPE  PropType,
                                                    LPCWSTR                Value,
                                                    teo200_EventList *     pEventList = NULL );

  bool                   os205_AddPropValue       ( CLUSTER_PROPERTY_TYPE  PropType,
                                                    const DWORD            Value,
                                                    teo200_EventList *     pEventList = NULL );

  bool                   os205_AddPropValue       ( CLUSTER_PROPERTY_TYPE  PropType,
                                                    const PBYTE            Value      = NULL,
                                                    DWORD                  ValueLen   = 0,
                                                    teo200_EventList *     pEventList = NULL );

};  

/*---------------------------------------------------------------------------*/

inline CLUSPROP_BUFFER_HELPER const *tos205_PropertyList::os205_PropertyListPtr() const
{ 
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_PropertyListPtr"));

  return &os205_PropList;
}

/*---------------------------------------------------------------------------*/

inline DWORD tos205_PropertyList::os205_PropertyListSize() const
{ 
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_PropertyListSize"));

  return os205_DataSize;
}

/*---------------------------------------------------------------------------*/

inline tos205_PropertyList::operator bool() const
{ 
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::operator bool()"));

  return ( os205_PropList.pList != NULL ); 
};


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS205_H */
