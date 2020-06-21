/*!================================================================
 module:    vpi01EXECSQL.h

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | EXEC SQL Verarbeitung
 
 description: EXEC SQL Verarbeitung
 
 see:  
 
 change history: 
         2001-11-08 D025086 Cancel session bugfix PTS 1112515 and PASCAL 
                    port from p01cancel.
         2000-11-16 d025086 (PTS 1108288) pr01EXECSQLVersion and PASCAL
                    port from p01version.


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

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

void pr01EXECSQLOptions(struct tpr01_SQLDesc *SQLDesc);
void pr01EXECSQLTraceLine(struct tpr01_SQLDesc *SQLDesc);
void pr01EXECSQLVersion(struct tpr01_SQLDesc *SQLDesc);
void pr01EXECSQLCancel(struct tpr01_SQLDesc *SQLDesc);
