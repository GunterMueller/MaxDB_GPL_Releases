/*!================================================================
 module:        PIn_Packet.h

 -------------------------------------------------------------------

 responsible:   TTK

 special area:  SQL

 description:   Packet

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

#ifndef PIn_PACKET_H
#define PIn_PACKET_H

#include "vsp001.h"
#include "SAPDB/PacketInterface/PIn_Segment.h"

/*!
  Class: PIn_SQLPacket
 */
class PIn_SQLPacket
{
protected:
    PIn_SQLPacket (tsp1_packet * packetPtr = NULL);
    ~PIn_SQLPacket ();

public:
    /*! Function: TraceOn

        description: dumps content of packet to @param{trace}.
     */
    void TraceOn (PIn_TraceStream * trace, int buflimit = -1);

    /*! Function: Length

        description: return the complete length in bytes
     */
    int Length () const;

    /*! Function: IsValid

        description:
     */
    bool IsValid () const;

    /*! Function: Invalidate

        description: makes the reference invalid.
     */
    void Invalidate ();

    /*! Function: GetRawPacket

        description:
        prototypes: 2
     */
    tsp1_packet * GetRawPacket ();
    const tsp1_packet * GetRawPacket () const;

    /*! Function: GetRawHeader

        description:
        prototypes: 2
     */
    tsp1_packet_header * GetRawHeader ();
    const tsp1_packet_header * GetRawHeader () const;

protected:
    tsp1_packet * rawPacket;
};

/*!
  EndClass: PIn_SQLPacket
 */

/*----------------------------------------*/

/*!
  Class: PIn_RequestPacket
 */
class PIn_RequestPacket : public PIn_SQLPacket
{
public:
    PIn_RequestPacket (tsp1_packet * packetPtr, int size, int unicode = 0);
    PIn_RequestPacket (tsp1_packet * packetPtr, int size, int unicode, const char *compname, const char *version);
    PIn_RequestPacket (tsp1_packet * packetPtr);        // This one is for the FASTLOADER mainly
    PIn_RequestPacket ();
    ~PIn_RequestPacket ();

    /*! Function: InitVersion

        description: sets the component string and version

        arguments:
            component   [in] a three character string
            version     [in] a five character string. A NULL value
                will query the version module vsp100 for the actual version.
     */
    void InitVersion (const char * component, const char * version = NULL);

    /*! Function: Reset

        description: reinits the package for a new request.
     */
    void Reset ();

    /*! Function: CloseSegment

        description:
     */
    void CloseSegment (PIn_RequestSegment & segment);

    /*! Function: AddSegment

        description: creates a new Segment.
     */
    /* PTS 1108446 sqlMode added */
    /* PTS 1112690 parsingAgain added */
    PIn_RequestSegment AddSegment (tsp1_cmd_mess_type_Param messType,
      const tsp1_sqlmode_Enum &sqlMode,
      const pasbool parsingAgain = false );

    /*! Function: AddMassSegment

          description: creates a new Segment for a mass command.
     */
    /* PTS 1119960, added */
    PIn_RequestSegment AddMassSegment (tsp1_cmd_mess_type_Param messType,
      const tsp1_sqlmode_Enum &sqlMode,
      const pasbool parsingAgain = false );
};

/*!
  EndClass: PIn_RequestPacket
 */

/*----------------------------------------*/

/*!
  Class: PIn_ReplyPacket
 */
class PIn_ReplyPacket : public PIn_SQLPacket
{
public:
    PIn_ReplyPacket (tsp1_packet * packetPtr = NULL);
    ~PIn_ReplyPacket ();

    /*! Function: GetFirstSegment

        description: returns the first Segment.
     */
    const PIn_ReplySegment GetFirstSegment () const;

    /*! Function: GetNextSegment

        description: returns the Segment following @parm{segment}.

            When @parm{segment} was the last Segment, an invalid Segment is
            returned.
     */
    const PIn_ReplySegment GetNextSegment (const PIn_ReplySegment & segment) const;
};

/*!
  EndClass: PIn_ReplyPacket
 */

#endif

