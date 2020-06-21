/*****************************************************************************/
/*!
  @file         Msg_Ucs2Arg.hpp
  @author       MartinKi
  @ingroup      Common

  @brief        Header file for class Msg_Ucs2Arg.

\if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2005 SAP AG

  This program is free software; you can redisytribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
\endif
*/
/*****************************************************************************/

#ifndef MSG_UCS2ARG_HPP
#define MSG_UCS2ARG_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Messages/Msg_IArg.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class Msg_IOptArg;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
 * @brief Helper class to ease handling of types that are encoded in UCS2.
 *
 * The template argument ToString must be a class that provides the
 * following method:
 *
 * \verbatim
 * void ToUtf8(SAPDB_UTF8* buffer, SAPDB_UInt bufferSize) const
 * \endverbatim
 *
 * This method is called with a buffer whose bufferSize is determined
 * by the size of an object of class ToString. It is assumed that
 * ToString's size is the size of a buffer that contains UCS2 encoded
 * data. bufferSize will then be about 1.5*sizeof( ToString ).
 *
 * \sa SQLMan_Identifier
 */
template<class ToString>
class Msg_Ucs2Arg : public Msg_IArg
{
private:
    // maximum size of an UTF8 character representation is 3 bytes
    // -> buffer size must be 3/2 * UCS2 buffer size + 1 byte for null byte
    SAPDB_UTF8 m_buffer[
        static_cast<SAPDB_UInt>( sizeof( ToString ) * 3 / 2 + 1) + 1];
    Msg_OptArg m_optArg;

public:
    /*!
      @brief Constructor.
      @param tag [in] tag name C-string
      @param argObj [in] argument object
     */
    Msg_Ucs2Arg(
        SAPDB_Char const *tag,
        const ToString&   argObj);

    virtual SAPDB_Char const *Tag() const;

    virtual SAPDB_UTF8 const *Value() const;

    virtual operator Msg_IArg const * () const
    {
        return this;
    }

    virtual Msg_IOptArg const *GetArg() const;
};

//-------------------------------------------------------------------------

template<class ToString>
Msg_Ucs2Arg<ToString>::Msg_Ucs2Arg(
    SAPDB_Char const *tag,
    const ToString&   argObj)
    : m_optArg( tag )
{
    argObj.ToUtf8( m_buffer, sizeof( m_buffer ) );
    m_buffer[ sizeof( ToString ) ] = '\0';
    m_optArg.SetValue( m_buffer );
}

//-------------------------------------------------------------------------

template<class ToString>
SAPDB_Char const *
Msg_Ucs2Arg<ToString>::Tag() const
{
    return m_optArg.Tag();
}

//-------------------------------------------------------------------------

template<class ToString>
SAPDB_UTF8 const *
Msg_Ucs2Arg<ToString>::Value() const
{
    return m_optArg.Value();
}

//-------------------------------------------------------------------------

template<class ToString>
Msg_IOptArg const *
Msg_Ucs2Arg<ToString>::GetArg() const
{
    return &m_optArg;
}

//-------------------------------------------------------------------------

#endif // MSG_UCS2ARG_HPP
