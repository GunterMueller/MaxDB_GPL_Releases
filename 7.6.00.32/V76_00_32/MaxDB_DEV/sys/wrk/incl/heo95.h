/*!
  @file           heo95.h
  @author         JoergM
  @special area   application startup Program
  @brief          description ...
  @see            example.html ...

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/



#ifndef HEO95_H
#define HEO95_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#include <process.h>
#include <sys/utime.h>
#include <direct.h>

#define F_READ  0200

#else

#include        <unistd.h>
#include        <utime.h>
#include        <errno.h>
#ifndef FREEBSD
#include        <termio.h>
#endif
#if ! T31 && ! T33 && ! T35
#include        <termios.h>
#endif
#if SVR4 || SOLARIS
#include        <wait.h>
#else
#if SCO
#include        <sys/wait.h>
#endif
#endif


#define TERM   "TERM"
#define DBTERM "DBTERM"

#define F_READ  S_IRUSR

#endif


#include "heo01.h"

/*-----------------------------------------------------------------------*/

/*-------------------------- global defines -----------------------------*/

#define CNULL  (char *) 0
#define TRUE            1
#define FALSE           0
#define NOTOK          -1
#define NO_TERM     FALSE
#define INIT_TERM    TRUE
#define INIT_WIN        2

#define MAX_DBNAME_LEN 8
#define MAX_HLP_FILES 10
#define MAX_HLP_LINE_LEN 80

#define HELP_ID  "h"
#define ALTERNATE_HELP_ID "?"
#define NO_HELP         (char **) 0
#define NOT_AVAIL       ""
#define NO_ALTERN       CNULL

/*--------------------- environment-vars using by SQL-DB ---------------*/

enum ProgramExecPathType
  {
    NoExecPath,
    IndependentPGM,
    IndependentBIN,
    DbrootPGM,
    DbrootBIN,
	DbrootSAP
  };

/*------------------- permissions using by stat / open-------------------*/
#define F_EX 0
#define F_WRITE 0400
#define F_RW 0600
#define F_RWX 0700

#define ADABAS_KERNELS     { "kernel","quickknl","slowknl", "omststknl" }
#define ADABAS_GATEWAYS    { "fstgwada","qckgwada","slwgwada" }
#define ORACLE_GATEWAYS    { "fstgwora","qckgwora","slwgwora" }

/*-------------------------- USAGES for bad parameters ------------------*/
#define PRI_USAGE(x) { { printf x; } }
#define SQL_HELP_HEADER "\ncorrect use of %s is:\n\n"

#define SQL_USAGE "USAGE: "

#define USAGE_CLEAR(progname) \
        PRI_USAGE (("\n%s %s <serverdb>\n", SQL_USAGE, progname ))

#define USAGE_SHOW(progname) \
        PRI_USAGE (("\n%s %s [-c]\n", SQL_USAGE, progname ))


#if _WIN32
#define USAGE_SERVER(progname) \
        PRI_USAGE (("\n%s %s [ %s | %s | %s | %s ]\n", SQL_USAGE, progname, \
                  SERVER_OPT_START, SERVER_OPT_STOP,                        \
                  SERVER_OPT_INSTALL, SERVER_OPT_REMOVE ));
#else
#define USAGE_SERVER(progname) \
        PRI_USAGE (("\n%s %s [ %s | %s ]\n", SQL_USAGE, progname, \
                                      SERVER_OPT_START, SERVER_OPT_STOP ));
#endif

/*-------------------------- defines OS/2 and WIN32 -------------------------*/
#ifdef WIN32

#define USAGE_CONS(progname) \
        PRI_USAGE (("\n%s %s [<dbname>] <command>\n", SQL_USAGE, progname ))

#  define       IS_OPT_CHR(c) ( c == '-' || c == '/' )
#  define       TERMIO_INFO             int
#  define       GET_TERMIO(f,t)
#  define       SET_TERMIO(f,t)

#define BEG_ENV_VAR '%'
#define END_ENV_VAR '%'

#define SQLDB_START  "x_start" , "xstart", NoExecPath, DbrootPGM, "start.exe\0strt.exe\0", NO_TERM
#define SQLDB_GWSTART  "x_gwstart" , "xgwstart", NoExecPath, DbrootPGM, "strt.exe\0" , NO_TERM
#define SQLDB_STOP   "x_stop"  , "xstop" , NoExecPath, DbrootPGM, "stop.exe\0stp.exe\0", NO_TERM
#define SQLDB_CONS   "x_cons"  , "xcons" , NoExecPath, DbrootPGM, "cons.exe\0"       , NO_TERM
#define SQLDB_SHOW   "x_show"  , "xshow" , NoExecPath, DbrootPGM, "show.exe\0"       , NO_TERM
#define SQLDB_SERV   "x_server","xserver", NoExecPath, IndependentPGM, "serv.exe\0"  , NO_TERM
#define SQLDB_PING   "x_ping" , NO_ALTERN, NoExecPath, DbrootPGM, "sqlping.exe\0"       , NO_TERM
#define SQLDB_DIAG   "x_diag" , "x_diagnose", NoExecPath, DbrootPGM, "diagnose.exe\0", INIT_TERM
#define SQLDB_QUERY  "xquery" , NO_ALTERN, NoExecPath, DbrootPGM, "query.exe\0"      , INIT_TERM
#define SQLDB_LOAD   "xload"  , NO_ALTERN, NoExecPath, DbrootPGM, "load.exe\0"       , INIT_TERM
#define SQLDB_PYTHON "x_python",NO_ALTERN, NoExecPath, DbrootPGM, "pysapdb.exe\0"    , NO_TERM
#define SQLDB_EASY   "xeasy"  , NO_ALTERN, NoExecPath, DbrootPGM, "easy.exe\0"       , INIT_TERM
#define SQLDB_PL     "xpl"    , NO_ALTERN, NoExecPath, DbrootPGM, "sqlpl.exe\0"      , INIT_TERM
#define SQLDB_PLRUN  "xplrun" , NO_ALTERN, NoExecPath, DbrootPGM, "plrun.exe\0"      , INIT_TERM
#define SQLDB_TOPL   "xtopl"  , NO_ALTERN, NoExecPath, DbrootPGM, "topl.exe\0"       , INIT_TERM
#define SQLDB_TOPLR  "xtplrun", NO_ALTERN, NoExecPath, DbrootPGM, "tplrun.exe\0"     , INIT_TERM
#define SQLDB_USER   "xuser"  , NO_ALTERN, NoExecPath, IndependentBIN, "xuserUnicode.exe\0"      , NO_TERM
#define SQLDB_AUDIT  "xaudit" , NO_ALTERN, NoExecPath, DbrootPGM, "audit.exe\0"      , INIT_TERM
#define SQLDB_CPC    "cpc"    , NO_ALTERN, NoExecPath, DbrootPGM, "callpc3.exe\0"    , NO_TERM
#define SQLDB_CLEAR  NOT_AVAIL ,NO_ALTERN, NoExecPath, DbrootBIN, "x_clear.exe\0"    , NO_TERM
#define SQLDB_CI     "xci"    , NO_ALTERN, NoExecPath, DbrootPGM, "slowci.exe\0"     , INIT_TERM
#define SQLDB_CI_O   "xoldci" , NO_ALTERN, NoExecPath, DbrootPGM, "oldci.exe\0"      , INIT_TERM
#define SQLDB_FILTER "xsql"   , NO_ALTERN, NoExecPath, DbrootPGM, "sqlfilt.exe\0"    , NO_TERM
#define SQLDB_FILTER_RO "xsqlro", NO_ALTERN, NoExecPath, DbrootPGM, "sqlfiltn.exe\0" , NO_TERM
#define SQLDB_MAKETI NOT_AVAIL, NO_ALTERN, NoExecPath, DbrootBIN, "x_maketi.exe\0"   , NO_TERM
#define SQLDB_VTTEST NOT_AVAIL, NO_ALTERN, NoExecPath, DbrootPGM, "vttest.exe\0"     , NO_TERM

#define SQLDB_SERVICE  "xservice" , "x_service", NoExecPath, DbrootPGM, "service.exe\0", NO_TERM
#define SQLDB_BACKUP   "xbackup"   , NO_ALTERN , NoExecPath, DbrootPGM, "backup.exe\0",  NO_TERM
#define SQLDB_RESTORE  "xrestore"  , NO_ALTERN , NoExecPath, DbrootPGM, "restore.exe\0", NO_TERM

/* PTS 1107637 FF 17-Aug-2000    xregcomp statt regsvr32                */
#define SQLDB_REGCOMP "xregcomp", NO_ALTERN, NoExecPath, DbrootPGM, "regcomp.exe\0"  , NO_TERM
#define SQLDB_KERNPROT "xkernprot" , NO_ALTERN, NoExecPath, DbrootPGM, "diagnose.exe\0", NO_TERM

/* PTS 1108841 FH 10-Jan-2001                                                    */
#define SQLDB_TRACESORT "xtracesort", NO_ALTERN, NoExecPath, DbrootPGM, "tracesort.exe\0", NO_TERM
#define SQLDB_SYSMON  "sysmon"    , NO_ALTERN , NoExecPath, DbrootSAP, "sysmon.exe\0"   , NO_TERM
#define SQLDB_WIZARD  "x_wizard"  , NO_ALTERN , NoExecPath, DbrootPGM, "wizard.exe\0" , NO_TERM
#define SQLDB_WIZTRC  "x_wiztrc"  , NO_ALTERN , NoExecPath, DbrootPGM, "wiztrc.exe\0" , NO_TERM
#define SQLDB_XPU     "xpu"       , NO_ALTERN , NoExecPath, DbrootPGM, "pu.exe\0"      , NO_TERM
#define SQLDB_WIZ     "x_wiz"     , NO_ALTERN , NoExecPath, DbrootSAP, "wiz.cmd\0"    , NO_TERM
#define SQLDB_WIZSTOP "x_wizstop" , NO_ALTERN , NoExecPath, DbrootSAP, "wizstop.cmd\0", NO_TERM
#define SQLDB_ANALYS  "x_analys"  , NO_ALTERN , NoExecPath, DbrootSAP, "analys.cmd\0" , NO_TERM
/*------------------------- function prototypes  -----------------------*/
#else

/*-------------------------- defines UNIX only --------------------------*/


#define USAGE_CONS(progname) \
        PRI_USAGE (("\n%s %s <dbname> [ command [ interval [ count ] ] ] \n", \
                   SQL_USAGE, progname ))


#define IS_OPT_CHR(c) ( c == '-' )

#if     defined(NCCS) && defined(TCSADRAIN)
#  define       TERMIO_INFO             struct termios
#  define       GET_TERMIO(f,t)         tcgetattr(f,t)
#  define       SET_TERMIO(f,t)         tcsetattr(f,TCSADRAIN,t)
#else
#  define       TERMIO_INFO             struct termio
#  define       GET_TERMIO(f,t)         ioctl(f,TCGETA,t)
#  define       SET_TERMIO(f,t)         ioctl(f,TCSETAW,t)
#endif

#define BEG_ENV_VAR '{'
#define END_ENV_VAR '}'

#define SQLDB_START  "x_start", NO_ALTERN, NoExecPath, DbrootPGM, "start\0"       , NO_TERM
#define SQLDB_GWSTART  "x_gwstart", NO_ALTERN, NoExecPath, DbrootPGM, "start\0"   , NO_TERM
#define SQLDB_STOP   "x_stop" , NO_ALTERN, NoExecPath, DbrootPGM, "stop\0"        , NO_TERM
#define SQLDB_CLEAR  "x_clear", NO_ALTERN, NoExecPath, DbrootPGM, "clr_kernel\0clr_ps_ipc\0",NO_TERM
#define SQLDB_CONS   "x_cons" , NO_ALTERN, NoExecPath, DbrootPGM, "cons\0console\0"     , NO_TERM
#define SQLDB_SERV   "x_server",NO_ALTERN, IndependentBIN, IndependentPGM, "vserver\0"     , NO_TERM
#define SQLDB_PING   "x_ping" , NO_ALTERN, NoExecPath, DbrootPGM, "sqlping\0"        , NO_TERM
#define SQLDB_QUERY  "xquery" , NO_ALTERN, NoExecPath, DbrootPGM, "query\0"       , INIT_TERM
#define SQLDB_LOAD   "xload"  , "xdbload", NoExecPath, DbrootPGM, "load\0"        , INIT_TERM
#define SQLDB_PYTHON "x_python",NO_ALTERN, NoExecPath, DbrootPGM, "python\0"      , NO_TERM
#define SQLDB_EASY   "xeasy"  , NO_ALTERN, NoExecPath, DbrootPGM, "easy\0"        , INIT_TERM
#define SQLDB_PL     "xpl"    , NO_ALTERN, NoExecPath, DbrootPGM, "sqlpl\0"       , INIT_TERM
#define SQLDB_PLRUN  "xplrun" , NO_ALTERN, NoExecPath, DbrootPGM, "plrun\0"       , INIT_TERM
#define SQLDB_TOPL   "xtopl"  , NO_ALTERN, NoExecPath, DbrootPGM, "topl\0"        , INIT_TERM
#define SQLDB_TOPLR  "xtplrun", NO_ALTERN, NoExecPath, DbrootPGM, "tplrun\0"      , INIT_TERM
#define SQLDB_AUDIT  "xaudit" , NO_ALTERN, NoExecPath, DbrootPGM, "audit\0"       , INIT_TERM
#define SQLDB_DIAG   "x_diagnose", NO_ALTERN, NoExecPath, DbrootPGM, "diagnose\0" , INIT_TERM
#define SQLDB_CI     "xci"    , NO_ALTERN, NoExecPath, DbrootPGM, "slowci\0"      , INIT_TERM
#define SQLDB_CI_O   "xoldci" , NO_ALTERN, NoExecPath, DbrootPGM, "oldci\0"       , INIT_TERM
#define SQLDB_MAKETI "x_maketi",NO_ALTERN, NoExecPath, IndependentPGM, "maketi\0" , INIT_TERM
#define SQLDB_VTTEST "xvttest", NO_ALTERN, NoExecPath, IndependentPGM, "vttest\0" , INIT_TERM
#define SQLDB_USER   "xuser"  , NO_ALTERN, NoExecPath, IndependentBIN, "xuserUnicode\0"       , NO_TERM
#define SQLDB_FILTER "xsql"   , NO_ALTERN, NoExecPath, DbrootPGM, "sqlfilter\0"   , NO_TERM
#define SQLDB_FILTER_RO "xsqlro", NO_ALTERN, NoExecPath, DbrootPGM, "sqlfiltern\0"   , NO_TERM
#define SQLDB_SHOW   "x_show" , NO_ALTERN, NoExecPath, DbrootBIN, "x_show\0"      , NO_TERM
#define SQLDB_CPC    NOT_AVAIL, NO_ALTERN, NoExecPath, DbrootBIN, "cpc\0"         , NO_TERM
/* PTS 1107637 FF 17-Aug-2000                                                    */
/*#define SQLDB_REGSVR32 "regsvr32", NO_ALTERN, NoExecPath, DbrootPGM, "regsvr32\0"   , NO_TERM*/
#define SQLDB_REGCOMP "xregcomp", NO_ALTERN, NoExecPath, DbrootPGM, "regcomp\0"  , NO_TERM
#define SQLDB_KERNPROT "xkernprot", NO_ALTERN, NoExecPath, DbrootPGM, "diagnose\0", NO_TERM

/* PTS 1108841 FH 10-Jan-2001                                                    */
#define SQLDB_BACKUP   "xbackup"   , NO_ALTERN , NoExecPath, DbrootPGM, "backup\0",  NO_TERM
#define SQLDB_RESTORE  "xrestore"  , NO_ALTERN , NoExecPath, DbrootPGM, "restore\0", NO_TERM
#define SQLDB_TRACESORT "xtracesort", NO_ALTERN, NoExecPath, DbrootPGM, "tracesort\0", NO_TERM
#define SQLDB_SYSMON  "sysmon"    , NO_ALTERN , NoExecPath, DbrootSAP, "sysmon\0"   , NO_TERM
#define SQLDB_WIZARD  "x_wizard"  , NO_ALTERN , NoExecPath, DbrootPGM, "wizard\0"   , NO_TERM
#define SQLDB_WIZTRC  "x_wiztrc"  , NO_ALTERN , NoExecPath, DbrootPGM, "wiztrc\0"   , NO_TERM
#define SQLDB_XPU     "xpu"       , NO_ALTERN , NoExecPath, DbrootPGM, "pu\0"       , NO_TERM
#define SQLDB_WIZ     "x_wiz"     , NO_ALTERN , NoExecPath, DbrootSAP, "wiz\0"    , NO_TERM
#define SQLDB_WIZSTOP "x_wizstop" , NO_ALTERN , NoExecPath, DbrootSAP, "wizstop\0", NO_TERM
#define SQLDB_ANALYS  "x_analys"  , NO_ALTERN , NoExecPath, DbrootSAP, "analys\0" , NO_TERM

#endif

int e952_call_start(struct program_id*,int, char**) ;
int e952_call_gwstart(struct program_id*,int, char**) ;
int e952_call_stop(struct program_id*,int, char**) ;
int e952_call_console(struct program_id*,int, char**) ;
int e952_call_show(struct program_id*,int, char**) ;
int e952_call_regcomp(struct program_id*,int, char**); /* PTS 1108471 */
int e952_call_vserver(struct program_id*,int, char**) ;
int eo952CallPing(struct program_id*,int, char**) ;
int e953_call_python(struct program_id*,int, char**) ;
int e950_call_component(struct program_id*,int, char**) ;
int e952_call_sqlfilter(struct program_id*,int, char**);
int e952_call_clear(struct program_id*,int, char**) ;
int e953_call_tracesort(struct program_id*,int, char**) ; /* PTS 1108471 */
int e953_call_kernprot(struct program_id*,int, char**) ;
int e953_call_PrecompiledTools(struct program_id*,int, char**);
int e952_call_backup(struct program_id*,int, char**);
int eo954PrintIndepDataPath(struct program_id*,int, char**);
int eo954PrintIndepProgramsPath(struct program_id*,int, char**);
int eo954PrintDBRootOfDB(struct program_id*,int, char**);
int eo954PrintRundirOfDB(struct program_id*,int, char**);
int eo954PrintInstInfo(struct program_id*,int, char**);
int eo954PrintSysRC(struct program_id*,int, char**);

#define SAPDB_GET_INDEP_DATA        "_gid"  , NO_ALTERN , NoExecPath, DbrootSAP, "\0", NO_TERM
#define SAPDB_GET_INDEP_PROGRAMS    "_gip"  , NO_ALTERN , NoExecPath, DbrootSAP, "\0", NO_TERM
#define SAPDB_GET_DBROOT_OF_DB      "_gdbr" , NO_ALTERN , NoExecPath, DbrootSAP, "\0", NO_TERM
#define SAPDB_GET_RUNDIR_OF_DB      "_grd"  , NO_ALTERN , NoExecPath, DbrootSAP, "\0", NO_TERM
#define SAPDB_GET_INSTALLATION_INFO "xinstinfo"  , NO_ALTERN , NoExecPath, DbrootSAP, "\0", NO_TERM
#define SAPDB_GET_SYSRC             "xsysrc"  , NO_ALTERN , NoExecPath, DbrootSAP, "\0", NO_TERM

/*------------------------- structure definitions -----------------------*/

typedef int             (* INT_FUNC_PTR  )(struct program_id*,int, char**);

typedef struct          program_id
                        { char *                    progid;
                          char *                    alter_progid;
                          enum ProgramExecPathType  callProgLocation;
                          enum ProgramExecPathType  ProgramLocation;
/*
  After console was renamed into cons and strt.exe was renamed into start.exe 'progname' is no longer
  a simple character string but a character string list terminated with doubled zero!!!!
  This allow to lookup different executables until an existing program is found and can be called...
 */
                          char *                    progname;
                          int                       init_term;
                          INT_FUNC_PTR              call_func ;
                          char **                   prog_helpfiles ;
                        } PROG_ID ;

/*-----------------------------------------------------------------------*/


/*------------------- global vars for MAIN-MUDUL only -------------------*/
#ifdef DEF_PROGLIST

static char *start_hlpfiles[]= { "xstart.use", CNULL } ;
static char *gwstart_hlpfiles[]= { "xgwstart.use", CNULL } ;
static char *stop_hlpfiles[] = { "xstop.use", CNULL } ;
static char *query_hlpfiles[]= { "general.use", "qu.use", CNULL } ;
static char *load_hlpfiles[] = { "general.use", "lo.use", CNULL } ;
static char *easy_hlpfiles[] = { "general.use", "ye.use", CNULL } ;
static char *pl_hlpfiles[]   = { "general.use", "plrun.use",
                                "plimp.use"  , "plexp.use", CNULL } ;
static char *plrun_hlpfiles[] = { "general.use", "plrun.use", CNULL } ;
static char *topl_hlpfiles[]  = { "general.use", CNULL } ;
static char *tplrun_hlpfiles[]= { "general.use", "plrun.use",
                                "plimp.use"  , CNULL } ;
static char *audit_hlpfiles[] = { "xtau.use" , CNULL } ;

#ifdef  WIN32
static char *server_hlpfiles[] = { "serv.use" , CNULL } ;
#else

static char *server_hlpfiles[] = { "vserver.use" , CNULL } ;
/* PTS 1107637 FF 17-Aug-2000 */
/*static char *regsvr32_hlpfiles[] = { "regsvr32.use" , CNULL } ;*/
#endif
/* PTS 1107637 FF 17-Aug-2000 */
static char *regcomp_hlpfiles[] = { "regcomp.use" , CNULL } ;

static char *kernprot_hlpfiles[] = { "kernprot.use" , CNULL } ;
static char *tracesort_hlpfiles[] = { "tracesort.use" , CNULL } ;

static PROG_ID  proglist[] = {
                { SQLDB_QUERY , e950_call_component , query_hlpfiles } ,
                { SQLDB_LOAD  , e950_call_component , load_hlpfiles } ,
                { SQLDB_PYTHON, e953_call_python    , NO_HELP } ,
                { SQLDB_EASY  , e950_call_component , easy_hlpfiles } ,
                { SQLDB_PL    , e950_call_component , pl_hlpfiles } ,
                { SQLDB_PLRUN , e950_call_component , plrun_hlpfiles } ,
                { SQLDB_TOPL  , e950_call_component , topl_hlpfiles } ,
                { SQLDB_TOPLR , e950_call_component , tplrun_hlpfiles } ,
                { SQLDB_CONS  , e952_call_console   , NO_HELP } ,
                { SQLDB_SHOW  , e952_call_show      , NO_HELP } ,
                { SQLDB_MAKETI, e950_call_component , NO_HELP } ,
                { SQLDB_VTTEST, e950_call_component , NO_HELP } ,
                { SQLDB_CPC   , e950_call_component , NO_HELP } ,
                { SQLDB_USER  , e950_call_component , NO_HELP } ,
                { SQLDB_DIAG  , e950_call_component , NO_HELP } ,
                { SQLDB_AUDIT , e950_call_component , audit_hlpfiles } ,
                { SQLDB_CI    , e950_call_component , NO_HELP } ,
                { SQLDB_CI_O  , e950_call_component , NO_HELP } ,
                { SQLDB_START , e952_call_start,      start_hlpfiles} ,
                { SQLDB_GWSTART, e952_call_gwstart,  gwstart_hlpfiles} ,
                { SQLDB_STOP  , e952_call_stop ,      stop_hlpfiles } ,
                { SQLDB_SERV  , e952_call_vserver   , server_hlpfiles } ,
                { SQLDB_PING  , eo952CallPing , NO_HELP } ,
                { SQLDB_FILTER, e952_call_sqlfilter , NO_HELP } ,
                { SQLDB_FILTER_RO, e952_call_sqlfilter , NO_HELP } ,
#ifdef WIN32
                { SQLDB_SERVICE, e950_call_component, NO_HELP } ,
                { SQLDB_REGCOMP, e950_call_component ,regcomp_hlpfiles } ,
#else
                { SQLDB_CLEAR , e952_call_clear,      NO_HELP } ,
                { SQLDB_REGCOMP,  e952_call_regcomp ,regcomp_hlpfiles } ,
/* PTS 1107636 FF 17-Aug-2000 */
/*                { SQLDB_REGSVR32, e950_call_component,regsvr32_hlpfiles } ,*/
#endif
                { SQLDB_KERNPROT, e953_call_kernprot, kernprot_hlpfiles } ,
/* PTS 1108841 FH 10-Jan-2001 */
                { SQLDB_BACKUP , e952_call_backup   , NO_HELP } ,
                { SQLDB_RESTORE, e952_call_backup  , NO_HELP } ,
                { SQLDB_TRACESORT, e953_call_tracesort, tracesort_hlpfiles } ,
                { SQLDB_SYSMON,   e953_call_PrecompiledTools, NO_HELP } ,
                { SQLDB_WIZTRC,   e953_call_PrecompiledTools, NO_HELP } ,
                { SQLDB_WIZARD,   e953_call_PrecompiledTools, NO_HELP } ,
                { SQLDB_XPU,      e953_call_PrecompiledTools, NO_HELP } ,
                { SQLDB_WIZ,      e953_call_PrecompiledTools, NO_HELP } ,
                { SQLDB_WIZSTOP,  e953_call_PrecompiledTools, NO_HELP } ,
                { SQLDB_ANALYS,   e953_call_PrecompiledTools, NO_HELP } ,
                { SAPDB_GET_INDEP_DATA,       eo954PrintIndepDataPath, NO_HELP } ,
                { SAPDB_GET_INDEP_PROGRAMS   ,eo954PrintIndepProgramsPath, NO_HELP } ,
                { SAPDB_GET_DBROOT_OF_DB     ,eo954PrintDBRootOfDB, NO_HELP } ,
                { SAPDB_GET_RUNDIR_OF_DB     ,eo954PrintRundirOfDB, NO_HELP } ,
                { SAPDB_GET_INSTALLATION_INFO,eo954PrintInstInfo, NO_HELP } ,
                { SAPDB_GET_SYSRC,eo954PrintSysRC, NO_HELP } ,
                } ;

#define PROGLIST_SIZE     (sizeof( proglist )/sizeof (PROG_ID))
#endif
externC int debug ;
/*-----------------------------------------------------------------------*/

#define DBG0(x)         { if ( debug ) { printf x; } }

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC void e950_PutEnvDBRoot           ( tsp00_Pathc DBRoot ) ;  /* PTS 1108841 */

externC int en950ApplStartupProgram      ( int NumArgs, char **Args ) ;

externC int e950_search_and_call_sqlprog ( char               *sql_prog,
                                   int                 num_params,
                                   char              **params ) ;

externC int e950_call_help               ( char               *progname,
                                   char              **helpfiles ) ;

externC void e950_print_params           ( char               *funcname,
                                   int                 num_params,
                                   char              **params ) ;

externC int e950_call_prog               ( char                *progname,
                                   int                  num_params,
                                   char               **params ) ;

/*externC int e950_call_component          ( PROG_ID             *sel_component,
                                   int                  num_args,
                                   char               **args ) ;
*/
externC void en950_PrintRteError         ( char               *HeaderInfo,
                                   tsp01_RteError     *RteError) ;

externC boolean en950_GetProgramExecPath ( enum ProgramExecPathType   ProgramLocation,
                                   char                      *GetProgramExecPath );

externC int eo950_ArgC ();

externC const char **  eo950_ArgV ();

externC int e951_f_access                ( char               *path,
                                   int                 mode,
                                   int                 pri_err );

externC char *e951_toupper_str           ( char               *str ) ;

externC int e951_create_path             ( char               *path_to_create,
                                   unsigned short      mode );

externC char *e951_getparam              ( char               *dbname,
                                   char               *param_id,
                                   char              **param );


externC int en952_GetDbrootPgmPath       ( char               *DbrootPgmPath ) ;

#endif  /* HEO95_H */
