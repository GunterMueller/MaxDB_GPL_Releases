/*!========================================================================

  @file         RTE_EnvironmentVariables.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Environment variable handling.

                Some functions to create, modify, read environment variables
                or to expand a string with imbedded environment variables.

  @since        2005-05-24  15:19
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <malloc.h>
#include <stdlib.h>
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_EnvironmentVariables.h"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(_WIN32)
# define START_DELIMITER  '%'
# define END_DELIMITER    '%'
#else
# define START_DELIMITER  '$'
# define START_DELIMITER2 '('
# define END_DELIMITER    ')'
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#ifndef round_up
#define  max(a,b)                 (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define  min(a,b)                 (((a) < (b)) ? (a) : (b))
#endif
#ifndef rnd_to
#define  rnd_to(_val,_rnd_val)    ((max(((_val) - 1),0) / (_rnd_val) + 1 ) * (_rnd_val))
#endif



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


static SAPDB_UInt4 CalculateExpandEnvVarsBufferSize ( SAPDB_Char const *  const  inputStr );
static SAPDB_Int4  LocateEnvVar                     ( SAPDB_Char  **             environment,
                                                      SAPDB_Char const * const   envVarName, 
                                                      SAPDB_UInt4                envVarNameLen );


/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


extern "C" SAPDB_Bool RTE_PutEnvVar ( SAPDB_Char const * const    inputStr )
{
#if defined (_WIN32)
    return 0 == putenv(inputStr);
#else
    SAPDB_Char*          envVar;
    static SAPDB_Char**  environment      = 0;
    SAPDB_Bool           dropVar          = false;
    SAPDB_Char const *   assignChar       = strchr (inputStr, '=');  /* end of env.var.*/

    if ( 0 == assignChar )
        return false;

    // Alloc temporary environment variable name buffer....
    SAPDB_UInt4  envVarNameLen = (SAPDB_UInt4)(assignChar - inputStr);
    SAPDB_Char*  envVarNameBuf = (SAPDB_Char*) alloca( envVarNameLen + 1 );
    
    // ... and copy the name into it
    SAPDB_strncpy_zero_terminated( envVarNameBuf, inputStr, envVarNameLen );
    envVarNameBuf[envVarNameLen] = '\0'; // is not zero terminated, we have to do it here!

#   if defined (DROP_ENVVAR_ALLOWED)
     dropVar = '\0' == inputStr[envVarNameLen+1];  // drop old buffer if variable content is empty ( "<var>=" )
#   endif

    if ( false == dropVar )
    {
        // Create variable buffer and copy input string content into it.
        envVar = (SAPDB_Char*)malloc ( strlen ( inputStr ) + 1 );

        if ( 0 == envVar )
            return false;

        SAPDB_strcpy( envVar, inputStr );
    }

    // environment array empty?
    if ( 0 == environment )
    {
        if ( dropVar )
        {   // Oops, drop variable that is not in our array!
            return 0 == putenv((char*)inputStr);
        }
        // create the first element of the array
        if ( 0 == (environment = (SAPDB_Char**)malloc(sizeof(SAPDB_Char *))))
        {
            free ( envVar );
            return false;
        }
        environment[0] = 0;
    }

    SAPDB_Int4 pos = LocateEnvVar ( environment, envVarNameBuf, envVarNameLen );

    if (( 0 < pos ) || ( 0 == environment[0] ))
    {
        if ( dropVar )
        {   // Oops, drop variable that is not in our array!
            return 0 == putenv((char*)inputStr);
        }

        SAPDB_Char** newEnv = (SAPDB_Char **)realloc( environment, sizeof(SAPDB_Char*) * (pos + 2) );

        if ( 0 == newEnv )
        {
            free ( envVar );
            return false;
        }
        // append string and end marker 
        environment          = newEnv; 
        environment[pos]     = envVar;
        environment[pos + 1] = 0;
    }
    else
    {
       pos = -pos;  // position must be a positive value

       // Environment variable is already in our array, free the old one!
        free(environment[pos]);

        if ( dropVar )
        {
            // move entries up ...
            for ( ; environment[pos]; pos++ ) 
                environment[pos] = environment[pos + 1];

            // ... and shrink the array
            SAPDB_Char** newEnv = (SAPDB_Char **)realloc( environment, sizeof(SAPDB_Char*) * pos );

            if ( newEnv )
                environment = newEnv; 

            return 0 == putenv((char*)inputStr);
        }

        environment[pos] = envVar;
        return 0 == putenv(envVar);
    }

    return 0 == putenv(envVar);

#endif
}

/*---------------------------------------------------------------------------*/

SAPDB_Char const * RTE_GetPtrToEnvVar  ( SAPDB_Char const * const  envVarName )
{
    return getenv( envVarName );
}

/*---------------------------------------------------------------------------*/

extern "C" SAPDB_Bool RTE_GetEnvVar ( SAPDB_Char const * const  envVarName,
                                      SAPDB_Char * const        outputBuffer,
                                      SAPDB_UInt4  * const      outputBufferSize )
{
    SAPDB_Char*  envVarContent = getenv( envVarName );

    if ( 0 != envVarContent )
    {
        SAPDB_UInt4  contentLen = (SAPDB_UInt4)strlen(envVarContent);

        if ( *outputBufferSize <= contentLen )
        {
            *outputBufferSize = contentLen;
            return false;
        }

        SAPDB_strcpy( outputBuffer, envVarContent );
        *outputBufferSize = contentLen;
    } 
    else  // ... has no content!
    {   
        *outputBufferSize = 1;

        if ( 0 != *outputBufferSize )
           outputBuffer[0] = '\0';
        else
            return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

extern "C" SAPDB_Bool RTE_ExpandEnvVars ( SAPDB_Char const * const  inputStr,
                                          SAPDB_Char * const        outputBuffer,
                                          SAPDB_UInt4 * const       outputBufferSize ) 
{
  SAPDB_Char *         dest              = outputBuffer;
  SAPDB_Char const *   src               = inputStr;
  SAPDB_UInt4          envVarNameBufSize = 0;
  SAPDB_UInt4          bufferSpaceLeft   = *outputBufferSize;
  SAPDB_UInt4          envVarNameLen;
  SAPDB_Char*          envVarNameBuf;
  SAPDB_Char*          envVarContent;
  SAPDB_UInt4          len;

  while( '\0' != *src )
  {
      if ( 1 >= bufferSpaceLeft ) // we need one byte for the zero termination
      {   
          // buffer size to small, try to copy input string to the output buffer
          SAPDB_strncpy_zero_terminated (outputBuffer, inputStr, *outputBufferSize );
          outputBuffer[(*outputBufferSize) - 1] = '\0';
          *outputBufferSize = CalculateExpandEnvVarsBufferSize( inputStr );
          return false;
      }

      // start of environment variable found?
      if ( START_DELIMITER == *src )
      {
          SAPDB_Char const *  envVarStart;

#         if defined(START_DELIMITER2)
           if ( START_DELIMITER2 == *(src + 1) )  // second delimiter found ?
           {
               char const *  tmpSrc = src + 2;  // start behind the start delimiters
#         else
           {
               char const *  tmpSrc = src + 1;  // start behind the start delimiter
#         endif
               envVarStart = tmpSrc;

               // find end of environment variable
               while(( '\0' != *tmpSrc ) && ( END_DELIMITER != *tmpSrc ))
                   ++tmpSrc;

               // end delimiter found ?
               if ( END_DELIMITER == *tmpSrc ) 
               {
                   // calculate length of environment variable name
                   envVarNameLen = (SAPDB_UInt4)(tmpSrc - envVarStart);

                   // Is the last allocated name buffer to small?
                   if ( envVarNameBufSize <= (SAPDB_UInt4)envVarNameLen )
                   {
                       // allocate a new one!
                       envVarNameBufSize = rnd_to((SAPDB_UInt4)envVarNameLen + 1, 64);
                       envVarNameBuf     = (SAPDB_Char*)alloca( envVarNameBufSize );
                   }
                   // we don't modify the input string, we are copying the variable name to temporary buffer!
                   SAPDB_strncpy_zero_terminated( envVarNameBuf, envVarStart, envVarNameLen );
                   envVarNameBuf[envVarNameLen] = '\0'; // is not zero terminated, we have to do it here!

                   // get variable content
                   envVarContent = getenv( envVarNameBuf );

                   // envornment variable found? ...
                   if ( envVarContent )
                   {   // ... copy the variable content
                       len = min((SAPDB_UInt4)strlen(envVarContent),bufferSpaceLeft - 1); 
                       SAPDB_strncpy_zero_terminated(dest, envVarContent, len );
                   }
                   else
                   {   // not enough space, copy the variable
                       len = min((SAPDB_UInt4)(tmpSrc - src + 1), bufferSpaceLeft - 1); // get the variable length length
                       SAPDB_strncpy_zero_terminated(dest, src, len);
                   }

                   src             = tmpSrc + 1;
                   dest           += len;
                   bufferSpaceLeft -= len;
                   continue;
               }
           }
      }

      // copy next character
      *dest = *src;
      ++src;
      ++dest;
      --bufferSpaceLeft;
  }

  // Zero termination
  *dest = '\0';
   --bufferSpaceLeft;

  *outputBufferSize = (*outputBufferSize) - bufferSpaceLeft;

  return true;
}

/*---------------------------------------------------------------------------*/

static SAPDB_UInt4 CalculateExpandEnvVarsBufferSize ( SAPDB_Char const * const  inputStr )
{
  SAPDB_Char const*   src    = inputStr;
  SAPDB_UInt4         envVarNameBufSize = 0;
  SAPDB_UInt4         bufferSpaceNeeded = 0;
  SAPDB_UInt4         envVarNameLen;
  SAPDB_Char*         envVarNameBuf;
  SAPDB_Char*         envVarContent;
  SAPDB_UInt4         len;


  while( '\0' != *src )
  {
      // start of environment variable found?
      if ( START_DELIMITER == *src )
      {
          SAPDB_Char const *  envVarStart;

#       if defined(START_DELIMITER2)
           if ( START_DELIMITER2 == *(src + 1) )  // second delimiter found ?
           {
               char const *  tmpSrc = src + 2;  // start behind the start delimiters
#       else
           {
               char const *  tmpSrc = src + 1;  // start behind the start delimiter
#       endif
               envVarStart = tmpSrc;

               // find end of environment variable
               while(( '\0' != *tmpSrc ) && ( END_DELIMITER != *tmpSrc ))
                   ++tmpSrc;

               // end delimiter found ?
               if ( END_DELIMITER == *tmpSrc ) 
               {
                   // calculate length of environment variable name
                   envVarNameLen = (SAPDB_UInt4)(tmpSrc - envVarStart);

                   // Is the last allocated name buffer to small?
                   if ( envVarNameBufSize <= (SAPDB_UInt4)envVarNameLen )
                   {
                       // allocate a new one!
                       envVarNameBufSize = rnd_to((SAPDB_UInt4)envVarNameLen + 1, 64);
                       envVarNameBuf     = (SAPDB_Char*)alloca( envVarNameBufSize );
                   }
                   // we don't modify the input string, we are copying the variable name to temporary buffer!
                   SAPDB_strncpy_zero_terminated( envVarNameBuf, envVarStart, envVarNameLen );
                   envVarNameBuf[envVarNameLen] = '\0'; // is not zero terminated, we have to do it here!

                   // get variable content
                   envVarContent = getenv( envVarNameBuf );

                   // envornment variable found? ...
                   if ( envVarContent )
                       len = (SAPDB_UInt4)strlen(envVarContent);
                   else // not enough space, copy the variable
                       len = (SAPDB_UInt4)(tmpSrc - src + 1); // get the variable length length

                   src                = tmpSrc + 1;
                   bufferSpaceNeeded += len;
                   continue;
               }
           }
      }

      // copy next character
      ++src;
      ++bufferSpaceNeeded;
  }
  ++bufferSpaceNeeded; // zero termination

  return bufferSpaceNeeded;
}

/*---------------------------------------------------------------------------*/

static SAPDB_Int4 LocateEnvVar ( SAPDB_Char  **           environment,
                                 SAPDB_Char const * const envVarName, 
                                 SAPDB_UInt4              envVarNameLen )
{
    SAPDB_Char **env  = environment;

    for ( ; *env; env++ ) 
    {
        // name substring found
        if (( 0 == SAPDB_strncasecmp( envVarName, *env, envVarNameLen ) ) && ( '=' == (*env)[envVarNameLen] ))
        {
            return((SAPDB_Int4)(environment - env)); // return position as negative value
        }
    }

    return(SAPDB_Int4)(env - environment); // return array size
}



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/