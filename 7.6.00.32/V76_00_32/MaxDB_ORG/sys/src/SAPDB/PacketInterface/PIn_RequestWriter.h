/*!================================================================

 module:        PIn_RequestWriter.h

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

#ifndef PIn_REQUESTWRITER_H
#define PIn_REQUESTWRITER_H

#include "SAPDB/PacketInterface/PIn_Packet.h"
#include "hsp81.h"

/*!
  Class: PIn_RequestWriter

  descriptions: allows to fill a RequestPacket sequentially.

    The RequestWriter keeps internally pointers to the current
    Segment and the current Part.
 */
class PIn_RequestWriter
{
public:
    /*! Function: constructor
        description: creates a new RequestWriter.
     */
    PIn_RequestWriter (PIn_RequestPacket packetParm);

    /*! Function: destructor
        description: releases ressources
     */
    virtual ~PIn_RequestWriter ();

    /*! Function: Reset
        description: reinitializes the writer for the next request.
     */
    void Reset ();

    /*! Function: AddSegment
        description: creates a new Segment of the given type.
     */
    /* PTS 1108446 sqlMode added */
    /* PTS 1112690 parsingAgain added */
    PIn_RequestSegment * AddSegment (tsp1_cmd_mess_type_Param messType,
      const tsp1_sqlmode_Enum &sqlMode,
      const pasbool parsingAgain = false );

    /*! Function: AddMassSegment
        description: creates a new Segment of the given type for a mass command.
     */
    /* PTS 1119960, added */
    PIn_RequestSegment * AddMassSegment (tsp1_cmd_mess_type_Param messType,
      const tsp1_sqlmode_Enum &sqlMode,
      const pasbool parsingAgain = false );

    /*! Function: CloseSegment
        description: closes the current Segment.

            This causes an implizit @method{ClosePart}. The Segment pointer
            from the last @method{AddSegment} is no longer valid.<BR>
            It shouldn't be necessary to call this directly as it
            is called implicitly by @method{Close} and
            @method{AddSegment}.
     */
    void CloseSegment ();

    /*! Function: AddPart
        description: creates a new Part of the given Kind.
     */
    PIn_Part * AddPart (tsp1_part_kind_Param partKind);

    /*! Function: CloseSegment
        description: closes the current Segment.

            This causes an implizit @method{ClosePart}. The Segment pointer
            from the last @method{AddSegment} is no longer valid.<BR>
            It shouldn't be necessary to call this directly as it
            is called implicitly by @method{Close} and
            @method{AddSegment}.
     */
    void ClosePart ();

    /*! Function: Close
        description: closes the current Part.

            The Part pointer
            from the last @method{AddPart} is no longer valid.<BR>
            It shouldn't be necessary to call this directly as it
            is called implicitly by @method{Close}, @method{AddSegment}
            and @method{AddPart}.
     */
    void Close ();

    /*! Function: InitDbs
        description: fills a new dbs command segment.

            This method creates a new Segment and Part. It fills
            the part with @param{cmd}. When @param{len} is -1 (the default),
            @param{cmd} is interpreted as a zero terminated string.

        return value: the command Part
     */

    /* PTS 1108446 sqlMode added */
    /* PTS 1112690 parsingAgain added */
    PIn_Part * InitDbs (const tsp1_sqlmode_Enum &sqlMode, const char *cmd, 
      int len = -1, const pasbool parsingAgain = false );

    /* PTS 1108446 sqlMode added. PTS 1112690 parsingAgain added */
    PIn_Part * InitDbs (const tsp1_sqlmode_Enum &sqlMode, const tsp81_UCS2Char *cmd, 
      int len = -1, const pasbool parsingAgain = false );

    /*! Function: InitDbsf
        description: fills a new dbs command segment using sp77vprintf.

            This method creates a new Segment and Part. It fills
            the part by calling sp77vprintf with @param{fmt} and
            the remaining arguments.

        return value: the command Part
     */

    /* PTS 1108446 sqlMode added. PTS 1112690 parsingAgain added */
    PIn_Part * InitDbsf (const tsp1_sqlmode_Enum &sqlMode, const pasbool parsingAgain, const char * fmt, ...);

    /*! Function: InitParse
        description: fills a new parse command segment.

            This method creates a new Segment and Part. It fills
            the part with @param{cmd}. When @param{len} is -1 (the default),
            @param{cmd} is interpreted as a zero terminated string.

        return value: the command Part
     */

    /* PTS 1108446 sqlMode added. PTS 1112690 parsingAgain added */
    PIn_Part * InitParse (const tsp1_sqlmode_Enum &sqlMode, const char *cmd, 
      int len = -1, const pasbool parsingAgain = false );

    /*! Function: InitMassParse
        description: fills a new parse command segment for a mass command.
     */
    /* PTS 1119960, added */
    PIn_Part * InitMassParse (const tsp1_sqlmode_Enum &sqlMode, const char *cmd, 
      int len = -1, const pasbool parsingAgain = false );

    /* PTS 1108446 sqlMode added. PTS 1112690 parsingAgain added */
    PIn_Part * InitParse (const tsp1_sqlmode_Enum &sqlMode, const tsp81_UCS2Char *cmd, 
      int len = -1, const pasbool parsingAgain = false );

    /*! Function: InitParsef
        description: fills a new parse command segment using sp77vprintf.

            This method creates a new Segment and Part. It fills
            the part by calling sp77vprintf with @param{fmt} and
            the remaining arguments.

        return value: the command Part
     */
    /* PTS 1108446 sqlMode added. PTS 1112690 parsingAgain added */
    PIn_Part * InitParsef (const tsp1_sqlmode_Enum &sqlMode, const pasbool parsingAgain, const char * fmt, ...);

    /*! Function: InitExecute
        description: creates a new execute Segment.

            @param{parseId} is copied into a new parseid part,
     */
    /* PTS 1108446 sqlMode added */
    void InitExecute (const tsp1_sqlmode_Enum &sqlMode, const tin02_ParseId * parseId);
    
    /*! Function: InitMassExecute
        description: creates a new execute Segment for a mass command.
     */
    /* PTS 1119960 */
    void InitMassExecute (const tsp1_sqlmode_Enum &sqlMode, const tin02_ParseId * parseId);

    /*! Function: AddDataPart
        description: creates a new data part.

        return value: the new data Part
     */
    PIn_Part * AddDataPart ();

protected:
    int VPrintf (const char * fmt, va_list ap);

    PIn_RequestPacket packet;
    PIn_RequestSegment segment;
    PIn_Part part;
};

/*! EndClass: PIn_RequestWriter */

#endif

