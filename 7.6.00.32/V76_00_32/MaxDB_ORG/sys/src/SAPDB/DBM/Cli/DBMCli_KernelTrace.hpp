/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_KernelTrace.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  KernelTrace Class
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

#ifndef _DBMCLI_KERNELTRACE_HPP_
#define _DBMCLI_KERNELTRACE_HPP_

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
class DBMCli_KernelTraceOption;
class DBMCli_KernelTraceProtOpt;

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef DBMCli_Array< DBMCli_KernelTraceOption, DBMCli_KernelTraceOption > DBMCli_KernelTraceOptionArray;
typedef DBMCli_Array< DBMCli_KernelTraceProtOpt, DBMCli_KernelTraceProtOpt > DBMCli_KernelTraceProtOptArray;

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_KernelTraceOption
  -----------------------------------------------------------------------------
  description: Class to handle kernel trace options.
  -----------------------------------------------------------------------------
*/
class DBMCli_KernelTraceOption 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_KernelTraceOption
      -------------------------------------------------------------------------
      description:  Constructor
      porototypes:  2
      -------------------------------------------------------------------------
    */
    DBMCli_KernelTraceOption ( );
    DBMCli_KernelTraceOption ( const DBMCli_String  & sName1,
                               const DBMCli_String  & sName2,
                               const DBMCli_String  & sName3,
                               const DBMCli_String  & sDescription,
                               const DBMCli_String  & sValue );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_KernelTraceOption
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_KernelTraceOption ( );

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Name ( SAPDB_Int nNum );

    /*!
      -------------------------------------------------------------------------
      function:     Name1-3
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Name1 ( ) { return m_sName1; };
    const DBMCli_String & Name2 ( ) { return m_sName2; };
    const DBMCli_String & Name3 ( ) { return m_sName3; };

    /*!
      -------------------------------------------------------------------------
      function:     Description
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Description( ) { return m_sDescription; };

    /*!
      -------------------------------------------------------------------------
      function:     Value
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Value ( ) { return m_sValue; };

    /*!
      -------------------------------------------------------------------------
      function:     SetValue
      -------------------------------------------------------------------------
    */
    void SetValue ( const DBMCli_String & sValue ) { m_sValue = sValue; };

    /*!
      -------------------------------------------------------------------------
      function:     Selected
      -------------------------------------------------------------------------
    */
    const SAPDB_Bool Selected ( ) { return m_bSelected; };

    /*!
      -------------------------------------------------------------------------
      function:     SetSelected
      -------------------------------------------------------------------------
    */
    void SetSelected ( const SAPDB_Bool bSelected) { m_bSelected = bSelected; };


  private:
    DBMCli_String m_sName1;
    DBMCli_String m_sName2;
    DBMCli_String m_sName3;
    DBMCli_String m_sDescription;
    DBMCli_String m_sValue;

    SAPDB_Bool    m_bSelected;
}; 

/*! EndClass: DBMCli_KernelTraceOption */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_KernelTraceProtOpt
  -----------------------------------------------------------------------------
  description: Class to handle kernel trace protocol options.
  -----------------------------------------------------------------------------
*/
class DBMCli_KernelTraceProtOpt 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_KernelTraceProtOpt
      -------------------------------------------------------------------------
      description:  Constructor
      porototypes:  2
      -------------------------------------------------------------------------
    */
    DBMCli_KernelTraceProtOpt ( );
    DBMCli_KernelTraceProtOpt ( const DBMCli_String  & sOption,
                                const DBMCli_String  & sDescription );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_KernelTraceProtOpt
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_KernelTraceProtOpt ( );

    /*!
      -------------------------------------------------------------------------
      function:     Option
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Option( ) { return m_sOption; };

    /*!
      -------------------------------------------------------------------------
      function:     Description
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Description( ) { return m_sDescription; };

    /*!
      -------------------------------------------------------------------------
      function:     Selected
      -------------------------------------------------------------------------
    */
    const SAPDB_Bool Selected ( ) { return m_bSelected; };

    /*!
      -------------------------------------------------------------------------
      function:     SetSelected
      -------------------------------------------------------------------------
    */
    void SetSelected ( const SAPDB_Bool bSelected) { m_bSelected = bSelected; };


  private:
    DBMCli_String m_sOption;
    DBMCli_String m_sDescription;

    SAPDB_Bool    m_bSelected;
}; 

/*! EndClass: DBMCli_KernelTraceProtOpt */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_KernelTrace
  -----------------------------------------------------------------------------
  description: Class to handle KernelTrace.
  -----------------------------------------------------------------------------
*/
class DBMCli_KernelTrace
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_KernelTrace
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_KernelTrace ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_KernelTrace
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_KernelTrace ( );

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
      function:     Refresh
      -------------------------------------------------------------------------
      description:  Refresh the array of kernel trace options.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     OptionsOn
      -------------------------------------------------------------------------
      description:  Turn slected options on.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool OptionsOn ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     OptionsOff
      -------------------------------------------------------------------------
      description:  Turn slected options off.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool OptionsOff ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Make
      -------------------------------------------------------------------------
      description:  Make kernel trace protocol.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Make ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Flush
      -------------------------------------------------------------------------
      description:  Flush kernel trace.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Flush ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear kernel trace.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Clear ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     IndexByOption
      -------------------------------------------------------------------------
      description:  Get a index from a option by the option name.
      return value: Index of option in array.
      -------------------------------------------------------------------------
    */
    SAPDB_Int IndexByName ( const DBMCli_String & sName, 
                            SAPDB_Int             nNum );

    /*!
      -------------------------------------------------------------------------
      function:     KernelTraceOptionArray 
      -------------------------------------------------------------------------
      description:  Get the array of available kernel trace options.
      return value: Reference to a array of available KernelTraceOptions.
      -------------------------------------------------------------------------
    */
    DBMCli_KernelTraceOptionArray & KernelTraceOptionArray ( ) { return m_aKernelTraceOption; }; 

    /*!
      -------------------------------------------------------------------------
      function:     KernelTraceProtOptArray 
      -------------------------------------------------------------------------
      description:  Get the array of available kernel trace protocol options.
      return value: Reference to a array of available kernel trace protocol 
                    options.
      -------------------------------------------------------------------------
    */
    DBMCli_KernelTraceProtOptArray & KernelTraceProtOptArray ( ) { return m_aKernelTraceProtOpt; }; 

  private:    
    void AddOption ( const DBMCli_String & sName1, 
                     const DBMCli_String & sName2,
                     const DBMCli_String & sName3,
                     const DBMCli_String & sDescription,
                     const DBMCli_String & sValue );

    void SetAllOptions ( const DBMCli_String & sValue );

    void SetOption ( const DBMCli_String & sOption, 
                     const DBMCli_String & sValue );

    const DBMCli_String OptionSpecList( const DBMCli_String & sAction );

    const DBMCli_String ProtOptSpecList( );

  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_KernelTraceOptionArray m_aKernelTraceOption;
    DBMCli_KernelTraceProtOptArray m_aKernelTraceProtOpt;
}; 

/*! EndClass: DBMCli_KernelTrace */

#endif // _DBMCLI_KERNELTRACE_HPP_

