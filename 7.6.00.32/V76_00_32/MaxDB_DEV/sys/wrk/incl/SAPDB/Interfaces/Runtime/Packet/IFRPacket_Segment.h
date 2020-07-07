/*!
  @file           IFRPacket_Segment.h
  @author         D030044
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
#ifndef IFRPACKET_SEGMENT_H
#define IFRPACKET_SEGMENT_H

#include "SAPDB/SAPDBCommon/SAPDB_ReuseLib.h"

class IFRPacket_Segment : public PIn_Segment {
public:
  // common header
  static int Len_O;  // int4
  static int Offs_O; // int4
  static int NoOfParts_O;    // int2
  static int OwnIndex_O;    // int2
  static int SegmKind_O;    // enum1
  // request segment
  static int MessType_O;    // enum1
  static int SqlMode_O;     // enum1
  static int Producer_O;    // enum1
  static int CommitImmediately_O;   // bool
  static int IgnoreCostwarning_O;   // bool
  static int Prepare_O;             // bool
  static int WithInfo_O;            // bool
  static int MassCmd_O;             // bool
  static int ParsingAgain_O;        // bool
  // reply segment
  static int SqlState_O;    // c5
  static int Returncode_O;  // int2
  static int ErrorPos_O;    // int4
  static int ExternWarning_O;   // set2
  static int InternWarning_O;   // set2
  static int FunctionCode_O;    // int2
  static int TraceLevel_O;      // int1
  static int Part_O;
  
private:
  virtual void dummy () = 0;
};

#endif // IFRPACKET_SEGMENT_H
