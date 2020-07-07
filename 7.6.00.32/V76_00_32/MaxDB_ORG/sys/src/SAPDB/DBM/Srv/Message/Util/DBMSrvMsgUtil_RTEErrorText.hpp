/*!
    \file    DBMSrvMsgUtil_RTEError.hpp
    \author  MarcW
    \ingroup messages
    \brief   class for handling RTE errors
    
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

#if !defined(DBMSrvMsgUtil_RTEErrorText_hpp)
#define DBMSrvMsgUtil_RTEErrorText_hpp

#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "ToolsCommon/CString/ToolsCStr_Str.hpp"
#include "gsp00_2.h"

/*! @brief to be used in constructor of DBMSrvMsg_RTEError... */
template<const int arraySize>
class DBMSrvMsgUtil_RTEErrorText {
public:
    DBMSrvMsgUtil_RTEErrorText(const tsp00_OldPascalString<arraySize>& errText);
    DBMSrvMsgUtil_RTEErrorText(const tsp00_CString<arraySize>& errText);
    /*! @brief strlen(errText) must be <= arraySize */
    DBMSrvMsgUtil_RTEErrorText(int osErr, const char* errText);
    /*! @brief arraySize is irrelevant when using this constructor, so use default */
    DBMSrvMsgUtil_RTEErrorText( Msg_List& errList );

    ~DBMSrvMsgUtil_RTEErrorText();

    operator const char* () const;

private:
    enum ReturnString {
        NOMEM,
        NOTEXT,
        FIXBUFFER,
        DYNBUFFER
    };

    const int m_ArraySize;
    ReturnString m_ReturnString;
    tsp00_CString<arraySize> m_FixBuffer;
    char* m_DynBuffer;

    static const char* const m_XErrorText;
};

template<const int arraySize>
inline DBMSrvMsgUtil_RTEErrorText<arraySize>::~DBMSrvMsgUtil_RTEErrorText() {
    if( m_ReturnString == DYNBUFFER )
        ToolsCStr_Str::Dealloc(m_DynBuffer);
}

template<const int arraySize>
inline DBMSrvMsgUtil_RTEErrorText<arraySize>::DBMSrvMsgUtil_RTEErrorText(const tsp00_OldPascalString<arraySize>& errText)
    : m_ReturnString(FIXBUFFER),
        m_DynBuffer(NULL),
        m_ArraySize(arraySize) {
    m_FixBuffer.p2c(errText);
}

template<const int arraySize>
inline DBMSrvMsgUtil_RTEErrorText<arraySize>::DBMSrvMsgUtil_RTEErrorText(const tsp00_CString<arraySize>& errText)
    : m_ReturnString(FIXBUFFER),
        m_DynBuffer(NULL),
        m_ArraySize(arraySize) {
    m_FixBuffer.rawAssign(errText);
}

template<const int arraySize>
inline DBMSrvMsgUtil_RTEErrorText<arraySize>::DBMSrvMsgUtil_RTEErrorText(int osErr, const char* errText)
    : m_ReturnString(NOMEM),
        m_DynBuffer(NULL),
        m_ArraySize(arraySize) {
    
    m_FixBuffer.rawAssign(errText);
    char osErrString[50];
    sprintf(osErrString, "%d", osErr);
    DBMSrvMsg_Error internalError;
    if( ToolsCStr_Str::Alloc(m_DynBuffer,
            (size_t)(strlen(m_XErrorText) +m_ArraySize-2 +strlen(osErrString)-2 +1),
            internalError) ) {
        sprintf(m_DynBuffer, m_XErrorText, m_FixBuffer.asCharp(), osErr);
        m_ReturnString = DYNBUFFER;
    }
    else
        m_ReturnString = NOMEM;
}

template<const int arraySize>
inline DBMSrvMsgUtil_RTEErrorText<arraySize>::DBMSrvMsgUtil_RTEErrorText( Msg_List& errList )
    : m_ReturnString(NOMEM),
        m_DynBuffer(NULL),
        m_ArraySize(arraySize) {

    if( errList.IsEmpty() )
        m_ReturnString = NOTEXT;
    else {
        DBMSrvMsg_Error internalError;
        SAPDB_UInt4 requiredSpaceLen(0);

        errList.MessageWithInsertedArguments( 0, m_DynBuffer, requiredSpaceLen, false);
        if( ToolsCStr_Str::Alloc(m_DynBuffer, (size_t)(requiredSpaceLen+1), internalError) ) {
            errList.MessageWithInsertedArguments(
                requiredSpaceLen,
                m_DynBuffer,
                requiredSpaceLen,
                false);
            m_ReturnString = DYNBUFFER;
        }
        else
            m_ReturnString = NOMEM;
    }
}

template<const int arraySize>
inline DBMSrvMsgUtil_RTEErrorText<arraySize>::operator const char* () const {
    switch(m_ReturnString) {
        case FIXBUFFER:
            return m_FixBuffer.asCharp();
            break;
        case DYNBUFFER:
            return m_DynBuffer;
            break;
        case NOTEXT:
            return "no_rteerrortext";
            break;
        case NOMEM:
            return "no_memory_for_rteerrorid";
            break;
        default:
            return "impossible";
    }
}

template<const int arraySize>
const char* const DBMSrvMsgUtil_RTEErrorText<arraySize>::m_XErrorText = "%s (OS error code %d)";

#endif //DBMSrvMsgUtil_RTEErrorText_hpp
