/****************************************************************************

  module      : hbd405.h

  -------------------------------------------------------------------------

  author      : AlexanderK
  responsible : UweH

  special area: Inverted Lists
  description : Create/Diagnose Index


  last changed: 1999-03-30  18:21
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1998-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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


#ifndef HBD405_H
#define HBD405_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gbd300.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externCpp void 
bd405CreateIndex (
                  cbd300_InvCurrent      &InvCurrent,        // [IN]
                  tgg00_FileId           &PrimFileId,        // [IN]
                  tgg00_StackDesc        &StackDesc,         // [IN]
                  tgg00_IndexMode         IndexMode,         // [IN]
                  tgg00_IndexCounter     &IndexCounter      // [OUT]
                  );

/*---------------------------------------------------------------------------*/

externCpp void 
bd405DiagnoseIndex (
                    cbd300_InvCurrent      &InvCurrent,        // [IN]
                    tbd_current_tree       &PrimCurrent,       // [IN]
                    tbd_current_tree       &TempCurrent,       // [IN]
                    tgg00_Rec              &RecBuffer,         // [IN]
                    tsp00_Int4             &PrimKeyDuplicates, // [OUT]
                    tsp00_Int4             &PrimKeyMissing     // [OUT]
                    );

/*---------------------------------------------------------------------------*/

externCpp void 
bd405ParticipantCreateIndex (
                             tbd_current_tree       &InvCurrent,        // [IN]
                             tbd_current_tree       &PrimCurrent,       // [IN]
                             tgg00_StackDesc        &StackDesc,         // [IN]
                             tsp00_BytePtr           pStartPrimKey,     // [IN]
                             tsp00_BytePtr           pStopPrimKey,      // [IN]
                             tsp00_Int4              StartPrimKeyLen,   // [IN]
                             tsp00_Int4              StopPrimKeyLen,    // [IN]
                             tsp00_Int4              NumLeavesToScan,   // [IN]
                             tsp00_Int4              AvgRecordsPerLeaf, // [IN]
                             tsp00_PageNo            PrimQueuePno,      // [IN]
                             tsp00_PageNo            SecQueuePno,       // [IN]
                             tsp00_Bool              bUseMemorySort,    // [IN]
                             tgg00_IndexMode         IndexMode,         // [IN]
                             tgg00_IndexCounter     &IndexCounter,      // [OUT]
                             tsp00_Int4             &TempLeafPageCount, // [OUT]
                             tsp00_PageNo           &TempInvRoot,       // [OUT]
                             tsp00_Bool             &bFileEntriesExist  // [OUT]
                             );
#endif  /* HBD405_H */

