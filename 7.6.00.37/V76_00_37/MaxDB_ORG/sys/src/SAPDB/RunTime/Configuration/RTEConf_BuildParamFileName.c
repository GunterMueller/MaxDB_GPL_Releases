/*!---------------------------------------------------------------------
  @file           RTEConf_BuildParamFileName.c
  @author         RobinW
  @brief          routines for building the full path to the parameter file

  first created  2000-05-08  17:49

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


#include "gsp01.h"  /* tsp01_RteError */
#include "gsp11.h"
#include "heo01.h"
#include "heo47.h"
#include "geo60.h"

#include "RunTime/Configuration/RTEConf_BuildParamFileName.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MOD__ "RTEConf_BuildParamFileName.c :"

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

externC SAPDB_Bool RTEConf_BuildParamFileNameWithPath ( SAPDB_Char     *DBNameC,
                                                        SAPDB_Int4     ParamVersion,
                                                        SAPDB_Char     *ParamFileNameWithPath,
                                                        tsp01_RteError *RteError)
{
    SAPDB_Bool     Ok ;

    Ok = sqlGetIndependentConfigPath ( ParamFileNameWithPath, TERM_WITH_DELIMITER_EO01, RteError ) ;
    if ( Ok )
    {
        strcat (ParamFileNameWithPath, DBNameC);

        if ( ParamVersion != XP_ACT_VERSION_SP11 )
        {
            SAPDB_Int4 ActFileNameLen = strlen(ParamFileNameWithPath) ;
            ParamFileNameWithPath[ActFileNameLen] = '.' ;
            ActFileNameLen++ ;
            if ( ParamVersion < 10 ) 
            {
                ParamFileNameWithPath[ActFileNameLen] = '0' ;
                ActFileNameLen++ ;
            }
            sql47_itoa( ParamVersion, ParamFileNameWithPath + ActFileNameLen, 
                sizeof(tsp00_Pathc) - ActFileNameLen );
        }
    }
    return ( Ok );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
