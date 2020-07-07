/*!---------------------------------------------------------------------
  @file           RTEConf_ParameterAccessKernelInterface.h
  @author         JoergM, RobinW
  @brief          C/Pascal interface for class RTEConf_ParameterAccess (class for kernel configurable parameter access)

  first created  2000-03-22  17:49

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
---------------------------------------------------------------------*/


#ifndef CONF_PARAMETER_ACCESS_H
#define CONF_PARAMETER_ACCESS_H

#include "vsp0011.h"
#include "geo002.h"

/*!
   @brief           get the value of an integer parameter as 4-byte integer from the internal parameter storage
   @param           conf_param_name     [in]    parameter name reference
   @param           conf_param_value    [out]   value reference as 4-byte integer
   @param           errtext             [out]   error text if unsuccessful
   @param           conf_param_ret      [out]   return value as defined in vsp0011.h

 */
externC void vconf_param_get_int4      ( tsp11_ConfParamName        VAR_ARRAY_REF conf_param_name,
                                         tsp00_Int4                 VAR_VALUE_REF conf_param_value,
                                         boolean                    VAR_VALUE_REF update_online,
                                         boolean                    VAR_VALUE_REF permanent_change,
                                         tsp00_ErrText              VAR_ARRAY_REF errtext,
                                         tsp11_ConfParamReturnValue VAR_VALUE_REF conf_param_ret );

/*!
   @brief           get the value of an integer parameter as 8-byte integer from the internal parameter storage
   @param           conf_param_name     [in]    parameter name reference
   @param           conf_param_value    [out]   value reference as 8-byte integer
   @param           errtext             [out]   error text if unsuccessful
   @param           conf_param_ret      [out]   return value as defined in vsp0011.h

 */
externC void vconf_param_get_integer   ( tsp11_ConfParamName        VAR_ARRAY_REF conf_param_name,
                                         tsp00_Int8                 VAR_VALUE_REF conf_param_value,
                                         boolean                    VAR_VALUE_REF update_online,
                                         boolean                    VAR_VALUE_REF permanent_change,
                                         tsp00_ErrText              VAR_ARRAY_REF errtext,
                                         tsp11_ConfParamReturnValue VAR_VALUE_REF conf_param_ret );

/*!
   @brief           get the value of a real parameter as longreal from the internal parameter storage
   @param           conf_param_name     [in]    parameter name reference
   @param           conf_param_value    [out]   value reference as longreal
   @param           errtext             [out]   error text if unsuccessful
   @param           conf_param_ret      [out]   return value as defined in vsp0011.h

 */
externC void vconf_param_get_real      ( tsp11_ConfParamName        VAR_ARRAY_REF conf_param_name,
                                         tsp00_Longreal             VAR_VALUE_REF conf_param_value,
                                         boolean                    VAR_VALUE_REF update_online,
                                         boolean                    VAR_VALUE_REF permanent_change,
                                         tsp00_ErrText              VAR_ARRAY_REF errtext,
                                         tsp11_ConfParamReturnValue VAR_VALUE_REF conf_param_ret );

/*!
   @brief           get the value of a string parameter from the internal parameter storage
   @param           conf_param_name     [in]    parameter name reference
   @param           conf_param_value    [out]   value reference as string
   @param           conf_param_length   [out]   length of the string
   @param           errtext             [out]   error text if unsuccessful
   @param           conf_param_ret      [out]   return value as defined in vsp0011.h

 */
externC void vconf_param_get_string    ( tsp11_ConfParamName        VAR_ARRAY_REF conf_param_name,
                                         tsp11_ConfParamValue       VAR_ARRAY_REF conf_param_value,
                                         tsp00_Int4                 VAR_VALUE_REF conf_param_length,
                                         boolean                    VAR_VALUE_REF update_online,
                                         boolean                    VAR_VALUE_REF permanent_change,
                                         tsp00_ErrText              VAR_ARRAY_REF errtext,
                                         tsp11_ConfParamReturnValue VAR_VALUE_REF conf_param_ret );

/*!
   @brief           get the value of a cryptinfo parameter from the internal parameter storage
   @param           conf_param_name     [in]    parameter name reference
   @param           conf_param_value    [out]   value reference as crypt info
   @param           errtext             [out]   error text if unsuccessful
   @param           conf_param_ret      [out]   return value as defined in vsp0011.h

 */
externC void vconf_param_get_cryptinfo ( tsp11_ConfParamName        VAR_ARRAY_REF conf_param_name,
                                         tsp00_CryptName            VAR_ARRAY_REF conf_param_value,
                                         boolean                    VAR_VALUE_REF update_online,
                                         boolean                    VAR_VALUE_REF permanent_change,
                                         tsp00_ErrText              VAR_ARRAY_REF errtext,
                                         tsp11_ConfParamReturnValue VAR_VALUE_REF conf_param_ret );

/*!
   @brief           put the value of a parameter into the internal parameter storage
   @param           conf_param_name     [in]    parameter name reference
   @param           conf_param_value    [in]    value reference as string,numeric
                                                parameters are given in their ASCII representation,
   @param           is_numeric          [in]    flag to indicate a numeric parameter
   @param           permanent_change    [in]    flag to indicate if the change is to be permanent
   @param           errtext             [out]   error text if unsuccessful
   @param           conf_param_ret      [out]   return value as defined in vsp0011.h

 */
externC void vconf_param_put           ( tsp11_ConfParamName        VAR_ARRAY_REF conf_param_name,
                                         tsp11_ConfParamValue       VAR_ARRAY_REF conf_param_value,
                                         boolean                                  is_numeric,
                                         boolean                                  permanent_change,
                                         tsp11_ConfParamReturnValue VAR_VALUE_REF conf_param_ret );
 
/*!
   @brief           Search given volume in configured volume lists and return its file system access type
   @param           volumeName          [in]    The pascal style volume name
   @param           fsType              [out]   file system access type
   @return value    true if volume found, false if not
 */
externC SAPDB_Bool RTEConf_GetVolumeType( tsp00_VFilename VAR_ARRAY_REF volumeName,
                                          RTE_FileSystemAccessType VAR_VALUE_REF fsType );

                                           
/*!
   @brief           Read the contents of a parameter file and put it into a given XParam structure according to 
                    internal tables. Error messages are reported via RTE_Message() !
                    This routine should only be called from modules that cannot handle Msg_Lists.
                    C++ modules should call RTEConf_ReadAndAnalyzeParam() and handle the Msg_List
                    returned!
   @param           ServerDB            [in]    Name of the database
   @param           XParam              [in]    Pointer of the XParam structure to put the parameters read into
   @return value    true if reading and analyzing parameters succeeded, false if not
 */
externC SAPDB_Bool RTEConf_ReadAndAnalyzeParamWithOutput ( tsp00_DbNamec VAR_ARRAY_REF    ServerDB,
                                                           RTE_XPARAM_REC   *XParam );

/*!
   @brief           Remove the (dynamically allocated) entries for task cluster 1..n from the xparam structure. To be 
                    called when the task cluster parameters have been parsed. Do not try to access the task cluster entries in the 
                    xparam structure after this function has been called!
   @param           XParam              [in]    Pointer of the XParam structure to remove the task cluster entries from
 */
externC void RTEConf_FreeTaskClusterArrayInXParamStructure( RTE_XPARAM_REC *XParam);

/*!
   @brief           Remove the (dynamically allocated) entries for a given devspace/volume type from the xparam structure.
                    To be called when the xparam structure itself is to be freed. Do not try to access the task cluster 
                    entries in the xparam structure after this function has been called!
   @param           XParam              [in]    Pointer of the XParam structure to 
   @param           idx                 [in]    the type of devspace/volume to free the entries for
 */
externC void RTEConf_FreeDevDescrInXParamStructure( RTE_XPARAM_REC *XParam,int idx );


/*!
   @brief           get the value of an integer parameter from the internal parameter storage
   @param           conf_param_name     [in]    the name of the parameter to get the value of
   @param           conf_param_value    [out]   contains the value of the parameter if found
   @param           errtext             [out]   contains an error message if something went wrong.
                                                Use RTEConf_ParameterAccessFreeErrorMessage() to free the memory 
                                                allocated for the message!
   @return value    true if a parameter with the given name could be found and is an integer, false if not
 */
externC SAPDB_Bool RTEConf_ParameterAccessGetInteger   ( SAPDB_UTF8 *conf_param_name,
									                     SAPDB_Int8  *conf_param_value,
									                     SAPDB_Char ** errtext);


/*!
   @brief           get the value of a real parameter from the internal parameter storage
   @param           conf_param_name     [in]    the name of the parameter to get the value of
   @param           conf_param_value    [out]   contains the value of the parameter if found
   @param           errtext             [out]   contains an error message if something went wrong.
                                                Use RTEConf_ParameterAccessFreeErrorMessage() to free the memory 
                                                allocated for the message!
   @return value    true if a parameter with the given name could be found and is a real, false if not
 */
externC SAPDB_Bool RTEConf_ParameterAccessGetReal      ( SAPDB_UTF8 *conf_param_name,
									                     SAPDB_Real8 *conf_param_value,
									                     SAPDB_Char ** errtext);


/*!
   @brief           get the value of a string parameter from the internal parameter storage
   @param           conf_param_name     [in]    the name of the parameter to get the value of
   @param           conf_param_value    [out]   contains the value of the parameter if found
   @param           errtext             [out]   contains an error message if something went wrong.
                                                Use RTEConf_ParameterAccessFreeErrorMessage() to free the memory 
                                                allocated for the message!
   @return value    true if a parameter with the given name could be found and is a string, false if not
 */
externC SAPDB_Bool RTEConf_ParameterAccessGetString    ( SAPDB_UTF8 *conf_param_name,
									                     SAPDB_Char **conf_param_value,
									                     SAPDB_Char ** errtext);


/*!
   @brief           get the value of a cyrptinfo parameter from the internal parameter storage
   @param           conf_param_name     [in]    the name of the parameter to get the value of
   @param           conf_param_value    [out]   contains the value of the parameter if found
   @param           errtext             [out]   contains an error message if something went wrong.
                                                Use RTEConf_ParameterAccessFreeErrorMessage() to free the memory 
                                                allocated for the message!
   @return value    true if a parameter with the given name could be found and is a cryptinfo, false if not
 */
externC SAPDB_Bool RTEConf_ParameterAccessGetCryptInfo ( SAPDB_UTF8 *conf_param_name,
									                     SAPDB_Byte *conf_param_value,
									                     SAPDB_Char ** errtext);


/*!
   @brief           set the value of the crash semaphore
   @param           conf_param_value    [in]    the new value of the crash semaphore: 0 if the database has stopped
                                                correctly, any other value to indicate that a crash has happened
   @param           errtext             [out]   contains an error message if something went wrong.
                                                Use RTEConf_ParameterAccessFreeErrorMessage() to free the memory 
                                                allocated for the message!
   @return value    true if succeeded, false if not
 */
externC SAPDB_Bool RTEConf_ParameterAccessSetCrashSem ( SAPDB_Int8 conf_param_value,
                                                        SAPDB_Char ** errtext);

/*!
   @brief           mark the current parameter set as verified
   @param           errtext             [out]   contains an error message if something went wrong.
                                                Use RTEConf_ParameterAccessFreeErrorMessage() to free the memory 
                                                allocated for the message!
   @return value    true if , false if not
 */
externC SAPDB_Bool RTEConf_ParameterAccessResetParamChangedFlag ( SAPDB_Char ** errtext);

/*!
   @brief           check if the current parameter set has been verified
   @return value    true if the current parameter set has been verified, false if not
 */
externC SAPDB_Bool RTEConf_ParameterAccessIsVerified            ( );

/*!
   @brief           get the value of the crash semaphore
   @return value    true if a crash has happened, false if not
 */
externC SAPDB_Bool RTEConf_ParameterAccessGetCrashSem           ( );

/*!
   @brief           Free an error message that has been returned by one of the functions RTEConf_ReadAndAnalyzeParam(),
                    RTEConf_ParameterAccessGetInteger(), RTEConf_ParameterAccessGetReal(), 
                    RTEConf_ParameterAccessGetString(), RTEConf_ParameterAccessGetCryptInfo(),
                    RTEConf_ParameterAccessSetCrashSem(), RTEConf_ParameterAccessResetParamChangedFlag()
   @param           errtext             [in]    the error text to free
 */
externC void RTEConf_ParameterAccessFreeErrorMessage(SAPDB_Char *errtext);

/*!
   @brief          Read some special parameters with the ability to produce message output
 */
externC void RTEConf_HandleSpecialParametersWithMessageOutput();

/*!
   @brief           Check if the configured volumes of a given type do not exceed a given limit
   @param           type                [in]    the type of volumes to check
   @param           XparamParameters    [in]    address of the XParam structure to use
   @param           limit               [in]    the limit in bytes to check the volume sizes against
   @return value    false if the size of one or more configured volumes exceeds the limit, true if not
 */
externC SAPDB_Bool RTEConf_CheckVolumeSize(RTE_VolumeType type,RTE_XPARAM_REC *XparamParameters,SAPDB_UInt8 limit);

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif /* CONF_PARAMETER_ACCESS_H */
