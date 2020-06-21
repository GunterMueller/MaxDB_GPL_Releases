/*!
  @file           IFR_ConnectionItem.h
  @author         D039759
  @ingroup        IFR_Connection
  @brief          
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
#ifndef IFR_CONNECTIONITEM_H
#define IFR_CONNECTIONITEM_H

#include "SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "Interfaces/Runtime/IFR_SQLWarning.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

IFR_BEGIN_NAMESPACE

class IFR_Connection;
class IFR_GetvalHost;
    class IFR_PutvalHost;

/**
 * @ingroup IFR_Connection
 * @brief Base class to centralize error handling and connection reference.
 * 
 * A connection item is an object that references a connection, and is
 * tightly coupled to it. A connection itself, a statement, or a result set 
 * are example usages. All connection items have an error and a warning member,
 * which may be filled by
 */
class IFR_ConnectionItem
{
public:
    /**
     * Gets the connection that belongs to this item.
     * @return A pointer to the connection stored, or 0 if no connection
     *         is stored.
     */
    virtual IFR_Connection* getConnection() const = 0;

    /**
     * Get a reference to the error that is stored here.
     * @return The error instance that is stored in this item.
     */
    inline IFR_ErrorHndl&  error() 
    {
        return m_error;
    }

    /**
     * Get a reference to the error that is stored here.
     * @return The error instance that is stored in this item (as constant reference).
     */
    inline const IFR_ErrorHndl& error() const 
    {
        return m_error;
    }
    
    /**
     * Clears the error that has been set.
     */
    void clearError();

    /**
     * Returns the SQL Warning stored in this connection item.
     * @return The SQL warning stored in this item.
     */
    IFR_SQLWarning*& warning();
    
    /**
     * Clears the SQL warnings.
     */
    void clearWarnings();
    
    /**
     * Adds an SQL warning.
     * @param warningToAdd the SQL warning.
     */
    inline void addWarning(IFR_SQLWarning *warningToAdd)
    {
        if(warningToAdd != 0) {
            if(warning()) {
                warning()->addWarning(warningToAdd);
            } else {
                warning()=warningToAdd;
            }
        }
    }

    /**
     * Retrieve the allocator.
     */
    inline SAPDBMem_IRawAllocator* getAllocator() 
    {
        return m_allocator;
    }

    /**
     * Get the 'PUTVAL' host of this connection item.
     * Return 0 if this connection item does not have an
     * output LOB locator interface.
     */
    virtual IFR_PutvalHost *getPutvalHost();

    /**
     * Get the 'GETVAL' host of this connection item.
     * Return 0 if this connection item does not have an
     * input LOB locator interface.
     */
    virtual IFR_GetvalHost *getGetvalHost();

protected:
    /** 
     * Constructor.
     * @param connection The connection this item depends upon.
     */
    IFR_ConnectionItem(IFR_Connection *connection);

    /** 
     * Constructor.
     * @param allocator The allocator that will be used for the error
     *        instance.
     */
    IFR_ConnectionItem(SAPDBMem_IRawAllocator& allocator);
   
    /**
     * Destructor. Deletes the associated SQL warning.
     */
    virtual ~IFR_ConnectionItem();
    
    
    /**
     * Asserts that the connection is open.
     * @return <code>IFR_OK</code> if the connection is open;
     * <code>IFR_NOT_OK</code> otherwise.
     */
    IFR_Retcode assertOpen ();

    //!< If @c false, an error happened.
    IFR_ErrorHndl                m_error;      //!< The error handle.
    IFR_SQLWarning              *m_warning;    //!< Handle for the warning.
    SAPDBMem_IRawAllocator      *m_allocator;  //!< Allocator needed for destruction.
    IFR_Bool                     m_closeflag;  //!< Flag set to true when this is destructed, for 
                                               //!< access error detection.
                                  
};

SQLDBC_IRuntime::TaskTraceContext *IFR_GetTraceContext(IFR_ConnectionItem* connectionitem);
IFR_TraceStream *IFR_GetTraceStream(IFR_ConnectionItem *connectionitem);

IFR_END_NAMESPACE

#endif // IFR_CONNECTIONITEM_H
