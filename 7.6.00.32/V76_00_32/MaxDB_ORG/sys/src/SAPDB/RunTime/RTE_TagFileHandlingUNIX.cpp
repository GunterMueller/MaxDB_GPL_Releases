/*!========================================================================

  @file         RTE_TagFileHandlingUNIX.cpp
  @author       StefanP
  @ingroup      Runtime
  @brief        Functions handling the Tag Files in '/usr/spool/sql/...'

  @since        2003-10-10  19:11
  @sa           

  ==========================================================================

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
 ============================================================================*/


#if defined(_WIN32)

/* UNIX only */

#else

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/RTE_saveUNIXcalls.h"
#include    "RunTime/RTE_TagFileHandlingUNIX.hpp"
#include    "SAPDBCommon/SAPDB_sprintf.h"
#include    "RunTime/RTE_IniFileHandling.h"
#include    "SAPDBCommon/SAPDB_Names.h"
#include    "RunTime/RTE_Messages.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


extern SAPDBTrace_Topic Runtime_Trace; ///<Trace Runtime


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/
/*!
     @brief			Check if the given path exists. If not it is tried to create the 
					directory.
                     
     @param         path [in]				- Full path name
     @param         messageList [out]		- Message list

     @return        true: Path exists or could be created
					false: Path does not exist or could not be created
 */

static	SAPDB_Bool    RTE_CheckDir       (RTE_Path const               path,
                                          SAPDBErr_MessageList &       messageList);


/*===========================================================================*
  *  METHODS                                                                  *
 *===========================================================================*/
/*
void    RTE_GetConfigDir 
( 
    RTE_Path                    path,
    SAPDB_Bool                  terminateWithDelimiter
)
{
    RTE_Path dataPath;
    RTE_IniFileErrtext ErrText;

    SAPDBTRACE_ROUTINE_DEBUG("RTE_GetConfigDir", Runtime_Trace, 1);

    if ( RTE_GetCommonDataPath ( dataPath,
                                 terminateWithDelimiter,
                                 ErrText ) )
    {
        strcpy((SAPDB_Char *)path, &dataPath[0]);
    }
    else
    {
    	strcpy ((SAPDB_Char *)path, SAPDB_OLD_GLOBAL_DIRECTORY);
        if (!terminateWithDelimiter)
        {
            memset (path + strlen (path) -1 , 0, 1);
        }
    }

	strcpy ((SAPDB_Char *)path, RTE_IPCDirectory);
}
*/
/*---------------------------------------------------------------------------*/

 /*!
     @brief			Build the path name of the IPC directory
                     
     @param         path [out]						- Full path name of the IPC directory
     @param         terminateWithDelimiter [in]		- Terminate with delimeter ('/') or not (true/false)
     @param         messageList [out]				- Message list 

     @return        true/false
 */

SAPDB_Bool   RTE_GetIPCDir 
( 
    RTE_Path &                  path,
    SAPDB_Bool                  terminateWithDelimiter,
    SAPDBErr_MessageList &      messageList
)
{
    tsp00_ErrTextc ErrText;

    SAPDBTRACE_ROUTINE_DEBUG("RTE_GetIPCDir", Runtime_Trace, 1);

    if (!RTE_GetCommonDataPath (path,
                                true,
                                ErrText ))
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_GET_COMMON_DATA_PATH,
                                            ErrText.asCharp ());
        return false;
    }

	strcat (path, SAPDB_SPOOL_IPC_DIRECTORY);
    if (terminateWithDelimiter)
    {
        strcat (path, "/");
    }

    return true;
}

/*---------------------------------------------------------------------------*/
/*
void    RTE_GetSQLDir 
( 
    RTE_Path                    path
)
{
    SAPDBTRACE_ROUTINE_DEBUG("RTE_GetSQLDir", Runtime_Trace, 1);

    RTE_GetSpoolDir (path);
	strcat ((SAPDB_Char *)path, SAPDB_SPOOL_SQL_DIRECTORY "/" );
}
*/
/*---------------------------------------------------------------------------*/

 /*!
     @brief			Remove a specific tag file
                     
     @param         userType [in]			- User type
     @param         serverDB [in]			- Name of the server DB
     @param         resourceType [in]		- Resource type
     @param         tagID [in]				- ID of the tag file
     @param         messageList [out]		- Message list

     @return        true/false
 */

SAPDB_Bool    RTE_RemoveIPCFile 
( 
    RTE_UserType const              userType,
    RTE_DBName const                serverDB,
    RTE_ResourceType const          resourceType,
    RTE_Path const                  tagID,
    SAPDBErr_MessageList &          messageList
)
{
    RTE_Path        file;

    SAPDBTRACE_ROUTINE_DEBUG("RTE_RemoveTagFile", Runtime_Trace, 1);
    
	if (!RTE_GetIPCFileName (file, userType, serverDB, resourceType, tagID, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_GET_FILENAME,
                                                          SAPDB_ToString (userType),
                                                          SAPDB_ToString (resourceType),
                                                          SAPDB_ToString (serverDB),
                                                          tagID);
        return false;
    }
    
	if (RTE_SYSTEMRC_NO_ERROR != RTE_save_unlink (file))
	{
       if (ENOENT != errno)
       {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_UNLINK_FILE,
                                                              file,
                                                              RTE_save_strerror (errno));
            return false;
       }
	}

	return true;
}

/*---------------------------------------------------------------------------*/
/*!
     @brief			Build tag filename from given paramters (including the full path)
                     
     @param         path [out]				- Tag filename including the entire path
     @param         userType [in]			- User type
     @param         serverDB [in]			- Name of the server DB
     @param         resourceType [in]		- Resource type
     @param         tagID [in]				- ID of the tag file (e.g. special name or handle (converted to string))
     @param         messageList [out]		- Message list

     @return        true/false
 */

SAPDB_Bool    RTE_GetIPCFileName 
( 
    RTE_Path &                      path,
    RTE_UserType const              userType,
    RTE_DBName const                serverDB,
    RTE_ResourceType const          resourceType,
    RTE_Path const                  tagID,
    SAPDBErr_MessageList &          messageList
)
{
    SAPDB_Char *     pUserTypePrefix;
    SAPDB_Char *     resourceTypePrefix;

    SAPDBTRACE_ROUTINE_DEBUG("RTE_GetTagFileName", Runtime_Trace, 1);

    switch (userType)
    {
    case ClientUser:
        pUserTypePrefix = (SAPDB_Char *)SAPDB_USER_DIRECTORY_PREFIX;
        break;

    case KernelUser:
        pUserTypePrefix = (SAPDB_Char *)SAPDB_DATABASE_DIRECTORY_PREFIX;
        break;

    default:
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_UNKNOWN_USER_TYPE,
                                            SAPDB_ToString (userType));
        return false;
    }

    switch (resourceType)
    {
    case Semaphore:
        resourceTypePrefix = (SAPDB_Char *) SAPDB_SEMAPHORE_FILE_PREFIX;
        break;

    case SharedMemory:
        resourceTypePrefix = (SAPDB_Char *) SAPDB_SHARED_MEMORY_FILE_PREFIX;
        break;

    case Mailslot:
        resourceTypePrefix = (SAPDB_Char *) SAPDB_MAILSLOT_FILE_PREFIX;
        break;

    default:
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_UNKNOWN_RESOURCE_TYPE,
                                            SAPDB_ToString (resourceType));
        return false;
    }

    if (!RTE_GetIPCDir (path, true, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_GET_IPC_DIRECTORY);
        return false;
    }

    SAPDB_sprintf (path + strlen (path), sizeof (path) - strlen (path), "%s%s/", pUserTypePrefix, serverDB);
    if (!RTE_CheckDir (path, messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_CHECK_IPC_DIRECTORY);
        return false;
    }

    SAPDB_sprintf (path + strlen (path), sizeof (path) - strlen (path), "%s%s", resourceTypePrefix, tagID);

    return true;
}


/*****************************************************************************/
/* STATIC part ***************************************************************/
/*****************************************************************************/

static	SAPDB_Bool    RTE_CheckDir
( 
    RTE_Path const              path,
    SAPDBErr_MessageList &      messageList
)
{
    RTE_SystemRc	rc = RTE_SYSTEMRC_NO_ERROR;
    struct stat		statbuf ;

    SAPDBTRACE_ROUTINE_DEBUG("RTE_CheckDir", Runtime_Trace, 1);

	rc = RTE_save_stat (path, &statbuf);
	if (RTE_SYSTEMRC_NO_ERROR != rc)
	{
        mode_t umask_old ;
		if (ENOENT != errno)
		{
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_MISSING_DIRECTOY,
                                                path,
                                                RTE_save_strerror (errno));
            return false;
		}
        else
        {
            /* For IPC directorys '0777' is needed to allow all users to use it */
            umask_old = umask (0);
            rc = mkdir (path, 0771);
            umask(umask_old);
            if (RTE_SYSTEMRC_NO_ERROR != rc)
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_MAKE_DIRECTORY,
                                                    path,
                                                    RTE_save_strerror (errno));
                return false;
            }
        }
	}
	else
	{
		if ( (statbuf.st_mode & S_IFMT) != S_IFDIR )
		{
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_TAG_MISSING_DIRECTOY, path,
                                                "File with the same name exists");
            return false;
		}
	}

	return true;
}

#endif /* UNIX only */
