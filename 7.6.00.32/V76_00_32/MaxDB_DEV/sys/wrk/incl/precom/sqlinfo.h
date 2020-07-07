/*
 sqlinfo.h - definitions/declarations for processing abap diagnose
       info. This file extended the general declaration from cpc.h
       of the SAPDB C/C++ Precompiler

       Copyright (c) 1998-2005 SAP AG. All rights reserved.



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




*/
#ifndef __VPR03_ABAPINFO_H__
#define __VPR03_ABAPINFO_H__

#ifndef externC
#ifdef __cplusplus
#define externC extern "C"
#else
#define externC
#endif
#endif

/* callbackfunction for handling diagnose function */
typedef int (SQLABAPInfoProc)(char *szABAPName, char *szABAPExtra);
externC SQLABAPInfoProc *sqlInitABAPInfo(SQLABAPInfoProc *fnABAPInfo);

/* callbackfunction for handling checkpoints */
#define sqlWaitCheckPoint   (1)
#define sqlNoWaitCheckPoint (0)
typedef int (SQLCheckPointProc)();
/* If the Checkpoint callback function is defined and returns
   sqlNoWaitCheckPoint the kernel doesn't wait of completing the checkpoint.
   On default the Kernel waits of checkpoints. */
externC SQLCheckPointProc *sqlInitCheckPoint(SQLCheckPointProc *fnCheckPoint);

#endif
