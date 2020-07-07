/*!
  @file           DBMSrvProc_EnvironmentVariable.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          change environment of a process

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

#ifndef _DBMSrvProc_EnvironmentVariable_HPP_
#define _DBMSrvProc_EnvironmentVariable_HPP_

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

/*!
  @brief     change environment of a process

             This class changes and resets environment variables.
             
             The class uses functions from vcn36.cpp, so you have to link this
             module and the dependencies too. Don't care vcn36.cpp exists in the cn layer
             but does not contain DBMServer specific code 

 */
class DBMSrvProc_EnvironmentVariable {

public:
    /*! @brief type of manipulation 
        Parameters or return values of this type describe the requested or theh ececuted 
        manipulation.
     */
    typedef enum {
        ManipulateAdd,           //! add the environment variable only if not exist
        ManipulateReplace,       //! replace the environment variable only if exist
        ManipulateAppend,        //! append the value separated by a separator to the environment variable only if exist
        ManipulatePrepend,       //! prepend the value separated by a separator to the environment variable only if exist
        ManipulateReplaceOrAdd,  //! replace if exist or add if not exist
        ManipulateAppendOrAdd,   //! append if exist or add if not exist
        ManipulatePrependOrAdd,  //! prepend if exist or add if not exist
        ManipulateRemove,        //! remove if exists or do nothing if not exist
        ManipulateNothing,       //! do nothing
        ManipulateError          //! a error occured
    } ManipulateType;

    /*! @brief constructor

        The construtor manipulate the given environment variable in the requested manner.
        The action done will be stored in the object and will be used in he Reset()
        member function.
        
        @param sName      [IN]  - name of environment variable
     */
    DBMSrvProc_EnvironmentVariable
        ( const char     * sName );

    /*! @brief destructor

        The destructor does not reset any changes done in the environment with a 
        object of this class. Only explizit call to the member function Reset() 
        will reset changes done by Manipulate().
     */
    ~DBMSrvProc_EnvironmentVariable
        (  );

    /*! @brief function to manipulate this enviroment variable

        During a call to Manipulate() the object stores information for the Reset() member
        function. Existing information for Reset() from the last call to Manipulate will
        be overwritten. 

        @param sValue     [IN]  - value for the environment variable
        @param sSeparator [IN]  - separator for modes append or prepend (like ':' or ';' for paths)
        @param oRequested [IN]  - requested mode of manipulation
        @param oMessage   [OUT] - contains informations about error after calling the function.

        @return manipulation done
     */
    ManipulateType Manipulate
        ( const char      * sValue,
          const char      * sSeparator,
          ManipulateType    oRequested,
          DBMSrvMsg_Error & oMessage);

    /*! @brief reset function

        This functions resets the changes done by the last (and only the last!) call to Manipulate(). 

        @param oMessage   [OUT] - contains informations about error after calling the function.
        @return manipulation done
     */
    ManipulateType Reset
        ( DBMSrvMsg_Error & oMessage );

    /*! @brief read current value

        This function returns the current value of the enviromnet variable. This value maybe a null pointer.
        The value is a copy of the original value from the environment provided in memory allocated by this 
        class. Therefore the pointer becomes invalid after destruction of the object.
     */
    const char * CurrentValue
        ( );

private:

    /*! @brief set value function
     */
    ManipulateType SetNewValue
        ( const char      * sName,
          const char      * sValue,
          const char      * sSeparator,
          const char      * sOldValue,
          ManipulateType    oRequested,
          DBMSrvMsg_Error & oMessage) const;

    char * m_sName;
    char * m_sOldValue;
    char * m_sCurrentValue;
    ManipulateType m_oDone;

}; // end class DBMSrvProc_EnvironmentVariable


#endif /* _DBMSrvProc_EnvironmentVariable_HPP_ */
