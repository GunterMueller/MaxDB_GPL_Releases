/*!
  \file    DBMSrvUsr_Password.hpp
  \author  BerndV
  \ingroup DBMServer User area
  \brief   declaration of class which handle passwords

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2005 SAP AG

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

#if !defined(DBMSrvUsr_Password_hpp)
#define DBMSrvUsr_Password_hpp

#include "gsp00.h"

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

/*! @brief A class which handles password in different formats
 */
class DBMSrvUsr_Password
{
public:
    //! Format of the password
    typedef enum {
        UnknownString,          //!< The format of the password is unknown.
        ClearString,            //!< The format of the password is a clear zero terminated string.
        CryptedString,          //!< The format of the password is a crypted zero terminated string.
        UnknownStringWithUser,  //!< The syntax is "<usr>,<pwd>". The format of the password is unknown.
        ClearStringWithUser,    //!< The syntax is "<usr>,<pwd>". The format of the password is a clear zero terminated string.
        CryptedStringWithUser,  //!< The syntax is "<usr>,<pwd>". The format of the password is a crypted zero terminated string.
        CryptedBufferDB         //!< The password is a binary buffer with database encryption
    } StringType;

    /*! @brief Constructor
     */
    DBMSrvUsr_Password
        ( );

    /*! @brief Constructor
        @param pPassword [IN] - password as zero terminated string
        @param oType     [IN] - type of password representation
     */
    DBMSrvUsr_Password
        ( const void * pPassword,
          StringType   oType = UnknownString );

    /*! @brief setPassword
        @param pPassword [IN] - password as zero terminated string
        @param oType     [IN] - type of password representation
     */
    void setPassword
        ( const void * pPassword,
          StringType   oType = UnknownString );

    /*! @brief asClearString
        @return Returns pointer to a internal buffer with the clear string representation. This
                Buffer becoms invalid if the object will be destroyed.
     */
    const char * asClearString() const;

    /*! @brief getUser
        @return Returns pointer to a internal buffer with the user name. If there is no username 
                available this will return an empty string. Buffer becoms invalid if the object 
                will be destroyed.
     */
    const char * getUser() const;

    /*! @brief isValid
        @return Return true if there was read a valid password or username/password combination from the token
     */
    bool isValid() const;

private:
    void setUserAndPassword
        ( const char * pUserPassword,
          StringType   oType );

    bool isHexDigit
        ( char c );

    void setClearStringPassword
        ( const char * pPassword );
    
    void setCryptStringPassword
        ( const char * pPassword );

    void setUnknownStringPassword
        ( const char * pPassword );

    void setCryptedBufferDBPassword
        ( const void * pPassword );

    tsp00_Namec m_szPassword;
    tsp00_Namec m_szUser;

}; // end class DBMSrvUsr_Password

#endif // DBMSrvUsr_Password_hpp
