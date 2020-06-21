/*!
  @file           vgg91.cpp
  @author         JuergenA
  @author         UweH
  @ingroup        GG_kernel_classes
  @brief          implements methods for tgg00_FileVersion and tgg91_SessionNo

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


*/

#include "ggg00.h"
#include "ggg91.h"
#include "hgg06.h"
#include "ServerTasks/SrvTasks_JobTypes.hpp"
/* ========================================================================== */
/* ==========                tgg91_FileVersion                      ========= */
/* ========================================================================== */

void tgg91_FileVersion::gg91AppendToLine (int        &LineLen,
                                          tsp00_Line &Line) const
{
    
    if ( this->gg91IsSpecialVers () )
    {
        gg90StrToLine (_T("special"), LineLen, Line);
        return;
    }
    
    if ( this->gg91IsDummyVers () )
    {
        gg90StrToLine (_T("dummy"), LineLen, Line);
        return;
    }
    
    this->gg90AppendToLine (LineLen, Line);
}



/* ========================================================================== */
/* ==========                 tgg91_SessionNo                       ========= */
/* ========================================================================== */

void tgg91_SessionNo::gg91BuildServerTaskTraceSession (SAPDB_UInt1 jobtype, /*SrvTasks_JobType*/
                                                       SAPDB_UInt2 jobno)
{
    this->data_IV [0] = NIL_BYTE_GG90;
    this->data_IV [1] = jobtype;
    this->data_IV [2] = (jobno & 0xFF00) >> 8;
    this->data_IV [3] = (jobno & 0x00FF);
}

// ----------------------------------------------------------------------------

void tgg91_SessionNo::gg91AppendToLine (int        &LineLen,
                                        tsp00_Line &Line) const
{
    // UH 2004-11-11 new format

    if ( gg90IsNil() )
        return;

    if ( NIL_BYTE_GG90 != this->data_IV [0] )
    {
        this->gg90AppendToLine (LineLen, Line);
        return;
    }
    // special SessionNo: FF ?? ?? ??
    
    if ( (NIL_BYTE_GG90 == this->data_IV [1]) && (0 == this->data_IV [2]) )
    {
        // TASK SessionNo: FF FF 00 <TaskType>
        
        g06TaskTypeToLine (this->data_IV [3], LineLen, Line);
        return;
    }
    
    // server SessionNo: <counter lo> <counter hi> <jobtype> FF
    const SrvTasks_JobTypes jobtype = this->data_IV [1] < jobLAST_DO_NOT_USE_OR_REMOVE
                                      ? static_cast<SrvTasks_JobTypes>(this->data_IV [1])
                                      : jobUnspecified;
    
    tgg90_Cint2 AuxCnt; 
    AuxCnt.becomes ( (char*) &(this->data_IV [2]) );

    gg90StrToLine (SrvTasks_JobTypesString[jobtype], LineLen, Line);
    LineLen++;
    AuxCnt.gg90AppendToLine (LineLen, Line);
}

/* ========================================================================== */
/* ==========                 tgg91_TransNo                         ========= */
/* ========================================================================== */

const char * tgg91_TransNo::gg91_MAX_TRANS_ID = "\xFF\xFF\xFF\x00\x00\x00";
