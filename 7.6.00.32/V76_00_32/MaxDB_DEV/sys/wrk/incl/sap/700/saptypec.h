/*@(#)$Id: //sapdb/V76/c_00/b_32/sys/src/sap/700/saptypec.h#1 $ SAP*/

/*--------------------------------------------------------------------*/
/* /usr/projects/sccs/src/s.saptypec.h */

/*
 *  ------------------------------------------------------------------
 *  SAP  Syteme, Anwendungen und Produkte in der Datenverarbeitung
 *
 *  This is a part of the central include file, which should be
 *  included by all SAP C software.
 *
 *  General Data Dictionary defined types.
 *  ======================================
 */

#ifndef SAPTYPEC_H
#define SAPTYPEC_H 8
#define SAPTYPEC_HV cU("$Id: //sapdb/V76/c_00/b_32/sys/src/sap/700/saptypec.h#1 $")

/* --------------------------------------------------------------- */
/*                                                                 */
/*     H o s t n a m e                                             */
/*                                                                 */
/* --------------------------------------------------------------- */
/*                                                                 */
/* The following constants are the new length limits for hostnames */
/*                                                                 */
/*   SAP_HOST_ID_LN:   the final length of the hostname used for   */
/*                     storing, moves, compares etc.               */
/*                                                                 */
/*   SAP_HOST_REAL_LN: intermediate limitation of the maximum real */
/*                     hostname length,                            */
/*                     for compatibility with the 13-char hostname */
/*                     currently contained in the instance name.   */
/*                                                                 */
/* --------------------------------------------------------------- */
#define SAP_HOST_REAL_LN      (CCQ_CAST(intU) 13)

#define SAP_HOST_ID_LN        (CCQ_CAST(intU) 32)
typedef  SAP_CHAR SAP_HOST_ID[SAP_HOST_ID_LN];

#define SAP_HOST_MAX_LN       SAP_HOST_ID_LN /* for edgar only */


/* Host-Name for external use, null term., must not be trunc.
 *
 * Attention: hostnames can be strings or into utf8 converted
 * strings. Therefore there should be no ccq cast to intU here!!
 *
 * el, 12.09.00
 */
#define SAP_EXT_HOST_ID_LN    129
typedef  SAP_CHAR SAP_EXT_HOST_ID[SAP_EXT_HOST_ID_LN];

/* Length of SAP Host-Address */
#define SAP_HOST_ADR_LN       4

/* SAP Server-Name, blank-padded to length 20, then NUL-teminated */
#define SAP_SRVNAME_LN        (CCQ_CAST(intU) 20)
typedef  SAP_CHAR SAP_SRVNAME[SAP_SRVNAME_LN + 1];

/* Mandant */
#define SAP_CLIENT_ID_LN      (CCQ_CAST(intU)  3)
typedef SAP_CHAR SAP_CLIENT_ID[SAP_CLIENT_ID_LN];

/* Username */
#define SAP_USER_ID_LN        (CCQ_CAST(intU) 12)
typedef SAP_CHAR SAP_USER_ID[SAP_USER_ID_LN];

/* Transaktionsnamen & Transaktionsvariante */
#define SAP_TCODE_LN          (CCQ_CAST(intU) 20)
#define SAP_TXVARIANT_LN      (CCQ_CAST(intU) 30)
typedef SAP_CHAR SAP_TCODE[SAP_TCODE_LN];
/* change dynpro version on changing this length */

/* ABAP-Report-/Programmname */
#define SAP_PROGRAM_ID_LN         (CCQ_CAST(intU) 40)
#define SAP_PROGRAM_ID_NAME_LN    (CCQ_CAST(intU) 30)
#define SAP_PROGRAM_ID_SUFFIX1_LN  (CCQ_CAST(intU) 5)
#define SAP_PROGRAM_ID_SUFFIX2_LN  (CCQ_CAST(intU) 5)
typedef SAP_CHAR SAP_PROGRAM_ID[SAP_PROGRAM_ID_LN];
/* change dynpro and abap version on changing this length */

/* maximal ABAP report line length */
#define REPORTLINESIZE            (CCQ_CAST(intU) 255 )

/* C program name (significant part) */
#define SAP_RSTRCMOD_LN           (CCQ_CAST(intU)  8)
typedef SAP_CHAR  SAP_RSTRCMOD [SAP_RSTRCMOD_LN];
typedef struct {
	SAP_RSTRCMOD str ;
} SAP_S_RSTRCMOD;

/* Zeitstempel : YYYYMMDDHHMMSS */
#define SAP_TIMESTMP_LN           (CCQ_CAST(intU) 14)
typedef SAP_CHAR SAP_TIMESTMP[SAP_TIMESTMP_LN];
/* former typedef : do not use it (saa reserved word) */
typedef SAP_CHAR SAP_TIMESTAMP[SAP_TIMESTMP_LN];

/* Zeitstempel mit us-Aufloesung: YYYYMMTTHHMMSSUUUUUU */
#define SAP_HIRESTIMESTMP_LN      (CCQ_CAST(intU) 20)
typedef SAP_CHAR SAP_HIRESTIMESTMP[SAP_HIRESTIMESTMP_LN];

/* tablename for pool and cluster tables */
#define SAP_POOL_CLUSTER_TABNAME_LN	(CCQ_CAST(intU) 10)
typedef SAP_CHAR SAP_POOL_CLUSTER_TABNAME[SAP_POOL_CLUSTER_TABNAME_LN];

/* tablename    */
#define SAP_TABNAME_LN        (CCQ_CAST(intU) 30)
typedef SAP_CHAR SAP_TABNAME[SAP_TABNAME_LN];
/* change dynpro and abap version on changing this length */

/* fieldname    */
#define SAP_FIELDNAME_LN      (CCQ_CAST(intU) 30)
typedef SAP_CHAR SAP_FIELDNAME[SAP_FIELDNAME_LN];

/* DB fieldname - this type and length has to be used when */
/* dealing with columns of repository tables containing    */
/* field names or when calling routines of the database    */
/* interface                                               */
#define SAP_DBFLDNAME_LN      (CCQ_CAST(intU) 30)
typedef SAP_CHAR SAP_DBFLDNAME[SAP_DBFLDNAME_LN];

/* old type for DB fields; use SAP_DBFLDNAME instead */
#define SAP_FLDNAME10_LN      SAP_DBFLDNAME_LN
typedef SAP_CHAR SAP_FLDNAME10[SAP_FLDNAME10_LN];


#define SAP_ROLLNAME_LN       (CCQ_CAST(intU) 30)


/* classname */
#define SAP_CLASSNAME_LN      (CCQ_CAST(intU) 30)
typedef SAP_CHAR SAP_CLASSNAME [SAP_CLASSNAME_LN];

/* methodname */
#define SAP_METHODNAME_LN     (CCQ_CAST(intU) 30)
typedef SAP_CHAR SAP_METHODNAME [SAP_METHODNAME_LN];

/* enhancement object name */
#define SAP_ENHANCEMNAME_LN     (CCQ_CAST(intU) 30)
typedef SAP_CHAR SAP_ENHANCEMNAME [SAP_ENHANCEMNAME_LN];

/* compnentname */
#define SAP_COMPONENTNAME_LN  (CCQ_CAST(intU) 61)
typedef SAP_CHAR SAP_COMPONENTNAME [SAP_COMPONENTNAME_LN];

/* domainname   */
#define SAP_DOMAINNAME_LN     (CCQ_CAST(intU) 20)
typedef SAP_CHAR SAP_DOMAINNAME[SAP_DOMAINNAME_LN];

/* version entry*/
#define  SAP_DDVERSION_LN     (CCQ_CAST(intU)  4)
typedef  SAP_CHAR SAP_DDVERSION[SAP_DDVERSION_LN];

/* release entry*/
#define  SAP_DDRELEASE_LN     (CCQ_CAST(intU)  4)
typedef  SAP_CHAR SAP_DDRELEASE[SAP_DDRELEASE_LN];

/* Dictionary - username (ist dies notwendig ?) */
#define  SAP_DDUSER_LN        (CCQ_CAST(intU) 12)
typedef  SAP_CHAR SAP_DDUSER[SAP_DDUSER_LN];

/* Dictionary - date entry (ist dies notwendig ?)  */
#define  SAP_DDDATE_LN         (CCQ_CAST(intU) 8)
typedef  SAP_CHAR SAP_DDDATE[SAP_DDDATE_LN];

/* Dictionary - time entry (ist dies notwendig ?)  */
#define  SAP_DDTIME_LN         (CCQ_CAST(intU) 6)
typedef  SAP_CHAR SAP_DDTIME[SAP_DDTIME_LN];

/* programname (s.o:SAP_PROGRAM_ID , wo ist der Unterschied ?) */
#define  SAP_PROGNAME_LN       SAP_PROGRAM_ID_LN
typedef  SAP_CHAR SAP_PROGNAME[SAP_PROGNAME_LN];

/* D021S-FNAM 132 */
#define SAP_DYNFLDNAM_LN     (CCQ_CAST(intU) 132)

/* CUA status */
#define  SAP_CUA_STATUS_LN    (CCQ_CAST(intU) 20)
typedef  SAP_CHAR SAP_CUA_STATUS[SAP_CUA_STATUS_LN];

/* function code */
#define  SAP_FCODE_LN		  SAP_TCODE_LN

/* dynpronumber */
#define  SAP_DYNPRONR_LN      (CCQ_CAST(intU)  4)
typedef  SAP_CHAR SAP_DYNPRONR[SAP_DYNPRONR_LN];
/* change dynpro and abap version on changing this length */

/* sap luw id / transaction id */
#define  SAP_TRANSACTION_ID_LN_R 32
#define  SAP_TRANSACTION_ID_LN (CCQ_CAST(intU) 32)
typedef  SAP_CHAR SAP_TRANSACTION_ID[SAP_TRANSACTION_ID_LN];

/* sap context id */
#define SAP_CONTEXT_ID_TID_LN   5
#define SAP_CONTEXT_ID_MODE_LN  3
#define SAP_CONTEXT_ID_STAMP_LN SAP_TRANSACTION_ID_LN
#define SAP_CONTEXT_ID_LN (SAP_CONTEXT_ID_TID_LN + \
			   SAP_CONTEXT_ID_MODE_LN + \
			   SAP_TRANSACTION_ID_LN)

/* w3c context id */
#define W3C_CONTEXT_ID_LN     128

/* sap session id */
#define SAP_SESSION_ID_LN_R  32
#define SAP_SESSION_ID_LN (CCQ_CAST(intU) 32)
typedef  SAP_CHAR SAP_SESSION_ID[SAP_SESSION_ID_LN];

/* sap rfc client id */
#define  SAP_RFC_CLIENT_ID_LN (CCQ_CAST(intU) 128)
typedef  SAP_CHAR SAP_RFC_CLIENT_ID[SAP_RFC_CLIENT_ID_LN];

/* spa/gpa memory id */
#define SAP_SPAGPA_ID_LN       20
/* change dynpro and abap version on changing this length */

/* spa/gpa value max length */
#define SAP_SPAGPA_VALUE_LN    255
/* change dynpro and abap version on changing this length */

/* dynpro group */
#define SAP_DYNPRO_GROUP_LN    4
/* change dynpro version on changing this length */

/* dynpro field group */
#define SAP_FIELD_GROUP_LN     3
/* change dynpro version on changing this length */

/* matchcode object */
#define SAP_MC_OBJECT_LN       30
/* change dynpro version on changing this length */

/* user conversion */
#define SAP_USERCONV_LN        5
/* change dynpro version on changing this length */

/* poolname     */
#define  SAP_POOLNAME_LN      (CCQ_CAST(intU) 30)
typedef  SAP_CHAR SAP_POOLNAME[SAP_POOLNAME_LN];

/* Dialogbaustein */
#define SAP_DIALOG_ID_LN      (CCQ_CAST(intU) 30)
typedef  SAP_CHAR SAP_DIALOG_ID[SAP_DIALOG_ID_LN];

/* Commandfile */
#define SAP_CMDFILENAME_LN    (CCQ_CAST(intU) 20      /* ua 20.05.97 */)
typedef  SAP_CHAR SAP_CMDFILENAME[SAP_CMDFILENAME_LN];

/* Language
 *-========
 * DDic:    SPRAS      , LANG    , T002
 *
 * Internal SAP language identifier: D=German, L=Polish,...
 */
#define                             SAP_LANGUAGE_LN (CCQ_CAST(intU)  1 )
typedef  SAP_CHAR  SAP_LANGUAGE    [SAP_LANGUAGE_LN];
typedef  SAP_CHAR* SAP_LANGUAGE_PTR                 ;
typedef const SAP_CHAR * SAP_CONST_LANGUAGE_PTR ; /* ptr to const lang*/
/*
 * Access macros.
 * Parameters of type SAP_LANGUAGE_PTR resp. SAP_CONST_LANGUAGE_PTR
 */
#define langcpy(to,from) (to)[0] = (from)[0]
#define langeq(l,r) ((l)[0] == (r)[0])
/* Later:
 *      langcpy(to,from) (to)[0] = (from)[0] ; \
 *              	 (to)[1] = (from)[1]
 *      langeq(l,r) (    ((l)[0] == (r)[0]) \
 *                    && ((l)[1] == (r)[1]) )
 */
/*--------
 * DDic:   LAISO      , CHAR(2)nolower, T002
 *
 * External ISO 639 language identifier: DE=German, PL=Polish
 */
#define                          SAP_LAISO_LN       (CCQ_CAST(intU)  2 )
typedef  SAP_CHAR  SAP_LAISO    [SAP_LAISO_LN];
typedef  SAP_CHAR* SAP_LAISO_PTR              ;
typedef const SAP_CHAR * SAP_CONST_LAISO_PTR ; /* ptr to const lang */
/*
 * Access macros.
 * Parameters of type SAP_LAISO_PTR resp. SAP_CONST_LAISO_PTR
 */
#define laisocpy(to,from) (to)[0] = (from)[0] , \
			  (to)[1] = (from)[1]
#define laisoeq(l,r) (    ((l)[0] == (r)[0]) \
		       && ((l)[1] == (r)[1]) )
/*
 * List of languages, internal coding. i.e. "DEF"
 */
#define                          SAP_LANGLIST_OLD_LG (CCQ_CAST(intU) 32)
#define                          SAP_LANGLIST_LG     (CCQ_CAST(intU) 256)
typedef  SAP_CHAR  SAP_LANGLIST [SAP_LANGLIST_LG];
typedef  SAP_CHAR* SAP_LANGLIST_PTR                 ;
typedef const SAP_CHAR * SAP_CONST_LANGLIST_PTR ; /* ptr to const list*/
/*
 * List of languages, external coding. i.e. "DE,EN,FR"
 */
#define                           SAP_LAISOLIST_LG  (CCQ_CAST(intU) 180)
typedef  SAP_CHAR  SAP_LAISOLIST [SAP_LAISOLIST_LG];
typedef  SAP_CHAR* SAP_LAISOLIST_PTR                  ;
typedef const SAP_CHAR * SAP_CONST_LAISOLIST_PTR; /* ptr to const list*/


/* Codepage
 *-========
 * DDic:     CPCODEPAGE , NUMC(4) , TCP00
 */
#define SAP_CODEPAGE_LN       (CCQ_CAST(intU)  4 )
typedef  SAP_CHAR SAP_CODEPAGE[SAP_CODEPAGE_LN];
/*
 * Often following datatype is used, because it
 * allows the usage of C assignments.
 */
typedef struct {
  SAP_CHAR str [SAP_CODEPAGE_LN] ;
} SAP_S_CODEPAGE ;
#define codepagecpy(to,from) (   ((to)[0] = (from)[0]) \
			       , ((to)[1] = (from)[1]) \
			       , ((to)[2] = (from)[2]) \
			       , ((to)[3] = (from)[3]) )
#define codepageeq(l,r)   (    ((l)[0] == (r)[0]) \
			    && ((l)[1] == (r)[1]) \
			    && ((l)[2] == (r)[2]) \
			    && ((l)[3] == (r)[3]) )
#define S_codepageeq(l,r) (    ((l).str[0] == (r).str[0]) \
			    && ((l).str[1] == (r).str[1]) \
			    && ((l).str[2] == (r).str[2]) \
			    && ((l).str[3] == (r).str[3]) )

/* Country, Area
 *-=============
 * DDic:     LAND1      , CHAR(3) , T005
 */
#define SAP_COUNTRY_LN           (CCQ_CAST(intU) 3)
typedef  SAP_CHAR SAP_COUNTRY[SAP_COUNTRY_LN];

/* NLS locale
 *-==========
 * DDic:     CPLOCALE_L , CHAR(255)
 */
#define SAP_LOCALE_LN           (CCQ_CAST(intU) 255)
typedef  SAP_CHAR SAP_LOCALE[SAP_LOCALE_LN];

/* Ext. Mode   */
#define SAP_EXTMODE_ID_LN         (CCQ_CAST(intU) 1)
typedef  SAP_CHAR SAP_EXTMODE_ID[SAP_EXTMODE_ID_LN];

/* System-Nummer    */
#define SAP_SYSTEMNR_LN           (CCQ_CAST(intU) 2)
typedef  SAP_CHAR SAP_SYSTEMNR[SAP_SYSTEMNR_LN];

/* WorkProcess - ID   */
#define SAP_WORKPROCESS_ID_LN     (CCQ_CAST(intU) 2)
typedef  SAP_CHAR SAP_WORKPROCESS_ID[SAP_WORKPROCESS_ID_LN];

/* max. snc overhead */
#define SAP_MAX_SNC_HDR_LN        (SAP_INT) 2048

/* Laenge eines APPC-Communication-Area Blocks */
/* must be >= CPIC_MAX_BUFFER_SIZE (cpic.h) + APPCHDR_LN (gwext.h) +
	      GW_SAP_PARAMS_LN  + snc-overhead */
#define SAP_APPC_CA_BLK_LN        ((SAP_INT) 32400 + SAP_MAX_SNC_HDR_LN)

/* Laenge eines WP-Communication-Area Blocks */
#define SAP_TM_MAX_BUFFER_SIZE    30000
#define SAP_WP_CA_BLK_LN          \
  (SAP_TM_MAX_BUFFER_SIZE + SAP_MAX_SNC_HDR_LN)

/* Laenge eines Service-Namens */
#define SAP_SERV_LN               (CCQ_CAST(intU) 20)
typedef SAP_CHAR SAP_SERV[SAP_SERV_LN];

/* Anmelde Service des SAP-Dispatchers */
#define SAP_DP_SERV_NAME          cU("sapdp")


/*--------------------------------------------------------------------*/
/* Enqueue definitions                                                */
/*--------------------------------------------------------------------*/

typedef SAP_CHAR SAP_ENQ_TS_MOD_T[2];
#define SAP_ENQ_TS_MOD_LN   sizeofU( SAP_ENQ_TS_MOD_T )

/*----------------------------------------*/
/* Attention:                             */
/* members of this structure must contain */
/* basic types SAP_CHAR only              */
/*----------------------------------------*/
typedef struct SAP_EN_KEY
{
  SAP_HIRESTIMESTMP  TimeStamp;
  SAP_ENQ_TS_MOD_T   TimeStampSerNr; /* .. if TimeStamp not unique */
  SAP_WORKPROCESS_ID WorkProcId;
  SAP_SYSTEMNR       SystemNr;
  SAP_HOST_ID        Host;

/* SAPUNICODEOK */
} SAP_EN_KEY;

#define SAP_EN_KEY_LN   (sizeofU(SAP_EN_KEY))
#define SAP_EN_KEY_LN_U (sizeofU(SAP_EN_KEY))
#define SAP_EN_KEY_LN_R (sizeofR(SAP_EN_KEY))

#define SAP_EN_SRV_LN (SAP_SRVNAME_LN + 2)

/* ENXX */
#define SAP_ENQ_ARG_LN	150
#define SAP_ENQ_OBJ_LN	16
#define SAP_ENQ_USER_LN	SAP_EN_KEY_LN
#define SAP_ENQ_USTP_LN	1
#define SAP_ENQ_MODE_LN	1
#define SAP_ENQ_STAMP_LN 24    /* utc timestamp with ns accuracy */

/*--- end of Enqueue definitions -------------------------------------*/


/* Laenge und Type der CPIC-Conversation-ID */
#define SAP_CONV_ID_LN            8
#define SAP_CONV_ID               SAP_RAW


/* Laenge der Message-Keys (T100) */
#define SAP_MSGID_LN              (CCQ_CAST(intU) 20)
/* change dynpro and abap version on changing this length */
typedef SAP_CHAR SAP_MSGID [SAP_MSGID_LN];
#define SAP_MSGNO_LN              (CCQ_CAST(intU)  3)
/* change dynpro and abap version on changing this length */
typedef SAP_CHAR SAP_MSGNO [SAP_MSGNO_LN];


/* Max. Anzahl externe und interne Modi */
#define MAXSESSIONS              17
#define MAXINTMODE                9

/* Max. Anzahl systemuebergreifender (RFC) Modi...........*/
#define MAXRFCUUIDS           MAXINTMODE * 10 /* max. 256 */

/* Identifier fuer Paging Block:
	 SAP_USHORT = 512 MB pageset max.
	 SAP_INT    = MAX_INT * 8kb
   !!: Poolsize 40 (Paging Admin) aendert sich.
 */
#define BLK_ID      SAP_INT

/* Gui version length */
#define SAP_GUIVERS_LN            10

/* GUI Protocollbits maximum length */
#define SAP_PROTOBIT_MAXSIZE      32

/* max. length of a terminal name */
#define SAP_TERMINAL_NAME_LN      20


/*    -------------------------                                       */
/* ----- database properties ---------------------------------------- */

/* See also: https:&&bis.wdf.sap.corp/twicki/bin/view/   ...
 *           ... /Sapinternal/JdbcMinutes20040311
 *
 * There is a table with name DBPROPERTIES.
 *
 * The table has a key field PROPKEY and a data field PROPVAL.
 * Both fields are of type INT2.
 *
 * Each database should have exactly one row with 
 *          PROPKEY = 67  .
 *
 * The data field shall contain either 65 or 69 or 85.
 * The value identifies which kind of characters are used in the
 * database.
 *
 * This technique is needed, because in some database UTF-8 or CESU-8
 * is used, and then it is not easy to differentiate between ASCII
 * based code pages and Unicode.
 *
 * In case of Unicode, this technique does not tell you, whether UTF-8,
 * CESU-8, UTF-16BE, UTF-16LE, UTF-32BE or UTF32LE is used. But that
 * is specific to the DBMS normally.
 *
 * The values are of integer type, so that you can ask for the character
 * code used, without the need to understand characters before.
 */

#define DBTYP_TABLENAME    cU("DBPROPERTIES")
#define DBTYP_KEY          cU("PROPKEY")
#define DBTYP_VALUE        cU("PROPVAL")

#define DBTYP_CODEPAGETYPE 67
#define DBTYP_C_ASCII      65
#define DBTYP_C_EBCDIC     69
#define DBTYP_C_UNICODE    85



/*    ----------                                                      */
/* ----- UUID ------------------------------------------------------- */

/* Note 1 by Matthias Mittelstein:
 * This include file saptypec.h is intended for type declarations,
 * where the original is in SAP's data dictionary.
 * Because of that, I think, this is not the correct place, to
 * put prototype.
 * Rainer, why didn't you use rfc.h or rfcuuid.h ?
 */
/* Note 2 by Matthias Mittelstein:
 * Good documentation about this type is in rfcuuid.c
 */
#ifndef SAP_UUID_TYPEDEFD
#define SAP_UUID_TYPEDEFD
typedef struct SAP_UUID
{
    SAP_UINT   a;
    SAP_USHORT b;
    SAP_USHORT c;
    SAP_RAW    d[8];
}
SAP_UUID;
#endif

externC void SapUUIDCreate( SAP_UUID  * pUuid );
/*-----------------------------------------------------------
 * creates a globaly unique id. It is guaranteed that this
 * id is not created a second time anywhere
 *
 * On NT we use the RPC uuid generator, on the other platforms
 * an own implementation.
 *
 * There is no need to take care of the internal structure of
 * SAP_UUID. The id generated are unique even binary, so that
 * they can be transported across platforms as X(16).
 *-----------------------------------------------------------
 */

externC SAP_UC* SapUUIDPrint ( SAP_UUID  * pUuid, SAP_UC* buffer );
/*-----------------------------------------------------------
 * generates a readable form of a UUID. Also this string
 * is globaly  unique.
 *
 * The input parameter 'buffer' is returned to ease the use
 * in some string manipulation functions.
 *
 * The buffer must be at least SAP_UUID_PRINT_MAX_SIZE large.
 *-----------------------------------------------------------
 */

#define SAP_UUID_PRINT_MAX_SIZE 48


/*-----------------------------------------------------------
 * Context id.
 *
 * used to identify a user context (Imode) in remote
 * communications.
 * necessary for lifetime management of remote objects.
 *-----------------------------------------------------------
 */
#define SAP_CTXBUFFER_ENTRIES 2

typedef struct
{
    unsigned    entries;     /* entries of pCTX */
    SAP_UUID *  pCTX;        /* context id with path to root */

    /* internal buffer */
    SAP_UUID    _ctxbuffer[SAP_CTXBUFFER_ENTRIES];
}
SAP_CTX;


/*-----------------------------------------------------------
 * SAP TIME stamp
 *-----------------------------------------------------------
 */
typedef struct {
  SAP_UINT s ;
  SAP_UINT us ;
}
rstrtime ;


/* VMC is active for releases >= 7.00!! */
#define SAPwithVMC

#endif  /* SAPTYPEC_H */
