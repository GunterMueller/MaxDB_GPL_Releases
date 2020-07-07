/*!
    @file           DBMSrvShM_LineLayoutScheduler.hpp
    @author         MarcW
    @brief          class for line layout for the one and only line of type Scheduler

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutBase.hpp"

#if !defined(DBMSrvShM_LineLayoutScheduler_HPP)
#define DBMSrvShM_LineLayoutScheduler_HPP

/*!
    @brief layout definition for the scheduler

    Communication between DBM server and scheduler is done here (one of them is the
    recipient). One byte (infotype) describes, what kind of data is in the variable
    part.
*/
class DBMSrvShM_LineLayoutScheduler: public DBMSrvShM_LineLayoutBase {
private:
    DBMSrvShM_SaveType<SAPDB_Byte> m_Recipient;
    DBMSrvShM_SaveType<SAPDB_Byte> m_InfoType;
    // what is actually in variable part is determined my m_InfoType
    DBMSrvShM_SaveType<size_t> m_LenUserName;
    DBMSrvShM_SaveType<size_t> m_LenVariablePart;

public:
    void setInitialSize() {
        m_LineSize.setValue(sizeof(*this));
    }

    void setVariablePart( const char* newContent );
    SAPDB_Byte* getVariablePart() const;
    size_t getVariablePartLen() const;

    void setUserName(const char* aUserName);
    SAPDB_Byte* getUserName() const;
    size_t getUserNameLen() const;

    void setRecipient(const SAPDB_Byte aRecipient) {m_Recipient.setValue(aRecipient);}
    SAPDB_Byte getRecipient() const;

    void setInfoType(const SAPDB_Byte aInfoType) {m_InfoType.setValue(aInfoType);}
    SAPDB_Byte getInfoType() const;

    DBMSrvShM_LineLayoutScheduler* getNext() const;
};

inline SAPDB_Byte DBMSrvShM_LineLayoutScheduler::getRecipient() const {
    SAPDB_Byte rec;
    m_Recipient.getValue(rec);
    return rec;
}

inline SAPDB_Byte DBMSrvShM_LineLayoutScheduler::getInfoType() const {
    SAPDB_Byte inf;
    m_InfoType.getValue(inf);
    return inf;
}

#endif //DBMSrvShM_LineLayoutScheduler_HPP
