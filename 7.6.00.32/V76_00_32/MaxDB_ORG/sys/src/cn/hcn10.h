/*!
  @file           hcn10.h
  @author         BerndV
  @brief          DBMServer main module

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
*/

#ifndef _HCN10_H_
#define _HCN10_H_

/* IMPORT
 * ------------------------------------------------------------------
 */

#include "gcn00.h"
#include "DBM/Srv/DBMSrv_Command.hpp"

#define OBSOLETE_MARK_CN10 "(obsolete"

//! \brief a structure for holding information about a DBMServer command
struct cn10DBMServerCommand {
  const char               * name;
  bool                       bNeedDBMLogon;
  bool                       bNeedSAPDBAdmi;
  bool                       bStaticAvailable;
  tsp00_Int2                 nDBNameState;
  tsp00_Int2                 nMinParam;
  tsp00_Int2                 nMaxParam;
  tsp00_Int2                 nTokens2Log;
  cnFunctionPrototype_CN00 * cnFunction;
  const char               * shortHelp;
  DBMSrv_Command::CommandKey cmdKey;
  bool                       bHSS;
  tsp00_Uint4                nRights;
} /*DBMServerCommand*/;

//! \brief an externaly known pointer to the array of DBMServer commands
extern const cn10DBMServerCommand * DBMServerCommandsArray_cn10;

/* 
  SPECIFICATION PUBLIC FUNCTIONS
  ------------------------------------------------------------------
 */

/*! @brief Check a static command */
bool cn10CheckStatic 
   ( const char * pCommand,
     bool       & bStaticAvailable,
     bool       & bNeedsDatabase,
     bool       & bNeedsUser );

#endif /* _HCN10_H_ */

