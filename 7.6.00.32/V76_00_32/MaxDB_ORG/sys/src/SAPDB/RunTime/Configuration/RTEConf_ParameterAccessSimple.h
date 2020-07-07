/*!---------------------------------------------------------------------
  @file           RTEConf_ParameterAccessSimple.h
  @author         RobinW, BerndV
  @brief          RTE configuration parameters: function for simple parameter access

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


#ifndef CONF_PARAMETER_ACCESS_SIMPLE_H
#define CONF_PARAMETER_ACCESS_SIMPLE_H

#include "gsp00.h"

/*!
   @brief           get the value of a parameter from a given parameter file
   @param           DBNameC             [in]    the name of the parameter file to get the parameter from
   @param           XpKeyC              [in]    the name of the parameter to get the value of
   @param           XpValueC            [out]   contains the value of the parameter if found
   @param           ErrText             [out]   contains an error text if the parameter could not be read from the file
   @return value    true if a parameter with the given name could be found, false if not
 */
externC bool 
        RTEConf_ParamaterAccessGetParam  
        ( 
#if defined (__cplusplus)
          const tsp00_DbNamec &DBNameC ,
          const tsp00_C256c   &XpKeyC,
          tsp00_C256c         &XpValueC,
          tsp00_ErrText       &ErrText 
#else
          const tsp00_DbNamec  DBNameC ,
          const tsp00_C256c    XpKeyC,
          tsp00_C256c          XpValueC,
          tsp00_ErrText        ErrText 
#endif
          );

/*!
   @brief           delete the parameter given from a given parameter file
   @param           DBNameC             [in]    the name of the parameter file to get the parameter from
   @param           XpKeyC              [in]    the name of the parameter to get the value of
   @param           ErrText             [out]   contains an error text if the parameter could not be deleted from the file
   @return value    true if a parameter with the given name could be found, false if not
 */
externC bool 
        RTEConf_ParamaterAccessDeleteParam  
        ( 
#if defined (__cplusplus)
          const tsp00_DbNamec &DBNameC ,
          const tsp00_C256c   &XpKeyC,
          tsp00_ErrText       &ErrText 
#else
          const tsp00_DbNamec  DBNameC ,
          const tsp00_C256c    XpKeyC,
          tsp00_ErrText        ErrText 
#endif
          );
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif /* CONF_PARAMETER_ACCESS_SIMPLE_H */
