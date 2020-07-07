/*!================================================================

 module:        PIn_RequestWriter.cpp

 -------------------------------------------------------------------

 responsible:   TTK

 special area:  SQL

 description:   RequestWriter

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

#include "SAPDB/PacketInterface/PIn_RequestWriter.h"
#include "hsp77.h"

/*----------------------------------------*/

PIn_RequestWriter::PIn_RequestWriter (PIn_RequestPacket packetParm)
   : packet (packetParm)
{
}

/*----------------------------------------*/

PIn_RequestWriter::~PIn_RequestWriter ()
{
}

/*----------------------------------------*/

void PIn_RequestWriter::Reset ()
{
    this->part.Invalidate ();
    this->segment.Invalidate ();
    this->packet.Reset ();
}

/*----------------------------------------*/

/* PTS 1108446 sqlMode added */
    /* PTS 1112690 parsingAgain added */
PIn_RequestSegment * PIn_RequestWriter::AddSegment (tsp1_cmd_mess_type_Param messType,
                                                          const tsp1_sqlmode_Enum &sqlMode,
                                                          const pasbool parsingAgain )
{
    this->CloseSegment ();
    /* PTS 1108446 sqlMode added */
    /* PTS 1112690 parsingAgain added */
    this->segment = this->packet.AddSegment (messType, sqlMode, parsingAgain ); 
    return &this->segment;
}

/*----- PTS 1119960 ----------------------*/

PIn_RequestSegment * PIn_RequestWriter::AddMassSegment (tsp1_cmd_mess_type_Param messType,
                                                              const tsp1_sqlmode_Enum &sqlMode,
                                                              const pasbool parsingAgain )
{
    this->CloseSegment ();
    this->segment = this->packet.AddMassSegment (messType, sqlMode, parsingAgain ); 
    return &this->segment;
}

/*----------------------------------------*/

void PIn_RequestWriter::CloseSegment ()
{
    if (this->segment.IsValid ()) {
        this->ClosePart ();
        this->packet.CloseSegment (this->segment);
        this->segment.Invalidate ();
    }
}

/*----------------------------------------*/

PIn_Part * PIn_RequestWriter::AddPart (tsp1_part_kind_Param partKind)
{
    this->ClosePart ();
    this->part = this->segment.AddPart (partKind);
    return &this->part;
}

/*----------------------------------------*/

void PIn_RequestWriter::ClosePart ()
{
    if (this->part.IsValid ()) {
        this->segment.ClosePart (this->part);
        this->part.Invalidate ();
    }
}

/*----------------------------------------*/

void PIn_RequestWriter::Close ()
{
    this->CloseSegment ();
}

/*----------------------------------------*/

/* PTS 1108446 sqlMode added */
/* PTS 1112690 parsingAgain added */
PIn_Part * PIn_RequestWriter::InitDbs (const tsp1_sqlmode_Enum &sqlMode, 
                                             const char *cmd, 
                                             int len,
                                             const pasbool parsingAgain )
{
  if (len == -1) {
      len = (int)strlen (cmd);
  }
  /* PTS 1108446 sqlMode added */
  /* PTS 1112690 parsingAgain added */
  this->AddSegment (sp1m_dbs, sqlMode, parsingAgain );   
  this->AddPart (sp1pk_command);
  if ( len > this->part.BytesRemaining ()) return NULL;
  this->part.AddArgument (cmd, len);
  return &this->part;
}

/*----------------------------------------*/

/* PTS 1108446 sqlMode added */
/* PTS 1112690 parsingAgain added */
PIn_Part * PIn_RequestWriter::InitDbs (const tsp1_sqlmode_Enum &sqlMode,
                                             const tsp81_UCS2Char *cmd, 
                                             int len,
                                             const pasbool parsingAgain )
{
  if (len == -1) {
      len = (int)sp81UCS2strlen (cmd) * sizeof(tsp81_UCS2Char);
  }
  /* PTS 1108446 sqlMode added */
  /* PTS 1112690 parsingAgain added */
  this->AddSegment (sp1m_dbs, sqlMode, parsingAgain );   
  this->AddPart (sp1pk_command);
  if ( len > this->part.BytesRemaining ()) return NULL;
  this->part.AddArgument ((char*)cmd, len);
  return &this->part;
}

/*----------------------------------------*/

/* PTS 1108446 sqlMode added */
/* PTS 1112690 parsingAgain added */
PIn_Part * PIn_RequestWriter::InitDbsf (const tsp1_sqlmode_Enum &sqlMode,
                                              const pasbool parsingAgain,
                                              const char *fmt, ...)
{
    va_list ap;
    int     cmdlen;

    va_start (ap, fmt);
    /* PTS 1108446 sqlMode added */
    /* PTS 1112690 parsingAgain added */
    this->AddSegment (sp1m_dbs, sqlMode, parsingAgain );   
    this->AddPart (sp1pk_command);
    cmdlen = this->VPrintf (fmt, ap);
    va_end (ap);
    if ( cmdlen )
      return &this->part;
    else
      return NULL;
}

/*----------------------------------------*/

/* PTS 1108446 sqlMode added */
/* PTS 1112690 parsingAgain added */
PIn_Part * PIn_RequestWriter::InitParse (const tsp1_sqlmode_Enum &sqlMode,
                                               const char *cmd, 
                                               int len,
                                               const pasbool parsingAgain )
{
  if (len == -1) 
    len = (int)strlen (cmd);
  /* PTS 1108446 sqlMode added */
  /* PTS 1112690 parsingAgain added */
  this->AddSegment (sp1m_parse, sqlMode, parsingAgain);   
  this->AddPart (sp1pk_command);
  if ( len > this->part.BytesRemaining ()) return NULL;
  this->part.AddArgument (cmd, len);
  return &this->part;
}

/*----- PTS 1119960 ----------------------*/

PIn_Part * PIn_RequestWriter::InitMassParse (const tsp1_sqlmode_Enum &sqlMode,
                                               const char *cmd, 
                                               int len,
                                               const pasbool parsingAgain )
{
  if (len == -1) 
    len = (int)strlen (cmd);
  /* PTS 1108446 sqlMode added */
  /* PTS 1112690 parsingAgain added */
  this->AddMassSegment (sp1m_parse, sqlMode, parsingAgain);   
  this->AddPart (sp1pk_command);
  if ( len > this->part.BytesRemaining ()) return NULL;
  this->part.AddArgument (cmd, len);
  return &this->part;
}

/*----------------------------------------*/

/* PTS 1108446 sqlMode added */
/* PTS 1112690 parsingAgain added */
PIn_Part * PIn_RequestWriter::InitParse (const tsp1_sqlmode_Enum &sqlMode,
                                               const tsp81_UCS2Char *cmd, 
                                               int len,
                                               const pasbool parsingAgain )
{
  if (len == -1) 
    len = (int)sp81UCS2strlen (cmd) * sizeof(tsp81_UCS2Char);
  /* PTS 1108446 sqlMode added */
  /* PTS 1112690 parsingAgain added */
  this->AddSegment (sp1m_parse, sqlMode, parsingAgain);   
  this->AddPart (sp1pk_command);
  if ( len > this->part.BytesRemaining ()) return NULL;
  this->part.AddArgument ((char*)cmd, len);
  return &this->part;
}

/*----------------------------------------*/

/* PTS 1108446 sqlMode added */
/* PTS 1112690 parsingAgain added */
PIn_Part * PIn_RequestWriter::InitParsef (const tsp1_sqlmode_Enum &sqlMode,
                                                const pasbool parsingAgain,
                                                const char *fmt, ...)
{
    va_list ap;
    int     cmdlen;

    va_start (ap, fmt);
    /* PTS 1108446 sqlMode added */
    /* PTS 1112690 parsingAgain added */
    this->AddSegment (sp1m_parse, sqlMode, parsingAgain);   
    this->AddPart (sp1pk_command);
    cmdlen = this->VPrintf (fmt, ap);
    va_end (ap);
    if ( cmdlen )
      return &this->part;
    else
      return NULL;
}

/*----------------------------------------*/

/* PTS 1108446 sqlMode added */
void PIn_RequestWriter::InitExecute (const tsp1_sqlmode_Enum &sqlMode,
                                        const tin02_ParseId * parseId)
{
    this->AddSegment (sp1m_execute, sqlMode);   /* PTS 1108446 sqlMode added */
    this->AddPart (sp1pk_command);
    this->part.AddArgument (parseId, sizeof (tin02_ParseId));
}

/*----- PTS 1119960 ----------------------*/


void PIn_RequestWriter::InitMassExecute (const tsp1_sqlmode_Enum &sqlMode,
                                            const tin02_ParseId * parseId)
{
    this->AddSegment (sp1m_execute, sqlMode);
    this->AddPart (sp1pk_parsid);
    this->part.AddArgument (parseId, sizeof (tin02_ParseId));
}

/*----------------------------------------*/

PIn_Part * PIn_RequestWriter::AddDataPart ()
{
    this->AddPart (sp1pk_data);
    return &this->part;
}

/*----------------------------------------*/

int PIn_RequestWriter::VPrintf (const char *fmt, va_list ap)
{
    char * writeBuf;
    int commandLen;
    int bytesRemaining;

    writeBuf = REINTERPRET_CAST (char *, this->part.GetWriteData ());
    bytesRemaining = this->part.BytesRemaining ();
    commandLen = sp77vsprintf (writeBuf, bytesRemaining, fmt, ap);
    if ( commandLen >= bytesRemaining) return 0;
    if (commandLen > 0) {
        this->part.ExtendLength (commandLen);
        this->part.AddArgument (NULL, -1);
    }
    return commandLen;
}

/*----------------------------------------*/


