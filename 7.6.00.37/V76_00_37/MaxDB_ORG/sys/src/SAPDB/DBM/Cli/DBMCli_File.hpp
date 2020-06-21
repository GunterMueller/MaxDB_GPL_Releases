/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_File.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  File Class
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

#ifndef _DBMCLI_FILE_HPP_
#define _DBMCLI_FILE_HPP_

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
class DBMCli_File;
class DBMCli_Diagnosis;

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/

typedef enum 
{ 
	DBMCLI_FILEMODE_UNKNOWN  = 0,
	DBMCLI_FILEMODE_ASCII    = 1,
	DBMCLI_FILEMODE_BINARY   = 2
} DBMCli_FileMode;

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define DBMCLI_FILEMODE_STRING_ASCII "ASCII"
#define DBMCLI_FILEMODE_STRING_BINARY "BINARY"

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_File
  -----------------------------------------------------------------------------
  description: Class to handle protocol file access.
  -----------------------------------------------------------------------------
*/
class DBMCli_File 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_File
      -------------------------------------------------------------------------
      description:  Constructor
      porototypes:  2
      -------------------------------------------------------------------------
    */
    DBMCli_File ( );
    DBMCli_File ( const DBMCli_String   & sName,
                        DBMCli_FileMode   nMode,
                  const DBMCli_String   & sSize,
                  const DBMCli_String   & sModified,
                  const DBMCli_String   & sComment,
                  const DBMCli_String   & sFilename );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_File
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_File ( );

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
      function:     Open
      -------------------------------------------------------------------------
      description:  Open a protocol file.
      arguments:    sName    [IN]  - name of protocol file
                    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Open ( const DBMCli_String        & sName,
                            Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Close
      -------------------------------------------------------------------------
      description:  Close a protocol file.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Close ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     GetLine
      -------------------------------------------------------------------------
      description:  Get the next line from protocol file.
      arguments:    sLine    [OUT] - line from protocol file
                    oMsgList [OUT] - object for indication of errors
      return value: True if success and the end of file is not reached, 
                    otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool GetLine ( DBMCli_String        & sLine,
                         Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     GetPart
      -------------------------------------------------------------------------
      description:  Get the next part from protocol file.
      arguments:    sPart    [OUT] - part from protocol file
                    nMaxlen  [IN]  - max len of part
                    oMsgList [OUT] - object for indication of errors
      return value: True if success and the end of file is not reached, 
                    otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool GetPart ( DBMCli_String        & sPart,
                         SAPDB_Int              nMaxLen,
                         Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Name
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Name ( ) { return m_sName; };

    /*!
      -------------------------------------------------------------------------
      function:     Mode
      -------------------------------------------------------------------------
    */
    const DBMCli_FileMode Mode( ) { return m_nMode; };

    /*!
      -------------------------------------------------------------------------
      function:     Size
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Size( ) { return m_sSize; };

    /*!
      -------------------------------------------------------------------------
      function:     Modiefied
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Modified ( ) { return m_sModified; };

    /*!
      -------------------------------------------------------------------------
      function:     Comment
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Comment ( ) { return m_sComment; };

    /*!
      -------------------------------------------------------------------------
      function:     Filename
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Filename ( ) { return m_sFilename; };

  private:
    SAPDB_Bool Next ( Msg_List & oMsgList );


  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_String m_sName;
    DBMCli_String m_sHandle;

    SAPDB_Bool m_bContinue;
    SAPDB_Bool m_bOpen;

    DBMCli_FileMode m_nMode;

    DBMCli_String m_sSize;
    DBMCli_String m_sModified;
    DBMCli_String m_sComment;
    DBMCli_String m_sFilename;

}; 

typedef DBMCli_Array< DBMCli_File,      DBMCli_File      > DBMCli_FileArray;
/*! EndClass: DBMCli_File */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Files
  -----------------------------------------------------------------------------
  description: Class to handle protocol file access.
  -----------------------------------------------------------------------------
*/
class DBMCli_Files 
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Files
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Files ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Files
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Files ( );

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
      description:  Open a protocol file.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     FileArray 
      -------------------------------------------------------------------------
      description:  Get the array of available files.
      return value: Reference to a array of available files.
      -------------------------------------------------------------------------
    */
    DBMCli_FileArray & FileArray ( ) { return m_aFile; }; 

  private:
    void AssignFileMode ( const DBMCli_String   & sVal,
                                DBMCli_FileMode * pVal );

  private:
    DBMCli_Database * m_pDatabase;

    DBMCli_FileArray m_aFile;
}; 

/*! EndClass: DBMCli_Files */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Diagnosis
  -----------------------------------------------------------------------------
  description: Class to handle one diagnosis save.
  -----------------------------------------------------------------------------
*/
class DBMCli_Diagnosis 
{
  public:
    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Diagnosis
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Diagnosis ( ) {}
    DBMCli_Diagnosis ( const DBMCli_DateTime & oTimestamp,
                       const DBMCli_String   & sPath);

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Diagnosis
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Diagnosis ( ) {}

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
    DBMCli_Database & GetDatabase ( ) { return *m_pDatabase; }

    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      description:  refresh the list
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     FileArray 
      -------------------------------------------------------------------------
      description:  Get the array of available files.
      return value: Reference to a array of available files.
      -------------------------------------------------------------------------
    */
    DBMCli_FileArray & FileArray ( ) { return m_oFiles; }

    /*!
      -------------------------------------------------------------------------
      function:     GetPath
      -------------------------------------------------------------------------
    */
    const DBMCli_String & GetPath ( ) { return m_sPath; }

    /*!
      -------------------------------------------------------------------------
      function:     GetPath
      -------------------------------------------------------------------------
    */
    const DBMCli_DateTime & GetTimestamp ( ) { return m_oTimestamp; }

  private:
    DBMCli_Database  * m_pDatabase;
    DBMCli_FileArray   m_oFiles;

    DBMCli_String      m_sPath;
    DBMCli_DateTime    m_oTimestamp;

}; 

typedef DBMCli_Array< DBMCli_Diagnosis, DBMCli_Diagnosis > DBMCli_DiagnosisArray;

/*! EndClass: DBMCli_Diagnosis */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Diagnosises
  -----------------------------------------------------------------------------
  description: Class to handle diagnosis saves.
  -----------------------------------------------------------------------------
*/
class DBMCli_Diagnosises  {
  public:
    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Diagnosises
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Diagnosises ( ) {}

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Diagnosises
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Diagnosises ( ) {}

    /*!
      -------------------------------------------------------------------------
      function:     SetDatabase
      -------------------------------------------------------------------------
      description:  Set the database member.
      -------------------------------------------------------------------------
    */
    void SetDatabase ( DBMCli_Database * p ) { m_pDatabase = p; }

    /*!
      -------------------------------------------------------------------------
      function:     GetDatabase
      -------------------------------------------------------------------------
      description:  Get the database member.
      return value: Reference to the database object.
      -------------------------------------------------------------------------
    */
    DBMCli_Database & GetDatabase ( ) { return *m_pDatabase; }

    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      description:  refresh the list
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     DiagnosisArray 
      -------------------------------------------------------------------------
      description:  Get the array of available diagnosis saves.
      return value: Reference to a array of available files.
      -------------------------------------------------------------------------
    */
    DBMCli_DiagnosisArray & DiagnosisArray ( ) { return m_oDiagnosises; };

  private:
    DBMCli_Database       * m_pDatabase;
    DBMCli_DiagnosisArray   m_oDiagnosises;
}; 

/*! EndClass: DBMCli_Diagnosises */

#endif // _DBMCLI_FILE_HPP_

