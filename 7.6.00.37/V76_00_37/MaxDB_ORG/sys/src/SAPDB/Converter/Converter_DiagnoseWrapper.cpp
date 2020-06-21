/*
  @file     Converter_DiagnoseWrapper.cpp
  @author   TorstenS
  @ingroup  Converter
  @brief    Interface of the Converter to the diagnose tool
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"


#include "Converter/Converter_Page.hpp"
#include "Converter/Converter_LeafPage.hpp"
#include "Converter/Converter_IndexPage.hpp"
#include "Converter/Converter_ParentReference.hpp"
#include "KernelCommon/Kernel_Page.hpp"

#include "IOManager/IOMan_BlockAddress.hpp"
#include "DataAccess/Data_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

externPascal void bd10GetPageHeader (
    tsp00_PageAddr           pPage,
    tsp00_Int4            &  Version,
    tsp00_Int4            &  DevNo,
    tsp00_Int4            &  BlockNo,
    tsp00_Int4            &  BaseNo,
    tsp00_Int4            &  TotalCnt,
    tsp00_Int4            &  ParentIndex,
    tsp00_Int4            &  PageEntryNo,
    pasbool               &  IsLeaf,
    pasbool               &  IsPerm,
    pasbool               &  IsDynamic)
{
    Kernel_Page::PageFrame      Frame (pPage, sizeof(*pPage));
    IOMan_BlockAddress          BlockAddress;
    Converter_ParentReference   Reference;

    if( Converter_IndexPageType == Converter_Page::ReadConverterPageType( Frame ))
    {
        // Handle index page
        Converter_IndexPage     Page( Frame );
        Page.SetDevNoBitCount( Page.ReadDeviceNoBitCount() );

        IsLeaf       = false;
        IsDynamic    = false;
        IsPerm       = true;
        BaseNo       = 0;
        Version      = Page.ReadVersion();
        TotalCnt     = Page.GetEntryCount();
        BlockAddress = Page.ReadBlockAddress();
        if( BlockAddress.IsValid() )
        {
            DevNo   = BlockAddress.GetDeviceNo();
            BlockNo = BlockAddress.GetBlockNo();
        }
        else
            DevNo = MAX_INT2_SP00;
        Reference    = Page.ReadParentReference();
        ParentIndex  = Reference.GetIndexEntryNo();
        PageEntryNo  = Reference.GetPageEntryNo();
    }
    else
    {
        // Handle leaf pages
        Converter_LeafPage Page( Frame );
        Page.SetDevNoBitCount( Page.ReadDeviceNoBitCount() );

        IsLeaf       = true;
        IsDynamic    = ( Data_Dynamic     == Page.ReadPageRecoveryMode().GetAddressingMode()) ? true : false;
        IsPerm       = ( Data_Recoverable == Page.ReadPageRecoveryMode().GetRecoverability()) ? true : false;
        BaseNo       = Page.ReadBasePageNo();
        Version      = Page.ReadVersion();
        TotalCnt     = Page.GetEntryCount();
        BlockAddress = Page.ReadBlockAddress();
        if( BlockAddress.IsValid() )
        {
            DevNo   = BlockAddress.GetDeviceNo();
            BlockNo = BlockAddress.GetBlockNo();
        }
        else
            DevNo = MAX_INT2_SP00;
        Reference    = Page.ReadParentReference();
        ParentIndex  = Reference.GetIndexEntryNo();
        PageEntryNo  = Reference.GetPageEntryNo();
    }
}

/*---------------------------------------------------------------------------*/

externPascal void bd10GetConvEntry (
    tsp00_PageAddr           pPage,
    tsp00_Int4               EntryNo,
    pasbool                  IsLeaf,
    tsp00_Int4            &  DevNo,
    tsp00_Int4            &  BlockNo,
    pasbool               &  IsMarked,
    pasbool               &  IsPending)
{
    Kernel_Page::PageFrame  Frame (pPage, sizeof(*pPage));
    IOMan_BlockAddress      BlockAddress;

    if( IsLeaf )
    {
        // Handle leaf page
        Converter_LeafPage     Page( Frame );
        Page.SetDevNoBitCount( Page.ReadDeviceNoBitCount() );
        if( Page.EntryInUse( EntryNo ))
        {
            if( Page.IsReserved( EntryNo ))
                DevNo = MAX_INT2_SP00 - 1;
            else
            {
                Page.ReadBlockAddress( EntryNo, BlockAddress );
                DevNo     = BlockAddress.GetDeviceNo();
                BlockNo   = BlockAddress.GetBlockNo();
                IsMarked  = Page.SavePagesRequested( EntryNo );
                IsPending = Page.SavePagesPending( EntryNo );
            }
        }
        else
            DevNo = MAX_INT2_SP00;
    }
    else
    {
        // Handle index page
        Converter_IndexPage     Page( Frame );
        Page.SetDevNoBitCount( Page.ReadDeviceNoBitCount() );

        if( Page.EntryInUse( EntryNo ))
        {
            Page.ReadBlockAddress( EntryNo, BlockAddress );
            DevNo     = BlockAddress.GetDeviceNo();
            BlockNo   = BlockAddress.GetBlockNo();
            IsMarked  = false;
            IsPending = false;
        }
        else
            DevNo = MAX_INT2_SP00;
    }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*
void SAPDBErr_MessageList::FillMessageList( SAPDB_Char  const * const  Component,
        SAPDB_Char  const * const  FileName,
        SAPDB_UInt4 const          LineNumber,
        MessageType const          Type,
        MessageID   const          ID,
        SAPDB_UInt4 const          SystemRC,
        SAPDB_Char  const * const  Message,
        SAPDB_UInt4 const          NumOfArgs,
        SAPDB_Char  const * const  Arg0,
        SAPDB_Char  const * const  Arg1,
        SAPDB_Char  const * const  Arg2,
        SAPDB_Char  const * const  Arg3,
        SAPDB_Char  const * const  Arg4,
        SAPDB_Char  const * const  Arg5,
        SAPDB_Char  const * const  Arg6,
        SAPDB_Char  const * const  Arg7,
        SAPDB_Char  const * const  Arg8,
        SAPDB_Char  const * const  Arg9 )
{
    // only a dummy
}


SAPDBErr_MessageList::~SAPDBErr_MessageList()
{
    // only a dummy
}
*/


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
