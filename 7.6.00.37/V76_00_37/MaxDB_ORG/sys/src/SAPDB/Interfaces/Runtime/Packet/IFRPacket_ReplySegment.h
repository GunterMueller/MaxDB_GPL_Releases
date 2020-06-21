/*!
  @file           IFRPacket_ReplySegment.h
  @author         D025086
  @ingroup        IFR_Packet
  @brief          Handles the contents of an order packet
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
#ifndef IFRPACKET_REPLYSEGMENT_H
#define IFRPACKET_REPLYSEGMENT_H

#include <stdio.h>
#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/IFR_String.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "Interfaces/Runtime/Packet/IFRPacket_Segment.h"
#include "Interfaces/Runtime/Packet/IFRPacket_ReplyPacket.h"
#include "Interfaces/Runtime/Util/IFRUtil_Vector.h"

IFR_BEGIN_NAMESPACE

class IFR_ErrorHndl;
class IFR_ParseID;
class IFRPacket_FunctionCode;

/**
 * A reply segment. A reply segment contains the response of one
 * database command in various parts.
 */
class IFRPacket_ReplySegment :
    public PIn_ReplySegment
{

public:
    IFRPacket_ReplySegment(const PIn_ReplySegment &replySegment,
                           IFR_StringEncoding encoding=IFR_StringEncodingAscii)
    :PIn_ReplySegment (const_cast<tsp1_segment*>(replySegment.GetRawSegment())),
     m_encoding(encoding)
    {
        if(IsValid()) {
            m_Part = IFRPacket_Part(replySegment.GetFirstPart(), m_encoding);
            memset(m_partcache, 0, sizeof(m_partcache));
            m_partcache_filled = false;
        }
    }

    IFRPacket_ReplySegment(const IFRPacket_ReplyPacket &replyPacket);

    /**
     * Gets the data part, if any is present.
     * @param part the data part will be assigned to this argument, if any is found.
     * @return <code>IFR_OK</code> on success, <code>IFR_NO_DATA_FOUND</code>
     *  otherwise.
     */
    IFR_Retcode getPart(IFRPacket_DataPart& part) const;

    /**
     * Gets the variable data part, if any is present.
     * @param part the data part will be assigned to this argument, if any is found.
     * @return <code>IFR_OK</code> on success, <code>IFR_NO_DATA_FOUND</code>
     *  otherwise.
     */
    IFR_Retcode getPart(IFRPacket_VarDataPart& part) const;

    /**
     * Gets the longdata part, if any is present.
     * @param part the longdata part will be assigned to this argument, if any is found.
     * @return <code>IFR_OK</code> on success, <code>IFR_NO_DATA_FOUND</code>
     *  otherwise.
     */
    IFR_Retcode getPart(IFRPacket_LongDataPart& part) const;


    IFR_Retcode getPart(IFRPacket_AbapOStreamPart& part) const;
    IFR_Retcode getPart(IFRPacket_AbapIStreamPart& part) const;

    /**
     * Returns the error message text from the segment object.
     * @param errText the error text will be assigned to this string, if found.
     * @return <code>IFR_OK</code> if an error text was found.
     */
    IFR_Retcode getErrorText(IFR_String &errText, IFR_Bool& memory_ok) const;

    /**
     * Gets the SQL state. This is a string of 5 characters or
     * empty if not available. A terminator is appended.
     * @param dest copy destination for sql state (6 bytes needed).
     */
    void getSQLState(char *dest) const;

    /**
     * Returns the parseid from the segment object.
     *
     * @return @c IFR_OK if the segment object contains an error
     *   message or @c IFR_NO_DATA_FOUND if not.
     */
    IFR_Retcode getParseID(IFR_ParseID &ParseID) const;

    /**
     * Returns the select parseid from the segment object.
     *
     * <H3>returns:</H3>
     * IFR_OK if the segment object contains an errormessage or IF_NO_DATA_FOUND if not.
     */
    IFR_Retcode getSelectParseID(IFR_ParseID &ParseID) const;

    /**
     * Returns the resultname from the segment object if exsist.
     *
     * <H3>returns:</H3>
     * IFR_OK if the segment object contains an result tablename.
     */
    IFR_Retcode getResultTableName(IFR_String &ResultName, IFR_Bool& memory_ok) const;

    /**
     * Gets a possibly returned serial part.
     * @param part The serial part to fill.
     * @return @c IFR_OK on success, @c IFR_NO_DATA_FOUND on error.
     */
    IFR_Retcode getSerialPart(IFRPacket_DataPart& part);

    /**
     * Returns the number of rows modified by the last SQL.
     *
     * <H3>returns:</H3>
     * IFR_OK if segment object contains column information.
     */
    IFR_Retcode getResultCount (IFR_Int4& ResCount) const;

    /**
     * Returns the <code>errorpos</code> field of the result segment.
     * @return the errorpos value, or 0 if none is available.
     */
    IFR_Int4 getErrorPos() const;

    /**
     * Returns the functioncode of the executed command.
     *
     * <H3>returns:</H3>
     * IFRPacket_FunctionCode class.
     */
    IFRPacket_FunctionCode FunctionCode() const;

    IFR_UInt1 getTraceLevel() const;

    /**
     * Parses the column name part, and puts the column names into
     * the vector.
     * @param columnNames The destination vector of column names.
     * @param allocator The allocator to use.
     * @param memory_ok Flags indicating the memory allocation status.
     */
    IFR_Retcode parseColumnNames (IFRUtil_Vector<IFR_String>** columnNames,
                                  SAPDBMem_IRawAllocator& allocator,
                                  IFR_Bool& memory_ok);


    IFRPacket_Part getNextPart ();

    IFRPacket_Part getFirstPart ();

    int partKind ();

    IFR_Byte *getPartDataPos () const;

    IFR_Byte *getPartDataPos ();

    /**
     * Gets the SQL error, if any from this reply segment.
     * @param error the error instance that will be initialised with the error
     *        contained in this segment.
     * @param allocator The allocator to be used.
     * @param notrace Don't SQL Trace the error.
     * @return <code>true</code> if an error was found, <code>false</code> otherwise.
     */
    IFR_Bool getSQLError(IFR_ErrorHndl& error, SAPDBMem_IRawAllocator& allocator, IFR_Bool notrace=false);


    /**
     * Finds the specified part in the segment and
     * assigns it to the part reference supplied
     * as parameter.
     * @param partkind the part kind
     * @param part the part (destination)
     * @return <code>IFR_OK</code> on success, or <code>IFR_NO_DATA_FOUND</code>
     *         if the part was not found.
     */
    IFR_Retcode getPart(IFRPacket_PartKind::PartKind partkind,
                        IFRPacket_Part& part) const;

    /**
     * Sets the encoding.
     * @param The encoding of string content of this segment.
     */
    inline void setEncoding(IFR_StringEncoding encoding)
    {
        m_encoding=encoding;
        m_Part.setEncoding(m_encoding);
    }

    /**
     * Get a possible ABAP Tab Id.
     * @param abaptabid     The ABAP Tab Id that is initialized if there is one.
     * @param output        @c true if it is an output stream, @c false otherwise
     * @param argumentcount For output streams, the argument count of the output
     *                      stream part..
     * @return @c IFR_OK if a tab id was found, @c IFR_NO_DATA_FOUND otherwise.
     */
    IFR_Retcode getABAPTabId(IFR_Int4& abaptabid, IFR_Bool& output, IFR_Int4& argumentcount);

    IFRPacket_PartKind::PartKind hasABAPStreams();
    
    /**
     * Get SQL mode from segment.
     */
    IFR_SQLMode getSQLMode();

    IFR_Bool isResultSetClosed() const 
    {
        return GetRawSegment()->sp1s_segm_header().sp1r_intern_warning().includes(sp1iw_warn0_resultset_closed);
    }
    
private:
    IFRPacket_Part findPart(IFR_Int4 partkind);

    IFR_Bool            m_partcache_filled;
    tsp1_part *         m_partcache[sp1pk_last_part_kind];
    IFRPacket_Part      m_Part;            //!< The current Part.
    IFR_StringEncoding  m_encoding;        //!< The encoding.
};

IFR_END_NAMESPACE

#endif //IFRPACKET_REPLYSEGMENT_H
