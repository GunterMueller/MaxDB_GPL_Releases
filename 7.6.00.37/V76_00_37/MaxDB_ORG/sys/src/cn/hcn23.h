/*

    * responsible:   BerndV
    *
    * purpose:       calculation module for parameter management

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




*/
#ifndef _HCN23_H_
#define _HCN23_H_

/* ------------------------------------------------------------------
 * INCLUDES
 * ------------------------------------------------------------------
 */
#include "gcn00.h"
#include "SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp"

/* SPECIFICATION PUBLIC FUNCTION cn23InitializeChecking
 * ------------------------------------------------------------------
 * purpose: assign the current value (szInput) of all parameters
 *          to the calculation value (szCalc) an set the calculation
 *          status to unknown
 */
void cn23InitializeChecking (
      DBMSrvPar_Parameter * pParameter);

/* SPECIFICATION PUBLIC FUNCTION cn23PrintDependencies
 * ------------------------------------------------------------------
 */
void cn23PrintDependencies (
      DBMSrvPar_Parameter * pFirst);

/* SPECIFICATION PUBLIC FUNCTION cn23CheckConstraint
 * ------------------------------------------------------------------
 */
tcn00_Error cn23CheckConstraint (
      DBMSrvPar_Parameters * pXParamData,
      DBMSrvPar_Parameter  * pParameter,
      char                 * replyData,
      int                  * replyLen);

/* SPECIFICATION PUBLIC FUNCTION cn23CalculateParameter
 * ------------------------------------------------------------------
 */
tcn00_Error cn23CalculateParameter (
      DBMSrvPar_Parameters * pXParamData,
      DBMSrvPar_Parameter  * pParameter,
      char                 * replyData,
      int                  * replyLen);

/* SPECIFICATION PUBLIC FUNCTION cn23GetCalcState
 * ------------------------------------------------------------------
 * purpose: analyze the check state and print an error/request in the
 *          answer buffer
 */
tcn00_Error cn23GetCalcState (
      DBMSrvPar_Parameter * pParameter,
      char                * pBuffer,
      DBMSrvPar_Parameter::XPPropValue    nVisible);

#endif /* _HCN23_H_ */
