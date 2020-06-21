/*!
  @file    Msg_SimpleArg.hpp
  @author  GertG
  @ingroup Common
  @brief   General purpose Msg_I(Opt)Arg implementation
  
  The class implements a general purpose Tag+Value class derived from Msg_IOptArg.

  The value string have to be specified as a simple 'const SAPDB_UTF8 *'
  
  The Tag and the value may be modified afterwards by using the SetTag() and SetValue()
  member functions if tag or values changed or are not known when Msg_OptArg is constructed.

  -------------------------------------------------------------------------

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
*****************************************************************************/

#ifndef MSG_SIMPLEARG_HPP
#define MSG_SIMPLEARG_HPP

#include "Messages/Msg_IOptArg.hpp"
#include "Messages/Msg_IArg.hpp"


/*!
  @class Msg_SimpleOptArg
  @brief Helper class to create optional argument from plain data buffer on stack
 */
class Msg_SimpleOptArg : public Msg_IOptArg
{
public:
    /// ctor
    Msg_SimpleOptArg()
        : m_Tag(0), m_Value(0)
    {}

    /*!
      @brief ctor
      @param tag [in]
      @param value [in]
     */
    Msg_SimpleOptArg(SAPDB_Char const *tag, SAPDB_UTF8 const *value)
        : m_Tag(tag), m_Value(value)
    {}

    /*!
      @brief get tag
      @return tag
     */
    virtual SAPDB_Char const *Tag() const { return m_Tag; }
    /*!
      @brief get value
      @return value
     */
    virtual SAPDB_UTF8 const *Value() const { return m_Value; }
    /*!
      @brief get argument
      @return pointer to Msg_IOptArg const *
     */
    virtual operator Msg_IOptArg const * () const { return this; }
    
    SAPDB_Char const *m_Tag; ///< the tag
    SAPDB_UTF8 const *m_Value; ///< the tagged value
};

/*!
  @class Msg_SimpleArg
  @brief Helper class to create argument array from plain data buffer on stack
 */
class Msg_SimpleArg : public Msg_IArg
{
public:
    /// ctor
    Msg_SimpleArg()
        : m_MsgArg()
    {}

    /*!
      @brief ctor
      @param tag [in]
      @param value [in]
     */
    Msg_SimpleArg(SAPDB_Char const *tag, SAPDB_UTF8 const *value)
        : m_MsgArg(tag, value)
    {}

    /*!
      @brief get tag
      @return tag
     */
    virtual SAPDB_Char const *Tag() const { return m_MsgArg.Tag(); }
    /*!
      @brief get value
      @return value
     */
    virtual SAPDB_UTF8 const *Value() const { return m_MsgArg.Value(); }
    /*!
      @brief cast operator
      @return pointer to Msg_IArg const *
     */
    virtual operator Msg_IArg const * () const { return this; }
    /*!
      @brief get argument
      @return pointer to Msg_IOptArg const *
     */
    virtual Msg_IOptArg const * GetArg() const { return &m_MsgArg; }
    
    /*!
      @brief get reference to tag
      @return reference to tag pointer
     */
    SAPDB_Char const * &TagRef() { return m_MsgArg.m_Tag; }
    /*!
      @brief get reference to value
      @return reference to tagged value
     */
    SAPDB_UTF8 const * &ValueRef() { return m_MsgArg.m_Value; }

    Msg_SimpleOptArg m_MsgArg; ///< message argument
};

#endif // MSG_SIMPLEARG_HPP
