/*!
  @file           IFRUtil_AsyncOperation.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Common
  @brief          Asynchronous operation base class.
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
#ifndef IFRUTIL_ASYNCOPERATION_H
#define IFRUTIL_ASYNCOPERATION_H

#include "Interfaces/Runtime/IFR_Types.h"

IFR_BEGIN_NAMESPACE

/**
 * @ingroup IFR_Common
 * An asynchronous operation.
 */
class IFRUtil_AsyncOperation
{
public:
    /**
     * Constructor.
     */
    inline IFRUtil_AsyncOperation()
    :m_executed(false)
    {}
    
    /**
     * Destructor.
     */
    virtual ~IFRUtil_AsyncOperation();

    /**
     * Executes the operation, and marks it as done.
     */
    inline void execute()
    {
        run();
        m_executed = true;
    }
    
    /**
     * The operation. Subclasses need to override this method.
     */
    virtual void run() = 0;
    
    inline IFR_Bool isExecuted() const
    {
        return m_executed;
    }


    virtual void reset() 
    {
        m_executed = false;
    }
    
private:
    IFR_Bool m_executed;
};

IFR_END_NAMESPACE

#endif

