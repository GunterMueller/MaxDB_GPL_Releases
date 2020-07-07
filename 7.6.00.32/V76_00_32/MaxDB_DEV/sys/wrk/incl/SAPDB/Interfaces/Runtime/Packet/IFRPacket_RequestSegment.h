/*!
  @file           IFRPacket_RequestSegment.h
  @author         D025086
  @ingroup        IFR_Packet
  @brief          Handles the contents of an order packet.
  @see            http://pwww.bea.sap.corp:1111/Kern/v72/xorder7.doc

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
#ifndef IFRPACKET_REQUESTSEGMENT_H
#define IFRPACKET_REQUESTSEGMENT_H

#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/IFR_String.h"
#include "Interfaces/Runtime/Packet/IFRPacket_Part.h"
#include "Interfaces/Runtime/Packet/IFRPacket_RequestPacket.h"

IFR_BEGIN_NAMESPACE

/**
 * A generic request segment. A request segment contains exactly one
 * database command and its arguments.
 */
class IFRPacket_RequestSegment 
{     
public:
    /**
     * Explicit constructor for bare initialisers.
     */
    explicit IFRPacket_RequestSegment()
    :m_requestpacket(0)
    {}

    /**
     * Creates a new request segment. 
     * @param requestpacket the packet that will contain the segment.
     * @param messagetype the messagetype of the segment.
     * @param resetpacket whether the packet should be resetted and the 
     *        segment should be added as the first segment of the packet. Default
     *        is to reset the packet.
     */
    IFRPacket_RequestSegment(IFRPacket_RequestPacket& requestpacket,
                             int messagetype,
                             IFR_Bool resetpacket=true);
    
                             
    
    /**
     * Creates a new <i>first</i> request segment (the packet is resetted). 
     * @param requestPacket the packet that will contain the segment
     * @param messagetype the message type of the segment
     * @param sqlmode the SQL mode of the packet
     * @param parsingAgain whether this segment is the result of a -8 error before.
     */
    IFRPacket_RequestSegment(IFRPacket_RequestPacket &requestPacket, 
                             int messagetype,
                             int sqlmode,
                             const IFR_Bool parsingAgain = false,
                             const IFR_Bool dontReset = false);
    
    
    /**
     * Copy constructor.
     * @param copy The segment to copy.
     */
    IFRPacket_RequestSegment(const IFRPacket_RequestSegment& copy);
    
    /**
     * Checks the status of the segment. A segment is valid after the 
     * construction and until it is closed or an new segment is added.
     * @return <code>TRUE</code> if the segment is valid. <code>FALSE</code>
     * if not.
     */
    IFR_Bool isValid() const;

    /**
     * Adds a parse id part.
     */
    IFR_Retcode addPart(IFRPacket_ParseIDPart& part);

    /**
     * Adds a command part.
     */
    IFR_Retcode addPart(IFRPacket_ApplParamPart& part);

    /**
     * Adds a command part.
     */
    IFR_Retcode addPart(IFRPacket_CommandPart& part);
    
    /**
     * Adds a cursorname part.
     */
    IFR_Retcode addPart(IFRPacket_TableNamePart& part);
    
    /**
     * Adds a data part.
     */
    IFR_Retcode addPart(IFRPacket_DataPart& part);

    /**
     * Adds a longdata part.
     */
    IFR_Retcode addPart(IFRPacket_LongDataPart& part);

    /**
     * Adds a long demand part.
     */
    IFR_Retcode addPart(IFRPacket_LongDemandPart& part);


    /**
     * Adds a long demand part.
     */
    IFR_Retcode addPart(IFRPacket_ErrorTextPart& part);

    /**
     * Adds a resultcount part.
     * @param part the result count part (output)
     * @return <code>IFR_OK</code> on success, 
     *   <code>IFR_NOT_OK</code> if the packet size
     *   is exhausted.
     */
    IFR_Retcode addPart(IFRPacket_ResultCountPart& part);
    
    /**
     * Adds a command info part.
     */
    IFR_Retcode addPart(IFRPacket_CommandInfoPart& part);
   
    /**
     * Adds a feature part.
     */
    IFR_Retcode addPart(IFRPacket_FeaturePart& part);

    /**
     * Closes the part currently open.
     */
    void closePart();
    
    /**
     * Closes this segment.
     */
    void close();
    
    /**
     * Adds a result count (fetch size) part to the packet. The fetchsize
     * is limited to an short value.
     * @param fetchsize is the maximum number of rows to be fetches. 
     *   if the fetchsize is the default <code>-1</code> value an undef
     *   resultcountpart is added to the segment.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code>
     *   if the packet size is exhausted.
     */
    IFR_Retcode addFetchSize (IFR_Int2 fetchsize = -1);

    /**
     * Adds a result count part to the packet.
     * @param count the result count. 
     *   if the count is the default <code>-1</code> value the count part
     *   contains the UNDEFSIGNAL
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code>
     *   if the packet size is exhausted.
     */

    IFR_Retcode addResultCount (IFR_Int4 count = -1);
    
    /**
     * Adds a parse id in its own part. 
     * @param parseid the parse id to add.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code>
     *   if the packet size is exhausted.
     */
    IFR_Retcode addParseID(IFR_ParseID& parseid);

    /**
     * Assignment operator.
     * @param segment The segment 
     * @return <code>*this</code>.
     */
    IFRPacket_RequestSegment& operator = (const IFRPacket_RequestSegment& segment);

    /**
     * Sets the request packet. This may be only necessary if it is necessary to
     * keep a segment and a request packet instance in sync (after copying).
     * @param packet The request packet.
     */
    inline void setRequestPacket(IFRPacket_RequestPacket& packet)
    {
        m_requestpacket=&packet;
        return;
    }

    /**
     * Sets the 'autocommit' header field.
     */
    void setCommitImmediately();

    /**
     * Sets the producer flag in the first request segment.
     */
    void setProducer (const tsp1_producer_Param & oProducer);

    void setParseWithPrepare();

    /**
     * Method from request packet, for convenience.
     */
    inline IFR_Int4 remainingBytes() const 
    {
        return m_requestpacket ? m_requestpacket->remainingBytes() : 0;
    }
    
    /**
     * Sets the SQL state of this segment.
     * @param sqlstate The SQL state to set.
     */
    void setSQLState(const char *sqlstate);
    
    /**
     * Sets the error code of this segment,
     * @param errorCode The error code to set.
     */
    void setErrorCode(IFR_Int4 errorCode);

    /**
     * Adds a new part to the segment.
     * @param partkind the kind of part to add.
     * @param part the created part will be assigned to this argument.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code>
     *   otherwise.
     */
    IFR_Retcode addPart(IFRPacket_PartKind::PartKind partkind, IFRPacket_Part& part);

    /**
     * Adds a new part data to the segment.
     * @param partkind the kind of part to add, either @c IFRPacket_PartKind::Data_C
     *   or @c IFRPacket_PartKind::VarData_C.
     * @param part the created part will be assigned to this argument.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code>
     *   otherwise.
     */
    IFR_Retcode addPart(IFRPacket_PartKind::PartKind partkind, IFRPacket_DataPart& part);

    void setScrollableOption(IFR_Bool scrollableoption);

    void setNoResultSetCloseNeeded();
    
    inline IFRPacket_RequestPacket& getRequestPacket() 
    {
        return *m_requestpacket;
    }
    
    void setMassCommand();

    /**
     * Removes the last recently added part.
     */
    IFR_Retcode rollbackLastPart();

    inline IFR_Int4 Size() { return m_segment.Size(); }
    inline IFR_Int4 Length() { return m_segment.Length(); }
private:

    /**
     * Adds a result count part containing UNDEFSIGNAL.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code>
     *   if the packet size is exhausted.
     */
    IFR_Retcode addUndefResultCount();

    /**
     * Get the character encoding of the segment. The encoding of the segment
     * has is the same as the encoding of the packet. 
     * @return the encoding of the segment.
     */
    const IFR_StringEncoding getEncoding() const;
    
    
private:
    IFRPacket_RequestPacket *m_requestpacket;  //!< the request packet this segment belongs to
    PIn_RequestSegment       m_segment;        //!< the segment
    IFRPacket_Part           m_lastpart;       //!< the last added part 
};

IFR_END_NAMESPACE

#endif //IFRPACKET_REQUESTSEGMENT_H

