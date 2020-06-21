/*!================================================================

 module:        PIn_Part.h

 -------------------------------------------------------------------

 responsible:   TTK

 special area:  SQL

 description:   Part

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

#ifndef PIn_PART_H
#define PIn_PART_H

#ifndef PIN_H
#include "SAPDB/PacketInterface/PIn.h"
#endif

#include "hsp81.h"
#include "SAPDB/PacketInterface/PIn_SimpleTraceStream.h"

/*!
  Class: PIn_Part
 */
class PIn_Part
{
public:
    /*! Function: constructor

        description: creates a new Part object.

            This should never be called directly. Part objects
            are create either by Segment::newPart or by
            PartIterator::next.
     */
    PIn_Part (tsp1_part * partArg = NULL);

    /*! Function: destructor
     */
    inline ~PIn_Part() {}

    /*! Function: TraceOn

        description: dumps content of Part to @param{trace}.
     */
    void TraceOn (PIn_TraceStream * trace, int buflimit = -1);

    /*! Function: Init

        description: initializes the part header with defaults
     */
    void Init (tsp1_part_kind_Param partKind, int segmOffset, int segmSize);

    /*! Function: Length

        description: returns the number of data bytes.
     */
    inline int Length () const
    {
        return this->rawPart->sp1p_buf_len ();
    }


    /*! Function: BytesRemaining

        description: returns the number of bytes available for writing
     */
    inline int BytesRemaining () const
    {
        return this->rawPart->sp1p_buf_size () - this->rawPart->sp1p_buf_len ();
    }

    /*! Function: IsValid

        description: returns true if Part points to an actual tsp1_part
     */
    inline bool IsValid () const 
    {
        return (this->rawPart != NULL);
    }

    /*! Function: Invalidate

        description: makes the reference invalid.
     */
    void Invalidate()
    {
        this->rawPart = NULL;
    }

    /*! Function: GetPartKind

        description: gets the part kind.
     */
    inline tsp1_part_kind_Param GetPartKind()
    {
        if(this->rawPart == 0) {
            return sp1pk_nil;
        } else {
            return rawPart->sp1p_part_header().sp1p_part_kind;
        }
    }

    /*! Function: GetRawPart

        description: returns the native tsp1_part.

            This should be obsolete when the Part class is
            complete

        prototypes: 2
      */
    inline tsp1_part * GetRawPart()
    {
        return rawPart;
    }
    
    inline const tsp1_part * GetRawPart () const 
    {
        return rawPart;
    }

    /*! Function: GetRawHeader

        description: returns the native tsp1_part_header.

            This should be obsolete when the Part class is
            complete

        prototypes: 2
      */
    inline tsp1_part_header * GetRawHeader ()
    {
        return &rawPart->sp1p_part_header ();
    }
    
    inline const tsp1_part_header * GetRawHeader () const
    {
        return &rawPart->sp1p_part_header ();
    }

    /*! Function: GetReadData

        description: returns a constant pointer to raw storage.
     */
    const teo00_Byte * GetReadData (int pos = 0) const;

    /*! Function: GetParameterData

        description: return a pointer to the output parameter at @param{pos}.

            This is a pointer to the end of the current part. When the
            undefined byte is set, a NULL pointer is returned.

        arguments:
            pos     [in] the position relative to the start of the data buffer
                of the part. This is a 0-based offset to the start of the actual
                data, the defined byte is read from (pos - 1). Therefor, the pos
                of the paramter info can be used as this parameter.
     */
    const teo00_Byte * GetParameterData (int pos) const;

    /*! Function: GetWriteData

        description: returns a pointer to data which can be written.

     */
    teo00_Byte * GetWriteData ();

    /*! Function: ExtendLength

        description: increments the Part length by @param{increment}
     */
    void ExtendLength (int increment);

    /*! Function AddData

        description: appends data to the part.

            The part length is adjusted.

        preconditions:
            * data must fit into Part

     */
    void AddData (const void * buf, int len);

    /*! Function AddDataAscii2UCS2

        description: appends data to the part with conversion from Ascii to UCS2
            The part length is adjusted.

        arguments:
            buf     [in] the data to be written. 
            len     [in] length of the Ascii data.

        preconditions:
            * converted data (UCS2) must fit into Part

     */
    void AddDataAsciiToUCS2 (const void * buf, int len, int swapped);

    /*! Function AddDataUTF8ToUCS2

        description: appends data to the part with conversion from UTF8 to UCS2
                     The part length is adjusted.

        arguments:
            buf     [in] the data to be written. 
            len     [in] length of the UTF8 data.

        preconditions:
            * converted data (UCS2) must fit into Part

     */
    void AddDataUTF8ToUCS2 (const void * buf, int len);


    /*! Function AddDataUTF8ToUCS2

        description: appends data to the part with conversion from UTF8 to UCS2
                     The part length is adjusted.

        arguments:
            buf     [in] the data to be written. 
            len     [in] length of the UTF8 data.
            swapped [in] whether it is swapped or not.
        preconditions:
            * converted data (UCS2) must fit into Part

     */
    void AddDataUTF8ToUCS2 (const void * buf, int len, int swapped);


    /*! Function AddDataUCS2ToAscii

        description: appends data to the part with conversion from UCS2 to ASCII
            The part length is adjusted.

        arguments:
            buf     [in] the data to be written. 
            len     [in] length of the UCS2 data (in number of Chars).

        preconditions:
            * converted data (ASCII) must fit into Part

     */
    void AddDataUCS2ToAscii (const void * buf, int len, int swapped);

    /*! Function: AddArgument

        description: adds @param{buf} to the part as an argument.

            @param{buf} can be NULL: only the argument count is increased.

     */
    void AddArgument (const void * buf, int len);
    void AddArgument ();

    /*! Function: AddParameterArg

        description: adds a new host variable value.

            Values are padded or truncated.
            The argument count of the part is increased. The data length
            of the part is increased if necessary.

        arguments:
            buf     [in] the data to be written. A NULL pointer will set the
                undef byte
            pos     [in] the position relative to the start of the data buffer
                of the part. This is a 0-based offset to the start of the actual
                data, the defined byte is written at (pos - 1). Therefor, the pos
                of the paramter info can be used as this parameter.
            len     [in] the actual length of the data.
            sqllen  [in] the required length as specified by the parameter info,
                which contains the defined byte
            typechar [in] this value is used both as the defined byte and
                the padding character
     */
    void AddParameterArg (const void * buf, int pos, int len, int sqllen, char typechar);

    /*! Function: AddParameterArgUCS2

        description: adds a new host UNICODE variable value 
                     with conversion from ASCII to UCS2 (if needed )

            Values are padded or truncated.
            The argument count of the part is increased. The data length
            of the part is increased if necessary.

        arguments:
            buf     [in] the data to be written. A NULL pointer will set the
                undef byte
            pos     [in] the position relative to the start of the data buffer
                of the part. This is a 0-based offset to the start of the actual
                data, the defined byte is written at (pos - 1). Therefore, the pos
                of the paramter info can be used as this parameter.
            len     [in] the actual length of the data (in Chars). 
            isAsciiInput  [in] true == source is ASCII 
            isAsciiOutput [in] true == destination is ASCII 
            swapped [in] UCS2 swapped flag
            sqllen  [in] the required length as specified by the parameter info,
                which contains the defined byte
            typechar [in] this value is used both as the defined byte and
                the padding character
     */
    void AddParameterArgUCS2 (const void * buf, int pos, int len, int isAsciiInput, 
      int isAsciiOutput, int swapped, int sqllen, tsp81_UCS2Char typechar);
    
    /*! Function: AdjArgCntForMass

        description: adjusts the argument count for mass commands

        arguments:
            sqln    [in] number of columns of a table
     */
    /* PTS 1119960, added */
    void AdjArgCntForMass(short sqln);

    /*! Function: setAttributes
       
        description: sets the attributes first/last for a mass command data part

        arguments:
            attrib  [in] is first, next, last
     */
    /* PTS 1119960, added */
    void setAttributes(const tsp1_part_attr attrib);

private:
    tsp1_part * rawPart;
};

/*! EndClass: PIn_Part */

#endif
