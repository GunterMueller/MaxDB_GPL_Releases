/*!================================================================

  module:       vpr09Config.h

 -------------------------------------------------------------------

  responsible:  BurkhardD

  special area: Handling Configuration (ini) file entrys


  description : Handling of '.ini' files on UNIX and registry on NT.

     This module based on Registry for UNIX written by Joerg Mensing.
     It appends ven09.c functionalty to do the same on NT but use there 
     the registry instead of files.

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

#ifndef __VPR09_CONFIG_H__
#define __VPR09_CONFIG_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gsp00.h"
#include "RunTime/RTE_IniFileHandling.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

#ifdef __cplusplus
extern "C" {                         /* Assume C declarations for C++ */
#endif  /* __cplusplus */

typedef enum ConfigResult_tpr09 
{ 
  ERR_OK_epr09, 
  ERR_NOT_OK_epr09, 
  ERR_NO_MORE_DATA_epr09,
  ERR_USER_RUNTIME_ENTRY_epr09,
  ERR_FOREIGN_USER_RUNTIME_ENTRY_epr09
} ConfigResult_tpr09;

typedef struct RTE_RegistryHandleStruct_ *ConfigHandle_tpr09;

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/
/*!
  Function:     pr09ConfigPutString ()

  description:  Writes or updates a Config Entry
  
   Inserts or replaces a string into the runtime registry file
   to a position descripted by szSection and 'szEntry'-name.
 
  arguments:
    szSection [in] Given section
    szEntry [in] Given key of 'key=value' entry
    szString [in] Given value of 'key=value' entry
    ErrText [out] Place for descriptive error message

  return value: ConfigResult_tpr09
    <UL>
        <LI>ERR_OK_epr090: ok
        <LI>ERR_NOT_OK_epr09: inspect ErrText
    </UL>
 */
ConfigResult_tpr09 pr09ConfigPutRuntimeString (
             const char *szSection,
             const char *szEntry, 
		     const char *szString,
		     tsp00_ErrTextc ErrText);

ConfigResult_tpr09 pr09ConfigPutRuntimeStringEx (
             const char *szSection,
		     const char *szEntry, 
		     const char *szString,
		     tsp00_ErrTextc ErrText);

/*!
  Function:     pr09ConfigGetRuntimeString ()

  description:  Reads the Stringvalue from the Runtime Config Entry
  
    Gives back a string from runtime registry in runtime section descripted by
    'szEntry'. It fails if either the file, the section or the
    entry does not exist.

  arguments:
    szSection [in] Given section
    szEntry [in] Given key of 'key=value' entry
    szString [out] Place to store the value of 'key=value' entry
    MaxStringLen [in] Maximum value buffer size
    ErrText [out] Place for descriptive error message

  return value: ConfigResult_tpr09
    <UL>
        <LI>ERR_OK_epr090: ok
        <LI>ERR_NOT_OK_epr09: inspect ErrText
    </UL>
 */
ConfigResult_tpr09 pr09ConfigGetRuntimeString (
             const char *szSection,
             const char * szEntry,
		     char * szString, 
		     const int  MaxStringLen, 
		     tsp00_ErrTextc ErrText);

ConfigResult_tpr09 pr09ConfigGetRuntimeStringEx (
             const char *szSection,
             const char * szEntry,
		     char * szString, 
		     const int  MaxStringLen, 
		     tsp00_ErrTextc ErrText);

/*!
  Function:     pr09ConfigRemoveEntry ()

  description:  Remove a Configentry from Registry
                
    Removes the entry specified 'szEntry' from runtime config file 

  arguments:
    szEntry [in] Given key of 'key=value' entry
    ErrText [out] Place for descriptive error message

  return value: ConfigResult_tpr09
    <UL>
        <LI>ERR_OK_epr090: ok
        <LI>ERR_NOT_OK_epr09: inspect ErrText
    </UL>
 */

ConfigResult_tpr09 pr09ConfigRemoveRuntimeEntry (
               const char *szSection,
               const char * szEntry,
		       tsp00_ErrTextc ErrText);

/*!
  Function:     pr09ConfigOpenRuntimeSection ()

  description:  Open the runtime registry for enumeration of the entrys
                
    Open the registery and position to the first entry of the given section.
    It is expected that a loop over 'pr09ConfigNextEntry()' is used to retrieve the
    entries.

    You must call pr09ConfigCloseSection after last usage of pr09ConfigNextEntry in all
    cases.
  
  arguments:
    szSection [in] Given section
    ErrText [out] Place for descriptive error message

  return value:
    <UL>
        handle  used in subsequent pr09ConfigNextEntry calls or NULL if an error occured
    </UL>
 */
ConfigHandle_tpr09 pr09ConfigOpenRuntimeSection (
                const char *szSection,
			    tsp00_ErrTextc ErrText);

/*!
  Function:     pr09ConfigNextEntry ()

  description:  Reads next entry from registry opened by pr09ConfigOpenSection ()
                
   Reads next 'szEntry' and 'szString' from section of config file opened by 
   pr09ConfigOpenSection (). If caller like, he can pass NULL pointer, which make
   it possible ie. just to count the entries of a section...

   Gives back 'ERR_NO_MORE_DATA_pr09' in return on end of section or end of file.
  
  arguments:
    hConfig [in] Handle of registry file
    pLocation [out] Location of registry information
    szEntry [out] Place to store to key of 'key=value' entry (NULL means dont store)
    MaxEntryLen [in] Maximum key buffer size
    szString [out] Place to store the value of 'key=value' entry (NULL means dont store)
    MaxStringLen [in] Maximum value buffer size
    ErrText [out] Place for descriptive error message

  return value: ConfigResult_tpr09
    <UL>
        <LI>ERR_OK_epr090: ok
        <LI>ERR_NO_MORE_DATA_pr09: no more Entrys in this section
        <LI>ERR_NOT_OK_epr09: inspect ErrText
    </UL>
 */
ConfigResult_tpr09 pr09ConfigNextRuntimeEntry (
              ConfigHandle_tpr09 hConfig,
              RTE_RegistryLocation *pLocation,
			  char *szEntry, 
			  const int MaxEntryLen, 
			  char *szString, 
			  const int MaxStringLen, 
			  tsp00_ErrTextc ErrText);

/*!
  Function:     pr09ConfigCloseSection ()

  description:  Closes the registry section opened by pr09ConfigOpenSection ()
                
    Close the registry. This invalidates the given handle.
    Subsequent calls to pr09ConfigNextEntry() must fail.
  
  arguments:
    hConfig [in] Handle of registry file
    ErrText [out] Place for descriptive error message

  return value: ConfigResult_tpr09
    <UL>
        <LI>ERR_OK_epr090: ok
        <LI>ERR_NOT_OK_epr09: inspect ErrText
    </UL>
 */
ConfigResult_tpr09 pr09ConfigCloseRuntimeSection (
             ConfigHandle_tpr09 hConfig, 
		     tsp00_ErrTextc ErrText);

#ifdef __cplusplus
}                                    /* End of extern "C" { */
#endif  /* __cplusplus */
#endif /* __VPR09_CONFIG__ */
