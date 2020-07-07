/*!
  @file           heo78.h
  @author         JoergM
  @special area   DCOM Registration for UNIX
  @brief          Registration Functions for Class implementation and Kernel.
  @see            

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



#ifndef HEO78_H
#define HEO78_H

/*! @name Convention */
 /*@(*/
/*@)*/

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define PROG_ID_PFX_MXEO78 32
#define PROJECT_MXEO78     32 
#define VERSION_MXEO78     12
/*
 * Programmatic Identifier  consists of the three items above plus
 * two delimiters (dots) replacing the 0 bytes... This is the maximum
 * approach...
 * Ferdinand Flohe mentioned a necessary size limitation for AS/400.
 */
#ifdef AS400
#define PROG_ID_MXEO78 20
#else
#define PROG_ID_MXEO78 \
	(PROG_ID_PFX_MXEO78 + PROJECT_MXEO78 + VERSION_MXEO78)
#endif
#define CLSID_MXEO78       39
#define MODULE_NAME_MXEO78 256

/*===========================================================================*
 *  TYPES                                                                    *
 *===========================================================================*/

typedef char teo78_ProgIDPfx[PROG_ID_PFX_MXEO78];
typedef char teo78_Project[PROJECT_MXEO78];
typedef char teo78_Version[VERSION_MXEO78];
typedef char teo78_ProgID[PROG_ID_MXEO78];
typedef char teo78_ClsID[CLSID_MXEO78];
typedef char teo78_ModuleName[MODULE_NAME_MXEO78];
typedef enum teo78_Result {
	success_eo78,
  illegal_parameter_eo78,
	access_denied_eo78,
	doubled_clsid_eo78,
	entry_not_found_eo78,
	clsid_version_mismatch_eo78,
  system_error_eo78
} teo78_Result;

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

/* none */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*! @name Registration Functions */
 /*@(*/
/*!
   @brief          This call registers a specified Com Object
   @param          ProgIDPfx [in] Project Prefix, i.e. "LiveCache"
   @param          Project [in] The Project name identifying the Class
   @param          Version [in] The Version
   @param          ClsID [in] Class Identifier
   @param          ModuleName [in] Name of DLL containing the Class implementation
   @return         The code returned
      &lt;UL>
              &lt;LI>success_eo78:           Success: Entry was already in registry
              &lt;LI>entry_not_found_eo78:   Success: Entry added to registry
              &lt;LI>illegal_parameter_eo78: Input parameter check failed
              &lt;LI>access_denied_eo78:     Failed to access registry
              &lt;LI>doubled_clsid_eo78:     CLSID already used for other registered project
      &lt;/UL>


     It is possible to register more then one version of the com object.
         In such a case the last registration wins! That means the last 
         registered version overwrites the version independent programmatic
         identifier entry. CLSID must be unique in the system.

 */

extern "C" teo78_Result sqlComObjectRegister( 
						      teo78_ProgIDPfx   ProgIDPfx,
						      teo78_Project     Project,
						      teo78_Version     Version,
						      teo78_ClsID       ClsID,
						      teo78_ModuleName  ModuleName );

/*!
   @brief          This call unregisters a specified Com Object.
   @param          ProgIDPfx [in] Project Prefix, i.e. "LiveCache"
   @param          Project [in] The Project name identifying the Class
   @param          Version [in] The Version
   @param          ClsID [in] Class Identifier
   @return         The code returned
      &lt;UL>
              &lt;LI>success_eo78:           Success
              &lt;LI>illegal_parameter_eo78: Input parameter check failed
              &lt;LI>access_denied_eo78:     Failed to access registry (i.e. dones not exist)
              &lt;LI>clsid_version_mismatch_eo78: CLSID/ProjectVersion mismatch
              &lt;LI>entry_not_found_eo78:   Project not found
      &lt;/UL>


     The information registered has to be removed. In the special case, 
         that the current version is removed, there is no other version
         select as current version, but no version is current version.
         Check is done for matching version/ClsID combination. No deletion
         is done if mismatch found.

 */

extern "C" teo78_Result sqlComObjectUnregister( 
						      teo78_ProgIDPfx ProgIDPfx,
						      teo78_Project   Project,
						      teo78_Version   Version,
						      teo78_ClsID     ClsID );

/*!
   @brief          This call retrieves a module name and ClsID usign given programmatic ID.
   @param          ProgID [in] Programmatic Identifier
   @param          ClsID [out] Filled with ClsID found
   @param          ModuleName [out] Filled with ModuleName found
   @return         The code returned
      &lt;UL>
              &lt;LI>success_eo78:           Success
              &lt;LI>illegal_parameter_eo78: Input parameter check failed
              &lt;LI>access_denied_eo78:     Failed to access registry (i.e. dones not exist)
              &lt;LI>entry_not_found_eo78:   Programmatic Identifier not found
      &lt;/UL>


     This call is used as helper function for the DCOM implementation.

 */

extern "C" teo78_Result sqlComObjectGet(
						      teo78_ProgID     ProgID,
						      teo78_ClsID      ClsID,
						      teo78_ModuleName ModuleName);

/*!
   @brief          This call retrieves a module name using given ClsID
   @param          ClsID [in] Class Identifier
   @param          ModuleName [out] Filled with ModuleName found
   @return         The code returned
      &lt;UL>
              &lt;LI>success_eo78:           Success
              &lt;LI>illegal_parameter_eo78: Input parameter check failed
              &lt;LI>access_denied_eo78:     Failed to access registry (i.e. dones not exist)
          &lt;LI>invalid_clsid_eo78:     ClsID not found
      &lt;/UL>


     This call is used as helper function for the DCOM implementation.
     Used by Kernel or Proxy server only.

 */

extern "C" teo78_Result sqlComObjectGetByClsID(
						      teo78_ClsID      ClsID,
						      teo78_ModuleName ModuleName );

/*@)*/
#endif  /* HEO78_H */
