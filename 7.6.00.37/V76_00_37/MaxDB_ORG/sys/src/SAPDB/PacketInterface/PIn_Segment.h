/*!================================================================
 module:        PIn_Segment.h

 -------------------------------------------------------------------

 responsible:   DanielD

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

#ifndef HIN202_H
#define HIN202_H

#include "vsp001.h"
#include "SAPDB/PacketInterface/PIn_Part.h"

/*----------------------------------------*/

/*!
  Class: PIn_Segment
 */
class PIn_Segment
{
protected:
    /*! Function: constructor

        description: creates a new Segment object.

            This can be called only by subclasses.

     */
    PIn_Segment (tsp1_segment * segmentArg = NULL);

public:
    /*! Function: destructor

        description: releases any ressources
     */
    ~PIn_Segment ();

    /*! Function: TraceOn

        description: dumps content of Segment to @param{trace}.
     */
    void TraceOn (PIn_TraceStream * trace, int buflimit = -1);

    /*! Function: Size

        return value: the maximal size of the Segment
     */

  void OptionsDump (PIn_TraceStream * trace, tsp1_segment_header * header);

    int Size () const;

    /*! Function: Length

        return value: the actual length of the Segment
     */
    int Length () const;

    /*! Function: IsValid

        return value: true if Segment points to an actual tsp1_segment
     */
    bool IsValid () const;

    /*! Function: Invalidate

        description: makes the reference invalid.
     */
    void Invalidate ();

    /*! Function: GetRawSegment

        return value: the native tsp1_segment.

        prototypes: 2
     */
    tsp1_segment * GetRawSegment ();
    const tsp1_segment * GetRawSegment () const;

    /*! Function: GetRawHeader

        return value: the native tsp1_segment_header

        prototypes: 2
     */
    tsp1_segment_header * GetRawHeader ();
    const tsp1_segment_header * GetRawHeader () const;


protected:
    tsp1_segment * rawSegment;
};

/*!
  EndClass: PIn_Segment
 */
/*----------------------------------------*/

/*!
  Class: PIn_RequestSegment
 */
class PIn_RequestSegment : public PIn_Segment
{
public:

    /*! Function: constructor

        description: creates a new RequestSegment

            This should never be called directly. ReplySegments
            are created by RequestPacket::AddSegment.
     */
    PIn_RequestSegment (tsp1_segment * segmentArg = NULL);

    /*! Function: Init

        description: initializes a RequestSegment.

            This should never be called directly. It is
            called implicitly by RequestPacket::AddSegment.
    */

    /* PTS 1108446 sqlMode added */
    /* PTS 1112690 parsingAgain added */
    void Init (tsp1_cmd_mess_type_Param messType, const tsp1_sqlmode_Enum &sqlMode,
      const pasbool parsingAgain,
      int offset, int index);

    /*! Function: MassInit

        description: initializes a RequestSegment for a mass command.
     */
    /* PTS 1119960, added */
    void MassInit (tsp1_cmd_mess_type_Param messType, const tsp1_sqlmode_Enum &sqlMode,
      const pasbool parsingAgain,
      int offset, int index);

    /*! Function: ExtendLength

        description: grows the segment by @param{increment} bytes.

            The packet grows by the same amount.
     */
    void ExtendLength (int increment);

    /*! Function: SetProducer

        description: sets the producer value

     */
    void SetProducer (const tsp1_producer_Param & oProducer);


    /*! Function: SetWithInfo

        description: sets the "With Info" flag

     */
    void SetWithInfo();


    inline void  AddCommandOption(const tsp1_command_option_Enum CommandOption )
    {
        tsp1_segment_header * header = &this->rawSegment->sp1s_segm_header ();

        if (false == header->sp1c_command_options().includes(CommandOption) )
        {
            header->sp1c_command_options().addElement(CommandOption);
        }
    }



    /*! Function: AddPart

        description: creates a new @param{partKind} part.
     */
    PIn_Part AddPart (tsp1_part_kind_Param partKind);

    /*! Function: AddPart

        description: creates a new @param{partKind} part. The part is 
        assigned to the 2nd argument.
    */
    void AddPart (tsp1_part_kind_Param partKind, PIn_Part& part);


    /*! Function: ClosePart

        description: closes @param{part}
     */
    void ClosePart (PIn_Part & part);

};

/*!
  EndClass: PIn_RequestSegment
 */

/*----------------------------------------*/

/*!
  Class: PIn_ReplySegment
 */
class PIn_ReplySegment : public PIn_Segment
{
public:
    /*! Function: constructor

        description: creates a new ReplySegment.

            This should never be called directly. ReplySegments
            are created by RequestPacket::nextSegment.

        arguments:
            segmentArg [in, optional] the segment pointer
     */
    PIn_ReplySegment (tsp1_segment * segmentArg = NULL);

    /*! Function: ErrorCode

        return value: the sql error code
     */
    int ErrorCode () const;

    /*! Function: FunctionCode

        return value: information about the executed command
     */
    int FunctionCode () const;

    /*! Function: GetFirstPart

        return value: the first Part of the Segment.
     */
    const PIn_Part GetFirstPart () const;

    /*! Function: GetNextPart

        return value: the next Part.

            When @param{part} is the last part, an invalid Part is returned
            When @param{part} is an invalid Part, then the first Part is used.
     */
    const PIn_Part GetNextPart (const PIn_Part & part) const;

    /*! Function: FindPart

        return value: the first Part of the given kind.

            When no Part of this kind is found, an invalid Part is returned.
     */
    const PIn_Part FindPart (tsp1_part_kind_Param partKind) const;

    /*! Function: FindParts

        description: allows to retrieve several parts in one operation.

        arguments:
            count [in]      the number of requested parts
            partKinds [in]  an array of tsp1_part_kind_Param
            parts [out]     each entry will contain either a Part
                of the kind in the corresponding slot of @param{partKinds}
                or an invalid Part.

        return value: the number of requested parts actually found
     */
    int FindParts (int count, const  tsp1_part_kind_Param partKinds [],
        PIn_Part parts []) const;

};

/*!
  EndClass: PIn_ReplySegment
 */

/*----------------------------------------*/

#endif

