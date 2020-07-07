/*


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
/* @lastChanged: "1998-06-24  13:15"
 *
 * @filename:    gcn00.h
 * @purpose:     "common header file for cn modules"
 * @release:     7.1. / 6.2.
 * @see:         "-.-"
 *
 * @copyright:   "Copyright by SAP AG, 1998-2004"
 *
 * ==================================================================
 *
 * responsible:   BerndV
 *
 * created:       1998-01-20 by BerndV
 *
 * purpose:       common header file for cn modules
 *
 * notice:        Wer Funktionen nachmacht oder verfaelscht oder sich
 *                nachgemachte oder verfaelschte Funktionen verschafft,
 *                um sie in Verkehr zu bringen wird mit nicht unter
 *                3 Jahren CONTROL bestraft!
 *
 * ==================================================================
 */

#ifndef _GCN00_H_
#define _GCN00_H_

/* ------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------
 */

#include <stdio.h>
#include "geo47.h"

#include "gsp00.h"
#include "geo572.h"
#include "gcn00_1.h"

#include "ToolsCommon/Tools_Arguments.hpp"

/***** Memory Debug BEGIN *****/
#ifdef BVMEMDEBUG
#pragma message ("Memory Debug ON")
#include <crtdbg.h>
#endif
/***** Memory Debug END *****/

#define CHAR_P_CAST_CN00
#define UCHAR_P_CAST_CN00


/* ------------------------------------------------------------------
 * memory management definitions
 * ------------------------------------------------------------------
 * Use BVMEMDEBUG only if your under NT 4.0 using MS Visual C(++) and if
 * you have specified the library libcmtd instead of libcmt for the
 * link process.
 *
 * In case of disregard you will get several errors while compiling an
 * linking.
 *
 * void cnMyMalloc (
 *       long    nSize,      IN
 *       void ** pPointer,   OUT
 *       BOOLEAN bOK);       OUT
 *
 * void cnMyFree (
 *       void *  pPointer);  IN
 *
 */
//  #define cn00MyMalloc(a, b, c ) sqlallocat(a, (tsp00_Uint1 **) b, (tsp00_Bool *) c); \
//                                 if ((*b != NULL) && (*c)) memset(*b, '\0', a)
//  #define cn00MyFree(a)          sqlfree((tsp00_Uint1 *) a)

//  #define cn00MyMalloc(a, b, c ) {char ** xyz = (char **) b; *xyz = new char [a]; \
//                                 if (*xyz != NULL) {memset(*xyz, '\0', a); *c = true;}}

  #define cn00MyMalloc(a, b, c ) {                                                                          \
                                   char * xyz; xyz = new char [a];                                          \
                                   if (xyz != NULL) {memset(xyz, '\0', a); *c = true;*((char **) b) = xyz;} \
                                   else { *b = NULL;}                                                       \
                                 }

  #define cn00MyFree(a)          delete [] ((char *) a);

/* ------------------------------------------------------------------
 * SPECIFICATION DATATYPES, CONSTANTS AND MACROS for vcn**c
 * ------------------------------------------------------------------
 */
#define  NAMESIZE_CN00 100

class cn50DBMUser;          //forward declaration to avoid inclusion of  vcn50.h
class DBMSrvBHist_HistorySession;  //forward declaration to avoid inclusion of  DBMSrvBHist_HistorySession.hpp
class DBMSrvBHist_EBIDList; //forward declaration to avoid inclusion of  DBMSrvBHist_EBIDList.hpp
class DBMSrvHSS_Nodes;      //forward declaration to avoid include
class DBMSrvMsg_Error;

class VControlDataT {
public:
    VControlDataT()
        : m_Stopped(false),
          bIsSAPDBAdmi(false),
          bIsLocalCommunication(false),
          bIsDropped(false),
          active(false),
          pXParamSection(NULL),
          pBackupSession(NULL),
          pBackupStageSession(0),
          pHistorySession(0),
          pEBIDList(0),
          pHSSNodes(NULL),
          pCommandObj(NULL),
          pParamHistory(NULL),
          pfCancelFlag(false),
          pCurrentUser(NULL),
          bUnicode(false),
          nNextCommandSkip(0),
          m_ClientPIDonClientMachine(0),
          m_ClientMachineID("")
    {
    }

    /*! @brief some initialisation, never fails, just until DBMSrvFrm_Server replaces VControlDataT */
    void start();

    /*! @brief clean up all resources and be able to report errors, just until DBMSrvFrm_Server replaces VControlDataT */
    bool stop( DBMSrvMsg_Error& errOut);

    ~VControlDataT();
public:
    bool                   bIsSAPDBAdmi;

    /*! @brief true if local communication between client and DBM server, false otherwise */
    bool                   bIsLocalCommunication;

    /*! @brief true if database was dropped successfully with this dbmserver, false otherwise */
    bool                   bIsDropped;
    bool                   active;
    tsp00_DbNamec          dbname;
    tsp00_Pathc            dbroot;
    tsp00_XpKeyTypec       szLastParameter;
    cn50DBMUser          * pCurrentUser;
    void                 * pXParamSection;
    void                 * pBackupSession;
    void                 * pBackupStageSession;
    DBMSrvBHist_HistorySession * pHistorySession;
    DBMSrvBHist_EBIDList * pEBIDList;
    DBMSrvHSS_Nodes      * pHSSNodes;
public:
    void                 * pCommandObj;
    void                 * pParamHistory;
    bool                 * pfCancelFlag;
    bool                   bUnicode;
    tsp00_C512c            szNextCommand;
    int                    nNextCommandSkip;
    SAPDB_Int8              m_ClientPIDonClientMachine;
    Tools_DynamicUTF8String m_ClientMachineID;

private:
    bool m_Stopped;
};

class CommandT {
public:
    CommandT() {
      nIndex    = CommandT::InvalidIndex;
      args      = NULL; 
    } // end CommandT

    void setArgs(const char * pCommand) 
    {
        args = pCommand;
        oArguments.setArguments(pCommand);
    } // end setArgs

    int              nIndex;
    const char *     args;
    Tools_Arguments  oArguments;
    static const int InvalidIndex;
};

class cn00DBMServerData {
  public:
    static VControlDataT * vcontrol() {return m_vcontrol;}
    static void setVcontrol(VControlDataT * vcontrol) {m_vcontrol = vcontrol;}

  private:
    static VControlDataT * m_vcontrol;

}; // end class cn00DBMServerData

/* error codes and error handling */

#define OK_CN00                         cn00_1_ErrId(OK_CN00_1)

#define ERR_CN00                        cn00_1_ErrId(ERR_CN00_1)
#define ERR_FILE_CN00                   cn00_1_ErrId(ERR_FILE_CN00_1)
#define ERR_PARAM_CN00                  cn00_1_ErrId(ERR_PARAM_CN00_1)
#define ERR_MEM_CN00                    cn00_1_ErrId(ERR_MEM_CN00_1)
#define ERR_NOHISTORYSESSION_CN00       cn00_1_ErrId(ERR_NOHISTORYSESSION_CN00_1)
#define ERR_TOOSMALL_CN00               cn00_1_ErrId(ERR_TOOSMALL_CN00_1)
#define ERR_NOMEDIA_CN00                cn00_1_ErrId(ERR_NOMEDIA_CN00_1)
#define ERR_MENOTFOUND_CN00             cn00_1_ErrId(ERR_MENOTFOUND_CN00_1)
#define ERR_MENOVALID_CN00              cn00_1_ErrId(ERR_MENOVALID_CN00_1)
#define ERR_BANOVALIDTYPE_CN00          cn00_1_ErrId(ERR_BANOVALIDTYPE_CN00_1)
#define ERR_NOHISTORY_CN00              cn00_1_ErrId(ERR_NOHISTORY_CN00_1)
#define ERR_XPNOTFOUND_CN00             cn00_1_ErrId(ERR_XPNOTFOUND_CN00_1)
#define ERR_XPSYNTAX_CN00               cn00_1_ErrId(ERR_XPSYNTAX_CN00_1)
#define ERR_PARAMEXIST_CN00             cn00_1_ErrId(ERR_PARAMEXIST_CN00_1)
#define ERR_NOXPSESSION_CN00            cn00_1_ErrId(ERR_NOXPSESSION_CN00_1)
#define ERR_XPSESSION_CN00              cn00_1_ErrId(ERR_XPSESSION_CN00_1)
#define ERR_XPCHECK_CN00                cn00_1_ErrId(ERR_XPCHECK_CN00_1)
#define ERR_XPDIRTY_CN00                cn00_1_ErrId(ERR_XPDIRTY_CN00_1)
#define ERR_XPREADONLY_CN00             cn00_1_ErrId(ERR_XPREADONLY_CN00_1)
#define ERR_SYSTEM_CN00                 cn00_1_ErrId(ERR_SYSTEM_CN00_1)
#define ERR_EXECUTE_CN00                cn00_1_ErrId(ERR_EXECUTE_CN00_1)
#define ERR_STATE_CN00                  cn00_1_ErrId(ERR_STATE_CN00_1)
#define ERR_DBRUN_CN00                  cn00_1_ErrId(ERR_DBRUN_CN00_1)
#define ERR_XPOVERFLOW_CN00             cn00_1_ErrId(ERR_XPOVERFLOW_CN00_1)
#define ERR_BAOVERWRITE_CN00            cn00_1_ErrId(ERR_BAOVERWRITE_CN00_1)
#define ERR_NOUSER_CN00                 cn00_1_ErrId(ERR_NOUSER_CN00_1)
#define ERR_BASAVETYPECON_CN00          cn00_1_ErrId(ERR_BASAVETYPECON_CN00_1)
#define ERR_BANOSAVETYPE_CN00           cn00_1_ErrId(ERR_BANOSAVETYPE_CN00_1)
#define ERR_BANOAUTOMED_CN00            cn00_1_ErrId(ERR_BANOAUTOMED_CN00_1)
#define ERR_DBEXIST_CN00                cn00_1_ErrId(ERR_DBEXIST_CN00_1)
#define ERR_USRREAD_CN00                cn00_1_ErrId(ERR_USRREAD_CN00_1)
#define ERR_USRFAIL_CN00                cn00_1_ErrId(ERR_USRFAIL_CN00_1)
#define ERR_USREXISTS_CN00              cn00_1_ErrId(ERR_USREXISTS_CN00_1)
#define ERR_NODBNAME_CN00               cn00_1_ErrId(ERR_NODBNAME_CN00_1)
#define ERR_DBNAME_CN00                 cn00_1_ErrId(ERR_DBNAME_CN00_1)
#define ERR_TOOMANYPARAM_CN00           cn00_1_ErrId(ERR_TOOMANYPARAM_CN00_1)
#define ERR_TOOFEWPARAM_CN00            cn00_1_ErrId(ERR_TOOFEWPARAM_CN00_1)
#define ERR_USRDELSELF_CN00             cn00_1_ErrId(ERR_USRDELSELF_CN00_1)
#define ERR_USRDELDBM_CN00              cn00_1_ErrId(ERR_USRDELDBM_CN00_1)
#define ERR_USRDELSDBA_CN00             cn00_1_ErrId(ERR_USRDELSDBA_CN00_1)
#define ERR_NEEDLOGON_CN00              cn00_1_ErrId(ERR_NEEDLOGON_CN00_1)
#define ERR_MISSRIGHT_CN00              cn00_1_ErrId(ERR_MISSRIGHT_CN00_1)
#define ERR_DBNORUN_CN00                cn00_1_ErrId(ERR_DBNORUN_CN00_1)
#define ERR_NOUSERS_CN00                cn00_1_ErrId(ERR_NOUSERS_CN00_1)
#define ERR_VERSION_CN00                cn00_1_ErrId(ERR_VERSION_CN00_1)
#define ERR_XPMIN_CN00                  cn00_1_ErrId(ERR_XPMIN_CN00_1)
#define ERR_XPMAX_CN00                  cn00_1_ErrId(ERR_XPMAX_CN00_1)
#define ERR_XPVALUESET_CN00             cn00_1_ErrId(ERR_XPVALUESET_CN00_1)
#define ERR_USRFRM_CN00                 cn00_1_ErrId(ERR_USRFRM_CN00_1)
#define ERR_NOREPLY_CN00                cn00_1_ErrId(ERR_NOREPLY_CN00_1)
#define ERR_TOOLCHK_CN00                cn00_1_ErrId(ERR_TOOLCHK_CN00_1)
#define ERR_MEDIUMCHK_CN00              cn00_1_ErrId(ERR_MEDIUMCHK_CN00_1)
#define ERR_PREPARE_CN00                cn00_1_ErrId(ERR_PREPARE_CN00_1)
#define ERR_DBREQ_CN00                  cn00_1_ErrId(ERR_DBREQ_CN00_1)
#define ERR_TOOLREQ_CN00                cn00_1_ErrId(ERR_TOOLREQ_CN00_1)
#define ERR_OPCHK_CN00                  cn00_1_ErrId(ERR_OPCHK_CN00_1)
#define ERR_POSTOP_CN00                 cn00_1_ErrId(ERR_POSTOP_CN00_1)
#define ERR_BACKUPOP_CN00               cn00_1_ErrId(ERR_BACKUPOP_CN00_1)
#define ERR_CLEANUP_CN00                cn00_1_ErrId(ERR_CLEANUP_CN00_1)
#define ERR_MISSMIRR_CN00               cn00_1_ErrId(ERR_MISSMIRR_CN00_1)
#define ERR_DBAUNKNOWN_CN00             cn00_1_ErrId(ERR_DBAUNKNOWN_CN00_1)
#define ERR_DBAWRONG_CN00               cn00_1_ErrId(ERR_DBAWRONG_CN00_1)
#define ERR_NOEBIDLIST_CN00             cn00_1_ErrId(ERR_NOEBIDLIST_CN00_1)
#define ERR_ASCII_CN00                  cn00_1_ErrId(ERR_ASCII_CN00_1)
#define ERR_NOTADMIN_CN00               cn00_1_ErrId(ERR_NOTADMIN_CN00_1)
#define ERR_BAWRONGMEDIUM_CN00          cn00_1_ErrId(ERR_BAWRONGMEDIUM_CN00_1)
#define ERR_BAWRONGSAVETYPE_CN00        cn00_1_ErrId(ERR_BAWRONGSAVETYPE_CN00_1)
#define ERR_NOPREVSTAGEBACKUP_CN00      cn00_1_ErrId(ERR_NOPREVSTAGEBACKUP_CN00_1)
#define ERR_MEEXISTSASGROUP_CN00        cn00_1_ErrId(ERR_MEEXISTSASGROUP_CN00_1)
#define ERR_MEEXISTSASMEDIUM_CN00       cn00_1_ErrId(ERR_MEEXISTSASMEDIUM_CN00_1)
#define ERR_MEAMBIGUOUS_CN00            cn00_1_ErrId(ERR_MEAMBIGUOUS_CN00_1)
#define ERR_NORESTORESTRATEGY_CN00      cn00_1_ErrId(ERR_NORESTORESTRATEGY_CN00_1)
#define ERR_BACKNOTINHIST_CN00          cn00_1_ErrId(ERR_BACKNOTINHIST_CN00_1)
#define ERR_NOTMASTER_CN00              cn00_1_ErrId(ERR_NOTMASTER_CN00_1)
#define ERR_SERVICEDBNOTFOUND_CN00      cn00_1_ErrId(ERR_SERVICEDBNOTFOUND_CN00_1)

#define ERR_UNKNOWN_CN00                cn00_1_ErrId(ERR_UNKNOWN_CN00_1)

typedef Msg_List::MessageID tcn00_Error;

typedef struct tcn00_ValueToText {
  long          nValue;
  const char  * szText;
} tcn00_ValueToText; /* end typedef */

#define ERROR_TO_TEXT_CN00 { \
  { OK_CN00,                    cn00_1_ErrTxt(OK_CN00_1)                   }, \
  { ERR_CN00,                   cn00_1_ErrTxt(ERR_CN00_1)                  }, \
  { ERR_NOHISTORYSESSION_CN00,  cn00_1_ErrTxt(ERR_NOHISTORYSESSION_CN00_1) }, \
  { ERR_MEM_CN00,               cn00_1_ErrTxt(ERR_MEM_CN00_1)              }, \
  { ERR_PARAM_CN00,             cn00_1_ErrTxt(ERR_PARAM_CN00_1)            }, \
  { ERR_FILE_CN00,              cn00_1_ErrTxt(ERR_FILE_CN00_1)             }, \
  { ERR_TOOSMALL_CN00,          cn00_1_ErrTxt(ERR_TOOSMALL_CN00_1)          }, \
  { ERR_NOMEDIA_CN00,           cn00_1_ErrTxt(ERR_NOMEDIA_CN00_1)          }, \
  { ERR_MENOTFOUND_CN00,        cn00_1_ErrTxt(ERR_MENOTFOUND_CN00_1)       }, \
  { ERR_MENOVALID_CN00,         cn00_1_ErrTxt(ERR_MENOVALID_CN00_1)        }, \
  { ERR_BANOVALIDTYPE_CN00,     cn00_1_ErrTxt(ERR_BANOVALIDTYPE_CN00_1)    }, \
  { ERR_NOHISTORY_CN00,         cn00_1_ErrTxt(ERR_NOHISTORY_CN00_1)        }, \
  { ERR_XPNOTFOUND_CN00,        cn00_1_ErrTxt(ERR_XPNOTFOUND_CN00_1)       }, \
  { ERR_XPSYNTAX_CN00,          cn00_1_ErrTxt(ERR_XPSYNTAX_CN00_1)         }, \
  { ERR_PARAMEXIST_CN00,        cn00_1_ErrTxt(ERR_PARAMEXIST_CN00_1)       }, \
  { ERR_NOXPSESSION_CN00,       cn00_1_ErrTxt(ERR_NOXPSESSION_CN00_1)      }, \
  { ERR_XPSESSION_CN00,         cn00_1_ErrTxt(ERR_XPSESSION_CN00_1)        }, \
  { ERR_XPCHECK_CN00,           cn00_1_ErrTxt(ERR_XPCHECK_CN00_1)          }, \
  { ERR_XPDIRTY_CN00,           cn00_1_ErrTxt(ERR_XPDIRTY_CN00_1)          }, \
  { ERR_XPREADONLY_CN00,        cn00_1_ErrTxt(ERR_XPREADONLY_CN00_1)       }, \
  { ERR_SYSTEM_CN00,            cn00_1_ErrTxt(ERR_SYSTEM_CN00_1)           }, \
  { ERR_EXECUTE_CN00,           cn00_1_ErrTxt(ERR_EXECUTE_CN00_1)          }, \
  { ERR_STATE_CN00,             cn00_1_ErrTxt(ERR_STATE_CN00_1)            }, \
  { ERR_DBRUN_CN00,             cn00_1_ErrTxt(ERR_DBRUN_CN00_1)            }, \
  { ERR_XPOVERFLOW_CN00,        cn00_1_ErrTxt(ERR_XPOVERFLOW_CN00_1)       }, \
  { ERR_BAOVERWRITE_CN00,       cn00_1_ErrTxt(ERR_BAOVERWRITE_CN00_1)      }, \
  { ERR_NOUSER_CN00,            cn00_1_ErrTxt(ERR_NOUSER_CN00_1)           }, \
  { ERR_BASAVETYPECON_CN00,     cn00_1_ErrTxt(ERR_BASAVETYPECON_CN00_1)    }, \
  { ERR_BANOSAVETYPE_CN00,      cn00_1_ErrTxt(ERR_BANOSAVETYPE_CN00_1)     }, \
  { ERR_BANOAUTOMED_CN00,       cn00_1_ErrTxt(ERR_BANOAUTOMED_CN00_1)      }, \
  { ERR_DBEXIST_CN00,           cn00_1_ErrTxt(ERR_DBEXIST_CN00_1)          }, \
  { ERR_USRREAD_CN00,           cn00_1_ErrTxt(ERR_USRREAD_CN00_1)          }, \
  { ERR_USRFAIL_CN00,           cn00_1_ErrTxt(ERR_USRFAIL_CN00_1)          }, \
  { ERR_USREXISTS_CN00,         cn00_1_ErrTxt(ERR_USREXISTS_CN00_1)        }, \
  { ERR_NODBNAME_CN00,          cn00_1_ErrTxt(ERR_NODBNAME_CN00_1)         }, \
  { ERR_DBNAME_CN00,            cn00_1_ErrTxt(ERR_DBNAME_CN00_1)           }, \
  { ERR_TOOMANYPARAM_CN00,      cn00_1_ErrTxt(ERR_TOOMANYPARAM_CN00_1)     }, \
  { ERR_TOOFEWPARAM_CN00,       cn00_1_ErrTxt(ERR_TOOFEWPARAM_CN00_1)      }, \
  { ERR_USRDELSELF_CN00,        cn00_1_ErrTxt(ERR_USRDELSELF_CN00_1)       }, \
  { ERR_USRDELDBM_CN00,         cn00_1_ErrTxt(ERR_USRDELDBM_CN00_1)        }, \
  { ERR_USRDELSDBA_CN00,        cn00_1_ErrTxt(ERR_USRDELSDBA_CN00_1)       }, \
  { ERR_NEEDLOGON_CN00,         cn00_1_ErrTxt(ERR_NEEDLOGON_CN00_1)        }, \
  { ERR_MISSRIGHT_CN00,         cn00_1_ErrTxt(ERR_MISSRIGHT_CN00_1)        }, \
  { ERR_DBNORUN_CN00,           cn00_1_ErrTxt(ERR_DBNORUN_CN00_1)          }, \
  { ERR_NOUSERS_CN00,           cn00_1_ErrTxt(ERR_NOUSERS_CN00_1)          }, \
  { ERR_VERSION_CN00,           cn00_1_ErrTxt(ERR_VERSION_CN00_1)          }, \
  { ERR_XPMIN_CN00,             cn00_1_ErrTxt(ERR_XPMIN_CN00_1)            }, \
  { ERR_XPMAX_CN00,             cn00_1_ErrTxt(ERR_XPMAX_CN00_1)            }, \
  { ERR_XPVALUESET_CN00,        cn00_1_ErrTxt(ERR_XPVALUESET_CN00_1)       }, \
  { ERR_USRFRM_CN00,            cn00_1_ErrTxt(ERR_USRFRM_CN00_1)           }, \
  { ERR_NOREPLY_CN00,           cn00_1_ErrTxt(ERR_NOREPLY_CN00_1)          }, \
  { ERR_TOOLCHK_CN00,           cn00_1_ErrTxt(ERR_TOOLCHK_CN00_1)          }, \
  { ERR_MEDIUMCHK_CN00,         cn00_1_ErrTxt(ERR_MEDIUMCHK_CN00_1)        }, \
  { ERR_PREPARE_CN00,           cn00_1_ErrTxt(ERR_PREPARE_CN00_1)          }, \
  { ERR_DBREQ_CN00,             cn00_1_ErrTxt(ERR_DBREQ_CN00_1)            }, \
  { ERR_TOOLREQ_CN00,           cn00_1_ErrTxt(ERR_TOOLREQ_CN00_1)          }, \
  { ERR_OPCHK_CN00,             cn00_1_ErrTxt(ERR_OPCHK_CN00_1)            }, \
  { ERR_POSTOP_CN00,            cn00_1_ErrTxt(ERR_POSTOP_CN00_1)           }, \
  { ERR_BACKUPOP_CN00,          cn00_1_ErrTxt(ERR_BACKUPOP_CN00_1)         }, \
  { ERR_CLEANUP_CN00,           cn00_1_ErrTxt(ERR_CLEANUP_CN00_1)          }, \
  { ERR_MISSMIRR_CN00,          cn00_1_ErrTxt(ERR_MISSMIRR_CN00_1)         }, \
  { ERR_DBAUNKNOWN_CN00,        cn00_1_ErrTxt(ERR_DBAUNKNOWN_CN00_1)       }, \
  { ERR_DBAWRONG_CN00,          cn00_1_ErrTxt(ERR_DBAWRONG_CN00_1)         }, \
  { ERR_NOEBIDLIST_CN00,        cn00_1_ErrTxt(ERR_NOEBIDLIST_CN00_1)       }, \
  { ERR_ASCII_CN00,             cn00_1_ErrTxt(ERR_ASCII_CN00_1)            }, \
  { ERR_NOTADMIN_CN00,          cn00_1_ErrTxt(ERR_NOTADMIN_CN00_1)         }, \
  { ERR_BAWRONGMEDIUM_CN00,     cn00_1_ErrTxt(ERR_BAWRONGMEDIUM_CN00_1)    }, \
  { ERR_BAWRONGSAVETYPE_CN00,   cn00_1_ErrTxt(ERR_BAWRONGSAVETYPE_CN00_1)  }, \
  { ERR_NOPREVSTAGEBACKUP_CN00, cn00_1_ErrTxt(ERR_NOPREVSTAGEBACKUP_CN00_1)}, \
  { ERR_MEEXISTSASGROUP_CN00,   cn00_1_ErrTxt(ERR_MEEXISTSASGROUP_CN00_1)  }, \
  { ERR_MEEXISTSASMEDIUM_CN00,  cn00_1_ErrTxt(ERR_MEEXISTSASMEDIUM_CN00_1) }, \
  { ERR_MEAMBIGUOUS_CN00,       cn00_1_ErrTxt(ERR_MEAMBIGUOUS_CN00_1)      }, \
  { ERR_NORESTORESTRATEGY_CN00, cn00_1_ErrTxt(ERR_NORESTORESTRATEGY_CN00_1)}, \
  { ERR_BACKNOTINHIST_CN00,     cn00_1_ErrTxt(ERR_BACKNOTINHIST_CN00_1)    }, \
  { ERR_NOTMASTER_CN00,         cn00_1_ErrTxt(ERR_NOTMASTER_CN00_1)        }, \
  { ERR_SERVICEDBNOTFOUND_CN00, cn00_1_ErrTxt(ERR_SERVICEDBNOTFOUND_CN00_1)}, \
  { ERR_UNKNOWN_CN00,           cn00_1_ErrTxt(ERR_UNKNOWN_CN00_1)          }}

#define ANSWER_OK_CN00         _T("OK")
#define ANSWER_ERR_CN00        _T("ERR")
#define VALUE_SEPSTRING_CN00   _T("\t")
#define LINE_SEPSTRING_CN00    _T("\n")
#define TOKEN_SEPSTRING_CN00   _T(",")
#define PWD_SEPARATOR_CN00     _T(',')
#define KEY_DIREND_CN00        _T("END     ")
#define KEY_DIRCONTINUE_CN00   _T("CONTINUE")

/* param access */
//#define XP_VALUE_LENGTH_MXCN00         100

// param values
#define XP_VAL_DUAL_CN00               "DUAL"
#define XP_VAL_YES_CN00                "YES"
#define XP_VAL_F_CN00                  "F"

/* devspace specifications */
#define XP_DEV_DAT_CN00                "DATA"
#define XP_DEV_LOG_CN00                "LOG"
#define XP_DEV_MLOG_CN00               "MLOG"
typedef enum {
  XPDevSpaceUnknown_ecn00,
  XPDevSpaceDat_ecn00,
  XPDevSpaceMDat_ecn00,
  XPDevSpaceLog_ecn00,
  XPDevSpaceMLog_ecn00,
  XPHSSNode_ecn00,
  XPHSSDelay_ecn00
} tcn00_XPDevSpace;
/* mode                     multi    mode */
#define XP_DEVSPACE_KEYWORDS_CN00   { \
  {XPDevSpaceDat_ecn00,     true,   false, XP_DEV_DAT_CN00    }, \
  {XPDevSpaceLog_ecn00,     true,   false, XP_DEV_LOG_CN00    }, \
  {XPDevSpaceMLog_ecn00,    true,   true,  XP_DEV_MLOG_CN00   }, \
  {XPDevSpaceUnknown_ecn00, false,  false, ""                 }}

typedef struct tcn00_DevspaceKeywords {
    tcn00_XPDevSpace    nDevSpace;
    bool                bMulti;
    bool                bMirror;
    const _TCHAR      * szText;
  } tcn00_DevspaceKeywords;

/*   mode                     name param                 type param                      size param                    mode                          max count               */
  #define XP_DEVSPACE_PARAMS_CN00   { \
    {XPDevSpaceDat_ecn00,     PAN_DATADEV_N"%04d",       PAN_DATADEV_N_TYPE"%04d",       PAN_DATADEV_N_SIZE"%04d",     PAN_DATA_VOLUME_MODE_N"%04d", PAN_MAXDATADEVSPACES }, \
    {XPDevSpaceLog_ecn00,     PAN_ARCHIVE_LOG_N"%03d",   PAN_ARCHIVE_LOG_N_TYPE"%03d",   PAN_ARCHIVE_LOG_N_SIZE"%03d", "",                           PAN_MAXARCHIVELOGS   }, \
    {XPDevSpaceMLog_ecn00,    PAN_M_ARCHIVE_LOG_N"%03d", PAN_M_ARCHIVE_LOG_N_TYPE"%03d", "",                           "",                           ""                   }, \
    {XPHSSNode_ecn00,         PAN_HS_NODE_N"%03d",       "",                             "",                           "",                           ""                   }, \
    {XPHSSDelay_ecn00,        PAN_HS_DELAY_TIME_N"%03d", "",                             "",                           "",                           ""                   }, \
    {XPDevSpaceUnknown_ecn00, "",                        "",                             "",                           "",                           ""                   }}

typedef struct tcn00_DevspaceParams {
    tcn00_XPDevSpace   nDevSpace;
    const _TCHAR     * szName;
    const _TCHAR     * szType;
    const _TCHAR     * szSize;
    const _TCHAR     * szMode;
    const _TCHAR     * szMaxCount;
} tcn00_DevspaceParams;

typedef tcn00_Error cnFunctionPrototype_CN00
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax);

/* supply a text for a boolean expression */
#define     BOOLSTR_CN00(boolval)   ((boolval) ? "True" : "False")

/* Database error codes */
#define DB_ERR_LOGFULL_CN00      (-901)
#define DB_ERR_ROWNOTFOUND_CN00  (100)

#ifdef CONSOLE_TRACE_CN
  #define CONSOLE_TRACE_MSG_CN00(Text)     printf("%s::%8d - %s\n",       (char *) __FILE__, (int) __LINE__, (char *) Text);
  #define CONSOLE_TRACE_ENTER_CN00         printf("%s::%8d - enter %s\n", (char *) __FILE__, (int) __LINE__, (char *) CN_FUNCTION);
  #define CONSOLE_TRACE_LEAVE_CN00         printf("%s::%8d - leave %s\n", (char *) __FILE__, (int) __LINE__, (char *) CN_FUNCTION);
#else
  #define CONSOLE_TRACE_MSG_CN00(Text)
  #define CONSOLE_TRACE_ENTER_CN00
  #define CONSOLE_TRACE_LEAVE_CN00
#endif

#define XDB_RC_CN00(n)          ((n == OK_CN00) ? OK_CN00 : n + 25100)

#ifndef _WIN32
  #define stricmp  strcasecmp
  #define strnicmp strncasecmp
#endif

typedef tsp00_Namec tcn00_UserNamec;

#define vf_t_directory          7

#endif /* _GCN00_H_ */
