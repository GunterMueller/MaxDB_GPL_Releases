/*!
  \file    Loader_ErrorMsgHandler.hpp
  \author  SteffenS
  \brief   Defines the main Loader.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

#ifndef LOADER_ERRORMSGHANDLER_HPP
#define LOADER_ERRORMSGHANDLER_HPP

//#include "Loader/Loader_Loader.hpp"
//#include "Loader/Loader_Types.hpp"
#include "Loader/Loader_Common.hpp"
#include "Loader/Loader_ErrorCode.hpp"


class Loader_ErrorMsgHandler
{
public:
    // Constructor; no error is set
    Loader_ErrorMsgHandler()
        : m_ErrorCode(0)
    {
        m_ErrorText[0] = 0;
    }
    
    // Destructor
    ~Loader_ErrorMsgHandler()
    {
        // do nothing so far
    }


    inline LOADER_Bool isError()
    {
        return m_ErrorCode != LOA_ERR_OK;
    }

    /**
     * Get the error code.
     */
    inline const LOADER_Int4 getErrorCode() const
    {
        return m_ErrorCode;
    }


    /**
     * Get the error message. Note that this is an UTF8 string.
     */
    const LOADER_Char* getErrorText() const
    {
        return m_ErrorText[0] == 0 ? ""
                                   : m_ErrorText;
    }


    inline const LOADER_Int4 getSQLErrorCode() const
    {
        return m_SQLErrorCode;
    }


    /**
     * Conversion operator. Returns true if the error is set.
     * This operator makes it possible to write constructs like
     * <code>if (error) { ...</code>.
     */
    inline operator LOADER_Bool () const 
    {
        return m_ErrorCode != LOA_ERR_OK;
    }



    /*!
    \brief Builds error string

    \param   lErrorCode  [IN]  - error code

    \return  None
    */
    void setLoaderError(const LOADER_ErrorCode lErrorCode, ...);


    void setLoaderErrorWSQL(const LOADER_ErrorCode lErrorCode,
                            const LOADER_Int4       lSQLError,
                            const LOADER_Char*      pszSQLErrorText,
                            ...);

    /*!
    \brief Insert new error string at the beginning of an existing error
           string (thus keeping history)

    \param   lErrorCode  [IN]  - error to insert

    \return  None
    */
    void insertLoaderError(const LOADER_ErrorCode lErrorCode, ...);

    inline void clearError()
    {
        m_ErrorCode         = LOA_ERR_OK;
        m_ErrorText[0]      = 0;
        m_SQLErrorCode      = 0;
        m_SQLErrorText[0]   = 0;
    }

//    void setLoaderError(const LOADER_ErrorCode lErrorCode);

private:
    LOADER_Char* getMessageText(const LOADER_ErrorCode lErrorCode);
    void formatErrorText(const LOADER_ErrorCode lErrorCode, va_list arglist);

private:


    // Member variables
    LOADER_Int4      m_ErrorCode;
    LOADER_Char      m_ErrorText[LOA_MAX_REPLY_LENGTH];


    LOADER_Int4      m_SQLErrorCode;
    LOADER_Char      m_SQLErrorText[LOA_MAX_REPLY_LENGTH];
};

#endif  // LOADER_ERRORMSGHANDLER_HPP