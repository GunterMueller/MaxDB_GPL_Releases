/*!***************************************************************************

  module      : gbd497.h

  -------------------------------------------------------------------------

  author      : TorstenS
  responsible : TorstenS

  special area: StackDescInfo
  description : 


  last changed: 1999-08-10  14:00
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG



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


*****************************************************************************/



#ifndef GBD497_H
#define GBD497_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

                     // Content of include files
#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types
#include "gbd00.h"   // PASCAL: BD_kernel_constants_and_types

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define	FIRST_COLUMN_BD497	1

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class cbd497_StackDescInfo
{
public:

    cbd497_StackDescInfo (tgg00_BasisError   &Error,
                         tgg00_StackDesc    &StackDesc);
    
    bool                bd497IsColumnDescending (tsp00_Int4 ColumnNo = 1) const;
    
    bool                bd497IsInvUnique() const {return m_IsInvUnique;};
    
    tsp00_Int2          bd497GetColumnLength (tsp00_Int4 ColumnNo) const;
    
	tgg00_StackDesc&	bd497GetStackDesc() const;

	tsp00_Int4			bd497GetStackDescSize() const;

    tgg00_StEntryAddr   bd497GetStackEntryPtr (tsp00_Int4 ColumnNo);

	tgg00_StackListPtr	bd497GetStackListPtr();

	tsp00_Int4			bd497GetStackSize() const;
    
    tsp00_Int4          bd497NumColumns() const {return m_NumColumns;};
    
    tsp00_Int4          bd497FirstColumn() const {return m_FirstCol;};
    
    tsp00_Int4          bd497LastColumn() const {return m_LastCol;};

     
private:
    tgg00_BasisError    &m_TrError;
    tgg00_StackDesc     &m_StackDesc;
    tgg00_StEntryAddr   m_pStack;
    /* */
    bool                m_IsInvUnique;
    /* */
    const tsp00_Int4    m_NumColumns;
    const tsp00_Int4    m_FirstCol;
    const tsp00_Int4    m_LastCol;
};

/*===========================================================================*
 *  DEFINITION OF METHODS                                                    *
 *===========================================================================*/

inline
cbd497_StackDescInfo::cbd497_StackDescInfo (tgg00_BasisError   &Error,
                                          tgg00_StackDesc    &StackDesc)
                                          :
m_TrError   ( Error ),
m_StackDesc ( StackDesc ),
m_pStack    ( REINTERPRET_CAST (tgg00_StEntryAddr, StackDesc.mst_addr())),
m_NumColumns( m_StackDesc.mmult_cnt ),
m_FirstCol  ( m_StackDesc.mmult_pos ),
m_LastCol   ( m_FirstCol + m_NumColumns - 1 )
{
    m_IsInvUnique = 
        ((op_unique == (m_pStack + m_FirstCol - POS_OFF_DIFF_BD00)->eop())
        ||
        (op_unique_desc == (m_pStack + m_FirstCol - POS_OFF_DIFF_BD00)->eop()));
    
}

/*---------------------------------------------------------------------------*/

inline bool
cbd497_StackDescInfo::bd497IsColumnDescending (tsp00_Int4 ColumnNo) const
{
    if ((ColumnNo < m_FirstCol) || (ColumnNo > m_LastCol)) 
    {   
        m_TrError = e_stack_type_illegal;
        return false;
    }
    
    return (
        (op_unique_desc == (m_pStack + ColumnNo - POS_OFF_DIFF_BD00)->eop()) ||
        (op_order_desc  == (m_pStack + ColumnNo - POS_OFF_DIFF_BD00)->eop())
        );
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int2 
cbd497_StackDescInfo::bd497GetColumnLength (tsp00_Int4 ColumnNo) const
{
    if ((ColumnNo < m_FirstCol) || (ColumnNo > m_LastCol)) 
    {
        m_TrError = e_stack_type_illegal;
        return 0;
    }
    return (m_pStack + ColumnNo - POS_OFF_DIFF_BD00)->elen_var();
}

/*---------------------------------------------------------------------------*/

inline tgg00_StackDesc&
cbd497_StackDescInfo::bd497GetStackDesc() const
{
	return m_StackDesc;
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd497_StackDescInfo::bd497GetStackDescSize() const
{
	return sizeof (tgg00_StackDesc);
}

/*---------------------------------------------------------------------------*/

inline tgg00_StEntryAddr   
cbd497_StackDescInfo::bd497GetStackEntryPtr (tsp00_Int4 ColumnNo)
{
     if ((ColumnNo < m_FirstCol) || (ColumnNo > m_LastCol)) 
    {
        m_TrError = e_stack_type_illegal;
        return NULL;
    }
    return (m_pStack + ColumnNo - POS_OFF_DIFF_BD00);
}

/*---------------------------------------------------------------------------*/

inline tgg00_StackListPtr   
cbd497_StackDescInfo::bd497GetStackListPtr ()
{
	return REINTERPRET_CAST (tgg00_StackListPtr, bd497GetStackEntryPtr (FIRST_COLUMN_BD497));
}
/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd497_StackDescInfo::bd497GetStackSize() const
{
	return (m_StackDesc.mst_max() * sizeof (tgg00_StackEntry));
}


#endif  /* GBD497_H */

 
 

 