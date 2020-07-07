/****************************************************************************

  module      : gbd05.h

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: general classes used in BD
  description : description ...


  last changed: 1999-09-06  17:01
  see also    : example.html ...

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


#ifndef GBD05_H
#define GBD05_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#ifndef GSP00_H
#   include "gsp00.h"
#endif
#ifndef GGG00_H
#   include "ggg00.h"
#endif
#ifndef GBD00_H
#   include "gbd00.h"
#endif

#ifndef HBD01_1_H
#   include "hbd01_1.h"
#endif
#ifndef SAPDB_MEMCOPYMOVE_HPP
#   include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class cbd05_Current : public tbd_current_tree
{
public:
    cbd05_Current ()
    {};

    cbd05_Current (tbd_current_tree &curr);

    cbd05_Current (tgg00_TransContext &Trans,
        tgg00_MessType_Enum MessType);

    cbd05_Current (tgg00_TransContext   &Trans, 
        tgg00_MessType_Enum   MessType,
        const tgg00_FileId   &FileId);

    void bd05SetCurrent (tgg00_TransContext  &Trans, 
        tgg00_MessType_Enum  MessType);

    void bd05SetCurrent (tgg00_TransContext    &Trans, 
        tgg00_MessType_Enum    MessType,
        const tgg00_FileId    &FileId);

};

/*===========================================================================*
 *  DEFINITION OF METHODS                                                    *
 *===========================================================================*/

inline cbd05_Current::cbd05_Current (tbd_current_tree &curr)
{
    *static_cast<tbd_current_tree*>(this) = curr;
};

inline void 
cbd05_Current::bd05SetCurrent (tgg00_TransContext  &Trans, 
							   tgg00_MessType_Enum  MessType)
{
    SAPDB_MemCopyNoCheck (STATIC_CAST (tbd_current_tree*,this), &b01nilcurrent, sizeof (b01nilcurrent)); 
    // following line has been rewritten because of better inlining (at least with VC++ 7.0 FF)
    // curr_action = (tgg00_MessType::fromConst(MessType)); 
	curr_action.becomes(MessType); 
    curr_trans  = &Trans;
}

/*---------------------------------------------------------------------------*/

inline void 
cbd05_Current::bd05SetCurrent (tgg00_TransContext   &Trans, 
							   tgg00_MessType_Enum   MessType,
							   const tgg00_FileId   &FileId)
{
    bd05SetCurrent (Trans,  MessType);
	curr_tree_id = FileId;
}

/*---------------------------------------------------------------------------*/
inline
cbd05_Current::cbd05_Current (tgg00_TransContext     &Trans, 
							  tgg00_MessType_Enum     MessType)
//:tbd_current_tree (b01nilcurrent) /* PTS 1105458 AK 26/01/2000 */ 
{
	SAPDB_MemCopyNoCheck (STATIC_CAST (tbd_current_tree*,this), &b01nilcurrent, sizeof (b01nilcurrent)); 
	curr_action.becomes(MessType); 
    curr_trans  = &Trans;
}

/*---------------------------------------------------------------------------*/
inline
cbd05_Current::cbd05_Current (tgg00_TransContext   &Trans, 
					    	  tgg00_MessType_Enum   MessType,
							  const tgg00_FileId   &FileId)
:tbd_current_tree (b01nilcurrent)
{
	curr_action.becomes(MessType); 
    curr_trans   = &Trans;
	curr_tree_id = FileId;
}

/*---------------------------------------------------------------------------*/

#endif  /* GBD05_H */
