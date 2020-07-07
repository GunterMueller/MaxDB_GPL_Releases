/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Parameter.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Parameter Class
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------



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




*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_Parameter.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PROP_CHANGE         "CHANGE"
#define PROP_INTERN         "INTERN"
#define PROP_MANDATORY      "MANDATORY"
#define PROP_CLEAR          "CLEAR"
#define PROP_DYNAMIC        "DYNAMIC"
#define PROP_CASESENSITIVE  "CASESENSITIVE"
#define PROP_OVERRIDE       "OVERRIDE"
#define PROP_DEVSPACE       "DEVSPACE"
#define PROP_MODIFY         "MODIFY"
#define PROP_GROUP          "GROUP"
#define PROP_DISPLAYNAME    "DISPLAYNAME"
#define PROP_VALUESET       "VALUESET"
#define PROP_MAX            "MAX"
#define PROP_MIN            "MIN"  
#define PROP_INSTANCES      "INSTANCES"
#define PROP_LASTKNOWNGOOD  "LASTKNOWNGOOD"

#define PROP_HELP           "HELP"
#define PROP_EXPLAIN        "EXPLAIN"

#define CHECK_ERR_REQUEST    "REQUEST"
#define CHECK_ERR_CONSTRAINT "CONSTRAINT"
#define CHECK_ERR_MANDATORY  "MANDATORY"
#define CHECK_ERR_UNKNOWN    "UNKNOWN"

/*    
  =============================================================================
  class: DBMCli_Parameter
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Parameter :: DBMCli_Parameter
  -----------------------------------------------------------------------------
*/
DBMCli_Parameter :: DBMCli_Parameter ( )
{
}

DBMCli_Parameter :: DBMCli_Parameter ( const DBMCli_String & sName )
                  : m_sName( sName )
{
}

DBMCli_Parameter :: DBMCli_Parameter ( const DBMCli_String & sName,
                                       const DBMCli_String & sType, 
                                       const DBMCli_String & sValue )
                  : m_sName( sName ),
                    m_sType( sType ),
                    m_sValue( sValue )
{
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Parameter :: ~DBMCli_Parameter
  -----------------------------------------------------------------------------
*/
DBMCli_Parameter :: ~DBMCli_Parameter ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameter :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_Parameter :: Clear ( )
{
  m_sType.Empty( );
  m_sValue.Empty( );
  m_sDefault.Empty( );

  m_bChange = false;
  m_bIntern = false;
  m_bMandatory = false;
  m_bNoCopy = false;
  m_bDynamic = false;
  m_bCasesensitive = false;
  m_bOverride = false;
  m_bDevSpace = false;
  m_bModify = false;
  m_nGroup = DBMCLI_PARAMGROUP_UNKNOWN;
  m_sGroup.Empty( );
  m_sDisplayName.Empty( );
  m_sValueSet.Empty( );
  m_sMax.Empty( );
  m_sMin.Empty( );
  m_sInstances.Empty( );
  m_sLastKnownGood.Empty( );

  m_sHelp.Empty( );
  m_sExplain.Empty( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameter :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameter :: Refresh ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_PARAM_GETFULL );
  sCmd += " ";
  sCmd += m_sName;
  
  Clear( );
  
  if ( oDB.Execute( sCmd, oMsgList ) ) {

    if( oResult.GetLine( m_sType ) ) {
      if( oResult.GetLine( m_sDefault ) ) {
        if( oResult.GetLine( m_sValue ) ) {

          DBMCli_String sProp;
          DBMCli_String sVal;

          SAPDB_Bool bOK = true;
          SAPDB_Bool bHelp = false;

          while( bOK && !bHelp ) {
            bOK = oResult.GetPropVal( sProp, sVal, "\t" );
            if( bOK ) {
              if( sProp != PROP_HELP ) {
                AssignProp( sProp, sVal );
              } else {
                bHelp = true;
              }
            }
          }

          if( bOK && bHelp ) {
            DBMCli_String sHelp;

            SAPDB_Bool bExplain = false;

            while( bOK && !bExplain ) {
              bOK = oResult.GetLine( sHelp );
              if( bOK ) {
                if( sHelp != PROP_EXPLAIN ) {
                  m_sHelp += sHelp;
                } else {
                  bExplain = true;
                }
              }
            }

            if( bExplain ) {
              DBMCli_String sExplain;

              while( oResult.GetLine( sExplain ) ) {
                m_sExplain += sExplain;
              } 
            }
          }
        }
      }
    }

    if( m_sLastKnownGood.IsEmpty( ) ) {
      m_sLastKnownGood = m_sValue;
    }

    bRC = true;
  }
   
  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameter :: Put
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameter :: Put ( const DBMCli_String & sValue,
                                     Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_PARAM_PUT );
  sCmd += " ";
  sCmd += m_sName;
  sCmd += " ";
  sCmd += sValue;

  if ( oDB.Execute( sCmd, oMsgList ) ) {
    m_sValue = sValue;

    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameter :: PutConfirm
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameter :: PutConfirm ( const DBMCli_String & sValue,
                                            Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_PARAM_PUTCONFIRM );
  sCmd += " ";
  sCmd += m_sName;
  sCmd += " ";
  sCmd += sValue;

  if ( oDB.Execute( sCmd, oMsgList ) ) {
    m_sValue = sValue;

    bRC = true;
  }

  return bRC;
}


/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

void DBMCli_Parameter :: AssignProp ( const DBMCli_String & sProp,
                                      const DBMCli_String & sVal )
{
  if( sProp == PROP_CHANGE ) {
    m_bChange = (sVal != DBMCLI_VAL_NO);
  } else if( sProp == PROP_INTERN ) {
    AssignBool( sVal, &m_bIntern);
  } else if( sProp == PROP_MANDATORY ) {
    AssignBool( sVal, &m_bMandatory );
  } else if( sProp == PROP_CLEAR ) {
    AssignBool( sVal, &m_bNoCopy );
  } else if( sProp == PROP_DYNAMIC ) {
    AssignBool( sVal, &m_bDynamic);
  } else if( sProp == PROP_CASESENSITIVE ) {
    AssignBool( sVal, &m_bCasesensitive );
  } else if( sProp == PROP_OVERRIDE ) {
    AssignBool( sVal, &m_bOverride);
  } else if( sProp == PROP_DEVSPACE ) {
    AssignBool( sVal, &m_bDevSpace );
  } else if( sProp == PROP_MODIFY ) {
    AssignBool( sVal, &m_bDevSpace );
  } else if( sProp == PROP_GROUP ) {
    AssignGroup( sVal, &m_nGroup );
    m_sGroup =sVal;
  } else if( sProp == PROP_DISPLAYNAME ) {
    m_sDisplayName = sVal;
  } else if( sProp == PROP_VALUESET ) {
    m_sValueSet = sVal;
  } else if( sProp == PROP_MAX ) {
    m_sMax = sVal;
  } else if( sProp == PROP_MIN) {
    m_sMin = sVal;
  } else if( sProp == PROP_INSTANCES ) {
    m_sInstances = sProp;
  } else if( sProp == PROP_LASTKNOWNGOOD ) {
    m_sLastKnownGood = sVal;
  }
}

void DBMCli_Parameter :: AssignBool ( const DBMCli_String & sVal,
                                            SAPDB_Bool    * pVal )
{
  if( sVal == DBMCLI_VAL_YES ) {
    *pVal = true;
  } else {
    *pVal = false;
  }
}

void DBMCli_Parameter :: AssignGroup ( const DBMCli_String         & sVal,
                                             DBMCli_ParameterGroup * pVal )
{
  if( sVal == DBMCLI_OPT_PARAM_GENERAL ) {
    *pVal = DBMCLI_PARAMGROUP_GENERAL;
  } else if( sVal == DBMCLI_OPT_PARAM_EXTENDED ) {
    *pVal = DBMCLI_PARAMGROUP_EXTENDED;
  } else if( sVal == DBMCLI_OPT_PARAM_SUPPORT ) {
    *pVal = DBMCLI_PARAMGROUP_SUPPORT;
  } else {
    *pVal = DBMCLI_PARAMGROUP_UNKNOWN;
  }
}


/*    
  =============================================================================
  class: DBMCli_Parameters
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Parameters :: DBMCli_Parameters
  -----------------------------------------------------------------------------
*/
DBMCli_Parameters :: DBMCli_Parameters ( )
{
  m_bOpen = false;

  Clear( );
}

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Parameters :: ~DBMCli_Parameters
  -----------------------------------------------------------------------------
*/
DBMCli_Parameters :: ~DBMCli_Parameters ( )
{
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_Parameters :: Clear ( )
{
  m_nGroup = DBMCLI_PARAMGROUP_UNKNOWN;
  m_aParameter.RemoveAll( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: Open
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameters :: Open ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  
  if( oDB.Execute( DBMCLI_CMD_PARAM_STARTSESSION, oMsgList ) ) {
    m_bOpen = true;
    bRC = true ;
  }
   
  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: Init
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameters :: Init ( const DBMCli_String & sInstanceType, 
                                       Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_PARAM_INIT );
  sCmd += " ";
  sCmd += sInstanceType;

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_ParameterGroup nGroup = DBMCLI_PARAMGROUP_GENERAL;
    DBMCli_ParameterStatus nStatus;
    DBMCli_String sBadParam;
    DBMCli_String sOwnValue;
    DBMCli_String sCompValue;
    if( Check( nGroup, nStatus, sBadParam, sOwnValue, sCompValue, oMsgList ) ) {
      bRC = true ;
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: Copy
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameters :: Copy ( const DBMCli_String & sDatabaseName, 
                                       Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_PARAM_COPY );
  sCmd += " ";
  sCmd += sDatabaseName;

  if( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true ;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: Close
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameters :: Close ( SAPDB_Bool             bCommit,
                                        Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd;

  if( bCommit )
    sCmd = DBMCLI_CMD_PARAM_COMMITSESSION;
  else
    sCmd = DBMCLI_CMD_PARAM_ABORTSESSION;
  
  if( oDB.Execute( sCmd, oMsgList ) ) {
    m_bOpen = false;
    bRC = true ;
  }
   
  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameters :: Refresh ( Msg_List & oMsgList )
{
  return Refresh( DBMCLI_PARAMGROUP_UNKNOWN, oMsgList );
}

SAPDB_Bool DBMCli_Parameters :: Refresh ( DBMCli_ParameterGroup   nGroup,
                                          Msg_List  & oMsgList )
{
  SAPDB_Bool bRC = false;
  
  SAPDB_Bool bClose = false;

  if( !m_bOpen ) {
    bClose = Open( oMsgList );
  }

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_PARAM_GETALL );
  sCmd += " ";
  sCmd += GetGroupOption( nGroup );
  
  Clear( );
  
  if ( oDB.Execute( sCmd, oMsgList ) ) {
    m_nGroup = nGroup;

    DBMCli_ResultBuf sLine;

    // read away the OK line
    oResult.GetLine(sLine);

    while( oResult.GetLine( sLine ) ) {
      DBMCli_String sName;
      DBMCli_String sType;
      DBMCli_String sValue;

      if( sLine.GetField( sName, "\t\n" ) ) {
        sName.Trim( );
        if( sLine.GetField( sType, "\t\n" ) ) {
          sType.Trim( );
          if( sLine.GetField( sValue, "\t\n" ) ) {
            sValue.Trim( );
          }
        }
      }

      DBMCli_Parameter oParameter( sName, sType, sValue );
      oParameter.SetDatabase( m_pDatabase );
      m_aParameter.Add( oParameter );
    
      sLine.Clear( );  
    }

    SAPDB_Int nParam = 0;
    SAPDB_Bool bRefresh = true;

    while( nParam < m_aParameter.GetSize( ) && bRefresh ) {
      bRefresh = m_aParameter[nParam].Refresh( oMsgList );
      nParam++;
    }

    bRC = bRefresh;
  }

  if( bClose ) {
    Close( false, oMsgList );
  }
   
  return bRC;
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: Remove
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameters :: Remove ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;
  
  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_PARAM_RMFILE );

  if( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: Restore
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameters :: Restore ( SAPDB_Int nVersion,
                                          Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;
  
  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd;
  
  sCmd.Format( "%s %d", DBMCLI_CMD_PARAM_RESTORE, nVersion );

  if( oDB.Execute( sCmd, oMsgList ) ) {
    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: Recover
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameters :: Recover ( const DBMCli_String & sMediumName,
                                          Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  DBMCli_String sCmd( DBMCLI_CMD_RECOVER_CONFIG );
  sCmd += " \"";
  sCmd += sMediumName;
  sCmd += "\"";

  if( oDB.SRVConnect( oMsgList ) ) {
    if( oDB.Execute( sCmd, oMsgList ) ) {
      bRC = true;
    }
    oDB.SRVRelease( oMsgList );
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: Check
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameters :: Check ( DBMCli_ParameterGroup    nGroup,
                                        DBMCli_ParameterStatus & nStatus,
                                        DBMCli_String          & sBadParam,
                                        DBMCli_String          & sOwnValue,
                                        DBMCli_String          & sCompValue,
                                        Msg_List   & oMsgList )
{
  SAPDB_Bool bRC = false;

  nStatus = DBMCLI_PARAMETERSTAT_OK;
  sBadParam.Empty();
  sOwnValue.Empty();
  sCompValue.Empty();
  
  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_PARAM_CHECKALL );
  sCmd += " ";
  sCmd += GroupNameFromGroup( nGroup );

  if( !oDB.Execute( sCmd, oMsgList ) ) {

    if( oResult.ReturnCode() == DBMCLI_SRV_RC_XPCHECK ) {

      oMsgList.ClearMessageList();

      nStatus = DBMCLI_PARAMETERSTAT_UNKNOWN;

      DBMCli_ResultBuf sLine;

      if ( oResult.GetLine( sLine ) ) {
        DBMCli_String sStatus;

        if( sLine.GetField( sBadParam, "\t\n" ) ) {
          sBadParam.Trim( );
          if( sLine.GetField( sStatus, "\t\n" ) ) {
            sStatus.Trim( );
            sStatus.MakeUpper( );

            if( sStatus == CHECK_ERR_REQUEST ) {
              nStatus = DBMCLI_PARAMETERSTAT_REQUEST;
            } else if( sStatus == CHECK_ERR_CONSTRAINT ) {
              nStatus = DBMCLI_PARAMETERSTAT_CONSTRAINT;
            } else if( sStatus == CHECK_ERR_MANDATORY ) {
              nStatus = DBMCLI_PARAMETERSTAT_MANDATORY;
            }

            if( oResult.GetLine( sOwnValue ) ) {
              if( oResult.GetLine( sCompValue ) ) {
                bRC = true;
              }
            }
          }
        }
      }
    }
  } else {
    bRC = true;
  }

  return bRC;
}
/*
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Parameters :: Check ( Msg_List   & oMsgList )
{
  DBMCli_ParameterStatus   nStatus;
  DBMCli_String            sBadParam;
  DBMCli_String            sOwnValue;
  DBMCli_String            sCompValue;
                                        
  return Check(DBMCLI_PARAMGROUP_GENERAL, nStatus, sBadParam, sOwnValue, sCompValue, oMsgList);
} // end DBMCli_Parameters :: Check

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: GroupFromGroupName
  -----------------------------------------------------------------------------
*/
DBMCli_ParameterGroup DBMCli_Parameters :: GroupFromGroupName ( const DBMCli_String & sGroup )
{
  DBMCli_String s( sGroup );

  s.Trim( );
  s.MakeUpper( );
  
  if( s == DBMCLI_OPT_PARAM_GENERAL ) {
    return DBMCLI_PARAMGROUP_GENERAL;
  } else if( s == DBMCLI_OPT_PARAM_EXTENDED ) {
    return DBMCLI_PARAMGROUP_EXTENDED;
  } else if( s == DBMCLI_OPT_PARAM_SUPPORT ) {
    return DBMCLI_PARAMGROUP_SUPPORT;
  } else {
    return DBMCLI_PARAMGROUP_UNKNOWN;
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: GroupNameFromGroup
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_Parameters :: GroupNameFromGroup ( const DBMCli_ParameterGroup nGroup )
{
  switch( nGroup ) {
    case DBMCLI_PARAMGROUP_GENERAL:
      return DBMCLI_OPT_PARAM_GENERAL;
    case DBMCLI_PARAMGROUP_EXTENDED:
      return DBMCLI_OPT_PARAM_EXTENDED;
    case DBMCLI_PARAMGROUP_SUPPORT:
      return DBMCLI_OPT_PARAM_SUPPORT;
    default:
      return DBMCLI_OPT_PARAM_GENERAL;
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: IndexByName
  -----------------------------------------------------------------------------
*/
SAPDB_Int DBMCli_Parameters :: IndexByName ( const DBMCli_String & sName )
{
  SAPDB_Bool bFound = false;
  SAPDB_Int nParam = 0;

  while( nParam < m_aParameter.GetSize( ) && !bFound ) {
    if( m_aParameter[nParam].Name( ) == sName ) {
      bFound = true;
    } else {
      nParam++;
    }
  }

  if( bFound ) {
    return nParam;
  } else {
    return -1;
  }
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: GetValueAsString
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_Parameters :: GetValueAsString ( const DBMCli_String & sName )
{
  DBMCli_String        sCmd;
  DBMCli_String        sValue;
  Msg_List oMsgList;

  sCmd = DBMCLI_CMD_PARAM_GETVALUE;
  sCmd += " ";
  sCmd += sName;

  if( GetDatabase().Execute( sCmd, oMsgList ) ) {
    if ( !GetDatabase().GetResult().GetLine(sValue) ) {
      sValue = "";
    } // end if
  }  // end if

  return sValue;

} // end DBMCli_Parameters :: GetValueAsString

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Parameters :: GetValueAsInt
  -----------------------------------------------------------------------------
*/
int DBMCli_Parameters :: GetValueAsInt ( const DBMCli_String & sName )
{
  return atoi(GetValueAsString(sName));
} // end DBMCli_Parameters :: GetValueAsInt

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Parameters :: GetGroupOption
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_Parameters :: GetGroupOption ( DBMCli_ParameterGroup nGroup )
{
  DBMCli_String sGroup;

  if( nGroup != DBMCLI_PARAMGROUP_UNKNOWN ) {
    
    sGroup = DBMCLI_OPT_PARAM_GROUP;

    if( nGroup & DBMCLI_PARAMGROUP_GENERAL ) {
      sGroup += DBMCLI_OPT_PARAM_GENERAL;
      sGroup += ",";  
    }
    if( nGroup & DBMCLI_PARAMGROUP_EXTENDED ) {
      sGroup +=  DBMCLI_OPT_PARAM_EXTENDED;
      sGroup += ",";
    }
    if( nGroup & DBMCLI_PARAMGROUP_SUPPORT ) {
      sGroup +=  DBMCLI_OPT_PARAM_SUPPORT;
      sGroup += ",";
    }

    sGroup = sGroup.Left( sGroup.GetLength( ) - 1 );
  }

  return sGroup;
}
