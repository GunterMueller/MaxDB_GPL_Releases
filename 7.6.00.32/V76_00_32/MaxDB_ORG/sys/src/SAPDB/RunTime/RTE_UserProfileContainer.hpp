/*!========================================================================

  @file         RTE_UserProfileContainer.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        This container class is used to handle the storage of
                user related profile data.

                Profile data has arbitrary length, but can
                be identified by a userid/SERVERDB combination.
                Each profile consists of length + bytestream.
                The profile data is only accessable via this interface.
                This is acting not on a ASCII-file....
                SERVERDB is used to identify a container file on per
                database base.

  @since        2004-08-30  14:51
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

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

  \endif
 ============================================================================*/


#ifndef RTE_USERPROFILECONTAINER_HPP
#define RTE_USERPROFILECONTAINER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "RunTime/RTE_Types.h"
#include "RunTime/RTE_MessageList.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define FIRST_TOKEN      0
#define LAST_TOKEN       ((SAPDB_UInt4)-1)
#define NO_TOKEN         ((SAPDB_UInt4)-2)


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  @class   RTE_ProfileContainer
  @brief   Forward declaration
*/
class RTE_ProfileContainer;


/*!---------------------------------------------------------------------------
  @class   RTE_UserProfileContainer
  @brief   This container class is used to handle the storage of
           user related profile data.

           Profile data has arbitrary length, but can
           be identified by a userid/SERVERDB combination.
           Each profile consists of length + bytestream.
           The profile data is only accessable via this interface.
           This is acting not on a ASCII-file....
           SERVERDB is used to identify a container file on per
           database base.
*/
class RTE_UserProfileContainer
{
public:
  /*!
     @brief          Constructor
  */
  RTE_UserProfileContainer();

  /*!
     @brief          Destructor

                     This flushes the profiles cached and frees memory.

     @return         none
   */

  ~RTE_UserProfileContainer();

  /*!
     @brief          Open container
                     
                     Opens the user profile container for subsequent operations.

     @param          serverDB        [in] - SERVERDB name used to find container image
     @param          readOnly        [in] - Open profile container in read only mode.
     @param          messageList    [out] - Error message, if any
     @return         true if ok, else false
   */
  bool Open( tsp00_DbNamec const     serverDB,
             bool                    readOnly,
             Msg_List                &messageList );

  /*!
     @brief          Open container
                     
                     Opens the user profile container for subsequent operations.

     @param          serverDB        [in] - SERVERDB name used to find container image
     @param          messageList    [out] - Error message, if any
     @return         true if ok, else false
   */
  bool Open( tsp00_DbNamec const     serverDB,
             Msg_List                &messageList );

  /*!
     @brief          Delete profile container.

                     Removes the profile container with all its profiles.

     @param          messageList    [out] - Error message, if any
     @return         true if deleted or not existing else false
   */
  bool DeleteContainer( Msg_List                &messageList );

  /*!
     @brief          Close profile container.

                     Close the profile container 

     @param          messageList    [out] - Error message, if any
     @return         true if closed else false
   */

  bool CloseContainer( Msg_List                &messageList );

  /*!
     @brief          Retrieve owner name of Profile Container

                     Uses Id of Profile Container creator to return Username of owner

     @param          username       [out] - The returned owner name of the profile container
     @param          messageList    [out] - Error message, if any
     @return         true if Username retrieved else false
   */

  bool GetOwnerName( tsp00_Namec             &username,
                     Msg_List                &messageList );

  /*!
     @brief          Retrieve group name of Profile Container (UNIX only)

                     Uses Id of Profile Container group to return Groupname of container

     @param          groupname      [out] - The group name associated with the container
     @param          messageList    [out] - Error message, if any
     @return         true if Groupname retrieved else false
   */
  bool GetGroupName( tsp00_Namec             &groupname,
                     Msg_List                &messageList );

  /*!
     @brief          Set group name of Profile Container (UNIX only)

                     Uses Id of Profile Container group to assign Groupname to container

     @param          groupname      [out] - The group name associated with the container
     @param          messageList    [out] - Error message, if any
     @return         true if Groupname assigned else false
   */
  bool SetGroupName( tsp00_Namec const       &groupname,
                     Msg_List                &messageList );


  /*!
     @brief          Set (Add or Update) a profile in ProfileContainer.

                     All given data is copied.

     @param          userName        [in] - The Username to add
     @param          length          [in] - The length of the profile data
     @param          pProfile        [in] - The pointer to the profile data
     @param          messageList    [out] - Error message, if any
     @return         true if added else false
   */

  bool SetProfile( tsp00_Namec const       userName,
                   SAPDB_Int4  const       length,
                   void * const            pProfile,
                   Msg_List                &messageList );

  /*!
     @brief          Delete a specific profile from ProfileContainer

     @param          userName        [in] - The Username connected with profile
     @param          messageList    [out] - Error message, if any
     @return         true if deleted or not existing else false
   */

  bool DelProfile( tsp00_Namec const       userName, 
                   Msg_List                &messageList );
  /*!
     @brief          Get a specific profile from ProfileContainer

                     In case of error (returning false) check length 
                     to make sure not only the buffer size was insufficient.

     @param          userName        [in] - The Username connected with profile
     @param          length       [inout] - On Input length of pBuffer given, on
                                            output always the length of the
                                            profile_data.
     @param          pProfile        [in] - The pointer to the caller supplied buffer
                                            for profile data. If NULL caller is not
                                            interested in profile data.
     @param          messageList    [out] - Error message, if any
     @return         true if deleted or not existing else false
   */

  bool GetProfile( tsp00_Namec const       userName, 
                   SAPDB_Int4              &length,
                   void * const            pProfile,
                   Msg_List                &messageList );

  /*!
     @brief          Scan ProfileContainer and return next profile

                     Using a token, which must be set to FIRST_TOKEN
                     to get the first profile, all profiles are returned
                     in order they are found by repeated calls to this function.
                     In such case reuse token.
                     Any delete or insert operation ('DelProfile' or
                     'SetProfile') during repeated calls to 'ScanProfiles'
                     will reset the token to FIRST_TOKEN.
                     If the last profile was found, the token will be set to LAST_TOKEN.
                     If no profile was found, the token will be set to NO_TOKEN.
                     To get only the username and profile length without the profile 
                     data, set pBuffer input parameter to NULL.
                     WARNING -- CHECKING THE RETURN CODE IS MANDATORY -- WARNING
                     In case of error ( methode is returning false) check the length 
                     parameter to make sure the buffer size was sufficient.
                     In case of insufficient buffer space you can reuse the token, but
                     you will end up in an endless loop if you do not check the return code!


     @param          token        [inout] - Enumeration token, updated in case of
                                            success, otherwise untouched!
                                            Start with FIRST_TOKEN.
                                            LAST_TOKEN signals that
                                            the last profile was found.
     @param          length       [inout] - On Input length of pBuffer given, on
                                            output always the length of the
                                            profile_data.
     @param          pProfile        [in] - The pointer to the caller supplied buffer
                                            for profile_data. If NULL caller is not
                                            interested in profile data.
     @param          userName       [out] - The Username of the entry found
     @param          messageList    [out] - Error message, if any
     @return         true if deleted or not existing else false
   */

  bool ScanProfiles( SAPDB_UInt4            &token,
                     SAPDB_Int4             &length,
                     void * const           pProfile,
                     tsp00_Namec            &userName,
                     Msg_List               &messageList );

private:
  RTE_ProfileContainer*         m_pContainer;
  bool                          m_Open;
};


#endif  /* RTE_USERPROFILECONTAINER_HPP */
