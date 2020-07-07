/*! 
  -----------------------------------------------------------------------------
 
  module: hcn51.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer Configuration Value Class
   
  description:  DBMServer Configuration Value Class - Specification

  version:      7.2.

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

#ifndef _HCN51_H_
#define _HCN51_H_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "gsp00.h"

#include "Messages/Msg_List.hpp"
/*
  -----------------------------------------------------------------------------
  public macros
  -----------------------------------------------------------------------------
 */
#define CFG_PARAMLOGON_CN51        (*(tsp00_Namec *) "SAPDB0"            )
#define CFG_DBMUSR_CN51            (*(tsp00_Namec *) "SAPDB1"            )
#define CFG_DBMPWD_CN51            (*(tsp00_Namec *) "SAPDB2"            )
#define CFG_SYSUSR_CN51            (*(tsp00_Namec *) "SAPDB3"            )
#define CFG_SYSPWD_CN51            (*(tsp00_Namec *) "SAPDB4"            )
#define CFG_DBAPWD_CN51            (*(tsp00_Namec *) "SAPDB6"            )
#define CFG_SQLUSR_CN51            (*(tsp00_Namec *) "SAPDB7"            )
#define CFG_SQLPWD_CN51            (*(tsp00_Namec *) "SAPDB8"            )
#define CFG_DOMUSR_CN51            (*(tsp00_Namec *) "SAPDB10"           )
#define CFG_DOMPWD_CN51            (*(tsp00_Namec *) "SAPDB11"           )
#define CFG_DBADTL_CN51            (*(tsp00_Namec *) "DBADTL"            )
#define CFG_XLOAD_CN51             (*(tsp00_Namec *) "XLOAD"             )
#define CFG_PYTHON_CN51            (*(tsp00_Namec *) "PYTHON"            )
#define CFG_AUTOSSAVE_CN51         (*(tsp00_Namec *) "AUTOSAVE"          )
#define CFG_AUTOEXTFLAG_CN51       (*(tsp00_Namec *) "AutoExtFlag"       )
#define CFG_AUTOEXTDIR_CN51        (*(tsp00_Namec *) "AutoExtDir"        )
#define CFG_AUTOEXTDIRLOG_CN51     (*(tsp00_Namec *) "AutoExtDirLog"     )
#define CFG_AUTOEXTDIRMLOG_CN51    (*(tsp00_Namec *) "AutoExtDirLogMirror")
#define CFG_AUTOEXTSIZE_CN51       (*(tsp00_Namec *) "AutoExtSize"       )
#define CFG_AUTOEXTSIZELOG_CN51    (*(tsp00_Namec *) "AutoExtSizeLog"    )
#define CFG_RUNDISPATCHERFLAG_CN51 (*(tsp00_Namec *) "RunEventDispatcher")
#define CFG_CFGMIGRVOL_CN51        (*(tsp00_Namec *) "MIGRVOL"           )
#define CFG_CFGMIGRDB_CN51         (*(tsp00_Namec *) "MIGRDB"            )
#define CFG_UPDATESTATISTICSWHENREACHINGONLINE_CN51 (*(tsp00_Namec *) "UpdStatsWhenOnline")

/*
  -----------------------------------------------------------------------------
  declarations
  -----------------------------------------------------------------------------
 */
class tcn51ConfigEntry;

#define SIZE_OF_CONFIG_BUFFER 600

/*!
  -----------------------------------------------------------------------------
  Class:        cn51DBMConfigValue
  -----------------------------------------------------------------------------

  description:  Configuration Value

                An instance of this class represents an configuration value
                of the DBM. The assigment operator stores the value persistent
                on the hard disk. For reading the value use the casting operator
                for an assignment to variables of type long or char *.
                
  -----------------------------------------------------------------------------
 */
class cn51DBMConfigValue
{
  public:
    typedef enum {
      ModeInstance,
      ModeInstallation,
      ModeGlobal,
      ModeAll
    } ConfigurationMode;

    /*!  
      Chapter: Construction and destruction of class
     */

    /*!
      -------------------------------------------------------------------------
      function:     cn51DBMConfigValue
      -------------------------------------------------------------------------
      description:  Constructor

                    Reads the value of this to the name 

      arguments:    szValueName [IN] - the name if the value
                    szDBName    [IN] - the name of the database, all values
                                       will be stored individually for
                                       a database
                    oMode       [IN] - The mode which configuration file will be used.
                                       ModeAll means read from all files and write to instance file.
                                       
      -------------------------------------------------------------------------
     */
    cn51DBMConfigValue ( const char        * szDbName, 
                         const char        * szValueName,
                         ConfigurationMode   oMode = ModeAll);
    /*!  
      EndChapter: Construction and destruction of class
     */

    /*!  
      Chapter: Writing values
     */

    /*!
      -------------------------------------------------------------------------
      function:     operator = (tsp00_Int4)
      -------------------------------------------------------------------------
      description:  Assignment operator for tsp00_Int4
      
                    This assignment operator is used for data source of type
                    "tsp00_Int4". The value is immediately stored on disk.
                    The value will be stored in crypted representation.

      arguments:    cfgValue [IN] - the new value as long

      return value: the object itself
      -------------------------------------------------------------------------
     */
    cn51DBMConfigValue operator =            (const tsp00_Int4 cfgValue  );

    /*!
      -------------------------------------------------------------------------
      function:     operator = (const char *)
      -------------------------------------------------------------------------
      description:  Assignment operator for "const char *"
      
                    This assignment operator is used for data source of type
                    "string". The value wille be immediately stored on disk. 
                    Values longer than 512 Bytes will be truncated without warning
                    or error message.

                    The value will be stored in clear representation.

      arguments:    cfgValue [IN] - the new value as zero terminated string

      return value: the object itself
      -------------------------------------------------------------------------
     */
    cn51DBMConfigValue operator =            (const char * cfgValue );

    /*!  
      EndChapter: Writing values
     */

    /*!  
      Chapter: Reading values
     */

    /*!
      -------------------------------------------------------------------------
      function:     operator tsp00_Int4
      -------------------------------------------------------------------------
      description:  Casting operator for tsp00_Int4
      
                    This operator provide the value as an Object of type "tsp00_Int4".
                    The value will be read from the disk!
      -------------------------------------------------------------------------
     */
    operator tsp00_Int4   ( );

    /*!
      -------------------------------------------------------------------------
      function:     operator const char *
      -------------------------------------------------------------------------
      description:  Casting operator for "const char *"
      
                    This operator provide the value as an Object of type "tsp00_NameC".
                    The value will be read from the disk!
      return value: A pointer to the buffer with the configuration value. This
                    buffer becomes invalid after destroying the configuration
                    value object. If the value does not exist this buffer will
                    contain an empty string. 
      -------------------------------------------------------------------------
     */
    operator const char * ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetName
      -------------------------------------------------------------------------
      description:  read the name of the value

      return value: pointer to null terminated string containing the name
      -------------------------------------------------------------------------
     */
    const char * GetName() const { return m_szName.asCharp();}

    /*!  
      EndChapter: Reading values
     */

     /*!  
      Chapter: Error handling
     */

    /*!
      -------------------------------------------------------------------------
      function:     LastEvent
      -------------------------------------------------------------------------
      description:  returns the last error message

      return value: the event object
      -------------------------------------------------------------------------
     */
    const Msg_List & LastEvent( ){ return m_LastEvent;}

     /*!  
      EndChapter: Error handling
     */

    /*!  
      Chapter: Migration of file
     */

    /*!
      -------------------------------------------------------------------------
      function:     migrateFile
      -------------------------------------------------------------------------
      description:  migrates the configuration file

      arguments:    szDBName    [IN] - the name of the database, all values
                                       will be stored individually for
                                       a database
      -------------------------------------------------------------------------
     */
     static void migrateFile   ( const tsp00_DbNamec szDbName );

    /*!  
      EndChapter: Migration of file
     */

  private:
    tsp00_DbNamec    m_szDbName;
    tsp00_Namec      m_szName;

    ConfigurationMode m_oReadMode;
    ConfigurationMode m_oSaveMode;

    void saveValue ( const tsp00_DbNamec   szDbName,
                     const tsp00_Namec     szName, 
                     const tsp00_C512c     szValue ) const;

    char *  readValue ( const tsp00_DbNamec   szDbName,
                        const tsp00_Namec     szName, 
                        char                  szValue[SIZE_OF_CONFIG_BUFFER] ) const;
   

    static tcn51ConfigEntry * readFile ( const tsp00_DbNamec      szDbName,
                                         ConfigurationMode        oMode,
                                         bool                     bMigrate);
    static void               saveFile ( const tsp00_DbNamec      szDbName,
                                         ConfigurationMode        oMode,
                                         tcn51ConfigEntry       * pAnchor  );

    static Msg_List   m_LastEvent;

    static bool migrateValue(char * pValue);

    char m_szValue[SIZE_OF_CONFIG_BUFFER];

    friend class cn51DBMConfigValueIterator;

};

/*! EndClass: cn51DBMConfigValue */

/*!
  -----------------------------------------------------------------------------
  Class:        cn51DBMConfigValueIterator
  -----------------------------------------------------------------------------

  description:  Iterator for configuration values
                
  -----------------------------------------------------------------------------
 */
class cn51DBMConfigValueIterator {
  public:
    /*!  
      Chapter: Construction and destruction of class
     */

    /*!
      -------------------------------------------------------------------------
      function:     cn51DBMConfigValueIterator
      -------------------------------------------------------------------------
      description:  Constructor

                    creates the iterator 

      arguments:    szDBName    [IN] - the name of the database, all values
                                       will be stored individually for
                                       a database
                    oMode       [IN] - The mode which configuration file will be used.
                                       ModeAll means read from all files.
                                       
      -------------------------------------------------------------------------
     */
    cn51DBMConfigValueIterator 
        ( const char                           * szDbName, 
          cn51DBMConfigValue::ConfigurationMode  oMode = cn51DBMConfigValue::ModeAll);
    
    /*!
      -------------------------------------------------------------------------
      function:     ~cn51DBMConfigValueIterator
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
     */
    ~cn51DBMConfigValueIterator();

    /*!  
      EndChapter: Construction and destruction of class
     */

    /*!  
      Chapter: Iteration
     */

    /*!
      -------------------------------------------------------------------------
      function:     IsEnd
      -------------------------------------------------------------------------
      description:  check for end

      return value: true if iteartor at the end
      -------------------------------------------------------------------------
     */
    bool IsEnd(){return (m_pCurrent == NULL);}

    /*!
      -------------------------------------------------------------------------
      function:     Next
      -------------------------------------------------------------------------
      description:  go to the next entry
      -------------------------------------------------------------------------
     */
    void Next();

    /*!
      -------------------------------------------------------------------------
      function:     GotoFirst
      -------------------------------------------------------------------------
      description:  resets the iterator
      -------------------------------------------------------------------------
     */
    void GotoFirst();

    /*!
      -------------------------------------------------------------------------
      function:     Goto
      -------------------------------------------------------------------------
      description:  goto to the required value

                    IsEnd() will be return true after a Goto() which does not exists.
                    The name will be searched from the beginning of the iterator 
                    (implicit GotoFirst())

      arguments:    szName [IN] - name of the requiered value
      -------------------------------------------------------------------------
     */
    void Goto(const char * szName);

    /*!
      -------------------------------------------------------------------------
      function:     GetCurrentName
      -------------------------------------------------------------------------
      description:  read the name of the current entry

                    The GetCurrentName function will overwrite data retrieves 
                    by a former call of GetCurrentName.

      return value: pointer to the name
      -------------------------------------------------------------------------
     */
    const char * GetCurrentName() const;

    /*!
      -------------------------------------------------------------------------
      function:     GetCurrentValueAsString
      -------------------------------------------------------------------------
      description:  read the value of the current entry as string

                    The GetCurrentValueAsString and GetCurrentValueAsInt function will 
                    overwrite data retrieves by a former call of GetCurrentValueAsString.

      return value: pointer to the value
      -------------------------------------------------------------------------
     */
    const char * GetCurrentValueAsString() const;

    /*!
      -------------------------------------------------------------------------
      function:     GetCurrentValueAsInt
      -------------------------------------------------------------------------
      description:  read the value of the current entry as int

                    The GetCurrentValueAsString and GetCurrentValueAsInt function will 
                    overwrite data retrieves by a former call of GetCurrentValueAsString.

      return value: pointer to the value
      -------------------------------------------------------------------------
     */
    tsp00_Int4   GetCurrentValueAsInt() const;

    /*!  
      EndChapter: Iteration
     */

  private:
    const char * GetCurrentName(tcn51ConfigEntry * pCurrent) const;

    cn51DBMConfigValue::ConfigurationMode m_oReadMode;
    cn51DBMConfigValue::ConfigurationMode m_oCurrentMode;

    tcn51ConfigEntry *                    m_pAnchor;
    tcn51ConfigEntry *                    m_pCurrent;
};
/*! EndClass: cn51DBMConfigValueIterator */


#endif /* _HCN51_H_ */

