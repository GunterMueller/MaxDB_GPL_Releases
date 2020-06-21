/*!
  @file           vos205.cpp
  @author         RaymondR
  @brief          MSCS - Implementation of the PropertyList class.
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

#if !defined(UNICODE)
# error "You can compile this file with -DUNICODE only!"
#endif
#define  _UNICODE

#define NEEDED_WIN32_WINNT 0x0500

#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif

#include "gos205.h"
#include "geo200.h"
#include "geo60.h"
#include "geo007_1.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE  inline
#endif

#define BUFFER_GROWTH_FACTOR 256


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


tos205_PropertyList::tos205_PropertyList( bool AllowInserts )
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::tos205_PropertyList"));

  os205_PropList.pList = NULL;
  os205_PropCurrent.pb = NULL;
  os205_BufferSize     = 0;
  os205_DataSize       = 0;

  os205_AllowInserts   = AllowInserts;

}

/*---------------------------------------------------------------------------*/

tos205_PropertyList::~tos205_PropertyList(void)
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::~tos205_PropertyList"));

  delete [] os205_PropList.pb;

}

/*---------------------------------------------------------------------------*/

void tos205_PropertyList::os205_ClearList(void)
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_ClearList"));

  delete [] os205_PropList.pb;

  os205_PropList.pList = NULL;
  os205_PropCurrent.pb = NULL;
  os205_BufferSize     = 0;
  os205_DataSize       = 0;
}

/*---------------------------------------------------------------------------*/

bool tos205_PropertyList::os205_FillPropertyList( CLUSPROP_BUFFER_HELPER       PropList,
                                                  const DWORD                  DataSize,
                                                  teo200_EventList *           pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_FillPropertyList"));
  bool Ok;

  // --- Allocate a default-sized buffer.
  Ok = os205_AllocPropList(DataSize, pEventList );

  if ( Ok == true )
  {
    CopyMemory( os205_PropList.pb, PropList.pb, DataSize );
    os205_DataSize       = DataSize;
    os205_PropCurrent.pb = os205_PropList.pb + DataSize - sizeof(CLUSPROP_SYNTAX);
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos205_PropertyList::os205_InsertProperty( LPCWSTR             Name,
                                                const CString &     Value,
                                                teo200_EventList *  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_InsertProperty"));
  bool      Ok = true;

  ASSERT(Name != NULL);

  if ( os205_AllowInserts )
  {
    CLUSPROP_BUFFER_HELPER           PropHlp;

    PropHlp = os205_FindPropName ( Name );

    if ( PropHlp.pb != NULL )
      os205_DelPropEntry( PropHlp );

    Ok = os205_AddPropName( Name, pEventList );
    
    if ( Ok == true )
    {
      // --- Add the property value
      Ok = os205_AddPropValue(CLUSPROP_TYPE_LIST_VALUE, Value, pEventList );
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos205_PropertyList::os205_GetPropertyValue( LPCWSTR                 Name,
                                                  WORD                    &Format,
                                                  DWORD                   &ValueLen,
                                                  void**                  Value )
{ 
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_GetPropertyValue"));

  ASSERT(Name != NULL);

  bool Ok = false;

  CLUSPROP_BUFFER_HELPER           PropHlp;

  PropHlp = os205_FindPropName ( Name );

  if ( PropHlp.pb != NULL )
  {
    // --- Advance the buffer pointer past the name entry.
    PropHlp.pb           += sizeof(*PropHlp.pName) + ALIGN_CLUSPROP(PropHlp.pName->cbLength);;

    if ( PropHlp.pBinaryValue->Syntax.wType == CLUSPROP_TYPE_LIST_VALUE )
    {
      Format   = PropHlp.pBinaryValue->Syntax.wFormat;
      ValueLen = PropHlp.pBinaryValue->cbLength;
      
      switch ( Format )
      {
      case CLUSPROP_FORMAT_DWORD:
        *Value = &PropHlp.pDwordValue->dw;
        break;
      case CLUSPROP_FORMAT_SZ:
        *Value = &PropHlp.pStringValue->sz;
        break;
      case CLUSPROP_FORMAT_BINARY:
        *Value = &PropHlp.pBinaryValue->rgb;
        break;
      }
      Ok = true;
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos205_PropertyList::os205_InsertProperty( LPCWSTR             Name,
                                                const DWORD         Value,
                                                teo200_EventList *  pEventList )
{ 
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_InsertProperty"));
  bool      Ok = true;

  ASSERT(Name != NULL);

  if ( os205_AllowInserts )
  {
    CLUSPROP_BUFFER_HELPER           PropHlp;

    PropHlp = os205_FindPropName ( Name );

    if ( PropHlp.pb != NULL )
      os205_DelPropEntry( PropHlp );

    Ok = os205_AddPropName( Name, pEventList );
    
    if ( Ok == true )
    {
      // --- Add the property value
      Ok = os205_AddPropValue( CLUSPROP_TYPE_LIST_VALUE, Value, pEventList );
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos205_PropertyList::os205_InsertProperty( LPCWSTR             Name,
                                                const PBYTE         Value,
                                                const DWORD         ValueSize,
                                                teo200_EventList *  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_InsertProperty"));
  bool      Ok = true;

  ASSERT(Name != NULL);

  if ( os205_AllowInserts )
  {
    CLUSPROP_BUFFER_HELPER           PropHlp;

    PropHlp = os205_FindPropName ( Name );

    if ( PropHlp.pb != NULL )
      os205_DelPropEntry( PropHlp );

    Ok = os205_AddPropName( Name, pEventList );
    
    if ( Ok == true )
    {
      // --- Add the property value
      Ok = os205_AddPropValue( CLUSPROP_TYPE_LIST_VALUE, Value, ValueSize, pEventList ); 
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

void tos205_PropertyList::os205_DeleteProperty( LPCWSTR          Name )
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_DeleteProperty"));

  ASSERT(Name != NULL);

  CLUSPROP_BUFFER_HELPER           PropHlp;

  PropHlp = os205_FindPropName ( Name );

  if ( PropHlp.pb != NULL )
    os205_DelPropEntry( PropHlp );
 
  return;
}


/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

bool tos205_PropertyList::os205_AddPropName( LPCWSTR                Name,
                                             teo200_EventList *     pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_AddPropName"));

  DWORD                   NameLen;
  DWORD                   DataSize;
  bool                    Ok;

  NameLen  = (lstrlenW(Name) + 1) * sizeof(WCHAR);
  DataSize = sizeof(*os205_PropCurrent.pName) + ALIGN_CLUSPROP(NameLen);

  // --- Allocate a default-sized buffer.
  Ok = os205_AllocPropList(DataSize, pEventList );

  if ( Ok == true )
  {
    os205_PropCurrent.pName->Syntax.wFormat = CLUSPROP_FORMAT_SZ;
    os205_PropCurrent.pName->Syntax.wType   = CLUSPROP_TYPE_NAME;
    os205_PropCurrent.pName->cbLength       = NameLen;
    lstrcpyW(os205_PropCurrent.pName->sz, Name);

    // --- Update insert position and increment the data size and property count.
    os205_PropCurrent.pb += DataSize;
    os205_DataSize       += DataSize;
    os205_PropList.pList->nPropertyCount++;

    // --- Set an endmark
    os205_PropCurrent.pSyntax->dw = CLUSPROP_SYNTAX_ENDMARK;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos205_PropertyList::os205_AddPropValue( CLUSTER_PROPERTY_TYPE  PropType,
                                              LPCWSTR                Value,
                                              teo200_EventList *     pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_AddPropValue"));

  DWORD                   ValueLen;
  DWORD                   DataSize;
  bool                    Ok;

  ValueLen  = (lstrlenW(Value) + 1) * sizeof(WCHAR);
  DataSize = sizeof(*os205_PropCurrent.pStringValue) + ALIGN_CLUSPROP(ValueLen);

  // --- Allocate a default-sized buffer.
  Ok = os205_AllocPropList(DataSize, pEventList );

  if ( Ok == true )
  {
    os205_PropCurrent.pStringValue->Syntax.wFormat = CLUSPROP_FORMAT_SZ;
    os205_PropCurrent.pStringValue->Syntax.wType   = PropType;
    os205_PropCurrent.pStringValue->cbLength       = ValueLen;
    lstrcpyW(os205_PropCurrent.pStringValue->sz, Value);

    // --- Update insert position and increment the data size.
    os205_PropCurrent.pb += DataSize;
    os205_DataSize       += DataSize;

    // --- Set an endmark, update insert position and increment the data size
    os205_PropCurrent.pSyntax->dw = CLUSPROP_SYNTAX_ENDMARK;
    os205_PropCurrent.pb         += sizeof(CLUSPROP_SYNTAX); // endmark;
    os205_DataSize               += sizeof(CLUSPROP_SYNTAX); // endmark;

    // --- Set an endmark
    os205_PropCurrent.pSyntax->dw = CLUSPROP_SYNTAX_ENDMARK;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos205_PropertyList::os205_AddPropValue( CLUSTER_PROPERTY_TYPE  PropType,
                                              const DWORD            Value,
                                              teo200_EventList *     pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_AddPropValue"));

  DWORD                   DataSize;
  bool                    Ok;

  DataSize = sizeof(*os205_PropCurrent.pDwordValue);

  // --- Allocate a default-sized buffer.
  Ok = os205_AllocPropList(DataSize, pEventList );

  if ( Ok == true )
  {
    os205_PropCurrent.pDwordValue->Syntax.wFormat = CLUSPROP_FORMAT_DWORD;
    os205_PropCurrent.pDwordValue->Syntax.wType   = PropType;
    os205_PropCurrent.pDwordValue->cbLength       = sizeof(DWORD);
    os205_PropCurrent.pDwordValue->dw             = Value;

    // --- Update insert position and increment the data size.
    os205_PropCurrent.pb += DataSize;
    os205_DataSize       += DataSize;

    // --- Set an endmark, update insert position and increment the data size
    os205_PropCurrent.pSyntax->dw = CLUSPROP_SYNTAX_ENDMARK;
    os205_PropCurrent.pb         += sizeof(CLUSPROP_SYNTAX); // endmark;
    os205_DataSize               += sizeof(CLUSPROP_SYNTAX); // endmark;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos205_PropertyList::os205_AddPropValue( CLUSTER_PROPERTY_TYPE  PropType,
                                              const PBYTE            Value,
                                              DWORD                  ValueLen,
                                              teo200_EventList *     pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_AddPropValue"));

  DWORD                   DataSize;
  bool                    Ok;

  DataSize = sizeof(*os205_PropCurrent.pBinaryValue) + ALIGN_CLUSPROP(ValueLen);

  // --- Allocate a default-sized buffer.
  Ok = os205_AllocPropList(DataSize, pEventList );

  if ( Ok == true )
  {
    os205_PropCurrent.pBinaryValue->Syntax.wFormat = CLUSPROP_FORMAT_BINARY;
    os205_PropCurrent.pBinaryValue->Syntax.wType   = PropType;
    os205_PropCurrent.pBinaryValue->cbLength       = ValueLen;

    if (ValueLen != 0)
      CopyMemory( os205_PropCurrent.pBinaryValue->rgb, Value, ValueLen );

    // --- Update insert position and increment the data size.
    os205_PropCurrent.pb += DataSize;
    os205_DataSize       += DataSize;

    // --- Set an endmark, update insert position and increment the data size
    os205_PropCurrent.pSyntax->dw = CLUSPROP_SYNTAX_ENDMARK;
    os205_PropCurrent.pb         += sizeof(CLUSPROP_SYNTAX); // endmark;
    os205_DataSize               += sizeof(CLUSPROP_SYNTAX); // endmark;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

CLUSPROP_BUFFER_HELPER tos205_PropertyList::os205_FindPropName ( LPCWSTR   Name )
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_FindPropName"));
  bool                            bFound = false;
  CLUSPROP_BUFFER_HELPER          PropHlp;
  BYTE *                          EndOfData;

  if ( os205_PropList.pb != NULL )
  {
    CLUSPROP_PROPERTY_NAME const *  pName;
    DWORD                           PropCount;
    DWORD                           Size;

    for ( PropCount  = os205_PropList.pList->nPropertyCount,
          EndOfData  = os205_PropList.pb + os205_DataSize,
          PropHlp.pb = os205_PropList.pb + sizeof(os205_PropList.pList->nPropertyCount);
          (PropCount           >  0)         && 
          (PropHlp.pb          <  EndOfData) && 
          (bFound              == false)     && 
          (PropHlp.pSyntax->dw != CLUSPROP_SYNTAX_ENDMARK);
          PropCount-- )
    {
      pName = PropHlp.pName;
      ASSERT(pName->Syntax.dw == CLUSPROP_SYNTAX_NAME);

      if ( lstrcmpiW(pName->sz, Name) == 0 )
        bFound = true;
      else
      {
        Size        = sizeof(*pName) + ALIGN_CLUSPROP(pName->cbLength);
        PropHlp.pb += Size;

        // --- Advance the buffer pointer past the value in the value list.
        while ((PropHlp.pSyntax->dw != CLUSPROP_SYNTAX_ENDMARK) && 
               (PropHlp.pb < EndOfData))
        {
          Size        = sizeof(*PropHlp.pValue) + ALIGN_CLUSPROP(PropHlp.pValue->cbLength);
          PropHlp.pb += Size;
        }

        // --- Advance the buffer pointer past the value list endmark.
        Size        = sizeof(*PropHlp.pSyntax); // --- endmark
        PropHlp.pb += Size;
      }
    }
  }

  if ( bFound == false )
    PropHlp.pb = NULL;

  return PropHlp;
}

/*---------------------------------------------------------------------------*/

void tos205_PropertyList::os205_DelPropEntry( CLUSPROP_BUFFER_HELPER  PropEntry )
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_DelPropEntry"));
  CLUSPROP_BUFFER_HELPER          PropHlp;
  DWORD                           Size;
  BYTE *                          EndOfData;

  PropHlp   = PropEntry;
  EndOfData = os205_PropList.pb + os205_DataSize;


  // --- Advance the buffer pointer past the name entry.
  Size                  = sizeof(*PropHlp.pName) + ALIGN_CLUSPROP(PropHlp.pName->cbLength);
  PropHlp.pb           += Size;
  os205_DataSize       -= Size;
  os205_PropCurrent.pb -= Size;

  // --- Advance the buffer pointer past the values in the value list.
  while ((PropHlp.pSyntax->dw != CLUSPROP_SYNTAX_ENDMARK) && 
         (PropHlp.pb < EndOfData))
  {
    Size                  = sizeof(*PropHlp.pValue) + ALIGN_CLUSPROP(PropHlp.pValue->cbLength);
    PropHlp.pb           += Size;
    os205_DataSize       -= Size;
    os205_PropCurrent.pb -= Size;
  }

  // --- Advance the buffer pointer past the value list endmark.
  Size                  = sizeof(*PropHlp.pSyntax); // endmark
  PropHlp.pb           += Size;
  os205_DataSize       -= Size;
  os205_PropCurrent.pb -= Size;


  os205_PropList.pList->nPropertyCount--;


  if (PropHlp.pb < EndOfData)
  {
    MoveMemory( PropEntry.pb, PropHlp.pb, 
                os205_DataSize - (PropEntry.pb - os205_PropList.pb) );
  }

}

/*---------------------------------------------------------------------------*/

bool tos205_PropertyList::os205_AllocPropList( DWORD                  Minimum,
                                               teo200_EventList *     pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos205_PropertyList::os205_AllocPropList"));

  DWORD    Total;

  ASSERT(Minimum > 0);

  // --- Add the size of the item count and final endmark.
  Minimum += sizeof(DWORD) + sizeof(CLUSPROP_SYNTAX);
  Total    = os205_DataSize + Minimum;

  if (os205_BufferSize < Total)
  {
    PBYTE  NewProplist;

    Minimum = max(BUFFER_GROWTH_FACTOR, Minimum);
    Total   = os205_DataSize + Minimum;

    // --- Allocate and zero a new buffer.
    NewProplist = new BYTE[Total];

    if ( NewProplist == NULL )
    {
      teo200_EventList EvtLst( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }

    ZeroMemory( NewProplist, Total );

    // --- If there was a previous buffer, copy and delete it.
    if ( os205_PropList.pb != NULL )
    {
      if ( os205_DataSize != 0 )
        CopyMemory( NewProplist, os205_PropList.pb, os205_DataSize );

      os205_PropCurrent.pb = NewProplist + (os205_PropCurrent.pb - os205_PropList.pb);
      delete [] os205_PropList.pb;
    }  
    else // --- if there was no previous buffer
    {
      os205_PropCurrent.pb = NewProplist + sizeof(os205_PropCurrent.pList->nPropertyCount);
      os205_DataSize       = sizeof(os205_PropCurrent.pList->nPropertyCount);

      // --- Set an endmark and increment the data size
      os205_PropCurrent.pSyntax->dw = CLUSPROP_SYNTAX_ENDMARK;
      os205_DataSize               += sizeof(CLUSPROP_SYNTAX); // endmark;
    }

    // --- Save the new buffer.
    os205_PropList.pb = NewProplist;
    os205_BufferSize  = Total;
  }

  return true;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
