/*!================================================================

  module:       vpr09Config.h

 -------------------------------------------------------------------

  responsible:  BurkhardD

  special area: Handling Configuration (ini) file entrys

  description:                 

  see also:

  -------------------------------------------------------------------------





    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




 ===================================================================*/

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include <stdio.h>
#include "vpr09Config.h"
#include "SAPDBCommon/SAPDB_Names.h"

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

ConfigResult_tpr09  pr09ConfigPutRuntimeString (
             const char *szSection,
		     const char *szEntry,
             const char *szString,
		     tsp00_ErrTextc ErrText)
{
    RTE_IniFileResult Ok;
    RTE_PutUserConfigString (NULL, SAPDB_RUNTIMES_INI_FILE, szSection, szEntry, szString, ErrText, &Ok);
    return ( SAPDB_INIFILE_RESULT_OK == Ok ? ERR_OK_epr09 : ERR_NOT_OK_epr09 );
}

ConfigResult_tpr09 pr09ConfigPutRuntimeStringEx (
                      const char *szSection,
					  const char *szEntry,
                      const char *szString,
					  tsp00_ErrTextc ErrText)
{
    return pr09ConfigPutRuntimeString(szSection, szEntry, szString, ErrText);
}

ConfigResult_tpr09 pr09ConfigGetRuntimeString (
             const char * szSection,
		     const char * szEntry,
		     char       * szString,
		     const int    MaxStringLen,
		     tsp00_ErrTextc ErrText)
{
    RTE_IniFileResult Ok;
    RTE_GetConfigString( SAPDB_RUNTIMES_INI_FILE, szSection, szEntry, szString, MaxStringLen, ErrText, &Ok);
    return ( SAPDB_INIFILE_RESULT_OK == Ok ? ERR_OK_epr09 : ERR_NOT_OK_epr09 );
}

ConfigResult_tpr09 pr09ConfigGetRuntimeStringEx (
                      const char * szSection,
					  const char * szEntry,
					  char       * szString,
					  const int    MaxStringLen,
					  tsp00_ErrTextc ErrText)
{
    return pr09ConfigGetRuntimeString(szSection, szEntry, szString, MaxStringLen, ErrText);
}

ConfigResult_tpr09 pr09ConfigRemoveRuntimeEntry(
    const char * szSection,                                   
    const char * szEntry,
    tsp00_ErrTextc ErrText)
{
  RTE_IniFileResult Ok;
  RTE_RemoveUserConfigString (NULL, SAPDB_RUNTIMES_INI_FILE, szSection, szEntry, ErrText, &Ok);
  return ( SAPDB_INIFILE_RESULT_OK == Ok ? ERR_OK_epr09 : ERR_NOT_OK_epr09 );
}

ConfigHandle_tpr09 pr09ConfigOpenRuntimeSection ( const char *szSection, 
                                                  tsp00_ErrTextc ErrText )
{
  RTE_IniFileResult Ok;
  ConfigHandle_tpr09 hConfig;

  if ( !szSection || !*szSection )
  {
      szSection = SAPDB_RUNTIME_SECTION;
  }

  hConfig = RTE_OpenConfigEnum(SAPDB_RUNTIMES_INI_FILE, szSection, ErrText, &Ok);
  if (Ok == SAPDB_INIFILE_RESULT_OK)
  {
      return hConfig;
  }
  return NULL;
}

ConfigResult_tpr09 pr09ConfigCloseRuntimeSection (ConfigHandle_tpr09 hConfig, tsp00_ErrTextc ErrText)
{
    RTE_IniFileResult Ok;
    RTE_CloseConfigEnum(hConfig, ErrText, &Ok);
    return ( Ok == SAPDB_INIFILE_RESULT_OK ? ERR_OK_epr09 : ERR_NOT_OK_epr09 );
}

ConfigResult_tpr09 pr09ConfigNextRuntimeEntry (
    ConfigHandle_tpr09 hConfig,
    RTE_RegistryLocation *pLocation,
    char *szEntry,
    const int MaxEntryLen,
    char *szString,
    const int MaxStringLen,
    tsp00_ErrTextc ErrText)
{
    RTE_IniFileResult Ok;
    int rc = RTE_NextConfigEnum(hConfig, szEntry, MaxEntryLen, szString, MaxStringLen, pLocation, ErrText, &Ok);
    if (!rc) 
    {
        if (Ok == SAPDB_INIFILE_RESULT_EOF )
        {
            return ERR_NO_MORE_DATA_epr09;
        }
        else if (Ok == SAPDB_INIFILE_RESULT_FILE_NOT_FOUND)
        {/* We have found a user SID key */
            return ERR_USER_RUNTIME_ENTRY_epr09;
        }
        else if (Ok == SAPDB_INIFILE_RESULT_ACCESS_DENIED)
        {/* We have found a SID key of a foreign user */
            return ERR_FOREIGN_USER_RUNTIME_ENTRY_epr09;
        }
        else
        {
            return ERR_NOT_OK_epr09;
        }
    }


    return ERR_OK_epr09;
}

