/*


    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end


*/
#ifndef _VMAKE_H
#define _VMAKE_H

#include "devconfig.h"

#include "vmakehistory.h"

# include   <stdlib.h>
#include    <stdio.h>
#  include  <stdarg.h>
# ifdef HAVE_UNISTD_H
#  include  <unistd.h>
# endif
# ifdef HAVE_PROTOTYPES_H
#  include  <prototypes.h>
# endif
# ifdef WIN32
#  include  <windows.h>
#  include  <windef.h>
#  include  <stdio.h>
#  include  <stdarg.h>
#  include  <winbase.h>
#  include  <io.h>
#  include  <direct.h>
typedef int pid_t ;
#  undef    link
#  define   link            rename
#  undef    mkdir
#  define   mkdir(dir,mode)     _mkdir(dir)
# else /* WIN32 */
   typedef  unsigned long   DWORD ;
   typedef  void            *LPVOID ;
# endif
# include        <string.h>
# include        <memory.h>
#include "regex.h"
#include "vmaketypes.h"

extern  char        * vmake_version ;
extern  char        * vmake_version;
extern  char        * vmake_coll ;
extern  regmatch_t  regexmatches[10];

extern  char        concomversion;
extern  int         concom_debug;    /* &gar (CTS 1105643) */

/* strings */
/*
    All directory names are terminated with '/'.
    This simplifies the build of file names.
*/
                    /* use $VMAKE_PATH */
#define     DIR_SRC                 "%s/sys/src/%s/"
#define     DIR_DSCR                "%s/sys/desc/%s/"
#define     DIR_NEWDSCR             "%s/sys/desc/%s/%s/"
#define     DIR_TBIN                "%s/tool/bin/"
#define     DIR_TLIB                "%s/tool/lib/"
#define     DIR_TSRCVMAK            "%s/tool/src/vmake/"

#define     DIR_INC                 pDIR_INC
#define     DIR_SIZE                pDIR_SIZE
#define     DIR_TMP                 pDIR_TMP
#define     DIR_OBJ                 pDIR_OBJ
#define     DIR_PROT                pDIR_PROT
#define     DIR_GSIZ                pDIR_GSIZ
#define     DIR_PSRC                pDIR_PSRC
#define     DIR_CSRC                pDIR_CSRC
#define     DIR_DATES               pDIR_DATES
#define     DIR_BROWSE              pDIR_BROWSE

#define     DIR_UPGM                "%s/usr/pgm/" 
#define     DIR_OWNTMP				"%s/tmp/"		  
#define     DIR_DPGM                "%s/pgm/"      /* used with DBROOT */
#define     DIR_DBIN                "%s/bin/"      /* used with DBROOT */
#define     DIR_DENV                "%s/env/"      /* used with DBROOT */
#define     DIR_DINC                "%s/incl/"     /* used with DBROOT */
#define     DIR_DLIB                "%s/lib/"      /* used with DBROOT */
#define     DIR_DWRK                "%s/wrk/"      /* used with DBROOT */
#define     DIR_DCONF               "%s/config/"   /* used with DBROOT */
#define     DIR_DDEMO               "%s/demo/"     /* used with DBROOT */
#define     DIR_DETC				"%s/etc/"	   /* used with DBROOT */

#define     DIR_SRC_SAPDB           "SAPDB/"       /* PTS 1105037 for DirMapping*/
#define     DIR_WRK                 pDIR_WRK       /* CTS 1105495 */
#define     STD_WRK_NAME            "wrk"          /* CTS 1105673 */
#define     STD_OUT_DIR             "sys/wrk/output/"   



#define     DSC_DEFAULT             "*"
#define     DSC_INC                 "inc"
#define     DSC_OBJDEMAND1          "odemand"
#define     DSC_OBJDEMAND2          "demand"
#define     DSC_SRCDEMAND           "sdemand"
#define     DSC_DEP                 "dep"
#define     DSC_EXTDEP              "extdep"
#define     DSC_OPT                 "opt"
#define     DSC_OBJ                 "obj"
#define     DSC_OUTPUT              "output"
#define     DSC_LANGUAGE            "lang"
#define     DSC_TYPE                "type"
#define     DSC_COMPILER            "comp"
#define     DSC_PARENTLINKOPTION    "parentlinkoption"
#define     DSC_NOCOMINFO           "nocomfile"
#define     DSC_NO_DEP_REMAKE       "no_dep_remake"
#define     DSC_ON_SUCCESS          "on_success"
#define     DSC_SPEC_COMFILE        "comfile"
#define     DSC_PROTSTYLE           "protstyle"


#define     DSC_INTERNVAR_LAYER           "layer"
#define     DSC_INTERNVAR_PUREBASENAME    "purebasename"
#define     DSC_INTERNVAR_BASENAME        "basename"
#define     DSC_INTERNVAR_FILENAME        "filename"
#define     DSC_INTERNVAR_PUREFILENAME    "purefilename"
#define     DSC_INTERNVAR_EXTENSION       "extension"
#define     DSC_INTERNVAR_LVARIANT        "localvariant"
#define     DSC_INTERNVAR_GVARIANTS       "globalvariants"
#define     DSC_INTERNVAR_OLD_ALLVARIANTS "*"
#define     DSC_INTERNVAR_OLD_LVARIANT    "-"
#define     DSC_INTERNVAR_OLD_GVARIANT    "+"



#define     TRGDSC_DEFLAYER         "defaultlayer"
#define     TRGDSC_REQUIRE          "require"
#define     TRGDSC_DEFVERSION       "defaultversion"
#define     TRGDSC_OUTPUT           "output"
#define     TRGDSC_LINKWITH         "linkwith"
#define     TRGDSC_LINKOPTION       "linkoption"
#define     TRGDSC_TOOLOPTION       "tooloption"
#define     TRGDSC_PROPAGATE        "propagate"
#define     TRGDSC_DISTRIB          "distribute"      /* &gar CTS 1000237 */
#define     TRGDSC_FILE_NODISTRIB   "(nodistrib)"
#define     TRGDSC_VMAKEOPTION      "vmakeoption"
#define     TRGDSC_NOSTLIBOPTION    "no_static_lib"
#define     TRGDSC_ERROR            "error"
#define     TRGDSC_PARENTLINKOPTION "parentlinkoption"
#define     TRGDSC_RESPONSIBLE      "responsible"
#define     TRGDSC_REPORT           "report"
#define     TRGDSC_EXECUTE          "execute"
#define     TRGDSC_REQU_VERSION     "requiredvmakeversion"

#define     SLOW                    stringSLOW 
#define     QUICK                   stringQUICK
#define     FAST                    stringFAST 
#define     EMPTY                   stringEMPTY

#define     SOCKET_SERVICE          "vmake"
#define     SOCKET_PROTOCOL         "tcp"

#if unix

#define     CMD_SHELL               "sh"
#define     CMD_LINK                "ln"
#define     CMD_COPY                "cp"
#define     CMD_MOVE                "mv"
#define     CMD_REMOVE              "rm"
#define     CMD_CHDIR               "cd"

#else /*unix*/

#define     CMD_SHELL               "cmd /c"
#define     CMD_LINK                "cp -m"
#define     CMD_COPY                "cp -m"
#define     CMD_MOVE                "mv"
#define     CMD_REMOVE              "rm"
#define     CMD_CHDIR               "cd"

#endif /*unix*/

#define     CMD_COMPLINT    "complint"
#define     CMD_MFSIZE      "mfsize"
#define     CMD_MF00CCC     "mf00ccc"
#define     CMD_ARCHIVE     "archive"
#define     CMD_LINKSHR     "linkshr"
#define     CMD_LINKREL     "linkrel"
#define     CMD_LINKDLL     "linkdll"
#define     CMD_LINKSHM     "linkshm"
#define     CMD_LINKLNK     "linklnk"
#define     CMD_LINKJAR     "linkjar"
#define     CMD_MFEXTRA     "mfextra"

#define     COM_LST         "com"
#define     LIB_LST         "lib"
#define     REL_LST         "rel"
#define     SHR_LST         "shr"
#define     DLL_LST         "dld"
#define     LNK_LST         "lnk"
#define     SHM_LST         "shm"
#define     PRJ_LST         "prj"
#define     MAC_LST         "mac"
#define     JP_LST          "jpr"
#define     EXTRA_LST       "Extra"
#define     LANG_LST        "Langinfo"
#define     LANGEXT_LST     "Langextinfo"
#define     DIRMAP_LST      "DirectoryMapping"   /* PTS 1105037 */
#define     DEFAULT_GLOBALS "Globals"

/* special characters */
#define     NODE_SEPARATOR      ':'
#define     MAP_SEPARATOR       ','    /* PTS 1105037 */
#define     COMMENT             '#'    /* PTS 1105037 */
#define     PATH_DELIMITER      ','
#define     DSC_DELIMITER       '='
#define     OPTION_SEPARATOR    '/'
#define     CMD_PREFIX          '!'
#define     INDEX_SYMBOL        ':'
#define     LBRACKET            '{'
#define     RBRACKET            '}'
#define     LBRACKET2           '('
#define     RBRACKET2           ')'
#define     INTERNAL_VARPREFIX   '%' 

/* CTS 1103620 */
#define     DOUBLEQUOTES        '\"';
#define     BACKSLASH           '\\';


#ifndef unix
# define    RELOC_LIB       1
#endif

#define     DEBUG           0

#include        <stdio.h>
#include        <ctype.h>
#include        <errno.h>
#include        <fcntl.h>
#include        <signal.h>
#include        <time.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#ifdef HAVE_AR_H
# include        <ar.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#   include <sys/wait.h>
#else 
#   ifdef HAVE_WAIT_H
#      include <wait.h>
#   endif
#endif
#ifdef WIN32
# include   <process.h>
#endif
# define    global
# define    TRUE    1
# define    FALSE   0
# define    SIG_RTRN    void
# if    defined(WIN32)
#  define   SIG_CAST    void(CRTAPI1*)(int)
# else
#  define   SIG_CAST    SIG_RTRN(*)()
# endif
# define    CINT        unsigned
# define    geterrno()  errno
# define    seterrno(x) errno=(x)
# define    COPY(d,s,l) (void) memcpy ( (char*)(d) , (char*)(s) , (CINT)(l) )
# define    FILL(d,v,l) (void) memset ( (char*)(d) ,   (int)(v) , (CINT)(l) )
# define    DWORD       int
# if ! SVR4 && ! SOLARIS
#  define        gettxt(_lbl,_dflt)      (_dflt)
# endif

/*
    Vmake levels are used to limit the search for target types
    to those types, which apply to the respective target.
    For instance, a library can be built from modules only, or
    a program can be built from relocs, libraries and modules only.
    Thus a target never can include targets from lower levels.
    With this knowledge, the search for the respective description
    files can be accelerated by avoiding to search for impossible
    combinations.
*/
#define         LEVEL_UNKNOWN           0
#define         LEVEL_MACRO             1
#define         LEVEL_PROJECT           2
#define         LEVEL_PROGRAM           3
#define         LEVEL_RELOC             4
#define         LEVEL_LIBRARY           5
#define         LEVEL_JAVAPROJECT       6
#define         LEVEL_MODULE            7

/* general target types */
#define         GTT_UNKNOWN             0
#define         GTT_KNOWN_DESC          1
#define         GTT_DESC                2
#define         GTT_SRC                 3
#define         GTT_LIST                4

/* target types */
#define         TT_COM                  -1      /* com-list, not realy a target */
#define         TT_UNKNOWN              0
#define         TT_FILE                 1       /* file */
#define         TT_INC                  2       /* include module */
#define         TT_MEM                  3       /* module in library */
#define         TT_MOD                  4       /* independent module */

#define         TT_LIB                  5       /* library */
#define         TT_REL                  6       /* relocatable */
#define         TT_DLL                  7       /* dynamic link library */
#define         TT_SHR                  8       /* reloc. with shared mem. */
#define         TT_PGM                  9       /* program */
#define         TT_SHM                  10      /* program with shared mem. */
#define         TT_PRJ                  11      /* project == macro */
#define         TT_JP                   12      /* java project */
#define         TT_MAC                  20     /* macro */

#define         TT_CMD                  21     /* shell command */
#define         TT_EXTRA                22     /* extra accessories file */
#define         TT_OPTION               23     /* option delivered to a tool */
#define         TT_VMAKEOPTION          24

#define         TT_DIRLIST              25     /* directory list */
#define         TT_EXECUTE              26     /* platform independend excution command */

/* make options */
#define         MK_NONE                 0
#define         MK_NAME                 1
#define         MK_FULL                 2

/* remote server requests */
#define         REM_TIME                1
#define         REM_STAT                2

#define         REM_OPEN                11
#define         REM_CLOSE               12
#define         REM_READ                13
#define         REM_WRITE               14
#define         REM_SEEK                15

#define         REM_DIROPEN             41
#define         REM_DIRCLOSE            42
#define         REM_DIRREAD             43

#define         REM_INCOPY              51
#define         REM_OUTCOPY             52
#define         REM_RENAME              53
#define         REM_UNLINK              54
#define         REM_INCOPY_BIN          55
#define         REM_OUTCOPY_BIN         56

#define         REM_MKDIR               57

/* max. time difference allowed between machines */
#define         HYSTERESIS              300
                           
/* file kind */
#define         FK_LEVEL        0x00ff  /* 0000 0000 1111 1111 */
#define         FK_TYPE         0xff00  /* 1111 1111 0000 0000 */

#define         FK_SOURCE       0x0100  /* 0000 0001 0000 0000, search local and remote */
#define         FK_OBJECT       0x0200  /* 0000 0010 0000 0000, remote can be disabled */
#define         FK_LIB          0x0400  /* 0000 0100 0000 0000, search only on local node */
#define         FK_TOOL         0x4000  /* 0100 0000 0000 0000, search only on local node */

/* host permission bits for search of file kinds */
#define         HOST_OBJECTS_DENIED 1
#define         HOST_TOOLS_DENIED   2

/* path status bits */
#define         PATH_LEVEL      0x00ff  /* 0000 0000 1111 1111 */
#define         PATH_TYPE       0xff00  /* 1111 1111 0000 0000 */

#define         PATH_OWN        0x0100  /* 0000 0001 0000 0000 */
#define         PATH_LOCAL      0x0200  /* 0000 0010 0000 0000 */
#define         PATH_REMOTE     0x0400  /* 0000 0100 0000 0000 */

/* valid bits for dates files. Level MUST be < 256. See also PATH_LEVEL */
typedef unsigned            valid_bit_arr [ 8 ];
#define VALID_BIT_FILL(_arr)        {int i=0;while(i<8)(_arr)[i++]=~0U;}
#define VALID_BIT_CLEAR(_arr)       {int i=0;while(i<8)(_arr)[i++]=0;}
#define VALID_BIT_SET(_lvl,_arr)    ((_arr)[(_lvl)>>5] |= 1U<<(_lvl&31))  /* 05 == 0x05 == 0000 0101*/
#define VALID_BIT_TST(_lvl,_arr)    ((_arr)[(_lvl)>>5] & (1U<<(_lvl&31))) /* 31 == 0x1f == 0001 1111*/

/* virtual file types */
#define         FT_LOCAL        0
#define         FT_REMOTE       1

#define         FT_TXT          0
#define         FT_BIN          1

#define         INIT_STATE      -4

/* sizes */
#define         DESCRIPTIONSIZE         12000  /* PTS 1107138 ( old 512 )*/
#define         LINESIZE                4192
#define         WRNCMDBUFFERSIZE        15360  /* PTS 1104090 */
#define         CMDBUFFERSIZE           30720 /* PTS 1104090 */
#define         MAXREMOTEFILES          20
#define         MAXCMDARGS              640
#define         EXTENSIONSIZE           8     /* CTS 1107469 */

/*  Tolerate utime diffs of a second on non-unix systems */
#ifdef unix
# define    TOL     0
#else
# define    TOL     1
#endif

/* special extensions */
#define     EXT_CPP     "cpp"
#define     EXT_HPP     "hpp"
#define     EXT_C       "c"
#define     EXT_H       "h"
#define     EXT_P_T     "t"
#define     EXT_P_P     "p"
#define     EXT_ASM     "s"

/* special resource extensions */
#define     EXT_RC      "rc"
#define     EXT_ICO     "ico"
#define     EXT_DEF     "def"
#define     EXT_MC      "mc"
#define     EXT_DLG     "dlg"
#define     EXT_IDL     "idl"
#define     EXT_YCC     "ycc"
#define     EXT_LEX     "lex"
#define     EXT_RGS     "rgs"

/* language chars */
#define     LANG_PASCAL_T       "t"
#define     LANG_PASCAL_P       "p"
#define     LANG_ASM            "a"
#define     LANG_C              "c"
#define     LANG_CPP            "x"
#define     LANG_J              "j"
#define     LANG_RC             "r"

/* dependency types */ 
#define DT_INCDEP       0u
#define DT_OBJDEP       1u
#define DT_OBJDEMDEP    2u
#define DT_SRCDEMDEP    3u
#define DT_NODEP        4u
#define DT_ALLDEP       5u

/* macros */
#define DBG0(x)     {void dbgdbg (char*,...);extern int dbgdbglvl;extern int dbgcurlvl;dbgcurlvl=0; dbgdbg x;}
#define DBG1(x)     {void dbgdbg (char*,...);extern int dbgdbglvl;extern int dbgcurlvl;if(dbgdbglvl>=1) {dbgcurlvl=1; dbgdbg x;}}
#define DBG2(x)     {void dbgdbg (char*,...);extern int dbgdbglvl;extern int dbgcurlvl;if(dbgdbglvl>=2) {dbgcurlvl=2; dbgdbg x;}}
#define DBG3(x)     {void dbgdbg (char*,...);extern int dbgdbglvl;extern int dbgcurlvl;if(dbgdbglvl>=3) {dbgcurlvl=3; dbgdbg x;}}
#define DBG4(x)     {void dbgdbg (char*,...);extern int dbgdbglvl;extern int dbgcurlvl;if(dbgdbglvl>=4) {dbgcurlvl=4; dbgdbg x;}}
#define DBG5(x)     {void dbgdbg (char*,...);extern int dbgdbglvl;extern int dbgcurlvl;if(dbgdbglvl>=5) {dbgcurlvl=5; dbgdbg x;}}
#define DBG6(x)     {void dbgdbg (char*,...);extern int dbgdbglvl;extern int dbgcurlvl;if(dbgdbglvl>=6) {dbgcurlvl=6; dbgdbg x;}}
#define DBG7(x)     {void dbgdbg (char*,...);extern int dbgdbglvl;extern int dbgcurlvl;if(dbgdbglvl>=7) {dbgcurlvl=7; dbgdbg x;}}
#define DBG8(x)     {void dbgdbg (char*,...);extern int dbgdbglvl;extern int dbgcurlvl;if(dbgdbglvl>=8) {dbgcurlvl=8; dbgdbg x;}}
#define DBG9(x)     {void dbgdbg (char*,...);extern int dbgdbglvl;extern int dbgcurlvl;if(dbgdbglvl>=9) {dbgcurlvl=9; dbgdbg x;}}

#define STRCPY(dest , src , destlen ) \
{ \
    int srclen = strlen( src ); \
    (void) strncpy( dest, src , destlen - 1 ); \
    dest[ ( srclen < destlen ) ? srclen : destlen - 1 ] = '\0'; \
}
#define STRCAT(dest , src , destlen ) \
{ \
    int srclen = strlen( src ) + strlen( dest ); \
    (void) strncat( dest, src , destlen - 1 ); \
    dest[ ( srclen < destlen ) ? srclen : destlen - 1 ] = '\0'; \
}

#define FREE_AND_SET_TO_NULL(mem_ptr) {if (mem_ptr != NULL) {free (mem_ptr); mem_ptr=NULL; } }


#if unix
#define stricmp strcasecmp
#endif

#define NO_DLL_OPT  "-nodll" /* &gar CTS 1000237 */

#define ERROR_NO_CASE_SENSITIVE -25 /* &gar PTS 1102705 */

/* CTS 1105673 -> */

char * pDIR_INC;
char * pDIR_SIZE;
char * pDIR_TMP;
char * pDIR_OBJ;
char * pDIR_PROT;
char * pDIR_GSIZ;
char * pDIR_PSRC;
char * pDIR_CSRC;
char * pDIR_DATES;
char * pDIR_BROWSE;
char * pDIR_WRK;
char * pDIR_OUT;

static char stringSLOW[]  = "slow";
static char stringQUICK[] = "quick";
static char stringFAST[]  = "fast";
static char stringEMPTY[] = "";


/* <- CTS 1105673 */

/* CTS NEWVARIANT -> */
/* COMTYPES */
#define COM_NORMAL_MODULE				1   /* ven70c (matches all variants of it if there is'nt a xxx-<variant> definition */
#define COM_OLDVARIANT_MODULE			2   /* ven70c-m (match only local variant */
#define COM_NEWVARIANT_MODULE			3	/* ven70c:variant(...) */
#define COM_GLOBAL						4   /* globals.com */
#define COM_WILDCARD					6   /* match all */
#define COM_OLDVARIANT_WILDCARD         7   /* match only local variants */
#define COM_NEWVARIANT_WILDCARD         8   /* match local and gloab variants */
#define COM_REGEXP						9   /* regular expression */

#define NEW_VARIANT_DEFINITION			":variant"
#define VARIANT_SEPARATOR				','

/* <- CTS NEWVARIANT */

#define XML_TAG_MAKE "MAKE"

#define EPROT_SEPARATOR "\n=====================================================================\n\n"

#ifdef	_IBMR2
#define		MAXLINELENGTH	160	/* command output broken this length */
#else
#define		MAXLINELENGTH	72	/* command output broken this length */
#endif

#define VMAKE_PIDFILE "vmake.pid"
#define PIDFILE_ERROR -15


#define MCI_LAYER	     1
#define MCI_GLOBALS      2 /* other handling for Globals.com */
#define MCI_OVERWRITE    3
#define MCI_EXTRA        4

#define VMAKE_SILENT_MODE_DOT		1
#define VMAKE_SILENT_MODE_ROTATE	2
#define VMAKE_SILENT_MODE_NOTHING	3
#define VMAKE_SILENT_MODE_FREE   	4

#define VMAKE_REPORT_BEGIN			"BEGIN"
#define VMAKE_REPORT_END			"END"
#define VMAKE_REPORT_BOTH			"BOTH"
#define VMAKE_REPORT_PERCENT		"PERCENT"
#define VMAKE_REPORT_ALL			"ALL"

#define VMAKE_REPORT_MODE_BEGIN		1
#define VMAKE_REPORT_MODE_END		2
#define VMAKE_REPORT_MODE_BOTH		3
#define VMAKE_REPORT_MODE_PERCENT	4
#define VMAKE_REPORT_MODE_ALL		7

global int suppress_all_concom;

#define PLUS_LAYER "Globals"
#define PLUS_NAME "Globals"

#define PROTSTYLE_STANDARD_TEXT      "standard"
#define PROTSTYLE_STANDARD_VALUE     1
#define PROTSTYLE_QUALIFIED_TEXT     "qualified"
#define PROTSTYLE_QUALIFIED_VALUE    2
#define PROTSTYLE_SILENT_TEXT        "silent"
#define PROTSTYLE_SILENT_VALUE       9
#endif
