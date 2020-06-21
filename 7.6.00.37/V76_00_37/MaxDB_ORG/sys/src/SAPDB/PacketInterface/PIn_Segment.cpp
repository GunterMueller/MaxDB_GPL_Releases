/*!================================================================
 module:        PIn_Segment.cpp

 -------------------------------------------------------------------

 responsible:   D030044

 special area:  SQL

 description:   Segment

 -------------------------------------------------------------------

 Copyright (c) 2000-2005 SAP AG




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

#include "SAPDB/PacketInterface/PIn_Segment.h"
/*----------------------------------------*/

static tsp1_packet *
PIn_GetSegmentPacket (
    tsp1_segment * segment)
{
    int segmOffset;
    teo00_Byte * segmAddr;
    teo00_Byte * packetAddr;
    tsp1_packet * packet;

    segmOffset = segment->sp1s_segm_offset ();
    segmAddr = REINTERPRET_CAST (teo00_Byte*, segment);
    packetAddr = segmAddr - segmOffset - sizeof (tsp1_packet_header);
    packet = REINTERPRET_CAST (tsp1_packet*, packetAddr);
    return packet;
}

/*----------------------------------------*/

PIn_Segment::PIn_Segment (tsp1_segment * segmentArg)
{
    this->rawSegment = segmentArg;
}

/*----------------------------------------*/

PIn_Segment::~PIn_Segment ()
{
    this->rawSegment = NULL;
}

/*----------------------------------------*/

const char * segmKindStrings [] = {
    "nil",
    "cmd",
    "return",
    "proccall",
    "procreply"
};

const PIn_EnumDesc segmKindDesc = {
    sp1sk_last_segment_kind,
    "Invalid segment kind",
    segmKindStrings
};

const char * messtypeStrings [] = {
    "nil",
    "cmd_lower_bound",
    "dbs",
    "parse",
    "getparse",
    "syntax",
    "cfill1",
    "cfill2",
    "cfill3",
    "cfill4",
    "cfill5",
    "cmd_upper_bound",
    "no_cmd_lower_bound",
    "execute",
    "getexecute",
    "putval",
    "getval",
    "load",
    "unload",
    "ncfill1",
    "ncfill2",
    "ncfill3",
    "ncfill4",
    "ncfill5",
    "no_cmd_upper_bound",
    "hello",
    "util_lower_bound",
    "utility",
    "incopy",
    "ufill1",
    "outcopy",
    "diag_outcopy",
    "ufill3",
    "ufill4",
    "ufill5",
    "ufill6",
    "ufill7",
    "util_upper_bound",
    "specials_lower_bound",
    "switch",
    "switchlimit",
    "buflength",
    "minbuf",
    "maxbuf",
    "state_utility",
    "sfill2",
    "sfill3",
    "sfill4",
    "sfill5",
    "specials_upper_bound",
    "wait_for_event",
    "procserv_lower_bound",
    "procserv_call",
    "procserv_reply",
    "procserv_fill1",
    "procserv_fill2",
    "procserv_fill3",
    "procserv_fill4",
    "procserv_fill5",
    "procserv_upper_bound"
};

const PIn_EnumDesc messtypeDesc = {
    sp1m_last_cmd_mess_type,
    "Invalid mess type %d",
    messtypeStrings
};

const char * sqlModeStrings [] = {
    "nil",
    "session",
    "internal",
    "ansi",
    "db2",
    "oracle"
};

const PIn_EnumDesc sqlModeDesc = {
    sp1sm_oracle,
    "Invalid SQL mode %d",
    sqlModeStrings
};

const char * producerStrings [] = {
    "nil",
    "user",
    "internal",
    "kernel",
    "installation"
};

const PIn_EnumDesc producerDesc = {
    sp1pr_installation,
    "invalid producer %d",
    producerStrings
};

const char * commandOptionsStrings [] = {
  "selfetch_off",
  "scrollable_cursor_on",
  "no_resultset_close_needed",
  "not_used2",
  "not_used3",
  "not_used4",
  "not_used5",
  "not_used6"
};

/*----------------------------------------*/

void PIn_Segment::TraceOn (PIn_TraceStream * trace, int buflimit)
{
    tsp1_segment_header * header = &this->rawSegment->sp1s_segm_header ();
    char tmpBuf [tmpBufSize_PI] [3];
    if (trace != NULL) {
      trace->printf ("   %s Segment %d at %d (%d of %d bytes)\n",
                     PIn_EnumDump (&segmKindDesc, header->sp1s_segm_kind (), tmpBuf [0]),
                     header->sp1s_own_index (), header->sp1s_segm_offset (),
                     header->sp1s_segm_len (), this->Size ());
      switch (header->sp1s_segm_kind()) {
      case sp1sk_cmd:
      case sp1sk_proccall:
        trace->printf ("        messtype: %s  sqlmode: %s  producer: %s\n",
                       PIn_EnumDump (&messtypeDesc, header->sp1c_mess_type (), tmpBuf [0]),
                       PIn_EnumDump (&sqlModeDesc, header->sp1c_sqlmode (), tmpBuf [1]),
                       PIn_EnumDump (&producerDesc, header->sp1c_producer (), tmpBuf [2]));
        OptionsDump (trace, header);

        break;
      case sp1sk_return:
      case sp1sk_procreply:
        trace->printf ("        RC: %d  %.5s  (Pos %d) Function %d\n",
                       header->sp1r_returncode (), (char *)(header->sp1r_sqlstate ()),
                       header->sp1r_errorpos (), header->sp1r_function_code ());
        break;
      default:
        trace->writeln ("unknown segment kind");
        trace->printf ("        messtype: %s  sqlmode: %s  producer: %s\n",
                       PIn_EnumDump (&messtypeDesc, header->sp1c_mess_type (), tmpBuf [0]),
                       PIn_EnumDump (&sqlModeDesc, header->sp1c_sqlmode (), tmpBuf [1]),
                       PIn_EnumDump (&producerDesc, header->sp1c_producer (), tmpBuf [2]));
        OptionsDump (trace, header);
        trace->printf ("        RC: %d  %.5s  (Pos %d) Function %d\n",
                       header->sp1r_returncode (), (char *)(header->sp1r_sqlstate ()),
                       header->sp1r_errorpos (), header->sp1r_function_code ());
        break;
      }
      trace->printf ("        %d parts:\n", header->sp1s_no_of_parts ());
      
      PIn_ReplySegment tmpSegment (this->rawSegment);
      PIn_Part part = tmpSegment.GetFirstPart ();
      while (part.IsValid ()) {
        part.TraceOn (trace, buflimit);
        part = tmpSegment.GetNextPart (part);
      }
    }
}

void PIn_Segment::OptionsDump (PIn_TraceStream * trace, tsp1_segment_header * header)
{
  trace->printf ("        Options: %s%s%s%s%s%s\n",
                 header->sp1c_commit_immediately () ? "commit " : "",
                 header->sp1c_ignore_costwarning () ? "ignore costwarning " : "",
                 header->sp1c_prepare () ? "prepare " : "",
                 header->sp1c_with_info () ? "with info " : "",
                 header->sp1c_mass_cmd () ? "mass cmd " : "",
                 header->sp1c_parsing_again () ? "parsing again " : "");
  int i=0;
  for(;i<sizeof(commandOptionsStrings)/sizeof(commandOptionsStrings[0]);i++) {
    if (header->sp1c_command_options().includes((tsp1_command_option_Enum)i))
      trace->printf("        %s\n", commandOptionsStrings[i]);
  }
}


/*----------------------------------------*/

int PIn_Segment::Size () const
{
    int result;
    tsp1_packet_header * packetHeader;

    packetHeader = &PIn_GetSegmentPacket (this->rawSegment)->sp1_header;
    result = packetHeader->sp1h_varpart_size
        - this->rawSegment->sp1s_segm_offset ();
    return result;
}

/*----------------------------------------*/

int PIn_Segment::Length () const
{
    return (this->rawSegment->sp1s_segm_len ());
}

/*----------------------------------------*/

bool PIn_Segment::IsValid () const
{
    return (this->rawSegment != NULL);
}

/*----------------------------------------*/

void PIn_Segment::Invalidate ()
{
    this->rawSegment = NULL;
}

/*----------------------------------------*/

tsp1_segment * PIn_Segment::GetRawSegment ()
{
    return this->rawSegment;
}

/*----------------------------------------*/

const tsp1_segment * PIn_Segment::GetRawSegment () const
{
    return this->rawSegment;
}

/*----------------------------------------*/

tsp1_segment_header * PIn_Segment::GetRawHeader ()
{
    return &this->rawSegment->sp1s_segm_header ();
}

/*----------------------------------------*/

const tsp1_segment_header * PIn_Segment::GetRawHeader () const
{
    return &this->rawSegment->sp1s_segm_header ();
}



/*----------------------------------------*/

PIn_RequestSegment::PIn_RequestSegment (tsp1_segment * segmParm)
    : PIn_Segment (segmParm)
{
}

/*----------------------------------------*/

/* PTS 1108446 sqlMode added      */
/* PTS 1112690 parsingAgain added */
void PIn_RequestSegment::Init (
    tsp1_cmd_mess_type_Param messType,
    const tsp1_sqlmode_Enum &sqlMode,
    const pasbool parsingAgain,
    int offset,
    int index)
{
    tsp1_segment_header * header = &this->rawSegment->sp1s_segm_header ();

    memset (header, 0, sizeof (tsp1_segment_header));
    header->sp1s_segm_len () = 0;
    header->sp1s_segm_offset () = offset;
    header->sp1s_own_index () = index;
    header->sp1s_segm_kind ().becomes (sp1sk_cmd);
    header->sp1c_mess_type ().becomes (messType);
//    header->sp1c_sqlmode ().becomes (sp1sm_session_sqlmode);
    header->sp1c_sqlmode ().becomes (sqlMode);      /* PTS 1108446 */
    header->sp1c_producer ().becomes (sp1pr_user_cmd);
    header->sp1c_parsing_again () = parsingAgain;
    // this will update packet as well
    this->ExtendLength (sizeof (tsp1_segment_header));
}

/*----- PTS 1119960 ----------------------*/

void PIn_RequestSegment::MassInit (
    tsp1_cmd_mess_type_Param messType,
    const tsp1_sqlmode_Enum &sqlMode,
    const pasbool parsingAgain,
    int offset,
    int index)
{
    this->Init(messType, sqlMode, parsingAgain, offset, index);
 
    tsp1_segment_header * header = &this->rawSegment->sp1s_segm_header ();
    header->sp1c_mass_cmd () = true;
}

/*----------------------------------------*/

void PIn_RequestSegment::ExtendLength (int increment)
{
    this->rawSegment->sp1s_segm_len () += increment;
    PIn_GetSegmentPacket (this->rawSegment)->sp1_header.sp1h_varpart_len += increment;
}

/*----------------------------------------*/

void PIn_RequestSegment::SetProducer (const tsp1_producer_Param & oProducer)
{
    tsp1_segment_header * header = &this->rawSegment->sp1s_segm_header ();
    header->sp1c_producer ().becomes (oProducer);
}


/*----------------------------------------*/

void
PIn_RequestSegment::SetWithInfo()
{
    tsp1_segment_header * header = &this->rawSegment->sp1s_segm_header ();
    header->sp1c_with_info() = 1;
}


/*----------------------------------------*/

PIn_Part PIn_RequestSegment::AddPart (tsp1_part_kind_Param partKind)
{
    tsp1_part * partAddr;
    tsp1_segment_header * header = this->GetRawHeader ();
    teo00_Byte * rawPtr;

    // calculate part address
    rawPtr = REINTERPRET_CAST (teo00_Byte *, this->rawSegment)
        + this->rawSegment->sp1s_segm_len ();
    partAddr = REINTERPRET_CAST (tsp1_part *, rawPtr);
    // create Part object
    PIn_Part result (partAddr);
    int bytesRemaining = this->Size () - header->sp1s_segm_len ();
    result.Init (partKind, header->sp1s_segm_offset (), bytesRemaining);
    ++header->sp1s_no_of_parts ();
    this->ExtendLength (sizeof (tsp1_part_header));
    // return Part
    return result;
}

/*----------------------------------------*/

void
PIn_RequestSegment::AddPart (tsp1_part_kind_Param partKind, PIn_Part& part)
{
    tsp1_part * partAddr;
    tsp1_segment_header * header = this->GetRawHeader ();
    teo00_Byte * rawPtr;

    // calculate part address
    rawPtr = REINTERPRET_CAST (teo00_Byte *, this->rawSegment)
        + this->rawSegment->sp1s_segm_len ();
    partAddr = REINTERPRET_CAST (tsp1_part *, rawPtr);
    // create Part object
    part = PIn_Part(partAddr);
    int bytesRemaining = this->Size () - header->sp1s_segm_len ();
    part.Init (partKind, header->sp1s_segm_offset (), bytesRemaining);
    ++header->sp1s_no_of_parts ();
    this->ExtendLength (sizeof (tsp1_part_header));
    return;
}


/*----------------------------------------*/

void PIn_RequestSegment::ClosePart (PIn_Part & part)
{
    int alignedLen = ALIGN_EO00 (part.Length (), packetAlignment_PI);

    this->ExtendLength (alignedLen);
}

/*----------------------------------------*/

PIn_ReplySegment::PIn_ReplySegment (tsp1_segment * segmParm)
    : PIn_Segment (segmParm)
{
}

/*----------------------------------------*/

int PIn_ReplySegment::ErrorCode () const
{
    return this->rawSegment->sp1r_returncode ();
}

/*----------------------------------------*/

int PIn_ReplySegment::FunctionCode () const
{
    return this->rawSegment->sp1r_function_code ();
}

/*----------------------------------------*/

const PIn_Part PIn_ReplySegment::GetFirstPart () const
{
  tsp1_part * rawPart;
  if ( this->rawSegment->sp1s_segm_header().sp1s_no_of_parts() )
    rawPart = &this->rawSegment->sp1p_part ();
  else
    rawPart = NULL;
  return PIn_Part (rawPart);
}

/*----------------------------------------*/

const PIn_Part PIn_ReplySegment::GetNextPart (const PIn_Part & part) const
{
    // get address of next part
    int alignedLen = ALIGN_EO00 (part.Length (), packetAlignment_PI);
    const tsp00_Byte * nextRawPart = part.GetReadData () + alignedLen;
    // check it's in range
    tsp00_Byte * endOfSegment = REINTERPRET_CAST (tsp00_Byte*, this->rawSegment)
        + this->rawSegment->sp1s_segm_len ();
    if (nextRawPart >= endOfSegment) {
        nextRawPart = NULL;
    }
    // convert pointer
    tsp1_part * rawPart = (tsp1_part *) nextRawPart;
    // return new Part
    return PIn_Part (rawPart);
}

/*----------------------------------------*/

const PIn_Part PIn_ReplySegment::FindPart (tsp1_part_kind_Param partKind) const
{
    PIn_Part part = this->GetFirstPart ();

    while (part.IsValid () && (part.GetRawHeader ()->sp1p_part_kind != partKind)) {
        part = this->GetNextPart (part);
    }
    // Exit reason 1: ! part.isValid () -> not found -> result is invalid
    // Exit reason 2: part.partKind = partKind -> result is ok
    return part;
}

/*----------------------------------------*/

int PIn_ReplySegment::FindParts (
    int count,
    const  tsp1_part_kind_Param partKinds [],
    PIn_Part parts []) const
{
    int i;
    int found = 0;
    tsp1_part_kind_Param partKind;

    for (i = 0; i < count; ++i) {
        parts [i].Invalidate ();
    }
    PIn_Part part = this->GetFirstPart ();
    while (part.IsValid ()) {
        partKind = part.GetRawHeader ()->sp1p_part_kind;
        for (i = 0; i < count; ++i) {
            if (partKind == partKinds [i]) {
                parts [i] = part;
                ++found;
                break;
            }
        }
        part = this->GetNextPart (part);
    }
    return found;
}

/*----------------------------------------*/


