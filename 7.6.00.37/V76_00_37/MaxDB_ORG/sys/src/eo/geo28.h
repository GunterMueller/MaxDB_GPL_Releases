/*!
  @file           geo28.h
  @author         RaymondR
  @special area   Profile Container Class
  @brief          This container class is used to handle the storage of
              profile data.
  @see            http://pwww/Tools/Autorisierung-Ergebnis981013.html


                Profile data has arbitrary length, but can
                be identified by a userid/SERVERDB combination.
                Each profile consists of length + bytestream.
                The profile data is only accessable via this interface.
                This is acting not on a ASCII-file....
                SERVERDB is used to identify a container file on per
                database base.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "geo200.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define FIRST_TOKEN_EO28      0
#define LAST_TOKEN_EO28       ((tsp00_Uint4)-1)
#define NO_TOKEN_EO28         ((tsp00_Uint4)-2)

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*
  Class: t_FileContainer
 */
class t_FileContainer;

/*!
  @class          teo28_ProfileContainer

 */

class teo28_ProfileContainer
{
public:
  /*!
     @brief          Constructor
     @param          ServerDB [in] - SERVERDB name used to find container image
     @param          ReadOnly [in] - Open profile container in read only mode.
     @param          pEventList [inout] - The event list to be used,                                 - if NULL throw EventList on error
     @return         none, but setting *pEventList on error.


                   This allocates internal resources.

   */

  teo28_ProfileContainer( tsp00_DbNamec const ServerDB,
                          bool                ReadOnly,
                          teo200_EventList *  pEventList = NULL );

  teo28_ProfileContainer( tsp00_DbNamec const ServerDB,
                          teo200_EventList *  pEventList = NULL );

  /*!
     @brief          Destructor
     @return         none


                   This flushes the profiles cached and frees memory.

   */

  ~teo28_ProfileContainer();

  /*!
     @brief          Delete profile container.
     @param          pEventList [inout] - The event list to be used,                                     if NULL throw EventList on error
     @return         true if deleted or not existing
              false else (setting *pEventList)


                   Removes the profile container with all its profiles.

   */

  bool eo28_DeleteProfileContainer( teo200_EventList *    pEventList = NULL );

  /*!
     @brief          Close profile container.
     @param          pEventList [inout] - The event list to be used,                                     if NULL throw EventList on error
     @return         true if closed
              false else (setting *pEventList)


                   close the profile container 

   */

  bool eo28_CloseProfileContainer( teo200_EventList *    pEventList = NULL );

  /*!
     @brief          Retrieve owner name of Profile Container
     @param          Username [out] - The returned owner name of the profile container
     @param          pEventList [inout] - The event list to be used,                                     if NULL throw EventList on error
     @return         true if Username retrieved
              false else (setting *pEventList)


                   Uses Id of Profile Container creator to return Username of owner

   */

  bool eo28_GetOwnerName( tsp00_Namec &Username,
                          teo200_EventList *    pEventList = NULL );

  /*!
     @brief          Retrieve group name of Profile Container (UNIX only)
     @param          Groupname [out] - The group name associated with the container
     @param          pEventList [inout] - The event list to be used,                                     if NULL throw EventList on error
     @return         true if Groupname retrieved
              false else (setting *pEventList)

       Uses Id of Profile Container group to return Groupname of container

   */
  bool eo28_GetGroupName( tsp00_Namec &Groupname,
                          teo200_EventList *    pEventList = NULL );

  /*!
     @brief          Set group name of Profile Container (UNIX only)
     @param          Groupname [out] - The group name associated with the container
     @param          pEventList [inout] - The event list to be used,                                     if NULL throw EventList on error
     @return         true if Groupname assigned
              false else (setting *pEventList)

       Uses Id of Profile Container group to assign Groupname to container

   */
  bool eo28_SetGroupName( tsp00_Namec const & Groupname,
                          teo200_EventList *    pEventList = NULL );


  /*!
     @brief          Set (Add or Update) a profile in ProfileContainer.
     @param          UserName [in] - The Username to add
     @param          Length [in] - The length of the profile data
     @param          pProfile [in] - The pointer to the profile data
     @param          pEventList [inout] - The event list to be used,                                     if NULL throw EventList on error
     @return         true if added
              false else (setting *pEventList)


                  All given data is copied.

   */

  bool eo28_SetProfile( tsp00_Namec const     UserName,
                        tsp00_Int4  const     Length,
                        void * const          pProfile,
                        teo200_EventList *    pEventList = NULL );

  /*!
     @brief          Delete a specific profile from ProfileContainer
     @param          UserName [in] - The Username connected with profile
     @param          pEventList [inout] - The event list to be used,                                  if NULL throw EventList on error
     @return         true if deleted or not existing
              false else (setting *pEventList)

   */

  bool eo28_DelProfile( tsp00_Namec const      UserName, 
                        teo200_EventList *     pEventList = NULL );
  /*!
     @brief          Get a specific profile from ProfileContainer
     @param          UserName [in] - The Username connected with profile
     @param          Length [inout] - On Input length of pBuffer given, on                                  output always the length of the                                   profile_data.
     @param          pProfile [in] - The pointer to the caller supplied buffer                                  for profile data. If NULL caller is not                                  interested in profile data.
     @param          pEventList [inout] - The event list to be used,                                  if NULL throw EventList on error
     @return         true if deleted or not existing
              false else (setting *pEventList)


                  In case of error (returning false) check length 
                  to make sure not only the buffer size was insufficient.

   */

  bool eo28_GetProfile( tsp00_Namec const      UserName, 
                        tsp00_Int4             &Length,
                        void * const           pProfile,
                        teo200_EventList *     pEventList = NULL );

  /*!
     @brief          Scan ProfileContainer and return next profile
     @param          token [inout] - Enumeration token, updated in case of                                  success, otherwise untouched!                                  Start with FIRST_TOKEN_EO28.                                  LAST_TOKEN_EO28 signals that                                   the last profile was found.
     @param          Length [inout] - On Input length of pBuffer given, on                                  output always the length of the                                   profile_data.
     @param          pProfile [in] - The pointer to the caller supplied buffer                                  for profile_data. If NULL caller is not                                  interested in profile data.
     @param          UserName [out] - The Username of the entry found
     @param          pEventList [out] - The event list to be used,                                  if NULL throw EventList on error
     @return         true if deleted or not existing
              false else (setting *pEventList)


                  Using a token, which must be set to FIRST_TOKEN_EO28
                  to get the first profile, all profiles are returned
                  in order they are found by repeated calls to this function.
                  In such case reuse token.
                  Any delete or insert operation ('eo28_DelProfile' or
                  'eo28_SetProfile') during repeated calls to 'eo28_ScanProfiles'
                  will reset the token to FIRST_TOKEN_EO28.
                  If the last profile was found, the token will be set to 
                  LAST_TOKEN_EO28.
                  If no profile was found, the token will be set to 
                  NO_TOKEN_EO28.
                  To get only the username and profile length without the profile 
                  data, set pBuffer input parameter to NULL.
                  WARNING -- CHECKING THE RETURN CODE IS MANDATORY -- WARNING
                  In case of error (returning false) check length 
                  to make sure not only the buffer size was insufficient.
                  In such a case you can reuse token, but you will end up in
                  an endless loop if you do not check the return code...

   */

  bool eo28_ScanProfiles( tsp00_Uint4         &Token,
                          tsp00_Int4          &Length,
                          void * const        pProfile,
                          tsp00_Namec         &UserName,
                          teo200_EventList *  pEventList = NULL );

private:
  t_FileContainer    *m_pContainer;
  bool               m_Open;
};


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/