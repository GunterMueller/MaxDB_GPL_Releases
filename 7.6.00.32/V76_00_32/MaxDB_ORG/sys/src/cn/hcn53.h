/*! 
  -----------------------------------------------------------------------------
 
  module: vcn53.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBA Action Protocol(s)
 
  description:  specification of the DBA History file

  version:      min 7.2.
 
  Copyright (c) 2001-2005 SAP AG
 
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



  -----------------------------------------------------------------------------
*/
 

#ifndef _HCN53_H_
#define _HCN53_H_

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "gsp00.h"
#include "gcn00_1.h"

/*!  
  -----------------------------------------------------------------------------
  Chapter: Macros and Constants
  -----------------------------------------------------------------------------
 */

/*! Declaration: DBA messages */

#define DBADTL_HEADER_CN53     "*** DBA Action starts:\nTimestamp: %s   Function: %s   Object: %s"
#define DBADTL_FOOTER_CN53     "*** DBA Action ends:\nTimestamp: %s   State:    %s"
#define DBADTL_BACMD_CN53      "*** SAVE/RESTORE request:\n%s"
#define DBADTL_BACMDOK_CN53    "*** SAVE/RESTORE request accepted:\n%s"
#define DBADTL_BACMDERR_CN53   "*** SAVE/RESTORE request rejected:\n%s"
#define DBADTL_BARESULT_CN53   "*** SAVE/RESTORE result:\n%s"
#define DBADTL_CDBCMD_CN53     "*** VERIFY command:\n%s"
#define DBADTL_CDBRESULT_CN53  "*** VERIFY result:\n%s"
    
/*! EndDeclaration: DBA messages */

/*! Declaration: DBA function ids */

// Autosave
#define DBAOP_DISAUTO_CN53      "ase"           // disable autosave log
#define DBAOP_ENAUTO_CN53       "aso"           // enable autosave log

// Backup
#define DBAOP_SAVEDATAC_CN53    "sdc"           // Complete database backup  (consistent)
#define DBAOP_SAVEDATA_CN53     "sda"           // Complete database backup
#define DBAOP_SAVEPAGESC_CN53   "spc"           // Incremental database backup (consistent)
#define DBAOP_SAVEPAGES_CN53    "spa"           // Incremental database backup
#define DBAOP_SAVELOG_CN53      "slo"           // Save archive logs
#define DBAOP_SAVESTAGE_CN53    "ssa"           // Save stanging area

// Restore
#define DBAOP_RESTDATA_CN53     "rda"           // Complete database restore
#define DBAOP_RESTPAGES_CN53    "rpa"           // Incremental database restore 
#define DBAOP_RESTLOG_CN53      "rlo"           // Restore archive logs
#define DBAOP_CHECKDATA_CN53    "cda"           // check complete database restore
#define DBAOP_CHECKPAGES_CN53   "cpa"           // check incremental database restore 
#define DBAOP_CHECKLOG_CN53     "clo"           // check restore archive logs

// Config
#define DBAOP_ADDDATVOL_CN53    "adv"           // add data volume
#define DBAOP_ADDLOGVOL_CN53    "alv"           // add log volume
#define DBAOP_CHANGECFG_CN53    "cco"           // change DB Konfiguration
#define DBAOP_LOADSYSTAB_CN53   "lst"           // load system tables

// check
#define DBAOP_CHECKDB_CN53      "cdb"           // check database structure
#define DBAOP_CHECKDBE_CN53     "cde"           // check database structure except
#define DBAOP_TABLECHECK_CN53   "cta"           // Check database structure (only tables) (xpu -v)

// statistics
#define DBAOP_OPTCHECK_CN53     "och"           // Check optimizer statistics
#define DBAOP_OPTCREATE_CN53    "osc"           // Create new optimizer statistics
#define DBAOP_OPTREFR_CN53      "osr"           // Refresh optimizer statistics
#define DBAOP_TABLESTAT_CN53    "cts"           // Create table statistics

/*! EndDeclaration: DBA function ids */

/*! Declaration: DBA objects */

#define DBA_BAOBJ_PAGES_CN53    "PAGES"         // backup/revover pages
#define DBA_BAOBJ_DATA_CN53     "DATA"          // backup/revover data
#define DBA_BAOBJ_LOG_CN53      "LOG"           // backup/revover log
#define DBA_OBJ_DB_CN53         "DATABASE"      // something done with the whole database
#define DBA_OBJ_PARAM_CN53      "PARAMETER"     // something done with a parameter
#define DBA_OBJ_VOLUME_CN53     "VOLUME"        // something done with a volume

/*! EndDeclaration: DBA objects */

/*!  
  -----------------------------------------------------------------------------
  EndChapter: Macros and Constants
  -----------------------------------------------------------------------------
 */


/*
  -----------------------------------------------------------------------------
  Class:        tcn53_DBAActionDetailLogFile
  -----------------------------------------------------------------------------

  description:  DBA Action Detail Log File Class

  -----------------------------------------------------------------------------
*/
class tcn53_DBAActionDetailLogFile :  public tin105_LogFile {

  protected:
    tcn53_DBAActionDetailLogFile (  ) : tin105_LogFile( LOG_ALL_MIN105 ) {}

    void setFile                 ( const tsp00_Pathc & szFilename ) { m_szCurrentFile = szFilename;}

    bool writeOneEventList       ( const Msg_List & EvtLst);

    tsp00_Pathc m_szCurrentFile;

    friend class tcn53_DBAAction;

};

/* EndClass: tcn53_DBAActionDetailLogFile */

/*!
  -----------------------------------------------------------------------------
  Class:        tcn53_DBAAction
  -----------------------------------------------------------------------------

  description:  DBA Action Log Class

  -----------------------------------------------------------------------------
*/
class tcn53_DBAAction {

  public:
    /*!
      -------------------------------------------------------------------------
      function:     tcn53_DBAAction :: start
      -------------------------------------------------------------------------
      description:  starts a DBA action

      arguments:    szDbName [IN] - name of database
                    szBegin  [IN] - begin of action timestamp ("YYYYMMDDhhmmss")
                    szFuncID [IN] - ID of function (see DBA function ids)
                    szObject [IN] - object of function (see DBA objects)

      prototypes:   2
      -------------------------------------------------------------------------
     */
    static void start         ( const tsp00_DbNamec    & szDbName,
                                const tsp00_Timestampc & szBegin,
                                const char *             szFuncID,
                                const char *             szObject );
    static void start         ( const tsp00_DbNamec    & szDbName,
                                const char *             szFuncID,
                                const char *             szObject );
    /*!
      -------------------------------------------------------------------------
      function:     tcn53_DBAAction :: finish
      -------------------------------------------------------------------------
      description:  finishs the action

                    writes a record to the DBA Action Log

      arguments:    szEnd    [IN] - end of action timestamp ("YYYYMMDDhhmmss")
                    bOK      [IN] - action result (success or not)

      prototypes:   2
      -------------------------------------------------------------------------
     */
    static void finish        ( const tsp00_Timestampc & szEnd,
                                bool                     bOK );
    static void finish        ( bool                     bOK );

    /*!
      -------------------------------------------------------------------------
      function:     tcn53_DBAAction :: writeDetail
      -------------------------------------------------------------------------
      description:  writes the text or eventlist to the DBA Action Detail Log 

      arguments:    EventList [IN] - EventList to write
                    szFormat  [IN] - messagetext or formatstring with %s
                    p*        [IN] - %s arguments for the formatstring
      
      prototypes:   2
      -------------------------------------------------------------------------
    */
    static void writeDetail( const Msg_List & EventList);
    static void writeDetail( const char             * szFormat,
                             const char             * p0 = NULL,
                             const char             * p1 = NULL,
                             const char             * p2 = NULL,
                             const char             * p3 = NULL,
                             const char             * p4 = NULL,
                             const char             * p5 = NULL,
                             const char             * p6 = NULL,
                             const char             * p7 = NULL,
                             const char             * p8 = NULL,
                             const char             * p9 = NULL);

    /*!
      -------------------------------------------------------------------------
      function:     enable
      -------------------------------------------------------------------------
      description:  enable DBAHist

      arguments:    bEnable     [IN] - set the state
      -------------------------------------------------------------------------
     */
    static void enable     (const bool bEnable = true );

  protected:
    tcn53_DBAAction     ( const tsp00_DbNamec    & szDbName );

    void startAction    ( const tsp00_Timestampc & szBegin,
                          const char *             szFuncID,
                          const char *             szObject );

    void finishAction   ( const tsp00_Timestampc & szEnd,
                          bool                     bOK );

    void writeAction    ( const tsp00_Timestampc & szEnd,
                          bool                     bOK,
                          bool                     bFinish);

    static const tsp00_Timestampc & timestamp ( tsp00_Timestampc & szTimestamp );

    // initialize file
    bool openFile        ( const tsp00_Pathc   szFilename,
                                tsp00_Int4   & hFile );

    // forbid copy and assignment
    tcn53_DBAAction(const tcn53_DBAAction& aFile) {}
    tcn53_DBAAction& operator = (const tcn53_DBAAction& aFile) {return *this;}

    tsp00_DbNamec    m_szDbName;
    tsp00_Timestampc m_szBegin;   
    tsp00_C3c        m_szFuncID;    
    tsp00_C14c       m_szObject;    
    
    tsp00_Pathc      m_szFile;
    tsp00_Pathc      m_szDetailFile;
    
    static tcn53_DBAAction   * m_pMySelf;
    static bool                m_bEnabled;

    tcn53_DBAActionDetailLogFile m_oDetailLogFile;
    tin100_Log                   m_oDetailLog;
};

/*! EndClass: tcn53_DBAAction */

/*!
   Example: Example of a DBA Action

   description: Using the DBA Action class

   code:        
                ...

                // here starts the dba action "autosave on"
                tsp00_Timestampc szTimeStamp;
                tsp00_DbNamec    szDbName = ...;

                cn90Timestamp(szTimeStamp);
                tcn53_DBAAction::start(szDbName, szTimeStamp, DBAOP_ENAUTO_CN53, DBA_BAOBJ_LOG_CN53);

                // do something 
                ...

                // write something to the detail protocol
                tcn53_DBAAction::writeDetail("Elch created");
                tcn53_DBAAction::writeDetail("The value of elch is %s!", szElch);

                // do something 
                ...

                // action is sucessfully finished
                cn90Timestamp(szTimeStamp);
                tcn53_DBAAction::finish(szTimeStamp, true);

                ...

*/

#endif  // _HCN53_H_
