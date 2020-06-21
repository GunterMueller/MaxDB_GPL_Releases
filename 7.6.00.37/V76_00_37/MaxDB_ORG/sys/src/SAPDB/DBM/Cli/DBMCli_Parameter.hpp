/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Parameter.hpp
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

#ifndef _DBMCLI_PARAMETER_HPP_
#define _DBMCLI_PARAMETER_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"

/*
  -----------------------------------------------------------------------------
  froward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_Database;
class DBMCli_Parameter;

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum 
{ 
	DBMCLI_PARAMGROUP_UNKNOWN  = 0x0,
	DBMCLI_PARAMGROUP_GENERAL  = 0x1,
	DBMCLI_PARAMGROUP_EXTENDED = 0x2,
	DBMCLI_PARAMGROUP_SUPPORT  = 0x4,
	DBMCLI_PARAMGROUP_ALL      = 0x7
} DBMCli_ParameterGroup;

typedef DBMCli_Array< DBMCli_Parameter, DBMCli_Parameter > DBMCli_ParameterArray;

typedef enum 
{
  DBMCLI_PARAMETERSTAT_OK         = 0,
  DBMCLI_PARAMETERSTAT_REQUEST    = 1,
  DBMCLI_PARAMETERSTAT_CONSTRAINT = 2,
  DBMCLI_PARAMETERSTAT_MANDATORY  = 3,
  DBMCLI_PARAMETERSTAT_UNKNOWN    = 4
} DBMCli_ParameterStatus;
 
/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Parameter
  -----------------------------------------------------------------------------
  description: Handle a single database parameter.
  -----------------------------------------------------------------------------
*/
class DBMCli_Parameter
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Parameter
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    sName [IN]  - name of parameter
                    sType [IN]  - type of parameter
                    sValue [IN]  - value of parameter
      prototypes:   3 
      -------------------------------------------------------------------------
    */
    DBMCli_Parameter ( );

    DBMCli_Parameter ( const DBMCli_String & sName );

    DBMCli_Parameter ( const DBMCli_String & sName,
                       const DBMCli_String & sType, 
                       const DBMCli_String & sValue );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Parameter
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Parameter ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetDatabase
      -------------------------------------------------------------------------
      description:  Set the database member.
      -------------------------------------------------------------------------
    */
    void SetDatabase ( DBMCli_Database * p ) { m_pDatabase = p; };

    /*!
      -------------------------------------------------------------------------
      function:     GetDatabase
      -------------------------------------------------------------------------
      description:  Get the database member.
      return value: Reference to the database object.
      -------------------------------------------------------------------------
    */
    DBMCli_Database & GetDatabase ( ) { return *m_pDatabase; };

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear parameter informations.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Put
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Put ( const DBMCli_String & sValue,
                     Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     PutConfirm
      -------------------------------------------------------------------------
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool PutConfirm ( const DBMCli_String & sValue,
                            Msg_List & oMsgList );

    /*!  
      Chapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Name ( ) { return m_sName; };

    /*!
      -------------------------------------------------------------------------
      function:     Type
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Type ( ) { return m_sType; };

    /*!
      -------------------------------------------------------------------------
      function:     Value
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Value ( ) { return m_sValue; };

    /*!
      -------------------------------------------------------------------------
      function:     Default
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Default ( ) { return m_sDefault; };

    /*!
      -------------------------------------------------------------------------
      function:     Change
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Change ( ) { return m_bChange; };

    /*!
      -------------------------------------------------------------------------
      function:     Intern
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Intern ( ) { return m_bIntern; };

    /*!
      -------------------------------------------------------------------------
      function:     Mandatory
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Mandatory ( ) { return m_bMandatory; };

    /*!
      -------------------------------------------------------------------------
      function:     NoCopy (Clear)
      -------------------------------------------------------------------------
    */
    SAPDB_Bool NoCopy ( ) { return m_bNoCopy; };

    /*!
      -------------------------------------------------------------------------
      function:     Dynamic
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Dynamic ( ) { return m_bDynamic; };

    /*!
      -------------------------------------------------------------------------
      function:     Casesensitive
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Casesensitive ( ) { return m_bCasesensitive; };

    /*!
      -------------------------------------------------------------------------
      function:     Override
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Override ( ) { return m_bOverride; };

    /*!
      -------------------------------------------------------------------------
      function:     DevSpace
      -------------------------------------------------------------------------
    */
    SAPDB_Bool DevSpace ( ) { return m_bDevSpace; };

    /*!
      -------------------------------------------------------------------------
      function:     Modify
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Modify ( ) { return m_bModify; };

    /*!
      -------------------------------------------------------------------------
      function:     Group
      -------------------------------------------------------------------------
    */
    DBMCli_ParameterGroup Group ( ) { return m_nGroup; };

    /*!
      -------------------------------------------------------------------------
      function:     GroupName
      -------------------------------------------------------------------------
    */
    const DBMCli_String & GroupName ( ) { return m_sGroup; };

    /*!
      -------------------------------------------------------------------------
      function:     DisplayName
      -------------------------------------------------------------------------
    */
    const DBMCli_String & DisplayName ( ) { return m_sDisplayName; };

    /*!
      -------------------------------------------------------------------------
      function:     ValueSet
      -------------------------------------------------------------------------
    */
    const DBMCli_String & ValueSet ( ) { return m_sValueSet; };

    /*!
      -------------------------------------------------------------------------
      function:     Max
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Max ( ) { return m_sMax; };

    /*!
      -------------------------------------------------------------------------
      function:     Min
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Min ( ) { return m_sMin; };

    /*!
      -------------------------------------------------------------------------
      function:     Instances
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Instances ( ) { return m_sInstances; };

    /*!
      -------------------------------------------------------------------------
      function:     LastKnownGood
      -------------------------------------------------------------------------
    */
    const DBMCli_String & LastKnownGood ( ) { return m_sLastKnownGood; };

    /*!
      -------------------------------------------------------------------------
      function:     Help
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Help ( ) { return m_sHelp; };

    /*!
      -------------------------------------------------------------------------
      function:     Explain
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Explain ( ) { return m_sExplain; };

    /*!  
      EndChapter: Properties
    */

  private:
    void AssignProp ( const DBMCli_String & sProp,
                      const DBMCli_String & sVal );

    void AssignBool ( const DBMCli_String & sVal,
                            SAPDB_Bool          * pVal );

    void AssignGroup ( const DBMCli_String         & sVal,
                             DBMCli_ParameterGroup * pVal );

  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_String m_sName;
    DBMCli_String m_sType;
    DBMCli_String m_sValue;
    DBMCli_String m_sDefault;

    SAPDB_Bool m_bChange;
    SAPDB_Bool m_bIntern;
    SAPDB_Bool m_bMandatory;
    SAPDB_Bool m_bNoCopy;
    SAPDB_Bool m_bDynamic;
    SAPDB_Bool m_bCasesensitive;
    SAPDB_Bool m_bOverride;
    SAPDB_Bool m_bDevSpace;
    SAPDB_Bool m_bModify;
    DBMCli_ParameterGroup m_nGroup;
    DBMCli_String m_sGroup;
    DBMCli_String m_sDisplayName;
    DBMCli_String m_sValueSet;
    DBMCli_String m_sMax;
    DBMCli_String m_sMin;
    DBMCli_String m_sInstances;
    DBMCli_String m_sLastKnownGood;

    DBMCli_String m_sHelp;
    DBMCli_String m_sExplain;
}; 

/*! EndClass: DBMCli_Parameter */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Parameters
  -----------------------------------------------------------------------------
  description: Handle database parameters access.
  -----------------------------------------------------------------------------
*/
class DBMCli_Parameters 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Parameters
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Parameters ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Parameters
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Parameters ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetDatabase
      -------------------------------------------------------------------------
      description:  Set the database member.
      -------------------------------------------------------------------------
    */
    void SetDatabase ( DBMCli_Database * p ) { m_pDatabase = p; };

    /*!
      -------------------------------------------------------------------------
      function:     GetDatabase
      -------------------------------------------------------------------------
      description:  Get the database member.
      return value: Reference to the database object.
      -------------------------------------------------------------------------
    */
    DBMCli_Database & GetDatabase ( ) { return *m_pDatabase; };

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear the parameters object members.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     Open
      -------------------------------------------------------------------------
      description:  Open a parameter session.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Open ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Init
      -------------------------------------------------------------------------
      description:  Initialize parameters.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Init ( const DBMCli_String & sInstanceType, 
                      Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Copy
      -------------------------------------------------------------------------
      description:  Copy parameters from an existing database.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Copy ( const DBMCli_String & sDatabaseName, 
                      Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Close
      -------------------------------------------------------------------------
      description:  Close a parameter session.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Close ( SAPDB_Bool             bCommit,
                       Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      description:  Refresh the the parameters.
      arguments:    nGroup   [IN]  - single group or combination of group.
                    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    SAPDB_Bool Refresh ( DBMCli_ParameterGroup nGroup,
                         Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Remove
      -------------------------------------------------------------------------
      description:  Remove the parameter file.
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Remove ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Restore
      -------------------------------------------------------------------------
      description:  Restore a parameter version file.
      arguments:    nVersion [IN]  - version number of param file.
                    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Restore ( SAPDB_Int nVersion,
                         Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Recover
      -------------------------------------------------------------------------
      description:  Recover a parameter file from a medium.
      arguments:    nVersion [IN]  - version number of param file.
                    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Recover ( const DBMCli_String & sMediumName,
                         Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Check
      -------------------------------------------------------------------------
      description:  Check the paramaters.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Check ( DBMCli_ParameterGroup    nGroup,
                       DBMCli_ParameterStatus & nStatus,
                       DBMCli_String          & sBadParam,
                       DBMCli_String          & sOwnValue,
                       DBMCli_String          & sCompValue,
                       Msg_List   & oMsgList );

    SAPDB_Bool Check ( Msg_List   & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Group
      -------------------------------------------------------------------------
      description:  Get the current group or group combination.
      return value: Parameter group.
      -------------------------------------------------------------------------
    */
    DBMCli_ParameterGroup Group( ) { return m_nGroup; }; 

    /*!
      -------------------------------------------------------------------------
      function:     ParameterArray
      -------------------------------------------------------------------------
      description:  Get the array of parameters.
      return value: Reference to an array of parameters objects.
      -------------------------------------------------------------------------
    */
    DBMCli_ParameterArray & ParameterArray ( ) { return m_aParameter; }; 

    /*!
      -------------------------------------------------------------------------
      function:     GroupFromGroupName
      -------------------------------------------------------------------------
      description:  Get the group value from a string.
      return value: Group value.
      -------------------------------------------------------------------------
    */
    DBMCli_ParameterGroup GroupFromGroupName ( const DBMCli_String & sGroup );

    /*!
      -------------------------------------------------------------------------
      function:     GroupNameFromGroup
      -------------------------------------------------------------------------
      description:  Get the group name from a group value.
      return value: Group name.
      -------------------------------------------------------------------------
    */
    DBMCli_String GroupNameFromGroup ( const DBMCli_ParameterGroup nGroup );

    /*!
      -------------------------------------------------------------------------
      function:     IndexByName
      -------------------------------------------------------------------------
      description:  Get a index from a parameter by the name.
      return value: Index of parameter in array.
      -------------------------------------------------------------------------
    */
    int IndexByName ( const DBMCli_String & sName );

    /*!
      -------------------------------------------------------------------------
      function:     GetValueAsString
      -------------------------------------------------------------------------
      description:  Get the value of the parameter as String
      return value: the value
      -------------------------------------------------------------------------
    */
    DBMCli_String GetValueAsString ( const DBMCli_String & sName );

    /*!
      -------------------------------------------------------------------------
      function:     GetValueAsInt
      -------------------------------------------------------------------------
      description:  Get the value of the parameter as Integer
      return value: the value
      -------------------------------------------------------------------------
    */
    int GetValueAsInt ( const DBMCli_String & sName );

  private:
    DBMCli_String GetGroupOption ( DBMCli_ParameterGroup nGroup );

  private:
    DBMCli_Database * m_pDatabase;

    SAPDB_Bool m_bOpen;

    DBMCli_ParameterGroup m_nGroup;
    DBMCli_ParameterArray m_aParameter;
}; 

/*! EndClass: DBMCli_Parameters */

#endif // _DBMCLI_PARAMETER_HPP_

