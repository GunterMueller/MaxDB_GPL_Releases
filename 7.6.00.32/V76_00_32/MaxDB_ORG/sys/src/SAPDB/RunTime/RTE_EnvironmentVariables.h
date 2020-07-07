/*!========================================================================

  @file         RTE_EnvironmentVariables.h
  @ingroup      Runtime
  @author       RaymondR

  @brief        Environment variable handling.

                Some functions to create, modify, read environment variables
                or to expand a string with imbedded environment variables.


  @since        2005-05-19  17:38
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end
  \endif
 ============================================================================*/


#ifndef RTE_ENVIRONMENTVARIABLES_HPP
#define RTE_ENVIRONMENTVARIABLES_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#if defined(__cplusplus)
 extern "C" {
#endif

 /*!
  @brief Create, modify, or remove environment variables.

         The input string must have the form varname=varcontent, where varname is the name of the environment 
         variable to be added or modified and varcontent is the variable's value. If varname is already part 
         of the environment, its value is replaced by string; otherwise, the new varname variable and its 
         string value are added to the environment. On some plattform its allowed to remove a variable 
         from the environment by specifying an empty string (varname=).

         Usually on UNIX plattforms the input string of putenv has to be a none automatic variable because the
         putenv call is not holding a copy of this string instead it is using the input string pointer as is.

         !!!!!! This function is holding its own copy of the input string !!!!!!!!!!

  @param inputStr         [in] input string of the form varname=varcontent

  @return true if call succeed, false if failed
 */
SAPDB_Bool RTE_PutEnvVar     ( SAPDB_Char const * const  inputStr );

/*!
  @brief Get a value from the current environment.

         Returns the value of the environment variable. To calculate the buffer size
         needed for the environment variable, call this function with 'outputBuffer' and 
         'outputBufferSize' set to zero. 
  
  @param envVarName       [in]     input string containing environment variables
  @param outputBuffer     [out]    pointer to the output buffer
  @param outputBufferSize [in/out] output buffer size / after the function return this
                                   variable contains the buffersize needed. 

  @return  true if call succeed, false if the output buffer is to small. The
           outputBufferSize is holding the buffersize needed.
 */

SAPDB_Bool RTE_GetEnvVar     ( SAPDB_Char const * const  envVarName,
                               SAPDB_Char * const        outputBuffer,
                               SAPDB_UInt4  * const      outputBufferSize );

/*!
  @brief Get a pointer to the environment variable value.

         Returns the pointer to the environment variable.
  
  @param envVarName       [in]     input string containing environment variables
                                   variable contains the buffersize needed. 

  @return  zero if the environment variable does not exist .
 */
SAPDB_Char const * RTE_GetPtrToEnvVar  ( SAPDB_Char const * const  envVarName );


/*!
  @brief Expands a string with imbedded environment variables.
         
         All environment variables imbedded in a given string will be expanded.

         The environment variables must have the form:
             UNIX:    $(ENVVAR)
             Windows: %ENVAR%

         A string like this "/progs/%myname%/pgm" might be expanded to
         "/progs/peters_programs/pgm".

         If the given output buffer is to small the input string is 
         copied to output buffer an the function returns false.


  @param inputStr         [in]     input string containing environment variables
  @param outputBuffer     [out]    expanded output string
  @param outputBufferSize [in/out] output buffer size / after the function return this
                                   variable contains the buffersize needed.

  @return  true if call succeed, false if the output buffer is to small. The
           outputBufferSize is holding the buffersize needed.
 */
SAPDB_Bool RTE_ExpandEnvVars ( SAPDB_Char const * const  inputStr,
                               SAPDB_Char * const        outputBuffer,
                               SAPDB_UInt4 * const       outputBufferSize );

#if defined(__cplusplus)
 }
#endif

#endif  /* RTE_ENVIRONMENTVARIABLES_HPP */
