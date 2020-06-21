/*!
  @file    ToolsMeMa_GuardedOperation.hpp
  @author  MarcW
  @brief   guarded memory manipulation operations

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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


*/

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Messages/Msg_List.hpp"

#if !defined(ToolsMeMa_GuardedOperation_hpp)
#define ToolsMeMa_GuardedOperation_hpp

class ToolsMeMa_GuardedOperation {
public:
    static void setComponent(const char* aComponent);
    static bool errorFound();
    static const Msg_List& getErrorList();

    static void* MemCopy(
                void* destination,
                const void* source,
                SAPDB_Long  lengthInBytes,
                SAPDB_Char  const    *fileName,
                SAPDB_UInt4 const     lineNumber);

    static void* MemMove(
                void* destination,
                const void* source,
                SAPDB_Long lengthInBytes,
                SAPDB_Char const* fileName,
                SAPDB_UInt4 const lineNumber);

    static void* MemFill(
                void* destination,
                SAPDB_Byte fillByte, 
                SAPDB_Long lengthInBytes,
                SAPDB_Char const* fileName,
                SAPDB_UInt4 const lineNumber);

private:
    static const char* m_Component;
    static Msg_List m_TotalList;
    static Msg_List m_CurrentList;
};

inline void ToolsMeMa_GuardedOperation::setComponent(const char* aComponent) {
    m_Component = aComponent;
}

inline bool ToolsMeMa_GuardedOperation::errorFound() {
    return !m_TotalList.IsEmpty();
}

inline const Msg_List& ToolsMeMa_GuardedOperation::getErrorList() {
    return m_TotalList;
}

inline void* ToolsMeMa_GuardedOperation::MemCopy(
                void* destination,
                const void *source,
                SAPDB_Long  lengthInBytes,
                SAPDB_Char  const    *fileName,
                SAPDB_UInt4 const     lineNumber) {

    m_CurrentList.ClearMessageList();
    void* outVal = SAPDB_MemCopy(
                        destination,
                        source,
                        lengthInBytes,
                        m_Component,
                        fileName,
                        lineNumber,
                        m_CurrentList);
    if( !m_CurrentList.IsEmpty() )
        m_TotalList.Overrule(m_CurrentList);
    return outVal;
}

inline void* ToolsMeMa_GuardedOperation::MemMove(
                void* destination,
                const void* source,
                SAPDB_Long lengthInBytes,
                SAPDB_Char const* fileName,
                SAPDB_UInt4 const lineNumber) {

    m_CurrentList.ClearMessageList();
    void* outVal = SAPDB_MemMove(
                        destination,
                        source,
                        lengthInBytes,
                        m_Component,
                        fileName,
                        lineNumber,
                        m_CurrentList);
    if( !m_CurrentList.IsEmpty() )
        m_TotalList.Overrule(m_CurrentList);
    return outVal;
}

inline void* ToolsMeMa_GuardedOperation::MemFill(
                void* destination,
                SAPDB_Byte fillByte, 
                SAPDB_Long lengthInBytes,
                SAPDB_Char const* fileName,
                SAPDB_UInt4 const lineNumber) {

    m_CurrentList.ClearMessageList();
    void* outVal = SAPDB_MemFill(
                        destination,
                        fillByte,
                        lengthInBytes,
                        m_Component,
                        fileName,
                        lineNumber,
                        m_CurrentList);
    if( !m_CurrentList.IsEmpty() )
        m_TotalList.Overrule(m_CurrentList);
    return outVal;
}

#endif //ToolsMeMa_GuardedOperation_hpp
