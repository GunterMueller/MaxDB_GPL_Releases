/*

    ========== licence begin LGPL
    Copyright (c) 2000-2005 SAP AG

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
/* begin of cpc.h */
#ifndef _CPC_H_1
#define _CPC_H_1 0

#define SQL_PC_VERSION 760
#define SQL_PC_BUILD_VERSION 32

typedef struct sqllongdesc {
  char *szBuf;
  int cbColLen;
  int cbBufLen;
  int cbBufMax;
} sqllongdesc;

typedef short          sqlint2;
typedef unsigned short sqluns2;


  typedef signed int     sqlint4;
  typedef unsigned int   sqluns4;
typedef signed long      sqllongint;
typedef unsigned short   sqlucs2;
typedef sqlucs2          SQLUCS2;
typedef unsigned short   sqlutf16;
typedef sqlutf16         SQLUTF16;

#ifndef _CHAR
#define _CHAR
typedef char CHAR;
#endif
#include <wchar.h>
#ifndef _WCHAR
#define _WCHAR
typedef wchar_t WCHAR;
#endif
#ifndef _LPSTR
#define _LPSTR
typedef char    *LPSTR;
#endif
#ifndef _LPWSTR
#define _LPWSTR
typedef wchar_t *LPWSTR;
#endif
#ifdef SAPDBwithUNICODE
#ifndef __TCHAR
#define __TCHAR
typedef SQLUCS2 TCHAR;
typedef SQLUCS2 _TCHAR;
#endif
#ifndef _LPTSTR
#define _LPTSTR
typedef LPWSTR LPTSTR;
#endif
#else
#ifndef __TCHAR
#define __TCHAR
typedef char TCHAR;
typedef char _TCHAR;
#endif
#ifndef _LPTSTR
#define _LPTSTR
typedef LPSTR LPTSTR;
#endif
#endif

#include "gpr00.h"
#include "vsp004.h"

#define       SQL_IS_NULL              (-1)
#define       SQL_OVERFLOW             (-2)
#define       SQL_DEFAULT_PARAM        (-5)
#define       SQL_IGNORE               (-6)

#define       sqlint2max               32767
#define       sqlint4min             - 2147483647
#define       sqlint4max               2147483647
#define       sqldbnamelengthmax       64
#define       sqlhostnamelengthmax     64
#define       sqlserverdblengthmax     18
#define       sqlservernodelengthmax   64
#define       sqlsnamelengthmax        12
#define       sqlnamelengthmax         18
#define       sqllnamelengthmax        32
#define       sqllinelengthmax         52
#define       sqlmacrolinelengthmax   126
#define       sqlgamax                  8
#define       sqlmamax                128
#define       sqlarglinelengthmax      132
#define       sqlmaxformkeys           20
#define       sqlmaxlabelkeys          96
#define       sqlnmax                 300
#define       sqldialogmax              4
#define       sqlvrpmx            8192000
#define       sqlldmx                 100
#define       sqlmaxpacketcnt           2    /* vsp001 */
#define       sqlidentifiermx          64    /* vsp00c */
#define       sqlIsTrue                1
#define       sqlIsFalse               0
#define       sqlvfilenamemax          256

/* SQLCODE macro for ANSI compatibility */

#define SQLCODE sqlca.sqlcode

/* SQLSTAE macro for ANSI compatibility */

#define SQLSTATE sqlca.sqlstate

/* Symbole fuer sqlvar - Eintraege */

/* colio: */
#define sqlinppar 0
#define sqloutpar 1
#define sqlinoutpar 2

/* colmode: */
#define sqlval    0
#define sqlundef  1

/* SAPDB column types specified in sqlvar->coltype */

/* numeric */
#define sqlfixed        0
#define sqlfloat        1
#define sqlsmallint    29
#define sqlinteger     30
#define sqlexpr        12
#define sqlboolean     23

/* character */
#define sqlchar         2
#define sqlvarchar     31

/* unicode */
#define sqlunicode     24
#define sqlvarunicode  36

/* date/time */
#define sqldate        10
#define sqltime        11
#define sqltimestamp   13

/* long character */
#define sqllong        19
#define sqloldlongchar  6

/* long unicode */
#define sqllonguni     35
#define sqloldlonguni  34

/* binary */
#define sqlbyte         4
#define sqldbebcdic    18

/* binary large objects */
#define sqllongbyte    21
#define sqloldlongbyte  8


/* SAPDB hostvariable data types specified in sqlvar->hostvartype */

/* numeric */
#define sqlvint1       49
#define sqlvint2        0
#define sqlvint4        1
#define sqlvint8       33
#define sqlvuns1       50
#define sqlvuns2       16
#define sqlvuns4       17
#define sqlvreal4       2
#define sqlvreal8       3

/* character */
#define sqlvchar        7
#define sqlvstring2    15
#define sqlvstring1    20
#define sqlvstring4    35

/* unicode character */
#define sqlvunic       42
#define sqlvucs2       52
#define sqlvutf16      53

/* long columns */
#define sqlvlongdesc   51

/* Macro Definitions for the VARCHAR Pseudotype: */

#define VARCHS(n) struct {sqluns2 len; unsigned char arr [n];}
#define VARCHD(t,n) struct t {sqluns2 len; unsigned char arr [n];}
#define VARCHR(t) struct t
#define VARCHS1(n) struct {unsigned char len; unsigned char arr [n];}
#define VARCHD1(t,n) struct t {unsigned char len; unsigned char arr [n];}
#define VARCHS4(n) struct {sqluns4 len; unsigned char arr [n];}
#define VARCHD4(t,n) struct t {sqluns4 len; unsigned char arr [n];}

/* Macro Definitions for the DECIMAL Pseudotype: */

#define DECIMS(n) struct {char arr [n];}

#ifdef sql_oracle
/* Macro Definition for the RAW Pseudotype: */

typedef  char raw;

#endif

/* Macro Definition for the SQLFILE Pseudotype: */

#define sqlfile char

/* SAA sqlda Description: */

#if defined(sql_db2)
#ifndef SQLDASIZE
#define SQLDASIZE(n) (sizeof(struct sqlda) + (n-1)*sizeof(struct sqlvar))
#endif
#endif

/* Unicode Macros and Types */

#ifndef SAPDBwithUNICODE
  #define SQLCPCCHECK sqlCPCCheck
  #define sqlctoR(par)   par
  #define sqlsizeofR(par) sizeof(par)
#else
  #define SQLCPCCHECK sqlCPCCheckU
  #define sqlctoR(par)  cR(par)		/* Macro for non-Unicode constants */
  #define cR(par)       par		/* cR is a dummy for sap compatible ccQ-check*/
  #define sqlsizeofR(par) sizeofR(par)  /* Macro for sizeof-operator for non-Unicode*/
  #define sizeofR(par)    sizeof(par)	/* sizeofR is a dummy for sap compatible ccQ-check*/
#endif

typedef float      sqlr4;
typedef double     sqlr8;

typedef unsigned char sqlparsid   [16] ,
                   sqlsname      [sqlsnamelengthmax] ,
                   sqlnname      [sqlnamelengthmax] ,
                   sqllname      [sqllnamelengthmax] ,
                   sqllinetype   [sqllinelengthmax] ,
                   sqlmacroline  [sqlmacrolinelengthmax] ,
                   sqldbname     [sqldbnamelengthmax] ,
                   sqldbnode     [sqlhostnamelengthmax] ,
                   sqlserverdb   [sqlserverdblengthmax] ,
                   sqlservernode [sqlservernodelengthmax] ,
                   sqlhostname_t   [sqlhostnamelengthmax] ,
                   sqlargline    [sqlarglinelengthmax] ,
                   sqluser [18],
                   sqlpassword [18];
typedef unsigned char sqlidentifier [ sqlidentifiermx ];
typedef unsigned char sqlvfilename [ sqlvfilenamemax];
typedef sqlint4    sqlcrypt [6];

typedef DECIMS(1) decimal1;
typedef DECIMS(2) decimal2;
typedef DECIMS(2) decimal3;
typedef DECIMS(3) decimal4;
typedef DECIMS(3) decimal5;
typedef DECIMS(4) decimal6;
typedef DECIMS(4) decimal7;
typedef DECIMS(5) decimal8;
typedef DECIMS(5) decimal9;
typedef DECIMS(6) decimal10;
typedef DECIMS(6) decimal11;
typedef DECIMS(7) decimal12;
typedef DECIMS(7) decimal13;
typedef DECIMS(8) decimal14;
typedef DECIMS(8) decimal15;

#if defined(__cplusplus)
typedef struct /* Descriptor for dynamic allocated memory */
#else
typedef struct sqlmdesc /* Descriptor for dynamic allocated memory */
#endif
{

	sqluns4 descMaxelem;	/* number of allocated elements */
	sqluns4 descNelem;	/* number of used elements */
	sqluns4 descElemSize;	/* element size in bytes */
	sqluns4 descIncr;	/* number of elements allocated in one step */
	void **descElemPtr;	/* pointer to pointer to first element */
} sqlmdesc;

struct SQLCOL
{
        char coltype;
        char colmode;
        char colio;
        char colfrac;
        sqlint4 collen;
        sqlint4 colpos;           /* column buffer position (relative) */
        sqlint2 coliolen;         /* column byte length */
        char colisnum;          /* (numeric column) ? 1 : 0 */
        char colisuni;          /* (unicode column) ? 1 : 0 */
        char colislong;         /* (long column) ? 1 : 0 */
        sqlint2 colvartype;       /* host variable type */
        sqlint2 colindtype;       /* indicator variable type */
#if defined(__cplusplus) || defined(__STDC__)
        int (*coltobuf) (const struct SQLCOL *col, char *buf, const void *va,
                sqlint4 cl, sqlint4 vs, sqlint2 vl, sqlint2 vf);
        int (*colfrombuf) (const struct SQLCOL *col, const char *buf,
                void *va, sqlint4 cl, sqlint4 vs, sqlint2 vl, sqlint2 vf);
                         /* input and output conversion functions */
        int (*coltoindi) (sqlint4 ival, void *indi,sqlint2 indl,sqlint2 indf);
        int (*colfromindi) (sqlint4 *ival, const void *indi, sqlint2 indl,
                sqlint2 indf); /* indicator conversion functions */
#else
        int (*coltobuf) ();
        int (*colfrombuf) (); /* input and output conversion functions */
        int (*coltoindi) ();
        int (*colfromindi) (); /* indicator conversion functions */
#endif
};

struct  SQLERROR
{
        sqlint2       ereturncode;
	sqlint2       etextlen;
	sqlint4       eerrorpos;
        char          ewarning [16];
        char          esqlstate [6];
        char          elzu;
        char          eprerr;
        unsigned char etext [70];
};

struct SQLROW
{
        sqlint4 ireclen; /* irecord length = Max (icolpos) + its (coliolen) */
        sqlint4 oreclen; /* orecord length = Max (ocolpos) + its (coliolen) */
};


struct SQLPCLD
{
#if defined(__cplusplus) || defined(__STDC__)
        void *ldhostaddr;
        void *ldindkaddr;
#else
        char *ldhostaddr;
        char *ldindkaddr;
#endif
        struct SQLCOL *ldcoladdr;
        sqlint4 ldhostlen;
        sqlint2 ldhosttype;
        sqlint2 ldindktype;
        sqlint2 ldindklen;
        sqlint2 ldindkfrac;
        sqlint2 ldrowno;
        sqlint2 ldcolno;
        tsp00_Uint1 ldsfiotype;
        tsp00_DataType ldsfdtype;
	sqlint2 ldfill;
        sqlint4 ldfileno;
        sqlint4 ldsfbufpos;
        sqlint4 ldhostlensum;
	struct SQLRESTBUF *ldrestdata;
};

struct OLDSQLSPLD
{
        char     ld_descriptor [8];
        char     ld_tabid [8];
        sqlint4  ld_maxlen;
        sqlint4  ld_intern_pos;
        char     ld_infoset;
        char     ld_state;
        char     ld_used_in_ak;
        char    ld_valmode;
        sqlint2 ld_valind;
        sqlint2 ld_unused_2;
        sqlint4 ld_valpos;
        sqlint4 ld_vallen;
};

struct NEWSQLSPLD
{
        char    ld_descriptor [8];
        char    ldb_serverdb_no [2];
        char    ld_change;
        char    ld_replicated;
        sqlint4 ldb_curr_pageno;
        sqlint2 ldb_curr_pos;
        char    ldb_colno;
        char    ldb_show_kind;
        sqlint4 ld_intern_pos;
        char    ldb_infoset;
        char    ldb_filler1;
        unsigned char ldb_filler1a;
        char    ld_valmode;
        sqlint2 ld_valind;
        sqlint2 ld_oldvallen;
        sqlint4 ld_valpos;
        sqlint4 ld_vallen;
        sqlint4 ldb_full_len;
        char    ldb_filler2 [4];
        sqlint4 ldb_last_i_pos;
        sqlint4 ldb_root_check;
};

struct SQLSPLD
{
        sqlint2 ldkind;
        sqlint2 ldlength;
        union
        {
                struct OLDSQLSPLD o;
                struct NEWSQLSPLD n;
        } ld;
};

struct SQLRESTBUF
{
	sqlint2 rbcolno;
	sqlint2 rbfiller1;
	sqlint4 rbfiller2;
	sqlint4 rbrestlen;
	sqlint4 rbhostlensum;
	sqlint4 rblongcolmax;
	sqlint4 rbbufsize;
#if defined(__cplusplus) || defined(__STDC__)
        void    *rbbufpart;
#else
        char    *rbbufpart;
#endif
};

/* declaration of struct SQLLD later */

typedef struct
{
 sqlvfilename   tatracefn;
 sqlint4        tatracepprt;
 sqlint4        tatraceno;
 sqlint2        tatracety;
 sqlint2        tamaxsecoutp;
 sqlvfilename   tamaxsecfn;
 sqlint4        tamaxsecno;
 sqlint4        tamaxsecpptr;
 tsp00_Date     tastartdate;
 tsp00_Date     taenddate;
 tsp00_Time     tastarttime;
 tsp00_Time     taendtime;
 sqlint2        tacnttrstm ;
 sqlint2        tatrout;
 sqlint2        tafiller;
 sqlint2        tastr80l;
 char         tastr80 [256];
}
sqltatype;

typedef tsp4_xuser_record sqlxuserrec;

typedef struct     {
                      sqlint4       gareference;
                      char          gamesscode [2];
                      sqlint2       gastatus;
                      sqlint2       gavaindex;
                      sqlint2       ganodeindex;
                      sqlint2       gareconnect;
                      sqlint2       ganolog;
                      sqlint2       gaprofinit;
                      sqlint2       gadbcodeset;
                      sqlidentifier gaatdbname;
                      sqlint2       gafiller2;
#if defined(__cplusplus) || defined(__STDC__)
                      void         *gapatchptr;
                      void         *gareqptr;
                      void         *garecptr;
                      void         *gapacketlist [sqlmaxpacketcnt];
#else
                      char         *gapatchptr;
                      char         *gareqptr;
                      char         *garecptr;
                      char         *gapacketlist [sqlmaxpacketcnt];
#endif
                      sqlxuserrec   gaxuserrec;
                      tsp00_Int4    gaKernelVersion;
                      char         *gaServerNode;
                      char          gaIsCanceled;
                      tpr00_SessionID gaKnlSessionID;
                      }
                   sqlgaentry;

typedef struct     {
  sqlxuserrec    gaopxuserrec;
  sqlint2        gauseropset;
  sqlint2        gamodisolation;
  sqlint2        gasqlconntyp;
  sqlint2        gaencodingcnt;
#if defined(__cplusplus) || defined(__STDC__)
  void           *gaencodingptr;
#else
  char           *gaencodingptr;
#endif
  sqlint2        gaasciicodeset;
  sqlint2        gaunicodeset;
  sqlint2        gacmpkind;
  tpr_cmd_state_Enum gaCmdState;
  struct tpr01_ConContainer *gaConnection;
  sqlgaentry    *gaentry [sqlgamax];
     }
     sqlgatype;

typedef struct
{
        char     ratermid [8];
        sqlint4  raprocessid;
        char     racomponent  [3];
        char     rasqlos;
        sqlint2  ralang;
        sqlint2  rasqlinit;
        sqlint2  raactsession;
        sqlint2  radbmode;
        sqlint2  rasqlansi;
        sqlint2  rakamode;
        sqlint2  ramodeswitch;
        sqlint2  ramesstype;
        sqlint4  ranumrows;
        sqlint2  ranotrtimeoutp;
        sqlint2         raopprof;
        sqlint4  raprofsec;
        char     raparse_again;
        char     rafiller [3] ;
        sqlint4  ra2filler;
#if defined(__cplusplus) || defined(__STDC__)
        void         *rasegptr;
        tpr00_PartCache rasegmpartptr;
        void         *raprofstmtp;
#else
        char         *rasegptr;
        tpr00_PartCache rasegmpartptr;
        char         *raprofstmtp;
#endif
        struct SQLLD  *rasqlldp;
        sqltatype     *rasqltap;
}
sqlratype;

typedef struct     {
 sqlvfilename    oamodtracefn;
 sqlint2         oamodtrace;
 sqlint2         oamodtracety;
 sqlint2         oamodsettrty;
 sqlint2         oamaxtrstm;
 sqlint2         oanosdf;
 sqlint2         oamaxsec;
 sqlint2         oapidflag;
 sqlint4         oapid;
}
sqloatype;  /**/

typedef struct     {
                      sqlint2       malen;
                      sqlmacroline  maval;
                   }
                   mavalue;
typedef mavalue    sqlmaentries [sqlmamax] ;
typedef struct     {
                      sqlint4       maversion;
                      sqlmaentries  maentry;
                   }
                   sqlmatype;

typedef struct     {
                      sqlint2          mfInit,
                                       mfSelectno,
                                       mfBufinit,
                                       mfReclen;
		      sqlint4	       mfBuflen,
                                       mfRecpos;
                      sqlint2          mfPrevfaspec,
                                       mfLastfaspec,
                                       mfRownotfound,
				       mfFetchget;
		      sqlint4          mfRecposbeg,
		                       mfMaxselrec,
				       mfLastrecno,
				       mfNextLoopCnt,
				       mfAllRecordsRead,
				       mfActualPos,
                                       mfBufbegno,
                                       mfBufendno,
                                       mfLongcolpos;
#if defined(__cplusplus) || defined(__STDC__)
		      void	      *mfBufpointer;
		      void            *mfrecpointer;
                      tpr00_PartCache mfsegmpartptr;
#else
		      char	      *mfBufpointer;
		      char            *mfrecpointer;
                      tpr00_PartCache mfsegmpartptr;
#endif
		      sqlint4	       mfrecptrlen;
		      sqlint2	       mforindex;
		      sqlint2	       mffetchkind;
		      sqlint4	       mfsumanzrec;
		      sqlint2	       mfopendata;
		      sqlint2	       mfPrecomindex;
		      sqlparsid        mfSelparsid;
                      sqlidentifier    mfresultname;
                      tpr05_StringEncoding PacketEncoding;
                   }
                   sqlmfentry;

typedef struct     {
                      sqlint2       mfselcnt,
                                    mfetch;
                      sqlint4       mffiller;
                      sqlmdesc      mfdesc;
                      sqlmfentry   *mfentry;
                   }
                   sqlmftype;

typedef struct tpr01_EnvDesc     sqlEnvironDesc;
typedef struct tpr01_ModuleDesc  sqlModuleDesc;
typedef struct tpr01_CommandDesc sqlCmdDesc;
typedef struct tpr01_SQLDesc     sqlSQLDesc;
typedef struct tpr00_OldAreas    sqlOldAreas;

typedef struct     {
                      sqlint2       xainit,
                                    xacmdinit,
                                    xakano;
                      sqlidentifier xaprogn;
                      sqlint2       xaprogc;
                      sqlidentifier xamodn;
                      sqlint2       xamodc,
                                    xalang;
                      sqlModuleDesc *sqlModDesc;
                      sqlSQLDesc    *xaSQLDesc;
                      sqlint4       xacfetposc;
                      sqlint2       xaprno;
                      sqlsname      xalcheck;
                      sqlidentifier xaprname;
                      sqlint2       xafiller1;
#if defined(__cplusplus) || defined(__STDC__)
                      void         *xasqldap;
#else
                      char         *xasqldap;
#endif
                      sqlint2       xacfetspec;
                      char          packet_filler [6];
                   }
                   sqlcxatype;    /**/

#if defined (sql_sqldb) || defined (sql_default)
typedef struct     {
                      sqlidentifier colname;
                      sqlint2       colio;
                      char          colmode,
				    colfill1 [3];
                      sqlint2       coltype;
                      sqlint4       collength;
                      sqlint2       colfrac,
                                    colfill2,
                                    hostvartype,
                                    hostcolsize;
                      sqlint4       hostindicator;
#if defined(__cplusplus) || defined(__STDC__)
                      void         *hostvaraddr;
#else
                      char         *hostvaraddr;
#endif
                      sqlint4      *hostindaddr;
                      struct SQLCOL col;
              }
                   sqlvartype;
typedef struct     {
                      char          sqldaid [8] ;
                      sqlint4       sqlmax;
                      sqlint2       sqln,
                                    sqld;
                      sqlint4       sqlloop,
                                    sqloffset;
                      sqlint2       sqlkano,
                                    sqlprno,
                                    sqlkamode,
                                    sqlfill;
                      struct SQLROW sqlrow;
                      sqlvartype    sqlvar [sqlnmax] ;
                   }
                   sqldatype;
#endif

#if defined (sql_db2)
/**/
/*  SAA - sqlda Description */
/**/
struct sqlda       {
                   unsigned char sqldaid [8];
                   sqlint4       sqldabc;
                   sqlint2       sqln;
                   sqlint2       sqld;
                   sqlint4       sqlloop,
                                 sqloffset;
                   sqlint2       sqlkano,
                                 sqlprno,
                                 sqlkamode,
                                 sqlfill;
                   struct SQLROW sqlrow;
                   struct sqlvar {
                                 sqlint2        sqltype;
                                 sqlint2        sqllen;
                                 unsigned char *sqldata;
                                 sqlint2         *sqlind;
                                 struct SQLNAME {
                                                sqlint2         length;
                                                unsigned char data [30];
                                                } sqlname;
                                 struct SQLCOL col;
                         } sqlvar [1];
                   };

/**/
#endif

#ifdef sql_oracle
struct SQLDA {
          unsigned char sqldaid [8];
          sqlint4 sqldabc;
          sqlint4 sqlfill;
          sqlint4 sqlloop;
          sqlint4 sqloffset;
          sqlint2 sqlkano,
                  sqlprno;
          sqlint2 sqlkamode,
                  sqlfill2;
          struct SQLROW sqlrow;
          struct SQLCOL *sqlcol;
          sqlint4    N;   /* Descriptor size in number of entries         */
          char     **V;   /* Ptr to Arr of addresses of main variables    */
          sqlint4   *L;   /* Ptr to Arr of lengths of buffers             */
          sqlint2   *T;   /* Ptr to Arr of types of buffers               */
          sqlint2  **I;   /* Ptr to Arr of addresses of indicator vars    */
          sqlint4    F;   /* Number of variables found by DESCRIBE        */
          char     **S;   /* Ptr to Arr of variable name pointers         */
          sqlint2   *M;   /* Ptr to Arr of max lengths of var. names      */
          sqlint2   *C;   /* Ptr to Arr of current lengths of var. names  */
          char     **X;   /* Ptr to Arr of ind. var. name pointers        */
          sqlint2   *Y;   /* Ptr to Arr of max lengths of ind. var. names */
          sqlint2   *Z;   /* Ptr to Arr of cur lengths of ind. var. names */
          /* only cobol */
          char     **FM;  /* Ptr to Arr of hostvar decimal pointers       */
          sqlint2  *FML;  /* Ptr to Arr of lengths of hostvar decimal     */
          sqlint2 *FCLP;  /* nil  */
          sqlint2 *FCRC;  /* nil  */
          };

#ifndef __cplusplus
typedef struct SQLDA SQLDA;
#endif
#endif

enum SQLVAINDI  {SQLVASC = 0, SQLVACM, SQLVAST, SQLVAPT};

typedef struct
{
        sqlint2 va1indi;
        sqlint2 va1indva2;
        sqlint2 va1indva3;
        sqlint2 va1fill;
#if defined(__cplusplus) || defined(__STDC__)
        void *va1addr;
        void *va1fill1;
#else
        char *va1addr;
        char *va1fill1;
#endif
}
sqlvascalar;

typedef struct
{
        sqlint2 va1indi;
        sqlint2 va1indva2;
        sqlint2 va1indva3;
        sqlint2 va1fill;
        sqlint4 va1offs;
        sqlint4 va1fill2;
}
sqlvacomponent;

typedef struct
{
        sqlint2 va1indi;
        sqlint2 va1cmpcnt;
        sqlint4 va1size;
#if defined(__cplusplus) || defined(__STDC__)
        void *va1addr;
#else
        char *va1addr;
#endif
        sqlint4 va1ix;
}
sqlvastruct;

typedef struct
{
        sqlint2 va1indi;
        sqlint2 va1fill3;
        sqlint4 va1ix;
#if defined(__cplusplus) || defined(__STDC__)
        void *va1addr;
        void *va1fill5;
#else
        char *va1addr;
        char *va1fill5;
#endif
}
sqlvaptr;

typedef union
{
        sqlvascalar sc;
        sqlvacomponent cm;
        sqlvastruct st;
        sqlvaptr pt;
}
sqlva1en;

typedef struct     {
                   sqlint2 va2typ;
                   sqlint2 va2digit;
                   sqlint4 va2size;
                   sqlint2 va2frac;
                   sqlint2 va2const;
                   }
                   sqlva2en;
typedef struct     {
                   sqllname va3name;
                   sqlint2 va3naml;
                   sqlint2 va3fill;
                   }
                   sqlva3en;

typedef sqlva1en *sqlv1pointer;
typedef sqlva2en *sqlv2pointer;
typedef sqlva3en *sqlv3pointer;

enum SQLPARINDI {SQLPARLO = 0,
		SQLPARST,
                SQLPAREL};

/* next lines will not include in cpc.h */
/* cpc.h will be generated from vpi00c-i */

#ifndef SQLDYNP
#define SQLDYNP(a) a
/* this macro will be use real pointer to areas  */
/* in cpc.h all pointers to areas are void.      */
#endif

typedef struct     {
                      sqlint2       pakind;
                      sqlint2       pava1index;
                      sqlint4       paloopcnt;
                      sqlint4       paloopmin;
}
        sqlparlooptyp;
typedef struct     {
                      sqlint2       pakind;
                      sqlint2       pavarst;
                      sqlint2       paindst;
                      sqlint2       paelcnt;
                      sqllongint    pafill4;
}
        sqlparstructyp;
typedef struct     {
                      sqlint2       pakind;
                      sqlint2       pavarno;
                      sqlint2       paindno;
}
        sqlparelemtyp;

typedef union     {
        sqlparlooptyp    lo;
        sqlparstructyp   st;
        sqlparelemtyp    el;
}
sqlparentry;

typedef struct
{
        sqlint2 faselectno;
        sqlint2 famfindex;
        sqlint4 fapos;
        sqlint2 faspec;
        sqlint2 fafetch;
        sqlint4 fareclen;
} sqlfaentry;

typedef struct     {
                      sqlint2       katyp,
                                    kastate,
                                    kapacount,
                                    kapaindex,
                                    kaStcount,
                                    kaStindex,
                                    kamacro,
                                    kaprindex,
                                    kadiindex,
                                    kaparserrc;
                      sqlint4       kaversion;
                      tpr00_ParseInfo kaParseInfo;
                      sqlint2       kaopendata;
                      sqlint2       kaparamcnt,
                                    kalineno,
                                    kafaindex,
                                    kaatindex,
                                    kacuindex,
                                    kamode;
                      struct SQLROW kasqlrow;
                      sqlfaentry    *kafae;
                   } sqlkaentry;

typedef struct     {
		      sqlint4	    orcolcnt;
		      sqlint4	    orcollen;
                      struct SQLROW orsqlrow;
		      struct SQLCOL *orcolptr;
		      sqlint4	    orcolcntacc;
                      sqlint2       oropennew;
		      sqlint4	    orrescnt;
		      sqlint4	    orreslen;
                      struct SQLROW orressqlrow;
		      struct SQLCOL *orresptr;
		      sqlint4	    orrescntacc;
		      sqlint4	    orresnamlen;
#if defined(__cplusplus) || defined(__STDC__)
		      void	      *orresnamptr;
#else
		      char	      *orresnamptr;
#endif
		      sqlint4	    orresnamacc;
		      void	    *orresnamencoding;
                   }
                   sqlorentry;

typedef struct     {
                      sqlint2       prStmtNameIndex,
			            prCursorNameIndex,
                                    prstate;
                      sqlidentifier prStmtName;
                      sqlint2       prkaindex,
                                    prarea,
			            prDescribe;
                      sqlidentifier prCursorName;
                      sqlint2       prcuindex,
                                    prstcount,
                                    prstindex;
                   }
                   sqlprentry;

typedef struct     {
                      sqlint4       stkano;
                      sqlint4       stllen;
                      sqllinetype   stline;
                   }
                   sqlstentry;

typedef struct
{
        sqlint2 fnkanomin;
        sqlint2 fnkanomax;
        sqlidentifier fnfilen;
        sqlint2 fnincllno;
} sqlfnentry;

typedef struct     {
                      sqlidentifier atdbname;
                      sqlint2       atdbindex;
                      sqldbnode     atdbstring;
                      sqlint2       atgaindex,
                                    atdbnamix;
                   }
                   sqlatentry;

typedef struct     {
                      sqlidentifier cuname;
                      sqlint2       cufill;
                      sqlint2       cuatindex;
                      sqlint4       cuerrd3;
                      sqlint2       custate,
                                    cukind,
			            cukaopenind;
                      sqlint4       culoopin;
                      sqlint4       culoopout;
                      void          *cusqlindap;
                      void          *cusqloutdap;
                   }
                   sqlcuentry;

typedef struct
{
	/** sqlsn-entry statementname ***/
        sqlidentifier  snname;
        sqlint2        sntype;
        sqlint2        snindex;
        sqlint2        sncount;
	sqlint2        snfiller1;
        char          *snstmtp;
        sqlint4        snbuflen;
        sqlint4        snstmtlen;
	sqlint2        snsession;
	sqlint2        snfiller2;
	tpr00_ParseInfo snParseInfo;
}
sqlsnentry;


#ifndef SQLDYNP
#define SQLDYNP(a) void
#endif

#ifndef SQLENUMA
#define SQLENUMA(VarName, MemberName ) sql##VarName##MemberName
#endif

#ifndef SQLXENUMA
#define SQLXENUMA(VarName, MemberName, VarType, MemberType ) SQLENUMA(VarName, MemberName)
#endif

typedef enum SQLAREAStag {
SQLXENUMA(sqlxa, sqlkap, sqlxatype, sqlkaentry),
SQLXENUMA(sqlxa, sqlpap, sqlxatype, sqlparentry),
SQLXENUMA(sqlxa, sqlorp, sqlxatype, sqlorentry),
SQLXENUMA(sqlxa, sqlprp, sqlxatype, sqlprentry),
SQLXENUMA(sqlxa, sqlstp, sqlxatype, sqlstentry),
SQLXENUMA(sqlunused, sqlunused1, sqlunused, sqlunused),
SQLXENUMA(sqlunused, sqlunused2, sqlunused, sqlunused),
SQLXENUMA(sqlxa, sqlfap, sqlxatype, sqlfaentry),
SQLXENUMA(sqlxa, sqlfnp, sqlxatype, sqlfnentry),
SQLXENUMA(sqlxa, sqlatp, sqlxatype, sqlatentry),
SQLXENUMA(sqlxa, sqlcup, sqlxatype, sqlcuentry),
SQLXENUMA(sqlxa, sqlkae, sqlxatype, sqlkaentry),
SQLXENUMA(sqlxa, sqlpae, sqlxatype, sqlparentry),
SQLXENUMA(sqlxa, sqlore, sqlxatype, sqlorentry),
SQLXENUMA(sqlxa, sqlpre, sqlxatype, sqlprentry),
SQLXENUMA(sqlxa, sqlste, sqlxatype, sqlstentry),
SQLXENUMA(sqlunused, sqlunused3, sqlunused, sqlunused),
SQLXENUMA(sqlunused, sqlunused4, sqlunused, sqlunused),
SQLXENUMA(sqlxa, sqlfae, sqlxatype, sqlfaentry),
SQLXENUMA(sqlxa, sqlfne, sqlxatype, sqlfnentry),
SQLXENUMA(sqlxa, sqlate, sqlxatype, sqlatentry),
SQLXENUMA(sqlxa, sqlcue, sqlxatype, sqlcuentry),
SQLXENUMA(sqlca, sqlgap, sqlcatype, sqlgatype),
SQLXENUMA(sqlca, sqlrap, sqlcatype, sqlratype),
SQLXENUMA(sqlca, sqloap, sqlcatype, sqloatype),
SQLXENUMA(sqlca, sqlmap, sqlcatype, sqlmatype),
SQLXENUMA(sqlca, sqlmfp, sqlcatype, sqlmftype),
SQLXENUMA(sqlgap, gaentry, sqlgatype, sqlgaentry),
SQLXENUMA(gaentry, gareference, sqlgaentry, sqlint4),
SQLXENUMA(gaentry, gareqptr, sqlgaentry, void*),
SQLXENUMA(gaentry, garecptr, sqlgaentry, void*),
SQLXENUMA(gaentry, gaxuserrec, sqlgaentry, sqlxuserrec)
} SQLAREAS;

typedef SQLDYNP(sqlparentry) *sqlparpointer;
typedef SQLDYNP(sqlkaentry) *sqlkapointer;
typedef SQLDYNP(sqlorentry) *sqlorpointer;
typedef SQLDYNP(sqlprentry) *sqlprpointer;
typedef SQLDYNP(sqlstentry) *sqlstpointer;
typedef SQLDYNP(sqlfaentry) *sqlfapointer;
typedef SQLDYNP(sqlfnentry) *sqlfnpointer;
typedef SQLDYNP(sqlatentry) *sqlatpointer;
typedef SQLDYNP(sqlcuentry) *sqlcupointer;
typedef SQLDYNP(sqlgatype) *sqlgapointer;
typedef SQLDYNP(sqlratype) *sqlrapointer;
typedef SQLDYNP(sqloatype) *sqloapointer;
typedef SQLDYNP(sqlmatype) *sqlmapointer;
typedef SQLDYNP(sqlmftype) *sqlmfpointer;

struct SQLLD
{
        sqlint2 ldmaxi;
        sqlint2 ldmaxo;
        sqlint2 ldindpc;
        sqlint2 ldindsp;
        sqlint2 ldcolkind;
        sqlint2 ldcount;
        char ldputvalsend;
        char ldallsend;
        sqlint2 ldretmaxi;
        sqlint2 ldretmaxo;
        sqlint2 ldretindpc;
        sqlint2 ldretindsp;
        struct SQLRESTBUF *ldrestbufp;
	sqlint4 lderrd3;
        struct SQLPCLD *ldpc;
        struct SQLSPLD *ldsp;
        sqlmdesc ldpc_mdesc;
        sqlmdesc ldsp_mdesc;
	struct SQLPCLD *ldpcarr;
	struct SQLSPLD *ldsparr;
        sqlmdesc ldpcarr_mdesc;
        sqlmdesc ldsparr_mdesc;
        sqlorpointer ldorelastcmd; /* ref. to sqlor area of cmd before putval/getval */
};

typedef struct
{
	sqlint4  snentcnt;
	sqlint4  snentact;
        sqlsnentry * snentptr;
}
sqlsn;

typedef struct     {
                      sqlint2       xainit,
                                    xacmdinit,
                                    xakano;
                      sqlidentifier xaprogn;
                      sqlint2       xaprogc;
                      sqlidentifier xamodn;
                      sqlint2       xamodc,
                                    xalang;
                      sqlModuleDesc *xaModDesc;
                      sqlSQLDesc    *xaSQLDesc;
                      sqlint4       xacfetposc; /* odbc */
                      sqlint2       xaprno;
                      sqlsname      xalcheck;
                      sqlidentifier xaprname;
                      sqlint2       xastopcnt,
                                    xallen;
                      sqllinetype   xaline;
                      sqlint2       xafiller3;
                      sqlint4       xatime;
                      sqlint2       xadescpos,
                                    xadesclen;
                      sqlint2       xaatcount,
                                    xaatmax;
                      sqlint4       xafiller4;
                      sqlparpointer sqlpap, sqlpae;
                      sqlkapointer  sqlkap, sqlkae;
                      sqlprpointer  sqlprp, sqlpre;
                      sqlstpointer  sqlstp, sqlste;
                      sqlfapointer  sqlfap, sqlfae;
                      sqlatpointer  sqlatp, sqlate;
                      sqlv1pointer  sqlv1p, sqlv1e;
                      sqlv2pointer  sqlv2p, sqlv2e;
                      sqlv3pointer  sqlv3p, sqlv3e;
                      void         *sqlfill1, *sqlfill2;
                      void         *sqlfill3, *sqlfill4;
                      sqlcupointer  sqlcup, sqlcue;
                      sqlorpointer  sqlorp, sqlore;
                      sqlfnpointer  sqlfnp, sqlfne;
                      sqlsn	    sqlsnp;
                      sqlmdesc	    sqlsndesc;
                   }
                   sqlxatype;

#if defined(sql_oracle) && ! defined(__cplusplus)
typedef struct sqlca {
#else
typedef struct     {
#endif
                      sqlint4       sqlenv;
                      char          sqlcaid [8] ;
#ifdef sql_oracle
                      sqlint4       sqlabc,
#else
                      sqlint4       sqlcabc,
#endif
                                    sqlcode;
#ifdef sql_oracle
                      struct {
                        sqluns2 sqlerrml;
                        char           sqlerrmc [70];
                        } sqlerrm;
#else
                      sqlint2       sqlerrml;
                      char          sqlerrmc [70] ;
#endif
                      char          sqlerrp [8] ;
                      sqlint4       sqlerrd [6] ;
#ifdef sql_oracle
                      char          sqlwarn [16];
#else
                      char          sqlwarn0,
                                    sqlwarn1,
                                    sqlwarn2,
                                    sqlwarn3,
                                    sqlwarn4,
                                    sqlwarn5,
                                    sqlwarn6,
                                    sqlwarn7,
                                    sqlwarn8,
                                    sqlwarn9,
                                    sqlwarna,
                                    sqlwarnb,
                                    sqlwarnc,
                                    sqlwarnd,
                                    sqlwarne,
                                    sqlwarnf;
#endif
                      char          sqlext [12] ;
                      sqlidentifier sqlresn;
                      sqlint2       sqlfill1,
                                    sqlmfetch;
                      sqlint4       sqltermref;
                      sqlint2       sqlfill5,
                                    sqldbmode,
                                    sqldatetime;
                      char          sqlstate [6];
                      sqlargline    sqlargl;
                      sqlgapointer  sqlgap;
                      sqlrapointer  sqlrap;
                      sqloapointer  sqloap;
                      sqlmapointer  sqlmap;
                      sqlmfpointer  sqlmfp;
#if defined(__cplusplus) || defined(__STDC__)
                      void         * sqlplp;
#else
                      char         * sqlplp;
#endif
                      struct SQLERROR *sqlemp;
		      sqlmdesc  sqlxadesc;
                      sqlcxatype    *sqlcxap;
                      sqlEnvironDesc  *sqlEnvDesc;
                   }
                   sqlcatype;

  /* _CPC_H_1 */
#endif

/* end of first part of cpc.h */

