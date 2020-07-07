/*!
  @file           IFRPacket_Segment.cpp
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Packet
  @brief          
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_Segment.h"

int IFRPacket_Segment::Len_O = 0;
int IFRPacket_Segment::Offs_O = 4;
int IFRPacket_Segment::NoOfParts_O = 8;
int IFRPacket_Segment::OwnIndex_O = 10;
int IFRPacket_Segment::SegmKind_O = 12;
int IFRPacket_Segment::MessType_O = 13;
int IFRPacket_Segment::SqlMode_O = 14; 
int IFRPacket_Segment::Producer_O = 15;
int IFRPacket_Segment::CommitImmediately_O = 16;
int IFRPacket_Segment::IgnoreCostwarning_O = 17;
int IFRPacket_Segment::Prepare_O = 18;
int IFRPacket_Segment::WithInfo_O = 19;
int IFRPacket_Segment::MassCmd_O = 20;
int IFRPacket_Segment::ParsingAgain_O = 21;
int IFRPacket_Segment::SqlState_O = 13;
int IFRPacket_Segment::Returncode_O = 18;
int IFRPacket_Segment::ErrorPos_O = 20;
int IFRPacket_Segment::ExternWarning_O = 24;
int IFRPacket_Segment::InternWarning_O = 26;
int IFRPacket_Segment::FunctionCode_O = 28;
int IFRPacket_Segment::TraceLevel_O = 30;
int IFRPacket_Segment::Part_O = 40;
