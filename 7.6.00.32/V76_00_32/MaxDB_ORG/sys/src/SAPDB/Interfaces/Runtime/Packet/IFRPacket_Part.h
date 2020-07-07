/*!
  @file           IFRPacket_Part.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Packet
  @brief          Various methods for specific data in parts.
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
#ifndef IFRPACKET_PART_H
#define IFRPACKET_PART_H

#include "SAPDB/SAPDBCommon/SAPDB_ReuseLib.h"
#include "Interfaces/Runtime/IFR_String.h"
#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/IFR_Datatypes.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/Util/IFRUtil_Vector.h"
#include "Interfaces/Runtime/IFR_ShortInfo.h"

IFR_BEGIN_NAMESPACE

class IFRConversion_Converter;
class IFRConversion_ConverterList;
class IFR_ShortInfo;
class IFR_ConnectionItem;
struct IFRPacket_LongDescriptor;

/**
 * Class to encapsulate part attributes.
 */
class IFRPacket_PartAttributes
{
public:
    enum {
        LastPacket_C          = 1, 
        NextPacket_C          = 2,
        FirstPacket_C         = 4
    };
    /* use not sp1pa_last_packet, sp1pa_next_packet, sp1pa_first_packet here,
       as they are 1, 2, 3, but we need a set of these. */
};

/**
 * Class to encapsulate part kinds.
 */
class IFRPacket_PartKind
{
public:
    enum PartKind {
        Nil_C                       =sp1pk_nil,
        ApplParameterDescription_C  =sp1pk_appl_parameter_description,
        Columnnames_C               =sp1pk_columnnames,
        Command_C                   =sp1pk_command,              
        ConvTablesReturned_C        =sp1pk_conv_tables_returned,
        Data_C                      =sp1pk_data,      
        Errortext_C                 =sp1pk_errortext,
        Getinfo_C                   =sp1pk_getinfo,                
        Modulname_C                 =sp1pk_modulname,                
        Page_C                      =sp1pk_page,             
        Parsid_C                    =sp1pk_parsid,                
        ParsidOfSelect_C            =sp1pk_parsid_of_select,
        Resultcount_C               =sp1pk_resultcount,        
        Resulttablename_C           =sp1pk_resulttablename,
        Shortinfo_C                 =sp1pk_shortinfo,       
        UserInfoReturned_C          =sp1pk_user_info_returned,
        Surrogate_C                 =sp1pk_surrogate,    
        Bdinfo_C                    =sp1pk_bdinfo,           
        Longdata_C                  =sp1pk_longdata,             
        Tablename_C                 =sp1pk_tablename,            
        SessionInfoReturned_C       =sp1pk_session_info_returned,
        OutputColsNoParameter_C     =sp1pk_output_cols_no_parameter,   
        Key_C                       =sp1pk_key,  
        Serial_C                    =sp1pk_serial,
        RelativePos_C               =sp1pk_relative_pos,
        AbapIStream_C               =sp1pk_abap_istream,            
        AbapOStream_C               =sp1pk_abap_ostream,            
        AbapInfo_C                  =sp1pk_abap_info,
        LongDemand_C                =sp1pk_long_demand,
        MessageList_C               =sp1pk_message_list,
#if IFR_HAS_VARDATA
        VarDataShortInfo_C          =sp1pk_vardata_shortinfo,
        VarData_C                   =sp1pk_vardata,
#else
        VarDataShortInfo_C          =sp1pk_nil,
        VarData_C                   =sp1pk_nil,
#endif

#if IFR_HAS_FEATUREPART
        Feature_C                   =sp1pk_feature,
#else
        Feature_C                   =sp1pk_nil,
#endif
#if IFR_HAS_CLIENTIDPART        
        ClientId_C                  = sp1pk_clientid
#else
        ClientId_C                  = sp1pk_nil
#endif
    };
};


/**
 * A part is a data item of a segment. 
 */
class IFRPacket_Part : public PIn_Part 
{
public:
    /**
     * Creates a new uninitialised part.
     * @param encoding the encoding to use for the part, default is raw ascii.
     */
    inline IFRPacket_Part (IFR_StringEncoding encoding=IFR_StringEncodingAscii) 
    :PIn_Part(),
     m_encoding(encoding)
    {
    }

    /**
     * Creates a new part as overlay on a <code>PIn_Part</code>.
     * @param part The <code>PIn_Part</code> where this part should be the overlay.
     * @param encoding the encoding to use for the part, default is raw ascii.
     */ 
    inline IFRPacket_Part (const PIn_Part &part, IFR_StringEncoding encoding)
    :PIn_Part (const_cast<tsp1_part*>(part.GetRawPart())),
     m_encoding(encoding)
    {
    }
    
    /**
     * Creates new part as overlay on the raw <code>tsp1_part</code>
     * structure.
     * @param rawpart The raw part.
     * @param encoding the encoding to use for the part, default is raw ascii.
     */
    inline IFRPacket_Part(tsp1_part* rawpart, IFR_StringEncoding encoding)
    :PIn_Part(rawpart),
     m_encoding(encoding)
    {
    }

    /**
     * Get the number of arguments this part has.
     * @return the number of part arguments, 0 if the part is not valid.
     */
    inline IFR_Int2 getPartArguments() const
    {
        const tsp1_part * part=GetRawPart();
        if(part) {
            return part->sp1p_part_header().sp1p_arg_count;
        } else {
            return 0;
        }
    }
    
    /**
     * Returns the buffer length of this part.
     * @return the buffer length, 0 if the part is not valid.
     */
    inline IFR_Int4 getBufferLength() const
    {
        const tsp1_part * part=GetRawPart();
        if(part) {
            return part->sp1p_part_header().sp1p_buf_len;
        } else {
            return 0;
        }
    }

    /**
     * Sets the part's buffer length.
     * @param newLength The new buffer length.
     */
    inline void setBufferLength(IFR_Int4 newLength)
    {
        tsp1_part * part=GetRawPart();
        if(part) {
            part->sp1p_part_header().sp1p_buf_len = newLength;
        }
    }
    
    /**
     * Get the character encoding of the part.
     */
    inline IFR_StringEncoding getEncoding() const
    {
        return m_encoding;
    }
   
    /**
     * Sets the encoding.
     * @param encoding the encoding
     */
    inline void setEncoding(IFR_StringEncoding encoding)
    {
        m_encoding=encoding;
    }

    inline IFR_Bool isValid() const
    {
        return IsValid();
    }
    
    /**
     * Gets the part's contents as text. 
     * @param text the will be returned in this string.
     * @param memory_ok Flag to indicate out of memory condition.
     * @param append Whether to append or assign the string, default is assign.
     * @return <code>IFR_OK</code> on success, 
     * <code>IFR_NO_DATA_FOUND</code> if the part is 
     * invalid or an out of memory error occurs.
     */
    IFR_Retcode getText(IFR_String& text,IFR_Bool& memory_ok, IFR_Bool append=false);
    
    /**
     * Sets the 'first packet' part attribute.
     */
    inline void setFirstPacket()
    {
        setPartAttribute(IFRPacket_PartAttributes::FirstPacket_C);    
    }

    /**
     * Sets the 'last packet' part attribute.
     */
    inline void setLastPacket()
    {
        setPartAttribute(IFRPacket_PartAttributes::LastPacket_C);    
    }

    /**
     * Sets the 'next packet' part attribute.
     */
    inline void setNextPacket()
    {
        setPartAttribute(IFRPacket_PartAttributes::NextPacket_C);    }

    /**
     * Gets the remaining bytes.
     * @return The number of bytes that remain in the part.
     */
    inline IFR_size_t getRemainingBytes() const
    {
        return (IFR_size_t)BytesRemaining();
    }


    /**
     * Gets the part kind.
     * @return The part kind of this part, or @c IFRPacket_PartKind::Nil_C if the part is not
     *         valid.
     */
    inline IFRPacket_PartKind::PartKind getPartKind() 
    {
        if(isValid()) {
            return (IFRPacket_PartKind::PartKind)((int )(this->GetRawHeader()->sp1p_part_kind));
        } else {
            return  IFRPacket_PartKind::Nil_C;
        }
    }

    /**
     * Determines if last part was set in part attributes
     * @return <code>true</code> if last part attribute has been set for this part
     *         <code>false</code> otherwise
     */
    inline IFR_Bool isLastPacket() const
    {
        const tsp1_part * part = GetRawPart();
        if(part == 0) {
            return false;
        } else {
            char *p =(char *) ((const char *)&(part->sp1p_part_header()));
            ++p;
            return (*p & (char)IFRPacket_PartAttributes::LastPacket_C) != 0;
        }
    }

    /**
     * Determines if first part was set in part attributes
     * @return <code>true</code> if first part attribute has been set for this part
     *         <code>false</code> otherwise
     */
    inline IFR_Bool isFirstPacket() const
    {
        const tsp1_part * part = GetRawPart();
        if(part == 0) {
            return false;
        } else {
            char *p =(char *) ((const char *)&(part->sp1p_part_header()));
            ++p;
            return (*p & (char)IFRPacket_PartAttributes::FirstPacket_C) != 0;
        }
    }

    /**
     * Sets the number of arguments in the data part. This is
     * the number of rows in a mass command, or the number of
     * LONG descriptors in a LONG data part.
     * @param argcount The number of arguments.
     * @return <code>IFR_OK</code> if correct, <code>IFR_NOT_OK</code> if the argument was 
     * less or equal 0.
     */
    inline IFR_Retcode setArgCount(IFR_Int2 argcount)
    {
        if(argcount <= 0) {
            return IFR_NOT_OK;
        }
        GetRawPart()->sp1p_part_header().sp1p_arg_count = argcount;
        return IFR_OK;
    }

    static const char *getPartKindName(IFR_Int4 partkind);

protected:
    /**
     * Gets an 8 bit integer from the specified offset.
     * @param position position within the part.
     */
    IFR_Int1 getInt1(IFR_Int4 position) const;

    /**
     * Gets a 16 bit integer from the specified offset.
     * @param position position within the part.
     */
    IFR_Int2 getInt2(IFR_Int4 position) const;

    /**
     * Gets a 32 bit integer from the specified offset.
     * @param position position within the part.
     */
    IFR_Int4 getInt4(IFR_Int4 position) const;
    
   
    IFR_StringEncoding m_encoding; //!< encoding for text retrieved/set into this part
private:
    void setPartAttribute(int attribute);
    
    
};


//----------------------------------------------------------------------
// various classes for different kinds of parts
//----------------------------------------------------------------------


/**
 * A result table name part, which has no special properties.
 */
typedef IFRPacket_Part IFRPacket_ResultTableNamePart; 


/**
 * An error text part. You can add an error text.
 */
class IFRPacket_ErrorTextPart
    : public IFRPacket_Part
{
public:
    /**
     * Constructor.
     */
    inline IFRPacket_ErrorTextPart() 
    {}

    /**
     * Constructor. Overlay on <code>PIn_Part</code>
     */
    inline IFRPacket_ErrorTextPart(const PIn_Part& cpart, IFR_StringEncoding encoding)
    :IFRPacket_Part(cpart, encoding)
    {}
    
    /**
     * Set the error text.
     * @param error The error that contains the text.
     */
    IFR_Retcode setErrorText(IFR_ErrorHndl &error);
    
};


/**
 * A parse id part. Contains a parse id.
 */
class IFRPacket_ParseIDPart
    : public IFRPacket_Part
{
public:
    /**
     * Constructor.
     */
    inline IFRPacket_ParseIDPart() 
    {}

    /**
     * Constructor. Overlay on <code>PIn_Part</code>
     */
    inline IFRPacket_ParseIDPart(const PIn_Part& cpart, IFR_StringEncoding encoding)
    :IFRPacket_Part(cpart, encoding)
    {}
    
    /**
     * Gets the parse id.
     * @param parseid this is set to the parse is found in the part.
     * @return <code>IFR_OK</code> on success, <code>IFR_NO_DATA_FOUND</code> 
     *         otherwise.
     */
    IFR_Retcode getParseID(IFR_ParseID& parseid) const;
    
    /**
     * Adds a parse id.
     * @param parseid the parse id to be set.
     * @return <code>IFR_OK</code> on success.
     */
    IFR_Retcode addParseID(IFR_ParseID& parseid);

    /**
     * Sets the 1st parse id.
     * @param parseid the parse id to be set.
     * @return <code>IFR_OK</code> on success.
     */
    IFR_Retcode setParseID(IFR_ParseID& parseid);
};

/**
 * A short info part. Contains a number of short field infos.
 */
class IFRPacket_ShortInfoPart 
    : public IFRPacket_Part
{
public:
    inline IFRPacket_ShortInfoPart() 
    {}

    inline IFRPacket_ShortInfoPart(const IFRPacket_Part& part)
    :IFRPacket_Part(part, part.getEncoding())
    {}
    
    /**
     * Gets the short infos. 
     * @param shortinfo pointer to array of shortinfos.
     * @return <code>IFR_OK</code> on success, <code>IFR_NO_DATA_FOUND</code> otherwise.
     */
    IFR_Retcode getShortInfos(IFR_ShortInfo* shortinfo);

    /**
     * Parses the short fields, hereby creating converter instances for
     * all parameter/columns.
     * @param convList Vector of converter instances.
     * @param clink Link to connection.
     */
    IFR_Retcode parseShortFields(IFRConversion_ConverterList& convList,  IFR_ConnectionItem& clink, IFR_Bool ignoreOutput=false);
    
};

/**
 * Part for variable input short infos.
 */
class IFRPacket_VarDataShortInfoPart 
    : public IFRPacket_ShortInfoPart
{
public:
    inline IFRPacket_VarDataShortInfoPart() 
    {}

    inline IFRPacket_VarDataShortInfoPart(const IFRPacket_Part& part)
    :IFRPacket_ShortInfoPart(part)
    {}

    IFR_Retcode parseShortFields(IFRConversion_ConverterList& convList,  IFR_ConnectionItem& clink);
};



/**
 * A command part makes it possible to set and add text (the command).
 */
class IFRPacket_CommandPart
    : public IFRPacket_Part
{
public:
    inline IFRPacket_CommandPart() 
    {}

    inline IFRPacket_CommandPart(const IFRPacket_Part& part)
    :IFRPacket_Part(part, part.getEncoding())
    {}
    
    /**
     * Set the command text.
     * @param text the command text.
     * @param clink Connection link for error reporting.
     * @return <code>IFR_OK</code> on success.
     */
    IFR_Retcode setText(const IFR_String& text, IFR_ErrorHndl &error);
    IFR_Retcode addText(const IFR_String& text, IFR_ErrorHndl &error);
};

/**
 * A data part. This contains input parameters and also results retrieved 
 * from the database.
 */
class IFRPacket_DataPart
    : public IFRPacket_Part
{
public:
    inline IFRPacket_DataPart() 
    :m_currentrecord(0),
     m_recordsize(0),
     m_extent(0),
     m_massextent(0),
     m_variableinput(false)
    {}

    inline IFRPacket_DataPart(const PIn_Part& cpart, IFR_StringEncoding encoding)
    :IFRPacket_Part(cpart, encoding),
     m_currentrecord(0),
     m_recordsize(0),
     m_extent(0),
     m_massextent(0),
     m_variableinput(false)
    {
    }

    /**
     * Creates a data part on a <code>tsp1_part</code> structure that is
     * already positioned at a goven position.
     * @param rawpart The raw part data structure.
     * @param currentrecord The current record in the part (start with 0).
     * @param recordsize The record size of one row.
     */
    inline IFRPacket_DataPart(tsp1_part *rawpart, IFR_StringEncoding encoding, IFR_Int2 currentrecord, IFR_Int2 recordsize)
    :IFRPacket_Part(rawpart, encoding),
     m_currentrecord(currentrecord),
     m_recordsize(recordsize),
     m_extent(0),
     m_massextent(0),
     m_variableinput(false)
    {}
    

    /**
     * Add binary data 
     * @param buffer the data
     * @param length the length of the data
     * @param shortinfo the shorfield info of the parameter
     * @return <code>IFR_OK</code> if the parameter is added, 
     * <code>IFR_DATA_TRUNC</code> if the argument was added but truncated
     * or <code>IFR_NOT_OK</code> if the given buffer argument can't convert
     * to the datapart.
     */
    IFR_Retcode addBinaryParameter(const void *buffer,
                                   IFR_Int4 length,
                                   IFR_ShortInfo &shortinfo);

    /**
     * Add binary data 
     * @param buffer the data
     * @param length the length of the data
     * @return <code>IFR_OK</code> if the parameter is added, 
     * <code>IFR_DATA_TRUNC</code> if the argument was added but truncated
     * or <code>IFR_NOT_OK</code> if the given buffer argument can't convert
     * to the datapart.
     */
    IFR_Retcode addBinaryParameter(const void *buffer,
                                   IFR_Int4 length);
                         
    /**
     * Adds a parameter with the encoding of the shortfield info and the
     * defined byte. The argument is padded if neccessary. Also, NULL values 
     * are honored. The parameter is added at the current record position.
     * @param buffer the data
     * @param length the length of the data
     * @param srcEncoding The encoding of the data
     * @param targetEncoding The target encoding.
     * @param shortinfo the shorfield info of the parameter
     * @param bin2hex Whether a hexstring -> binary conversion is done.
     * @return <code>IFR_OK</code> if the parameter is added, 
     * <code>IFR_DATA_TRUNC</code> if the argument was added but truncated
     * or <code>IFR_NOT_OK</code> if the given buffer argument can't convert
     * to the datapart.
     */
    IFR_Retcode addParameter(const void *buffer,
                             IFR_Int4 length,
                             IFR_StringEncoding srcEncoding,
                             IFR_StringEncoding targetEncoding,
                             IFR_ShortInfo &shortinfo,
                             IFR_Bool bin2hex);

    /**
     * Appends to a parameter. As a precondition, data must have been added before,
     * and it must be non-NULL-
     * @param buffer the data
     * @param length the length of the data
     * @param srcEncoding the encoding of the data
     * @param shortinfo the short field info of the parameter
     * @param offset  The offset (in bytes) within the data.
     * @param bin2hex Whether a hexstring -> binary conversion is done.
     * @return <code>IFR_OK</code> if the parameter is added, 
     * <code>IFR_DATA_TRUNC</code> if the argument was added but truncated
     * or <code>IFR_NOT_OK</code> if the given buffer argument can't convert
     * to the datapart.
     */
    IFR_Retcode appendToParameter(const void *buffer,
                                  IFR_Int4 length,
                                  IFR_StringEncoding srcEncoding,
                                  IFR_StringEncoding targetEncoding,
                                  IFR_ShortInfo &shortinfo,
                                  IFR_Length&    offset,
                                  IFR_Bool bin2hex
        );

    IFR_Retcode appendBinaryToParameter(const void *buffer,
                                        IFR_Int4 length,
                                        IFR_ShortInfo &shortinfo,
                                        IFR_Length&    offset);
    
    
    /**
     * Adds a @c NULL value.
     * @param shortinfo Parameter info.
     */
    IFR_Retcode addNullInput(IFR_ShortInfo& shortinfo);
    
    /**
     * Adds a @c DEFAULT value.
     * @param shortinfo Parameter info.
     */
    IFR_Retcode addDefaultInput(IFR_ShortInfo& shortinfo);

    IFR_Retcode finishData(IFR_Length length, IFR_ShortInfo& shortinfo);
private:
    /**
     * Adds plain data at the specified position. The data must
     * include the defined byte.
     * @param buffer the data location.
     * @param position the data position + 1
     * @param iolength the length to copy.
     */
    void addData(void *buffer,
                 IFR_Int4    position,
                 IFR_Int4    iolength);

    
    /**
     * Adds data at the specified position, setting the defined
     * byte of the data to the provided value, and leaving the 
     * data itself untouched.
     * @param definedbyte the definedbyte to set
     * @param position the data position (defined byte is at position - 1)
     * @param iolength the input/output length this parameter has.
     */
    void addData(IFR_Int1 definedbyte,
                 IFR_Int4 position,
                 IFR_Int4 iolength);

public:
    /**
     * Gets the data in this part at this position in the current
     * row. The absolute position looked up is <code>currentrecord *
     * recordsize + position</code>. The part must be valid.
     * @param position position within the current row
     * @return A pointer to the data, positioned at the <i>defined byte</i>
     *         of the data entry.
     */
    inline char* getOutputData(IFR_Int4 position)
    {
        return (char*)GetReadData(0) + m_massextent + position;
    }

    
    inline char* getInputData(IFR_ShortInfo& shortinfo) 
    {
        if(m_variableinput) {
            return (char * )GetReadData(0) + GetRawPart()->sp1p_buf_len() + (shortinfo.iolength > 251 ? 3 : 1);
        }  else {
            return (char *)GetReadData(0) + m_massextent + shortinfo.pos.bufpos;
        }
    }
    
    /**
     * Set the start position of the current row (0-based).
     * @param val The start position.
     */
    inline void setCurrentRowStart(IFR_Int4 val)
    {
        m_massextent = val;
    }

    
    /**
     * Sets the current record position.
     * @param currentrecord the current record position, counted from 0.
     */
    inline void setCurrentRecord(IFR_Int2 currentrecord)
    {
        m_currentrecord=currentrecord;
    }
    
    /**
     * Sets the record size in bytes.
     * @param recordsize the record size.
     */
    inline void setRecordSize(IFR_Int2 recordsize)
    {
        m_recordsize=recordsize;
    }
    
    /**
     * Checks whether the data part can additionally occupy
     * the given number of bytes.
     * @param bytes the number of bytes that should be added.
     * @return <code>true</code> if <code>bytes</code> bytes
     *   will fit into the packet, <code>false</code> if
     *   they would exhaust the packet.
     */
    IFR_Bool hasRoomFor(IFR_UInt4 bytes);
        
    /**
     * Gets the current record.
     */
    inline IFR_Int2 getCurrentRecord() const 
    { 
        return m_currentrecord; 
    }
    
    /**
     * Gets the record size.
     */
    inline IFR_Int2 getRecordSize() const
    {
        return m_recordsize;
    }

    
    /**
     * @brief Moves to the next record in data part (for mass cmds)
     */
    inline void moveRecordBase()
    {
        m_massextent = GetRawPart()->sp1p_buf_len();
    }


    /**
     * Adds a stream that is empty.
     * @param longdescriptor Pointer to the LONG descriptor within the data part.
     * @param lastdata True if this is a LAST DATA, not an ALL DATA.
     */
    void addEmptyStream(char *longdescriptor, IFR_Bool lastdata);

    /**
     * Add stream data. The pointer <code>datastart</code> is adjusted depending
     * on how much data has been inserted. 
     * If the <code>VALPOS</code> field of the LONG descriptor is 0 (which can't be),
     * it is initialised correctly to the current offset, otherwise it is assumed 
     * that at <code>VALPOS</code> there are already <code>VALLEN</code> bytes
     * of data for this LONG, and further addition has to occur after this, and
     * correct only <code>VALLEN</code>.
     * @param datastart Pointer to the start of the data that is being inserted.
     * @param dataend   Pointer to one after the end of the data that is being 
     *                  inserted.
     * @param sourceencoding Encoding of the data.
     * @param targetencoding Encoding of the data part.
     * @param clink          Connection link for error reporting.
     * @param longdescriptor Pointer to the LONG descriptor within the data part.
     * @return <code>IFR_OK</code> if all was being inserted, <code>IFR_DATA_TRUNC</code>
     *         if some data was not inserted (and <code>datastart</code> is adjusted).
     */
    IFR_Retcode addStreamData(char *& datastart,
                              char *dataend,
                              char *longdescriptor,
                              IFR_StringEncoding sourceencoding,
                              IFR_StringEncoding targetencoding,
                              IFR_ConnectionItem& clink,
                              IFR_Bool bin2hex);
    /**
     * Sets the <code>VALMODE</code> field for a certain LONG descriptor
     * within this data part.
     * @param longdescriptor Pointer to the LONG descriptor within this part.
     * @param valmode The <code>VALMODE</code> to set.
     */
    void setStreamValMode(char *longdescriptor, IFR_Int1 valmode);
    
    /**
     * Clears the offsets. The start of the current row is set to zero.
     */
    inline void clearOffsets()
    {
        m_currentrecord = 0;
        m_massextent    = 0;
    }

    /**
     * Sets the extent. This is useful when it is expected that a full row is set.
     * The extent is not decreased, it is only set if the argument is greater 
     * than the current extent. Also the <code>buf_len</code> field is set, 
     * that means the extent passed to this method is allocated in the datapart.
     * @param extent The new extent.
     * @return The old buffer length of the data part.
     */
    inline IFR_Int4 extendRow(IFR_Int4 extent)
    {
        IFR_Int4 result = GetRawPart()->sp1p_buf_len();

        if(!m_variableinput) {
            if(result >= m_massextent + extent) {
                return result;
            } else {
                GetRawPart()->sp1p_buf_len() = m_massextent + extent;
                return result;
            }
        } else {
            return result;
        }
    }
    
    inline void setLength(IFR_Int4 buf_length) 
    {
        GetRawPart()->sp1p_buf_len() = buf_length;
    }
        

    /**
     * Sets the input parameter count, necessary for each row on variable
     * input.
     * @param inputparametercount The number of input parameters.
     */
    inline void setInputParameterCount(IFR_Int2 inputparametercount)
    {
        if(m_variableinput) {
            char *p = GetRawPart()->sp1p_buf().asCharp() + m_massextent;
            *p     = (char) (inputparametercount >> 8);
            *(p+1) = (char) (inputparametercount & 0xFF);
            if((m_massextent + 2) > GetRawPart()->sp1p_buf_len()) {
                GetRawPart()->sp1p_buf_len() = m_massextent + 2;
            }
        }
    }

    inline IFR_Bool hasVariableInput()
    {
        return m_variableinput;
    }

    inline void setVariableInput(IFR_Bool variableinput)
    {
        m_variableinput = variableinput;
    }

    /**
     * Adds a LONG descriptor. The long descriptor is added, the argument count 
     * is incremented by one.
     */
    IFR_Retcode addDescriptor(IFRPacket_LongDescriptor& longdesc);
    
private:
    IFR_Int2 m_currentrecord;     //!< The index of the current record within the data part.
    IFR_Int2 m_recordsize;        //!< The size of one row.
protected:
    IFR_Int4 m_extent;            //!< End of current input parameter that is filled in.
    IFR_Int4 m_massextent;        //!< The current row start in the data part. 
    IFR_Bool m_variableinput;     //!< Whether this is a vardata part.
};

/**
 * Vardata Part, only for output iteration.
 */
class IFRPacket_VarDataPart
    : public IFRPacket_Part
{
public:
    inline IFRPacket_VarDataPart() 
    :m_current_position(0),
     m_rowcolumns(0),
     m_currentcolumn(0)
    {}

    inline IFRPacket_VarDataPart(const PIn_Part& cpart, IFR_StringEncoding encoding)
    :IFRPacket_Part(cpart, encoding)
    {}

    IFR_Retcode nextRow();
    
    IFR_Retcode nextField(char *&data, IFR_Length& datalength);
    
private:    
    unsigned char *m_current_position;
    IFR_Int2       m_rowcolumns;
    IFR_Int2       m_currentcolumn;
};

/**
 * A longdata part. This contains input long parameters and also results retrieved 
 * from the database.
 */
class IFRPacket_LongDataPart
    : public IFRPacket_DataPart
{
public:
    inline IFRPacket_LongDataPart() 
    :IFRPacket_DataPart()
    {}

    inline IFRPacket_LongDataPart(const IFRPacket_DataPart& datapart)
    :IFRPacket_DataPart(datapart)
    {}

    inline IFRPacket_LongDataPart(const IFRPacket_LongDataPart& datapart)
    :IFRPacket_DataPart(datapart)
    {}
    
    /**
     * Inserts a LONG descriptor for the close of a PUTVAL
     * operation (with VALMODE set to <code>vm_close</code>.
     * @return <code>IFR_OK</code> if the insertion was successful,
     *         <code>IFR_NOT_OK</code> if there was not enough room.
     */
    IFR_Retcode closePutval();

};

/**
 * A long demand part. This supports positioned long data reading
 * from the database.
 */
class IFRPacket_LongDemandPart
    : public IFRPacket_DataPart
{
public:
    inline IFRPacket_LongDemandPart() 
    :IFRPacket_DataPart(),
     m_extent(0)
    {}

    inline IFRPacket_LongDemandPart(const IFRPacket_DataPart& datapart)
    :IFRPacket_DataPart(datapart),
     m_extent(0)
    {}

    inline IFRPacket_LongDemandPart(const IFRPacket_LongDemandPart& datapart)
    :IFRPacket_DataPart(datapart),
     m_extent(datapart.m_extent)
    {}

    /**
     * Add next triple of parameters for positioned reading of long data.
     * @param immediately <code>true</code>, if the long data should be delivered together
     *                    with the non-long data
     * @param position the position in bytes from which to start reading long data (counting from 1).
     *                 If position is negative, the start position is calculated from the end of the
     *                 long data.
     * @param length The length in bytes of the long data to be read, starting with 1.
     * @return <code>IFR_OK</code> if successful, <code>IFR_NOT_OK</code> otherwise.
     */
    IFR_Retcode addPosParams (const IFR_Bool immediately, 
                              const IFR_Int4 position, 
                              const IFR_UInt4 length);
private:
    IFR_Int4 m_extent; //!< to be deleted.
};

/**
 * A session info part.
 * This kind of part is returned after the connect.
 */
class IFRPacket_SessionInfoPart
    : public IFRPacket_Part
{
public:
    inline IFRPacket_SessionInfoPart()
    {}
    
    /**
     * Constructor. Overlay on <code>PIn_Part</code>
     * @param cpart the part structure this part will reuse.
     */
    inline IFRPacket_SessionInfoPart(const PIn_Part& cpart, IFR_StringEncoding encoding)
    :IFRPacket_Part(cpart, encoding)
    {}
    
    /**
     * Gets the data contained in this part.
     * @return a pointer to a character array  of 2002 bytes.
     */
    inline char* getData()
    {
        return (char*) GetParameterData(0);
    }
    
};

/**
 * A resultcount part. Contains a resultcount as VDN number.
 */
class IFRPacket_ResultCountPart
    : public IFRPacket_Part
{
public:
    /**
     * Constructor.
     */
    inline IFRPacket_ResultCountPart() 
    {}

    /**
     * Constructor. Overlay on <code>PIn_Part</code>
     */
    inline IFRPacket_ResultCountPart(const PIn_Part& cpart, IFR_StringEncoding encoding)
    :IFRPacket_Part(cpart, encoding)
    {}
    
    /**
     * Gets the resultcount.
     * @param ResCount this is set to the resoultcount found in the part.
     * @return <code>IFR_OK</code> on success, <code>IFR_NO_DATA_FOUND</code> 
     *         otherwise.
     */
    IFR_Retcode getResultCount(IFR_Int4 &ResCount) const;

    /**
     * Sets the result count.
     * @param resultcount the result count to set.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code>
     *   if the packet size was reached.
     */
    IFR_Retcode setResultCount(IFR_Int4 resultcount);


    /**
     * Sets the fetch size count.
     * @param fetchsize the fetch size to set.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code>
     *   if the packet size was reached.
     */
    IFR_Retcode setFetchSize(IFR_Int2 fetchsize);

    /**
     * Sets the result count to <code>UNDEFSIGNAL</code>
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code>
     *   if the packet size was reached.
     */
    IFR_Retcode setUndefResultCount();

};    

/**
 * A tablename part. Contains a (result)tablename.
 */
class IFRPacket_TableNamePart
    : public IFRPacket_CommandPart
{
public:
    /**
     * Constructor.
     */
    inline IFRPacket_TableNamePart() 
    {}

    /**
     * Constructor. 
     */
    inline IFRPacket_TableNamePart(const IFRPacket_Part& part)
    :IFRPacket_CommandPart(part)
    {}
    
    /**
     * Gets the (result)tablename from a part.
     * @param ResName result table name (output)
     * @return <code>IFR_OK</code> on success, <code>IFR_NO_DATA_FOUND</code> 
     *         otherwise.
     */
    IFR_Retcode getResultName(IFR_String &ResName, IFR_Bool& memory_ok) const;
    
};    

/**
 * A <i>columnnames</i> part. Received after parsing a procedure call,
 * or describing a result.
 */
class IFRPacket_ColumnNamesPart
    : public IFRPacket_Part
{
public:
    /**
     * Constructor.
     */
    inline IFRPacket_ColumnNamesPart() 
    {}

    /**
     * Constructor. Overlay on <code>PIn_Part</code>
     */
    inline IFRPacket_ColumnNamesPart(const PIn_Part& cpart, IFR_StringEncoding encoding)
    :IFRPacket_Part(cpart, encoding)
    {}

    /**
     * Retrieves the column names from the part. They are inserted
     * into the <code>columnnames</code> vector.
     * @param columnnames The destination vector.
     * @return <code>IFR_OK</code> if the column names are retrieved,
     *   <code>IFR_NO_DATA_FOUND</code> if no column names are found.
     */
    IFR_Retcode getColumnNames(IFRUtil_Vector<IFR_String>& columnnames,
                               IFR_Bool& memory_ok);
};



/**
 * An <i>applparam</i> part. To be sent to the DB on prepare,
 * to assist the server in finding the correct type for the
 * parameter.
 */
class IFRPacket_ApplParamPart
    : public IFRPacket_Part
{
public:
    /**
     * Constructor.
     */
    inline IFRPacket_ApplParamPart() 
    {}

    /**
     * Constructor. Overlay on <code>PIn_Part</code>
     */
    inline IFRPacket_ApplParamPart(const PIn_Part& cpart, IFR_StringEncoding encoding)
    :IFRPacket_Part(cpart, encoding)
    {}
    
    /**
     * Adds an argument. The argument count is increased, the 
     * part size is incremented by 4.
     * @param datatype The requested data type.
     * @param frac The number of fractional digits, if any.
     * @param length The <i>logical</i> length (number of characters or digits).
     */
    void addArgument(IFR_SQLType datatype,
                     IFR_Int1    frac,
                     IFR_size_t  length);
    
    friend IFR_TraceStream& operator << (IFR_TraceStream& s, const enum IFRPacket_PartKind::PartKind p );
};

/**
 * An 'ABAP Input Stream' part.
 */
class IFRPacket_AbapIStreamPart 
    : public IFRPacket_Part
{
public:
    inline IFRPacket_AbapIStreamPart()
    {}

    inline IFRPacket_AbapIStreamPart(const PIn_Part& cpart, IFR_StringEncoding encoding)
    :IFRPacket_Part(cpart, encoding)
    {}

    inline IFR_Retcode getABAPTabId(IFR_Int4& tabid)
    {
        if(isValid()) {
            memcpy(&tabid, GetReadData(), sizeof(IFR_Int4));
            return IFR_OK;
        } else {
            return IFR_NO_DATA_FOUND;
        }
    }
    
    inline void * getMask()
    {
        if(Length() <= sizeof(IFR_Int4)) {
            return 0;
        } else {
            return (void *) (GetReadData() + sizeof(IFR_Int4)); 
        }
    }
    
    inline IFR_Length getMaskLength()
    {
        return Length() <= sizeof(IFR_Int4) ? 0 : Length() - sizeof(IFR_Int4);
    }
    
};

/**
 * An 'ABAP Output Stream' part.
 */
class IFRPacket_AbapOStreamPart 
    : public IFRPacket_Part
{
public:
    inline IFRPacket_AbapOStreamPart()
    {}

    inline IFRPacket_AbapOStreamPart(const PIn_Part& cpart, IFR_StringEncoding encoding)
    :IFRPacket_Part(cpart, encoding)
    {}

    inline IFR_Retcode getABAPTabId(IFR_Int4& tabid)
    {
        if(isValid()) {
            memcpy(&tabid, GetReadData(), sizeof(IFR_Int4));
            return IFR_OK;
        } else {
            return IFR_NO_DATA_FOUND;
        }
    }

    inline void * getMask()
    {
        if(Length() <= sizeof(IFR_Int4)) {
            return 0;
        } else {
            return (void *) (GetReadData() + sizeof(IFR_Int4)); 
        }
    }

    inline IFR_Length getMaskLength()
    {
        return Length() <= sizeof(IFR_Int4) ? 0 : Length() - sizeof(IFR_Int4);
    }
};

/**
 * A command info part makes it possible to set and add a command info.
 */
class IFRPacket_CommandInfoPart
    : public IFRPacket_Part
{
public:
    inline IFRPacket_CommandInfoPart() 
    {}

    inline IFRPacket_CommandInfoPart(const PIn_Part& part, IFR_StringEncoding encoding)
    :IFRPacket_Part(part, encoding)
    {}
    
    /**
     * Set the command info text.
     * @param text the command info text.
     * @param lineNumber the command info line number text.
     * @param clink Connection link for error reporting.
     * @return <code>IFR_OK</code> on success.
     */
    IFR_Retcode addCommandInfo(const IFR_String& text, IFR_Int4 lineNumber, IFR_ErrorHndl &error);
};


struct IFRPacket_Feature
{
    enum Feature {
#if IFR_HAS_FEATUREPART
        MultipleDropParseID_C   = sp1f_multiple_drop_parseid,
        SpaceOption_C           = sp1f_space_option,
        VariableInput_C         = sp1f_variable_input,
        OptimizedStreams_C      = sp1f_optimized_streams,
        CheckScrollableOption_C = sp1f_check_scrollableoption
#else
        NoFeature_C
#endif
    };
};


/**
 * A feature request/reply part.
 */
class IFRPacket_FeaturePart
    : public IFRPacket_Part
{
public:
    /**
     * Default constructor.
     */
    inline IFRPacket_FeaturePart()
    {}
    
    /**
     * Requests a feature. If the feature is already reqeuested, nothing
     * happens, else the part is extended so that the feature is requested.
     */
    IFR_Retcode requestFeature(IFRPacket_Feature::Feature feature);

    /**
     * Access the data for reading.
     * @return Pointer to data buffer.
     */
    inline char *getData()
    {
        return GetRawPart()->sp1p_buf().asCharp();
    }
};

IFR_END_NAMESPACE

#endif // IFRPACKET_PART_H
