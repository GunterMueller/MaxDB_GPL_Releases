/*!================================================================
 module:    vpr04Long.h

 responsible: BurkhardD, ThomasS

 special area:   CPCDrv | Client | Longvarchar

 description: Longvarchar Conversion

 see:




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




 ===================================================================*/

#ifndef __VPR04_LONG_H__
#define __VPR04_LONG_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "vpr01Cursor.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL MEMBERS                                                *
 *==================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void pr04LongGetHostInfo(sqlcatype *sqlca, sqlxatype *sqlxa,
			   tpr_longvarcharrec *lvcrec, char inp);
  void pr04LongInitLD(sqlcatype *sqlca, sqlxatype *sqlxa);
  void pr04LongLvcInitRec (sqlcatype *sqlca, tpr_longvarcharrec *lvcrec);
  bool pr04LongILvcOneRecord (sqlcatype*, sqlxatype*, sqlgaentry*,
			      tpr00_ParseId*, tsp1_part_ptr,
			      tsp1_part_ptr);
  bool pr04LongPutvalInput (sqlcatype*, sqlxatype*, sqlgaentry*);
  bool pr04LongOutputLvc (sqlcatype*, sqlxatype*, sqlgaentry*,
			  struct tpr_sqlloop);
  bool pr04LongGetvalOutput (sqlcatype*, sqlxatype*, sqlgaentry*, int);
  bool pr04LongCloseDescriptors (tpr01_ConDesc*);
  void pr04LongRemoveDescriptors (sqlcatype*);
  boolean pr04LongPOdbcData (sqlcatype *sqlca, sqlxatype *sqlxa,
			     sqlgaentry *ga, tpr_longvarcharrec *lvcrec,
			     tsp00_Int2 *spindex, bool *first);
  boolean pr04LongGOdbcData (sqlcatype *sqlca, sqlxatype *sqlxa,
			     sqlgaentry *ga, tpr_longvarcharrec *lvcrec,
			     tsp00_Int2 *spindex);
  boolean pr04LongOdbcColIsTrunc (sqlcatype *sqlca, int colno);
  void pr04LongTraceInit (sqlcatype *sqlca, int trtxt, tsp00_Lname *vnam, int colno);
  boolean pr04LongGetTotalLength (sqlcatype *sqlca, int ind, int *len);
#ifdef __cplusplus
}
#endif

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

#endif
