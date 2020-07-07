/*!
    \file    ToolsCStr_Str.hpp
    \author  TiloH
    \ingroup infrastructure for tools group
    \brief   some useful functions combining C Strings with message lists

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#if !defined(TOOLSCSTR_STR_HPP)
#define TOOLSCSTR_STR_HPP

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

/*! \brief a class supplying a bunch of functions for handling classic C
    strings as static functions */
class ToolsCStr_Str
{
  public:
    /*! \brief allocate space for a zero terminated string using global operator
            new. If parameter dest is not 0, operator delete is used to free
            the memory it is pointing to. Space for a terminating zero is added
            automatically.
        \param dest [IN/OUT] pointer to previous character string and resultimg string
        \param length [IN] space needed for the new string, the byte for the
            terminating zero is added automatically
        \param error [out] message list for error cases
        \return true if successful, false otherwise */
    static bool Alloc(
        char           *& dest,
        size_t            length,
        DBMSrvMsg_Error & error);

    /*! \brief create a copy of a zero terminated string, including the terminating zero
            using global operator new. If parameter dest is not 0, operator delete
            is used to free the memory it is pointing to.
        \param dest [IN/OUT] pointer to previous character string and resulting string
        \param src  [IN] pointer to string to copy
        \param error [out] message list for error cases
        \return true if successful, false otherwise */
    static bool AllocCpy(
        char           *& dest,
        const char      * src,
        DBMSrvMsg_Error & error);

    /*! \brief create a copy of a part of a string. A terminating zero is added automatically
            to the result. Global operator new and delete are used for memory handling.
            If parameter dest is not 0, operator delete is used to free the memory it is
            pointing to.

        \param dest [IN/OUT] pointer to previous character string and resulting string
        \param src  [IN] pointer to string to copy
        \param length [IN] number of char to copy (excluding a terminating zero)
        \param error [out] message list for error cases
        \return true if successful, false otherwise */
    static bool NAllocCpy(
        char           *& dest,
        const char      * src,
        size_t            length,
        DBMSrvMsg_Error & error);

    /*! @brief free the memory that dest points to using delete, if dest is not 0. Set dest
            to point to 0.

        @param dest [IN/OUT] pointer to character string.
    */
    static void Dealloc(
        char *& dest);

    /*! \brief create a copy of a zero terminated string without leading or trailing
            spaces using global operator new. If parameter dest is not 0, operator delete
            is used to free the memory it is pointing to. The resulting string is zero
            terminated

        \param dest [IN/OUT] pointer to previous character string and resulting string
        \param src  [IN] pointer to string to copy
        \param error [out] message list for error cases
        \return true if successful, false otherwise */
    static bool AllocCpyWithoutLeadingTrailingSpaces(
        char           *& dest,
        const char      * src,
        DBMSrvMsg_Error & error);

    /*! \brief copy a part of a string and add a terminating zero to the result

        \param dest [IN] string to copy to
        \param src  [IN] string to copy from
        \param length [IN] number of charcters to copy (excluding the terminating zero) */
    static void NCpy(char * dest, const char * src, size_t length);

    /*! \brief function for converting an string into a signed integer. The function fails,
            if the string cannot be converted into a signed integer of type int.

        \param integer [IN] the resulting integer
        \param buf [IN] the string representing a integer
        \param error [OUT] message list for errors
        \return true if buf can be converted into a int, false otherwise */
    static bool ToInt(int & integer, const char * buf, DBMSrvMsg_Error & error);

    /*! \brief function for converting a string into a unsigned 64-bit integer. The
            function fails, if the string cannot be converted into a unsigned
            64-bit integer.

        \param unsignedInteger8 [IN] the resulting integer
        \param buf [IN] the string representing a integer
        \param error [OUT] message list for errors
        \return true if buf can be converted into a unsigned 64-bit integer, false otherwise */
    static bool ToUInt8(teo00_Uint8 & unsignedInteger8, const char * buf, DBMSrvMsg_Error & error);
};

#endif
