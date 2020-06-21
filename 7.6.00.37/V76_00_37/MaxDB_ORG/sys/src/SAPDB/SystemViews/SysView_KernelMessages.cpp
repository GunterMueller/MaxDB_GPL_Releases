/****************************************************************************/
/*!

  @file        SysView_KernelMessages.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "KernelMessages" view class.

  @see        

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_KernelMessages.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SQLManager/SQLMan_Context.hpp"
/*
#include "RunTime/MemoryManagement/RTEMem_AllocatorInfoReader.hpp"
*/


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
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_KernelMessages::Create()
{

	m_Table->AppendCol (ITOCT_FIXED, SV_ERRORCODE, 5);    
	m_Table->AppendCol (ITOCT_CHAR,  SV_ERRORTEXT, 80);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_KernelMessages::GetColCount()
{
	return SV_CC_KERNELMESSAGES;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_KernelMessages::EstimateRows()
{
	return SV_ER_KERNELMESSAGES;
}

/*---------------------------------------------------------------------------*/

void SysView_KernelMessages::Execute()
{
	const char          *errorText;
	SAPDB_Int2           msgno;
    SQLManErr_Interface &errorHandlingInstance = SQLManErr_Interface::GetInstance();

    m_Table->GetCatalogTable();

    if (m_Context.IsOk())
    {
		errorText = errorHandlingInstance.GetFirstText(&msgno);
        while (NULL != errorText )
        {
            m_Table->MoveToCol (ITOVT_INT2, &msgno, 0);
			m_Table->MoveToCol (ITOVT_CHARPTR, (void*)errorText, strlen(errorText));
			errorText = errorHandlingInstance.GetNextText(&msgno);
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
