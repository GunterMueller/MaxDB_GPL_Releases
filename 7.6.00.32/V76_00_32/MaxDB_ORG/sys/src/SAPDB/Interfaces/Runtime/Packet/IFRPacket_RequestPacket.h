/*!
  @file           IFRPacket_RequestPacket.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Packet
  @brief          Request Packet.
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
#ifndef IFRPACKET_REQUESTPACKET_H
#define IFRPACKET_REQUESTPACKET_H

#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/IFR_String.h"

#include "Interfaces/Runtime/Util/IFRUtil_Vector.h"
#include "Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h"

IFR_BEGIN_NAMESPACE

class IFRPacket_Lock;
class IFR_ConnectionItem;
class IFRConversion_Converter;
class IFRConversion_ConverterList;
class IFRUtil_RuntimeItem;

/**
 * @ingroup IFR_Packet
 * The individual message types defined for SAP DB. Each segment
 * has a command message type.
 */
class IFRPacket_CommandMessageType
{
public:
    enum MessageType {
        Nil_C     = sp1m_nil,          //!< NIL command.
        Dbs_C     = sp1m_dbs,          //!< DBS command (immediate execute).
        Parse_C   = sp1m_parse,        //!< PARSE command (prepare).
        Syntax_C  = sp1m_syntax,       //!< SYNTAX command (syntaxcheck)
        Execute_C = sp1m_execute,      //!< EXECUTE command (after PARSE).
        Putval_C  = sp1m_putval,       //!< PUTVAL command (for LONG data).
        Getval_C  = sp1m_getval,       //!< GETVAL command (for LONG data).
        Load_C    = sp1m_load,         //!< LOAD
        Unload_C  = sp1m_unload,       //!< UNLOAD
        Hello_C   = sp1m_hello,        //!< HELLO (check availability of database).
        GetChallenge_C = sp1m_get_challenge   //!< GET CHALLENGE (MySQL connect).
        // sp1m_get_challenge 
    }; 
};


/**
 * @ingroup IFR_Packet
 * A packet used in the request. A request packet contains
 * one or several command segments, which contain separate commands.
 */
class IFRPacket_RequestPacket 
    : public PIn_RequestPacket, 
      public IFRUtil_RuntimeItem
{
public:
    enum PacketType {
        Root_C,         //!< The root packet. This is the packet instance handed out by the RTE.
        Root_Ref_C,     //!< An application packet instance that uses the root packet.
        Dynamic_C,      //!< A totally dynamic packet.
        Any_C           //!< Any kind of packet (for use in interfaces).   
    };

    /**
     * Copy constructor. 
     * @param requestpacket The request packet to copy.
     */
    IFRPacket_RequestPacket(const IFRPacket_RequestPacket& requestpacket);

    /**
     * Creates an invalid, uninitialised request packet.
     */
    IFRPacket_RequestPacket(IFRUtil_RuntimeItem& runtimeitem);

    /**
     * Assignment operator.
     * @param requestpacket The packet to assign to this one.
     * @return A reference to this instance.
     */
    // IFRPacket_RequestPacket& operator=(const IFRPacket_RequestPacket& requestpacket);
    

    /**
     * Assignment function.
     * @param  requestpacket The packet to be assigned to this packet.
     * @param  error The error to be set when the assignment fails.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> on
     *         disastrous errors.
     */
    IFR_Retcode assign(const IFRPacket_RequestPacket& requestpacket, IFR_ErrorHndl& error, IFR_Int4* in_use=0);
    
    /**
     * Destructor. Detaches from the packet, if appropriate.
     */
    ~IFRPacket_RequestPacket();

    /**
     * Sets the application and version. 
     * @param application a <code>char[3]</code> identifying the application.
     * @param appversion a <code>char[5]</code> identifying the version, if not specified
     *                   <code>"80000"</code> is used.
     */
    void initVersion(const char *application,
                     const char *version=0);

    /**
     * Gets the encoding of the packet. This is stored in the raw header 
     * (<code>mess_code</code> and <code>mess_swap</code>.
     * @return the encoding of the packet, or 0 if no valid encoding was found,
     *         or the packet is not valid.
     */
    const IFR_StringEncoding getEncoding() const;
    
    /**
     * Sets the encoding of the packet. This is stored in the raw header 
     * (<code>mess_code</code> and <code>mess_swap</code>.
     * @return the last encoding was set.
     */
    const IFR_StringEncoding setEncoding(const IFR_StringEncoding encoding) const;
    

    /**
     * Resets the packet, i.e. clears all segment information.
     */
    inline void reset() 
    {
        Reset();
    }

    /**
     * Get the status of the packet.
     * @return <code>true</code> if the request packet is correctly
     *         initialised, <code>false</code> otherwise.
     */
    inline IFR_Bool isValid() const 
    {
        return IsValid();
    }
    
    /**
     * Adds a segment.
     * @param messagetype the message type of the segment.
     * @param parseagain whether the <code>PARSE AGAIN</code> field should be
     *        set (valid only for PARSE messages).
     * @return the created request segment.
     */
    PIn_RequestSegment addSegment(int messagetype, IFR_Bool parseagain=false)
    {
        return addSegment(messagetype, parseagain, m_CurrentSqlMode);
    }
    
    /**
     * Adds a segment.
     * @param messagetype the message type of the segment.
     * @param parseagain whether the <code>PARSE AGAIN</code> field should be
     *        set (valid only for PARSE messages).
     * @param sqlmode the SQL mode to use.
     * @return the created request segment.
     */
    PIn_RequestSegment addSegment(int messagetype, IFR_Bool parseagain, int sqlmode);
    
    /**
     * Switches the SQL Mode. 
     * @param newMode The new SQL Mode.
     * @return The old SQL Mode.
     * @see IFR_SQLMode
     */
    inline IFR_Int4 switchSqlMode (IFR_Int4 newMode)
    {
        IFR_Int4 result = m_CurrentSqlMode;
        m_CurrentSqlMode = newMode;
        return result;
    }
    
    
    /**
     * Gets the current SQL mode.
     * @return The current SQL mode.
     */
    inline int getSQLMode() 
    {
        return m_CurrentSqlMode;
    }


    /**
     * Gets a pointer to the 1st segment.
     */
    IFR_Byte *getFirstSegmentPtr ();
    
    /**
     * Sets the "mass command" flag in the first request segment.
     * @todo Move this method to the request segment.
     */
    void setMassCommand ();

    /**
     * Sets the "parse again" flag in the first request segment.
     * @todo Move this method to the request segment.
     */
    void setParseAgain();
    
    /**
     * Sets the "with info" flag in the first request segment.
     * @todo Move this method to the request segment.
     */
    void setWithInfo ();

//     /**
//      * Sets the "commit immediately" flag in the first request segment.
//      * @todo Move this method to the request segment.
//      */
//     void setCommitImmediately ();

    /**
     * Gets the lock instance that manages this packet.
     */
    IFRPacket_Lock* getLock();
    
    /**
     * Retrieves wether the memory of this packet is the
     * shared memory provided by the RTE.
     * @return <code>true</code> is this packet refers to the RTE 
     *         packet.
     */
    inline IFR_Bool isRoot() const
    {
        return m_packettype==Root_Ref_C ||
            m_packettype==Root_C;
    }
    
    /**
     * Copies the contents of <code>source</code> into this packet.
     * @param source The source packet, which must be valid.
     * @return <code>false</code> if the packet could be copied, 
     * <code>false</code> otherwise
     */
    IFR_Bool copyInto(IFRPacket_RequestPacket& source);
    
    /**
     * Check whether the packet is swapped (= little endian) or not.
     * @return <code>true</code> if the packet is little endian, <code>false</code>
     *         if the packet is big endian.
     */
    IFR_Bool isSwapped() const;


    /**
     * Gets the number of segments in this packet.
     */
    IFR_Int2 getNumberOfSegments() const;

    /**
     * Gets the bytes that are remaining in this packet.
     */
    IFR_Int4 remainingBytes() const;

    /**
     * Gets the number of bytes occupied by segments and parts
     */
    IFR_Int4 dataSize() const;

    /**
     * Replaces the parse id found in the 1st parse id part of
     * the first segment.
     * @param parseid Pointer to the parse id.
     * @param length Original overall length of the request packet.
     * @param old_bufpos The original buffer positions of the input.
     * @param shortinfo The new parsed fields.
     * @param clink Connection link for error reporting.
     * @return <code>IFR_OK</code> on success, <code>IFR_NOT_OK</code> if
     *         either the parse id or the packet are invalid for this
     *         operation.
     */
    IFR_Retcode replaceParseIDAdjustData(IFR_ParseID* parseid,
                                         IFR_size_t   length,
                                         IFRUtil_Vector<IFR_Int4>& old_bufpos,
                                         IFRConversion_ConverterList& shortinfo,
                                         IFR_ConnectionItem& clink);
    /**
     * Invalidates the packet. This is done by assigning an invalid 
     * packet to this one.
     */
    inline void invalidate()
    {
        destroyPacketData();
        m_CurrentSqlMode = IFR_SessionSqlmode;
        m_lock = 0;
        m_hasexclusivelock = false;
        m_packettype = IFRPacket_RequestPacket::Root_Ref_C;
        rawPacket = 0;
    }


    /**
     * Initialises a request packet. There is no lock acquired, as this 
     * request packet is stored in a member of the connection, only 
     * the handed-out copies acquire the lock.
     * @param packetptr pointer to raw packet (output from <code>SqlAConnect</code>).
     * @param size packet size (output from <code>SqlAConnect</code>).
     * @param manager The manager this packet will be registered with.
     * @param unicode whether it is an unicode packet or not (1=unicode, 0=ascii).
     * @param sqlmode the sql mode to be used.
     * @param runtimeitem The reference to runtime and allocator.
     * @todo enhance the <code>unicode</code> parameter, such that UTF8 can also
     *       be used.
     */
    IFRPacket_RequestPacket(tsp1_packet* packetptr, 
                            int size,
                            int unicode,
                            int sqlmode,
                            const char *client_application,
                            const char *client_version,
                            IFRUtil_RuntimeItem& runtimeitem,
                            IFR_Bool& memory_ok);
private:
    /**
     * Initialises a request packet. There is no lock acquired, as this 
     * request packet is stored in a member of the connection, only 
     * the handed-out copies acquire the lock.
     * @param packetptr pointer to raw packet (output from <code>SqlAConnect</code>).
     * @param size packet size (output from <code>SqlAConnect</code>).
     * @param manager The manager this packet will be registered with.
     * @param unicode Whether it is an unicode packet or not (1=unicode, 0=ascii).
     * @param sqlmode The sql mode to be used.
     * @param lockinstance The lock instance used.
     * @todo enhance the <code>unicode</code> parameter, such that UTF8 can also
     *       be used.
     */
    IFRPacket_RequestPacket(tsp1_packet *packetptr,
                            int size,
                            int unicode,
                            int sqlmode,
                            const char *client_application,
                            const char *client_version,
                            PacketType packettype,
                            IFRPacket_Lock *lockinstance,
                            IFRUtil_RuntimeItem& runtimeitem);
    
    /**
     * Delete the data present in the packet.
     */
    void destroyPacketData();
    
    IFR_Int4 m_CurrentSqlMode;  //!< Current SQL mode.
    
    /**
     * The lock that watches this packet.
     */
    IFRPacket_Lock *m_lock;
    
    /**
     * Whether to release the exclusive lock in the destructor.
     */
    IFR_Bool        m_hasexclusivelock;

    /**
     * The type of this packet.
     */
    PacketType      m_packettype;
    
    friend class IFR_Connection;
    friend class IFR_ConnectionSynthPacketData;
    friend class SQLDBC_ClientRuntime;
};

/**
 * Trace operator.
 * @param s The stream.
 * @param type The packet type.
 */
IFR_TraceStream& operator << (IFR_TraceStream& s, 
                              const IFRPacket_RequestPacket::PacketType type);

IFR_END_NAMESPACE

#endif //IFRPACKET_REQUESTPACKET_H
