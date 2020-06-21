/*!---------------------------------------------------------------------
  @file           RTEConf_ParameterAccessSimple.cpp
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


#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessSimple.h"
#include "SAPDBCommon/SAPDB_ToString.hpp"

externC bool 
        RTEConf_ParamaterAccessGetParam  ( const tsp00_DbNamec &DBNameC ,
                                           const tsp00_C256c   &XpKeyC,
                                                 tsp00_C256c   &XpValueC,
                                                 tsp00_ErrText &ErrText )
{
  RTEConf_Parameter       oConfig(DBNameC.asCharp(),
                                  0,
                                  RTECONF_MAXNAMELENGTH,
                                  RTECONF_MAXSTRINGLENGTH);
  SAPDBErr_MessageList    oMsg;
  RTEConf_Parameter::Type oType;

  RTEConf_Parameter::Integer       xpValueInt;
  RTEConf_Parameter::Real          xpValueReal;
  RTEConf_Parameter::StringElement xpValueString[RTECONF_MAXSTRINGLENGTH + 1];
  RTEConf_Parameter::CryptInfo     xpValueCrypt;

  oConfig.GetTypeAndValueDirect((RTEConf_Parameter::Name) XpKeyC.asCharp(), oType, xpValueInt, xpValueReal, xpValueString, xpValueCrypt, oMsg);

  if (oMsg.IsEmpty()) {
    switch (oType) {
      case RTEConf_Parameter::t_Integer:
      {
        SAPDB_ToStringClass oString(xpValueInt);
        STRCPY_UTF8(xpValueString, oString);
        break;
      }
      case RTEConf_Parameter::t_Real:
      {
        SAPDB_ToStringClass oString(xpValueReal);
        STRCPY_UTF8((char *) xpValueString, oString);
        break;
      }
      case RTEConf_Parameter::t_String:
        break;
      case RTEConf_Parameter::t_CryptInfo:
        xpValueString[0] = 0;
        break;
    } /* end switch */
  } /* end if */

  if (oMsg.IsEmpty()) {
    XpValueC.rawAssign((const char *) xpValueString);
  } else {
    ErrText.c2p(oMsg.Message());
  } /* end if */

  return oMsg.IsEmpty();
} // end RTEConf_ParamaterAccessGetParam

/* *************************************************************************** */

externC bool 
        RTEConf_ParamaterAccessDeleteParam  ( const tsp00_DbNamec &DBNameC ,
                                              const tsp00_C256c   &XpKeyC,
                                                    tsp00_ErrText &ErrText )
{
  RTEConf_Parameter       oConfig(DBNameC.asCharp(),
                                  0,
                                  RTECONF_MAXNAMELENGTH,
                                  RTECONF_MAXSTRINGLENGTH);
  SAPDBErr_MessageList    oMsg;

  SAPDB_Bool                       bFileExist;

  oConfig.Read(bFileExist, oMsg);

  if (oMsg.IsEmpty()) {
    oConfig.Delete((RTEConf_Parameter::Name) XpKeyC.asCharp(), oMsg);
  } /* end if */

  if (oMsg.IsEmpty()) {
    oConfig.Write(oMsg);
  } // end if

  if (!oMsg.IsEmpty()) {
    ErrText.c2p(oMsg.Message());
  } /* end if */

  return oMsg.IsEmpty();
} // end RTEConf_ParamaterAccessGetParam
