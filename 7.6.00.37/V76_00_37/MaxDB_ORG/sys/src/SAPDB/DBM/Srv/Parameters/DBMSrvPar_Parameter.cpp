/*!
  @file           DBMSrvPar_Parameter.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer database parameters handling - Implementation

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameter.hpp"

/*-----------------------------------------------------------------------------*/
/*! @brief public constructor */
DBMSrvPar_XPLine::DBMSrvPar_XPLine
  (const char *    szBuffer,
   LineType        xpLineIn,
   long            nNumberIn)
   : szLine(NULL),
     xpLine(xpLineIn),
     pNext(NULL),
     nNumber(nNumberIn)
{
  szLine  = new char[strlen(szBuffer) + 1];
  if (szLine != NULL) {
    SAPDB_strcpy(szLine, szBuffer);
  } // end if
} // end DBMSrvPar_XPLine::DBMSrvPar_XPLine

/*-----------------------------------------------------------------------------*/
/*! @brief public destructor */
DBMSrvPar_XPLine::~DBMSrvPar_XPLine
  ( ) 
{
  if (pNext != NULL) {
    DBMSrvPar_XPLine * pTemp = Next();
    SetNext(NULL);
    delete pTemp;
  } // end if

  if (szLine  != NULL) {
    delete [] szLine;
  } 
} // end DBMSrvPar_XPLine::~DBMSrvPar_XPLine

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
const char * DBMSrvPar_XPLine::Append
  (const char *    szBuffer)
{
  char * pTemp = new char[strlen(szBuffer) + strlen(szLine) + 2];
  if (pTemp != NULL) {
    SAPDB_strcpy(pTemp, szLine);
    SAPDB_strcat(pTemp, " ");
    SAPDB_strcat(pTemp, szBuffer);
    delete [] szLine;
    szLine = pTemp;
  } // end if
  return pTemp;
} // end DBMSrvPar_XPLine::DBMSrvPar_XPLine

/*-----------------------------------------------------------------------------*/
/*! @brief public member */
void DBMSrvPar_XPLine::SetNext
  (DBMSrvPar_XPLine * pNextIn)
{
  pNext = pNextIn;
} // end DBMSrvPar_XPLine::SetNext

/*-----------------------------------------------------------------------------*/
/*! @brief public constructor */
DBMSrvPar_Parameter::DBMSrvPar_Parameter
    ( )
    : nCalcState(XPCalcStateUnknown), 
      bHelpExist(false),     
      bParamExist(false),    
      pNext(NULL),
      pCode(NULL),
      pConstraint(NULL),
      pExplain(NULL),
      pHelp(NULL),
      nClasses(XPClassValueNothing),
      oWriteMode(XPWriteModePermanent)
{
  int nIndex = 0;

  szParameterName.Init();
  szDefValue.Init();                    
  szGoodValue.Init();                   
  szActiveValue.Init();                 
  szOldValue.Init();                    
  szNewValue.Init();                    
  szComment.Init();

  for (nIndex = 0; nIndex < XP_DEFAULTFOR_MXCN002; ++nIndex) {
    szDefaultFor[nIndex][0].Init();
    szDefaultFor[nIndex][1].Init();
  } // end for

  xpRecord.xpType = DBMSrvPar_RTEConfParameter::t_Invalid;
  memset(&xpRecord.xpValue, 0, sizeof(xpRecord.xpValue));

  for (nIndex = 0; nIndex < XPPropClassMAX; ++nIndex) {
    xpProperties[nIndex]= XPPropValueUnknown;
    szProperties[nIndex].Init();
  } // end for

} // end DBMSrvPar_Parameter::DBMSrvPar_Parameter

/*-----------------------------------------------------------------------------*/
/*! @brief public destructor */
DBMSrvPar_Parameter::~DBMSrvPar_Parameter
    ( )
{
  if (pCode       != NULL) delete pCode;
  if (pConstraint != NULL) delete pConstraint;
  if (pExplain    != NULL) delete pExplain;
  if (pHelp       != NULL) delete pHelp;
} // end DBMSrvPar_Parameter::~DBMSrvPar_Parameter
