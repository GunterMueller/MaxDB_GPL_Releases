/*!
  @file           DBMSrvPar_Parameter.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer database parameters handling - Specification

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

#ifndef _DBMSrvPar_Parameter_HPP_
#define _DBMSrvPar_Parameter_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "gcn002.h"
#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"

/*! @brief parameter line object */
class DBMSrvPar_XPLine {

public:
  /*! @brief line types in pcf file */
  typedef enum {
    XPLineBOF,                     /* Begin Of File                   Old  New */
    XPLineID,                      /* ID-Line                         Old  New */
    XPLineTYPE,                    /* TYPE-Line                            New */
    XPLineDEFAULT,                 /* DEFAULT-Line                         New */
    XPLineDEFAULTFOR,              /* DEFAULT_FOR-Line                     New */
    XPLineProperty,                /* Property-Line                        New */
    XPLineCODE,                    /* CODE-Line                            New */
    XPLineENDCODE,                 /* ENDCODE-Line                         New */
    XPLineCodeCALC,                /* CALC-Line                       Old  New */
    XPLineCodeCALCFollow,          /* CALC-Follow-Line                Old      */
    XPLineCodeFORCE,               /* FORCE-Line                      Old      */
    XPLineCodeFORCEFollow,         /* FORCE-Follow-Line               Old      */
    XPLineCodeIF,                  /* IF-Line                         Old  New */
    XPLineCodeIFFollow,            /* IF-Follow-Line                  Old      */
    XPLineCodeELSIF,               /* ELSIF-Line                      Old  New */
    XPLineCodeELSIFFollow,         /* ELSIF-Follow-Line               Old      */
    XPLineCodeELSE,                /* ELSE-Line                       Old  New */
    XPLineCodeCONSTRAINT,          /* CONSTRAINT-Line                 Old  New */
    XPLineCodeCONSTRAINTFollow,    /* CONSTARINT-Follow-Line          Old      */
    XPLineEXPLAIN,                 /* EXPLAIN-Line                    Old  New */
    XPLineEXPLAINFollow,           /* EXPLAIN-Follow-Line             Old  New */
    XPLineENDEXPLAIN,              /* ENDEXPLAIN-Line                      New */
    XPLineHELP,                    /* HELP-Line                       Old  New */
    XPLineHELPFollow,              /* HELP-Follow-Line                Old  New */
    XPLineENDHELP,                 /* ENDHELP-Line                         New */
    XPLineEOF,                     /* End Of File                     Old  New */
    XPLineUnknown                  /* Special Value                            */
  } LineType;

  /*! @brief  Constructor */
  DBMSrvPar_XPLine
    (const char *   szBuffer,
     LineType       xpLineIn,
     long           nNumberIn);

  /*! @brief  Destructor */
  ~DBMSrvPar_XPLine();

  /*! @brief  append buffer */
  const char * Append
    (const char *   szBuffer);

  /*! @brief  set next pointer */
  void SetNext
    (DBMSrvPar_XPLine * pNextIn);

  /*! @brief  get content */
  const char * Line() const {return szLine;}
  /*! @brief  get type */
  LineType Type() const {return xpLine;}
  /*! @brief  get  number */
  long Number() const {return nNumber;}
  /*! @brief  get next pointer */
  DBMSrvPar_XPLine * Next() const {return pNext;}

private:
  char                    * szLine;
  LineType                  xpLine;
  long                      nNumber;
  DBMSrvPar_XPLine        * pNext;

}; // end class DBMSrvPar_XPLine

/*! @brief parameter object */
class DBMSrvPar_Parameter {

public:
  /*! @brief  write mode */
  typedef enum {
    XPWriteModeUnknown   = 0x00000000,
    XPWriteModePermanent = 0x00000001,
    XPWriteModeOnline    = 0x00000002,
    XPWriteModeAll       = 0x00000003
  } XPWriteMode;

  /*! @brief  property values */
  typedef enum {
    XPClassValueNothing  = 0x00000000,
    XPClassValueGeneral  = 0x00000001,
    XPClassValueExtended = 0x00000002,
    XPClassValueSupport  = 0x00000004,
    XPClassValueInfo     = 0x00000008,
    XPClassValueTrace    = 0x00000010,
    XPClassValueEvents   = 0x00000020,
    XPClassValueUnknown  = 0x11111111
  } XPClassValue;

  /*! @brief  property values */
  typedef enum {
    XPPropValueUnknown,
    XPPropValueYes,
    XPPropValueNo,
    XPPropValueHigher,
    XPPropValueGeneral,
    XPPropValueExtended,
    XPPropValueSupport,
    XPPropValueTrace,
    XPPropValueEvents,
    XPPropValueInfo,
    XPPropValueOffline,
    XPPropValueRunning
  } XPPropValue;

  /*! @brief  calculation state */
  typedef enum {
    XPCalcStateUnknown,     /* oops, what's that?                         */
    XPCalcStateOK,          /* use value from calculation                 */
    XPCalcStateConstraint,  /* constraint not satisfied                   */
    XPCalcStateMandatory    /* missing value for mandatory                */
  } CalcState;

  /*! @brief  Param record type */
  typedef struct XPRecord {
      DBMSrvPar_RTEConfParameter::Type    xpType;
      union
      {
        DBMSrvPar_RTEConfParameter::Integer       xpValueInt;
        DBMSrvPar_RTEConfParameter::Real          xpValueReal;
        tcn002_XpValueString             xpValueString;
        DBMSrvPar_RTEConfParameter::CryptInfo     xpValueCrypt;
      } xpValue;
    } XPRecord;

  /*! @brief  Constructor */
  DBMSrvPar_Parameter();

  /*! @brief  Destructor */
  ~DBMSrvPar_Parameter();

  tcn002_XpValueName         szParameterName;     /* name of parameter                                 */

  bool                       bHelpExist;          /* flag for existence of parameter in help file      */
  bool                       bParamExist;         /* flag for existence of parameter in param file     */

  tcn002_XpValueString       szDefaultFor[XP_DEFAULTFOR_MXCN002][2]  ;    /* default value              */

  tcn002_XpValueString       szDefValue;                                  /* default value              */
  tcn002_XpValueString       szGoodValue;                                 /* last known good            */
  tcn002_XpValueString       szActiveValue;                               /* active value in database   */
  tcn002_XpValueString       szOldValue;                                  /* value read from paramfile  */
  tcn002_XpValueString       szNewValue;                                  /* input value                */
  tsp00_C80c                 szComment;                                   /* comment for a change       */

  XPRecord                   xpRecord;

  CalcState                  nCalcState;         /* check state value                                 */

  XPPropValue                xpProperties[XPPropClassMAX];  /* properties                             */
  tsp00_C512c                szProperties[XPPropClassMAX];  /* properties value                       */

  DBMSrvPar_Parameter      * pNext;              /* pointer for list management (help file order)     */

  DBMSrvPar_XPLine         * pCode;              /* code lines                                        */
  DBMSrvPar_XPLine         * pConstraint;        /* constraint line                                   */
  DBMSrvPar_XPLine         * pExplain;           /* explain lines                                     */
  DBMSrvPar_XPLine         * pHelp;              /* help lines                                        */


  SAPDB_UInt4                nClasses;

  XPWriteMode                oWriteMode;

}; // end class DBMSrvPar_Parameter

#endif /* _DBMSrvPar_Parameter_HPP_ */

