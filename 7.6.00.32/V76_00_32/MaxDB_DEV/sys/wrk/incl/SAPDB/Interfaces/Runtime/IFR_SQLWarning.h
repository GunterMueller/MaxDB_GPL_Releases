/*!
  @file           IFR_SQLWarning.h
  @author         D039759
  @ingroup        IFR_Common
  @brief          SQL Warnings
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
#ifndef IFR_SQLWARNING_H
#define IFR_SQLWARNING_H

#include "Interfaces/Runtime/IFR_String.h"
#include "Interfaces/Runtime/IFR_Types.h"

IFR_BEGIN_NAMESPACE

/**
 * A list of all kinds of warnings this interfaces supports.
 */
enum IFR_SQLWarningCode
{
    IFR_SQLWARNING_DATA_TRUNCATED = SQLDBC_SQLWARNING_DATA_TRUNCATED, //!< Input or Output Data has been truncated.
    IFR_SQLWARNING_MAX=SQLDBC_SQLWARNING_MAX                 //!< For checks.
};


/**
 * An SQL Warning. An SQL Warning is a state after an execution that is not
 * critical, but worth being mentioned, as it may cause problems, or be an
 * user error. Such an SQL Warning may originate from the database, or from
 * the interface.
 */
class IFR_SQLWarning
{
public:
    /**
     * Creates a new sql warning. 
     * @param warningcode the kind of warning.
     */
    IFR_SQLWarning(IFR_SQLWarningCode warningcode);

    /**
     * Destructor. Deletes all attached subsequent SQL warnings.
     */
    ~IFR_SQLWarning();
    
    /**
     * Gets the next sql warning of the chain.
     * @return a pointr to the next warning, or 0 if no warning exists.
     */
    inline IFR_SQLWarning*  getNextWarning() const
    {
        return m_nextwarning;
    }
    
    /**
     * Adds a warning to the end of this warning chain.
     */
    void addWarning(IFR_SQLWarning* next);
    
    /**
     * Gets the warnings's code.
     */
    inline IFR_SQLWarningCode getWarningCode() const
    {
        return m_warningcode;
    }

private:
    IFR_SQLWarningCode m_warningcode; //!< The code of the warning.
    IFR_SQLWarning    *m_nextwarning; //!< Link to the next warning.
};

IFR_END_NAMESPACE

#endif //IFR_ERRORHNDL_H
