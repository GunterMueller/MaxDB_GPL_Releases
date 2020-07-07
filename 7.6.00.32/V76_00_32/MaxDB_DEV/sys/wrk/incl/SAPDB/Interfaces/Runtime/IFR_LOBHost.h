/*!
  @file           IFR_LOBHost.h
  @author         D039759
  @ingroup        
  @brief          LOB host classes.
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
#ifndef IFR_LOBHOST_H
#define IFR_LOBHOST_H

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/Util/IFRUtil_Vector.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Getval.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Putval.h"
#include "Interfaces/Runtime/Packet/IFRPacket_ReplySegment.h"
#include "Interfaces/Runtime/Packet/IFRPacket_RequestSegment.h"
#include "Interfaces/Runtime/Packet/IFRPacket_RequestPacket.h"
#include "Interfaces/Runtime/IFR_Connection.h"

IFR_BEGIN_NAMESPACE

/**
 * Base class for objects that handle LOBs and LONG data.
 */
class IFR_LOBHost
{
public:
    friend class IFR_LOB;

    /**
     * Constructor.
     */
    IFR_LOBHost(SAPDBMem_IRawAllocator& allocator);
    /**
     * Destructor
     */
    virtual ~IFR_LOBHost();
    
    /**
     * Physically deletes all stored LOB objects. This should be done only
     * to invalidate the LOB objects.
     */
    void clearLOBs();

    /**
     * Invalidates the LOB objects. That means, they are all set to 'closed'.
     */
    void invalidateLOBs();
    
    /**
     * Adds a LOB.
     */
    void addLOB(IFR_LOB *lob, IFR_Bool& memory_ok);

    /**
     * Checks that the LOB pointer is known. <b>Note that
     * this is not 100% safe, as released space for the
     * LOB may be again used for another LOB.</b>
     *
     * @param lob The LOB pointer to check.
     * @return @c true if the LOB is valid.
     */
    IFR_Bool checkLOB(IFR_LOB *lob);
    
    /**
     * Closes the LOB, if it isn't already closed.
     * @param lob The LOB to be closed.
     */
    virtual IFR_Retcode closeLOB(IFR_LOB* lob) = 0;
    
    /**
     * Access method for a LOB.
     */
    inline IFR_LOB *getLOB(IFR_Int4 index) { return m_lobs[index]; }
    
    /**
     * Get the number of LOB objects stored.
     */
    inline IFR_size_t getLOBCount() { return m_lobs.GetSize(); }
private:
    IFRUtil_Vector<IFR_LOB *> m_lobs;
};

/**
 * A class that hosts PUTVAL objects which are used to write LONG data
 */
class IFR_PutvalHost
    : virtual public IFR_LOBHost
{
public:
    IFR_PutvalHost(SAPDBMem_IRawAllocator& allocator);
    
    /**
     * Destructor.
     */
    virtual ~IFR_PutvalHost();
    
    /**
     * Clears the input longs.
     */
    void clearInputLongs();

    /**
     * Puts data for a putval.
     */
    virtual IFR_Retcode putData(IFR_LOB *lob, void *data, IFR_Length* lengthindicator) = 0;
    
    /**
     * Gets the input long at the specified index.
     * @param index The index (0-based).
     * @return The PUTVAL object stored at @c index.
     */
    inline IFRConversion_Putval* getInputLong(IFR_Int2 index) 
    {
        return m_inputlongs[index];
    }
    
    /**
     * Finds the input long.
     * @param column The column.
     * @param row The row, 0 means ignore.
     */
    IFRConversion_Putval* findInputLong(IFR_Int2 column, IFR_Int4 row);
    
    /**
     * Adds an input long, and sets the 'valindex'.
     * @param putval The PUTVAL instance to add.
     * @param memory_ok Memory allocation check flag.
     */
    void addInputLong(IFRConversion_Putval* putval, IFR_Bool& memory_ok);

    
    /**
     * Gets the number of PUTVAL objects stored.
     * @return The number of PUTVAL objects currently stored.
     */
    inline IFR_Int2 getInputLongCount() const { return (IFR_Int2) m_inputlongs.GetSize(); }

private:
    IFRUtil_Vector<IFRConversion_Putval *>m_inputlongs;    
};

/**
 * A long descriptor "accidentally" opened due to
 * fetch operations.
 */
struct IFR_OpenLongDescriptor
{
    IFR_OpenLongDescriptor()
    {}

    IFR_OpenLongDescriptor(IFR_Int4 r, IFR_Int4 c, const IFRPacket_LongDescriptor& ld)
    :row(r),
     column(c)
    {
        memcpy(&longdesc, &ld, sizeof(IFRPacket_LongDescriptor));
    }

    IFR_OpenLongDescriptor(const IFR_OpenLongDescriptor& cp)
    :row(cp.row),
     column(cp.column)
    {
        memcpy(&longdesc, &cp.longdesc, sizeof(IFRPacket_LongDescriptor));
    }
     
    
    
    IFR_Int4                 row;
    IFR_Int4                 column;
    IFRPacket_LongDescriptor longdesc;
};


/**
 * A class that handles GETVAL operations.
 */
class IFR_GetvalHost
    : virtual public IFR_LOBHost
{
public:
    /**
     * Constructor.
     */
    IFR_GetvalHost(IFR_Connection *connection, SAPDBMem_IRawAllocator& allocator);
    
    /**
     * Destructor
     */
    virtual ~IFR_GetvalHost();
    
    /**
     * Clears the stored getval objects.
     */
    void clearOutputLongs();

    /**
     * Closes the stored getval objects.
     */
    void closeOutputLongs();

    /**
     * Gets the input long at the specified index.
     * @param index The index (0-based).
     */
    IFRConversion_Getval* getOutputLong(IFR_Int4 index);
    
    /**
     * Finds the output long.
     * @param column The column.
     * @param row The row, 0 means ignore.
     */
    IFRConversion_Getval* findOutputLong(IFR_Int2 column, IFR_Int4 row);
    
    /**
     * Adds an output long, and sets the 'valindex'. Also, if the long
     * is already opened and listed in the 'open long list', it is removed
     * from there.
     * @param getval The getval instance to add.
     * @param memory_ok Memory allocation check flag.
     */
    void addOutputLong(IFRConversion_Getval *getval, IFR_Bool& memory_ok);
    
    void checkOpenLongs(IFRConversion_ConverterList& cols,
                        IFR_Int4 start,
                        IFR_Int4 end,
                        IFR_Int4 recordSize,
                        IFRPacket_DataPart& datapart);
    /**
     * Adds an opened long descriptor that has no corresponding 
     * GETVAL instance.
     * @param row The row of the long.
     * @param column The column of the long.
     * @param descriptor The descriptor.
     */
    void addOpenLong(IFR_Int4 row, IFR_Int4 column, IFRPacket_LongDescriptor& descriptor);

    /**
     * Updates the contents of the descriptors.
     * @param segment The reply segment containing a LONGDATA part.
     * @param citem The connection item.
     * @param updateCount Returns the number of long descriptors that have been updated.
     * @param longDataReadSizeCounter Updates the profiling counter LONGDATAREADSIZE.
     * @return @c IFR_OK on success, @c IFR_NOT_OK in case inplausible data is found
     *   in the long descriptor.
     */
    IFR_Retcode updateOutputLongs(IFRPacket_ReplySegment& segment, IFR_ConnectionItem& citem, IFR_Int4& updateCount, IFR_UInt8& longDataReadSizeCounter);


    /**
     * Retrieve the number of output longs stored.
     * @return The number of output long values.
     */
    inline IFR_Int2 getOutputLongCount() const { return (IFR_Int2) m_outputlongs.GetSize(); }
    
    /**
     * Read data.
     */
    virtual IFR_Retcode getData(IFR_LOB    *lob,
                                void       *data,
                                IFR_Length *lengthindicator,
                                IFR_Length  bytelength,
                                IFR_Length *posindicator,
                                IFR_Bool    terminate) = 0;


    /**
     * Get the length of a LOB.
     */
    virtual IFR_Length getLength(IFR_LOB *lob) = 0;
        
    
    IFR_Retcode closeGetval(IFRConversion_Getval* getval, IFR_Bool& memory_ok);
    
private:
    IFRUtil_Vector<IFRConversion_Getval *>   m_outputlongs;
    IFRUtil_Vector<IFR_OpenLongDescriptor>   m_open_longs;
    IFR_Connection                          *m_connection;
};

IFR_END_NAMESPACE

#endif
