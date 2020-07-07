/* @(#) $Id: //sapdb/V76/c_00/b_32/sys/src/sap/700/dptrace.h#1 $ SAP*/

#ifndef DPTRACE_H
#define DPTRACE_H

/* needed by va_list */
#include <stdarg.h>

BEGIN_externC

/*include-------------------------------------------------------------*/
/*                                                                    */
/* Beschreibung:                                                      */
/* ============                                                       */
/*                                                                    */
/* Intern wird der Name auf CT umgestellt (bisher DPTRC)              */
/*                                                                    */
/* dptrace.h enthaelt Makros zum Schreiben von "Entwickler"-Trace     */
/* (E-Trace), SAP-Trace (S-Trace) und SAP-Syslog, und                 */
/* Funktionsdeklarationen zur Initialisierung des E-Trace.            */
/*                                                                    */
/* Die Macros sind hist(e/o)risch gewachsen und daher stellenweise    */
/* etwas redundant (staendig neue Anforderungen, aber altes ist schon */
/* ueberall eingebaut). Zu empfehlen sind TRC () fuer normalen Trace, */
/* TRCERR () fuer Fehlermeldungen ohne Syslog, und RSLGFUN () fuer    */
/* Syslog-Eintraege. Um Sichern langer variabler Texte im Syslog      */
/* wird das Macro RSLGSAVTXT verwendet.                               */
/* Alle Tracemacros verlangen Unicodestrings. Um (z.B. im Internet-   */
/* bereich) auch plain ASCII Daten ausgeben zu koennen, gibt es die   */
/* A7_TRC macros. Alle Parameter sind dabei Ascii(EBCDIC) 7Bit        */
/*                                                                    */
/* Der tracelevel wird in der globalen Variablen ct_level gehalten.   */
/* Dies ist (z.B. bei Threads) etwas unflexibel. Um einen Benutzer-   */
/* trace ziehen zu koennen und dabei nicht die globale Variable       */
/* aendern zu muessen, gibt es nun die Tracemacros mit einem "U" als  */
/* Praefix. Mit dem ersten Parameter der Macros bestimmt die Anwen-   */
/* dung den aktuellen Tracelevel (UTRC = UserTrace).                  */
/*                                                                    */
/* Macros:                                                            */
/* ======                                                             */
/*                                                                    */
/* TRC                        Entwickler- und SAP-Trace freies Format */
/* TRC1                       Wie TRC, ab Trace-Level 1 statt 2       */
/* TRC3                       Wie TRC, ab Trace-Level 3 statt 2       */
/* TRCERR                     Fehlermeldung in E- und S- Trace        */
/* TRCRET                     wie TRCERR, aber mit autom. Return      */
/* UTRC			      Usertrace (eigener Wert, nicht ct_level)*/
/* SYSERR                     wie TRCERR, aber mit syscall-errno Info */
/* SYSRET                     wie SYSERR, aber mit autom. Return      */
/* LOGERR                     Fehlermeldung in SAP-Syslog, E- und S-  */
/*                            Trace (freies Format)                   */
/* LOGRET                     wie LOGERR, aber mit autom. Return      */
/* RSLG                       frei-formatigen Text in SAP-Syslog,     */
/*                            E- und S- Trace (siehe Kommentar rslg.h)*/
/* RSLGFUN                    "teil"-formatierten Text in SAP-Syslog, */
/*                            E- und S- Trace (siehe Kommentar rslg.h)*/
/* RSLGSAVTXT                 sichert langen variablen Text im Syslog */
/* TRCTIME                    Datum und Uhrzeit im E-Trace ablegen    */
/* VTRC			      Wie TRC, level aber als Parameter       */
/*                                                                    */
/*                                                                    */
/* Aufruf-Beispiele s.u.                                              */
/* Alle Fehler-Macros erzeugen autom. Informationen ueber Modul/Line. */
/*                                                                    */
/*                                                                    */
/* Funktionen:                                                        */
/* ==========                                                         */
/*                                                                    */
/* Komponenten-Schnittstelle:                                         */
/* . es wird nur 1 Trace-File pro Prozess verwendet                   */
/* . es lassen sich einzelne Komponenten getrennt an- / ausschalten   */
/*   (aus Kompatibilitaets-Gruenden werden die alten Macros weiter    */
/*    unterstuetzt. Die Handles sind jedoch keine echten File-Pointer */
/*    mehr)                                                           */
/*                                                                    */
/* Schnittstelle fuer Komponenten-Trace:                              */
/* CTrcInit                   (E-) Komponenten-Trace initialisieren   */
/* CTrcNewComp                Neue Komponente einrichten              */
/* CTrcSwitch                 Level / aktive Komponenten aendern      */
/* CTrcSetParam               Parameter einer Komponente aendern      */
/* CTrcPrintAct               Level/Comps Info in Trace-File schreiben*/
/* CTrcFileClose              Trace File schliessen                   */
/* CTrcCompLev                interne Funktion wg. Aufwaertskompatib. */
/* CTrcFp                     Trace File zu Handle liefern            */
/* CTrcSetFp                  Trace file fuer Tracing setzen	      */
/* CTrcSetTraceLevel          Tracelevel fuer Tracing setzen(Macro)   */
/* CTrcStack                  C-Stack in File tracen                  */
/* CTrcSignal                 ANSI C Signaldaten in Trace schreiben   */
/* CTrcTransHdl               Handle in Filepointer uebersetzen       */
/*                                                                    */
/* CTrcThrOpen                Threadspezifische Tracedatei oeffnen    */
/*                                                                    */
/* alte Schnittstelle:                                                */
/* trc_init                   ... mit einlesen des Trace-Parameters   */
/* trc_append                 ... im Append-Modus schreiben           */
/*                                                                    */
/*end-----------------------------------------------------------------*/


/*--------------------------------------------------------------------*/
/* Version der CT-Schicht                                             */
/*                                                                    */
/* CT_VERSION wird bei allen Aenderungen der CT-Schicht               */
/* inkrementiert                                                      */
/*                                                                    */
/*  1  22.07.96 cr   trace_init zum Makro umgeschrieben, so dass      */
/*                     dptrclib.o unabhaengig von sapparam wird.      */
/*                   Hookfunktion fuer CTrcSwitch eingefuehrt.        */
/*  2  20.09.96 hq   DpTrc so veraendert, dass alle TRCx-Calls        */
/*                   Ausgaben ohne Newline unterstuetzen (Sonderbeh.  */
/*                   bei Komponentenwechsel)                          */
/*  3  09.11.99 ol   Bei multithreaded applications ist es wichtig,   */
/*                   dass der Output nicht auseinandergerissen wird.  */
/*                   Minimierung der Systemcalls                      */
/*  4  18.02.00 ol   Tracen von plain ASCII A7 Stings                 */
/*                   (wichtig fuer Internetdaten)                     */
/*  5  03.05.00 ol   Tracedateien fuer verschiedene Threads           */
/*  6  24.10.00 ol   Modulnamen auf 8.3 verlaengert		      */
/*  7  20.12.00 ol   Tracemacros fuer Tracing aus Threads heraus      */
/*  8  10.01.01 ol   Neue Fkt: CTrcSetFp, CTrcSetTraceLevel	      */
/*  9  12.07.01 ol   Set Tracelevel for syslog: ct_log_level	      */
/* 10  25.09.02 ol   New functions for warnings                       */
/* 11  17.06.03 ol   Indent fuer Traces                               */
/* 12  27.10.03 ol   auto truncate/pattern search fuer Traces         */
/* 13  23.12.03 ol   Hookfunktionen fuer pattern, tracefilesize       */
/* 14  23.04.04 jb   Funktion um Trace logging zu bestimmen           */
/* 15  27.10.04 ol   VTRC Macros hinzugefuegt (variable trace)        */
/*                                                                    */
/*--------------------------------------------------------------------*/

#define CT_COMPNAME       cU("CT (Component Trace)")
#define CT_VERSION        8

#define CT_LAST_MODIFIED cU("Sep 28 2002")

/*--------------------------------------------------------------------*/
/* (0) Compiler-Parameter                                             */
/*--------------------------------------------------------------------*/

#define GEN_TRACE       1  /* Code generieren fuer Entwickler Trace   */
#define SYS_LOG         1  /* Code generieren fuer SAP-Syslog Aufrufe */

/*--------------------------------------------------------------------*/
/* (1) Variablen- und Funktionsdeklarationen                          */
/*--------------------------------------------------------------------*/

/* Flags fuer CTrcInit () */
#define CTRC_TRUNC      1
#define CTRC_APPEND     2

/* Flags fuer CTrcSwitch () */
#define CTRC_ADD        1       /* angegebene Komponenten einschalten */
#define CTRC_SUB        2       /* angegebene Komponenten ausschalten */
#define CTRC_SET        3       /* Komp. entsprechend String setzen   */

#define CTRC_all        cU("all")   /* fuer alle Komponenten: bis Level 1 */
#define CTRC_ALL        cU("All")   /* fuer alle Komponenten: auch > L. 1 */



/*--------------------------------------------------------------------*/
/* Liste der registrierten TraceComps fuer:                           */
/* CTrcNewComp, CTrcLastComp, CTrcSwitch                              */
/*--------------------------------------------------------------------*/

#define CTRC_COMP_ABAP		cU('A')
#define CTRC_COMP_DBHIGHLEV	cU('B')
#define CTRC_COMP_DBLOWLEV	cU('C')
#define CTRC_COMP_DIAG		cU('D')
#define CTRC_COMP_ENQUEUE	cU('E')
#define CTRC_COMP_JSTARTUP	cU('F')
#define CTRC_COMP_NLS		cU('G')
#define CTRC_COMP_ICF		cU('H')
#define CTRC_COMP_IPC		cU('I')
#define CTRC_COMP_JAVA		cU('J')
#define CTRC_COMP_BATCHLOG	cU('L')
#define CTRC_COMP_MAIN		cU('M')
#define CTRC_COMP_SECURITY	cU('N')
#define CTRC_COMP_RPAG		cU('P')
#define CTRC_COMP_RROL		cU('R')
#define CTRC_COMP_SPOOL		cU('S')
#define CTRC_COMP_DEBUG		cU('T')
#define CTRC_COMP_WEBGUI	cU('W')
#define CTRC_COMP_EM		cU('X')
#define CTRC_COMP_DYNP		cU('Y')


/*--------------------------------------------------------------------*/
/* Datentype fuer TracePattern Hooks und FileSize Hooks               */
/*--------------------------------------------------------------------*/

/* maximale Anzahl an Trace Pattern */
#define CTRC_MAX_TRACE_PATTERN	32

/* TODO: 64 bit files */
typedef long	ctrc_file_size_t;


/* Hookfunktion, wenn pattern gefunden wurde */
typedef void	CT_PATTERN_HOOK_FUNC (FILE		*hdl,
				      SAP_UC		*pattern);

/* Hookfunktion, wenn Tracedatei bestimmte Groesse ueberschreitet */
typedef void	CT_FILESIZE_HOOK_FUNC (FILE		*hdl,
				       ctrc_file_size_t	size);


/*--------------------------------------------------------------------*/
/* (2) Holger Stasch: export Specification needed on NT for variables */
/*                    specified in dwexport.nt                        */
/*--------------------------------------------------------------------*/

# undef __SAP_DLL_DECLSPEC
# if defined(__SAP_DESIGNATED_DLL)
#  if defined(SAPonNT)
#   if defined(__SAP_DLL_EXPORT)
#    define __SAP_DLL_DECLSPEC __declspec(dllexport)
#   else
#    define __SAP_DLL_DECLSPEC __declspec(dllimport)
#   endif
#  else
#   define __SAP_DLL_DECLSPEC
#  endif
# else
#  define __SAP_DLL_DECLSPEC
# endif

/*--------------------------------------------------------------------*/
/* Encoding der String-Parameter:                                     */
/*   UC         Unicode parameter (Default)                           */
/*   A7         ASCII 7Bit                                            */
/*                                                                    */
/*--------------------------------------------------------------------*/

typedef enum
{
    CTRC_ENC_NONE    = 0,       /* Unbekanntes Encoding               */
    CTRC_ENC_A7      = 1,       /* ASCII A7 Encoding                  */
    CTRC_ENC_UC      = 2        /* USC Encoding                       */
} CTRC_ENCODING;

/*--------------------------------------------------------------------*/
/* allgemeine CTRC-Parameter CTP_... fuer CTrcSet/GetParam            */
/*   bei GetParam sind Pointer zu uebergeben, wo (*) vor dem Parameter*/
/*   steht                                                            */
/*                                                                    */
/* LEVEL       , int (*)level                                         */
/*             tracelevel fuer diese Komponente                       */
/*                                                                    */
/* HOOKFUNC    , CT_HOOK_FUNC (*)* function                           */
/*             Hookfunktion, die bei Tracelevel-Switch gerufen wird   */
/*--------------------------------------------------------------------*/

typedef enum {
    CTP_LEVEL    = 0x10000000, /* CTRC_PAR constants MUST be integer */
    CTP_FILEP    = 0x10000001, /* because of the variable number of  */
    CTP_HOOKFUNC = 0x10000002  /* arguments in CTrcSetParam()! QST   */
} CTRC_PAR;

typedef void     CT_HOOK_FUNC (int);


/*--------------------------------------------------------------------*/
/* globale Variablen                                                  */
/*--------------------------------------------------------------------*/

__SAP_DLL_DECLSPEC extern int		ct_level;
__SAP_DLL_DECLSPEC extern int		ct_log_level;
__SAP_DLL_DECLSPEC extern int		ct_time_res;
#ifndef CT_NO_TRACE
  /* TRACE sollte nicht mehr benutzt werden, wg. Konflikten mit z.B. MFC */
  #define TRACE ct_level
#endif

__SAP_DLL_DECLSPEC extern int		EntLev;
__SAP_DLL_DECLSPEC extern SAP_UC	savloc[];  /* Speicher fuer Modul,Line*/

__SAP_DLL_DECLSPEC extern FILE		*tf;	   /* dp and th Module        */
__SAP_DLL_DECLSPEC extern FILE		*ab_tf;    /* abap                    */
__SAP_DLL_DECLSPEC extern FILE		*dytf;     /* dynp Module             */
__SAP_DLL_DECLSPEC extern FILE		*dytr;     /* diag (frueher dynp)     */
__SAP_DLL_DECLSPEC extern FILE		*rrol_tf;  /* roll Module             */
__SAP_DLL_DECLSPEC extern FILE		*rpag_tf;  /* paging Module           */
__SAP_DLL_DECLSPEC extern FILE		*ipc_tf;   /* ipc Module              */
__SAP_DLL_DECLSPEC extern FILE		*log_tf;   /* log interface           */
__SAP_DLL_DECLSPEC extern FILE		*eq_tf;    /* enqueue                 */
__SAP_DLL_DECLSPEC extern FILE		*spoo_tf;  /* Spool                   */
__SAP_DLL_DECLSPEC extern FILE		*java_tf;  /* java vm                 */
__SAP_DLL_DECLSPEC extern FILE		*nls_tf;   /* NLS, I18n               */
__SAP_DLL_DECLSPEC extern FILE		*wgui_tf;  /* Web Gui (ITS)           */
__SAP_DLL_DECLSPEC extern FILE		*icf_tf;   /* Internet Com. Fr. (ICF) */
__SAP_DLL_DECLSPEC extern FILE		*jsf_tf;   /* Java Startup Framework  */

#ifdef SAPonOS2_2x
extern FILE				*temu_tf;  /* OS2 gui                 */
#endif



/*--------------------------------------------------------------------*/
/* DPTRACE API Funktionen                                             */
/*--------------------------------------------------------------------*/

extern FILE             *CTrcInit       (const SAP_UC   *file,
                                         SAP_RAW        mode,
                                         int            tracelevel);
extern SAPRETURN        CTrcNewComp     (SAP_CHAR       compid,
                                         FILE           **hdl,
                                         CT_HOOK_FUNC   *hook_function);
extern SAPRETURN        CTrcLastComp    (SAP_CHAR       compid,
                                         FILE           **hdl,
                                         CT_HOOK_FUNC   *hook_function);
extern SAPRETURN        CTrcSwitch      (SAP_CHAR       *compstr,
                                         SAP_RAW        mode,
                                         SAP_INT        level,
                                         SAP_CHAR       **last_compstr,
                                         SAP_INT        *last_level);

extern SAPRETURN        CTrcGetParam    (FILE           *hdl,
#ifndef SAPonOS400
                                         CTRC_PAR       param,
#else
                                         int            param,
#endif
                                         ...);
extern SAPRETURN        CTrcSetParam    (FILE           *hdl,
#ifndef SAPonOS400
                                         CTRC_PAR       param,
#else
                                         int            param,
#endif
                                         ...);
extern SAPRETURN        CTrcPrintAct  	(void);
extern SAPRETURN        CTrcFileClose   (FILE           *hdl);
extern SAPRETURN	CTrcFileTrunc	(FILE		*hdl);
extern SAP_INT          CTrcCompLev     (FILE           *hdl,
                                         const SAP_UC   *fmt,
                                         ...);
extern SAPRETURN        CTrcStack       (FILE           *fp);
extern SAPRETURN        CTrcStack2      (FILE           *fp,
                                         void           *context);
extern SAPRETURN        CTrcSignal      (FILE           *fp,
                                         int            sig);
extern FILE *           CTrcFp          (FILE           *hdl);
extern FILE *		CTrcSetFp	(FILE		*fp,
                                         int            tracelevel);
extern void             DpLock          (void);
extern void             DpUnlock        (void);
extern void             CTrcRslgw1param (SAP_UC         *p_param,
                                         int            p_lg);
/* SECHANGE */
extern void             CTrcSetTraceLevelFct(int tracelevel);
extern int              CTrcGetTraceLevelFct(void);
extern FILE *           CTrcGetFpTf(void);
void                    CTrcSetFpTf(FILE * fp);


/*--------------------------------------------------------*/
/* DpTrcGetRecursionCount ()                              */
/*                                                        */
/* Mit dieser Funktion kann zur Verhinderung von Dead-    */
/* locks abgepüft werden, ob gerade ein anderer Thread    */
/* in einer Traceroutine ist.                             */
/* Es wird nur bei SAPwithTHREADS und CTwithTHREADS ein   */
/* Wert > 0 geliefert.                                    */
/* Unter Windows wird vor dem holen des DpTrc Locks       */
/* das Flag atomar gesetzt, sonst unter dem Schutz des    */
/* DpTrc Locks.                                           */
/*                                                        */
/* INPUT   none                                           */
/*                                                        */
/* OUTPUT  none                                           */
/*                                                        */
/* RETURNS                                                */
/*         > 0          anderer Thread gerade im Tracing  */
/*           0          kein anderer Thread im Tracing    */
/*	   < 0	        Fehler				  */
/*                                                        */
/*--------------------------------------------------------*/
extern SAP_INT		DpTrcGetRecursionCount(void);


/*--------------------------------------------------------*/
/* CTrcGetLoggingInfo( )                                  */
/*                                                        */
/* A simple parser for the profile parameter              */
/* rdisp/TRACE_LOGGING.                                   */
/*                                                        */
/* INPUT parvalue     NULL terminated string with the     */
/*                    value of the profile parameter.     */
/*                                                        */
/* OUTPUT logging_info pointer to ctrc_logging_info       */
/*                     structure. If active is FALSE the  */
/*                     value of the other parts is        */
/*                     undefined.                         */
/*                                                        */
/* RETURN SAP_O_K fine                                    */
/*        other   the parameter string is not valid       */
/*--------------------------------------------------------*/
typedef struct
{
  SAP_BOOL         active;
  ctrc_file_size_t max_filesize;
  SAP_BOOL         global_reset;
} ctrc_logging_info;
SAPRETURN CTrcGetLoggingInfo (const SAP_UC      *parvalue,
                              ctrc_logging_info *logging_info);


/*--------------------------------------------------------*/
/* CTrcPrintPgmHeader ()                                  */
/* CTrcPrintPgmHeaderExt ()                               */
/*                                                        */
/* Einheitlichen Header mit Version, System, Patchlevel,  */
/* pid, in das Tracefile schreiben.                       */
/*                                                        */
/* INPUT   hdl1          Trace Handle                     */
/*         relno         Releasenummer                    */
/*         patchlevel    Patchlevel			  */
/*         patchno	 Patchnummer			  */
/*         sysno	 System Nummer                    */
/*         sid		 3stellige Systemkennung (zB BCO) */
/*                                                        */
/* OUTPUT                                                 */
/*                                                        */
/* RETURNS                                                */
/*         SAP_O_K	Header erfolgreich geschrieben    */
/*	   sonst	Fehler				  */
/*                                                        */
/*--------------------------------------------------------*/
extern SAPRETURN	CTrcPrintPgmHeader	(FILE		*hdl,
						 SAP_INT	relno,
						 SAP_INT	patchlevel,
						 SAP_INT	patchno,
						 SAP_INT	intno);
extern SAPRETURN	CTrcPrintPgmHeaderExt	(FILE		*hdl,
						 SAP_INT	relno,
						 SAP_INT	patchlevel,
						 SAP_INT	patchno,
						 SAP_INT	intno,
						 SAP_UC		*sysno,
						 SAP_UC		*sid);

/* set handle indent */
extern SAPRETURN	CTrcChangeHdlIndent	(FILE		*hdl,
						 SAP_INT	delta);
extern SAPRETURN	CTrcSetHdlIndent	(FILE		*hdl,
						 SAP_INT	indent);
extern SAPRETURN	CTrcChangeGlobalIndent	(SAP_INT	delta);
extern SAPRETURN	CTrcSetGlobalIndent	(SAP_INT	delta);
extern void		CTrcSaveLocation	(const SAP_UC	*file,
						 const SAP_INT	line);


/*--------------------------------------------------------*/
/* DpTrcSetFileSizeHook ()                                */
/*                                                        */
/* Callbackfunktion registrieren, die aufgerufen wird,    */
/* wenn das Tracefile eine bestimmte Groesse erreicht.    */
/* Callback kann nur global gesetzt werden, nicht fuer    */
/* einzelne Traces.					  */
/*                                                        */
/* INPUT   file_size	 Groesse, bei der Callback aufge- */
/*                       rufen werden soll                */
/*         func          Callbackfunktion                 */
/*                                                        */
/* OUTPUT                                                 */
/*                                                        */
/* RETURNS                                                */
/*         SAP_O_K	Callback registriert		  */
/*	   sonst	Fehler				  */
/*                                                        */
/*--------------------------------------------------------*/
extern SAPRETURN	DpTrcSetFileSizeHook	(ctrc_file_size_t	file_size,
						 CT_FILESIZE_HOOK_FUNC	*func);
/*--------------------------------------------------------*/
/* DpTrcGetFileSize ()                                    */
/*                                                        */
/* Liefert die maximale Dateigroesse bei deren erreichen  */
/* die Callbackfunktion gerufen wird die mit              */
/* DpTrcSetFileSizeHook registriert wurde.                */
/*                                                        */
/* INPUT                                                  */
/* OUTPUT                                                 */
/*                                                        */
/* RETURNS                                                */
/*           -1:  Fehler, sonst die maximale Dateigroesse */
/*                                                        */
/*--------------------------------------------------------*/
extern ctrc_file_size_t DpTrcGetFileSize( void );

/*--------------------------------------------------------*/
/* DpTrcRmFileSizeHook ()                                 */
/*                                                        */
/* Callbackfunktion fuer Tracefilegroese deregistrieren   */
/*                                                        */
/* INPUT						  */
/*                                                        */
/* OUTPUT                                                 */
/*                                                        */
/* RETURNS                                                */
/*         SAP_O_K	Callback deregistriert		  */
/*	   sonst	Fehler				  */
/*                                                        */
/*--------------------------------------------------------*/
extern SAPRETURN	DpTrcRmFileSizeHook	(void);

/*--------------------------------------------------------*/
/* DpTrcAddPattern ()                                     */
/*                                                        */
/* Callbackfunktion registrieren, die aufgerufen wird,    */
/* nachdem ein Muster in das Tracefile geschrieben wurde. */
/* Das Muster kann als Sonderzeichen "*" und "?" ent-     */
/* halten. Mit "\" koennen die Sonderzeichen geschuetzt   */
/* werden.						  */
/* Es koennen bis zu CTRC_MAX_TRACE_PATTERN Callbacks     */
/* registriert werden.                                    */
/* Callback kann nur global gesetzt werden, nicht fuer    */
/* einzelne Traces.					  */
/*                                                        */
/* INPUT   pattern	 Muster, fuer das Callback aufge- */
/*                       rufen werden soll                */
/*         func          Callbackfunktion                 */
/*                                                        */
/* OUTPUT                                                 */
/*                                                        */
/* RETURNS                                                */
/*         SAP_O_K	Callback registriert		  */
/*	   sonst	Fehler bei der Registrierung	  */
/*                                                        */
/*--------------------------------------------------------*/
extern SAPRETURN	DpTrcAddPattern		(SAP_UC			*pattern,
						 CT_PATTERN_HOOK_FUNC	*func);


/*--------------------------------------------------------*/
/* DpTrcGetPatterns ()                                    */
/*                                                        */
/* Liefert die Anzahl und die Werte der registrierten     */
/* Trace Patterns die mittels DpTrcAddPattern registriert */
/* wurden.                                                */
/*                                                        */
/* INPUT   pattern	 Array von SAP_UC Pointern in dem */
/*                       Referenzen auf die registrierten */
/*                       Patterns zurück geliefert werden.*/
/*                                                        */
/*         buffer_size   Die Anzahl Eintraege im Pattern- */
/*                       Array. Sollte den Wert von       */
/*                       CTRC_MAX_TRACE_PATTERN sein, dann*/
/*                       bekommt man gleich alles.        */
/*                       Ansonsten nur soviel wie man     */
/*                       uebergeben hat.                  */
/*                                                        */
/* OUTPUT                                                 */
/*                                                        */
/* RETURNS                                                */
/*         Anzahl der registrierten Trace Patterns.       */
/*         Liefert immer die Gesammtzahl, unabhängig vom  */
/*         Wert von buffer_size.                          */
/*                                                        */
/*--------------------------------------------------------*/
extern unsigned int DpTrcGetPatterns( const SAP_UC **pattern,
                                      unsigned int buffer_size);


/*--------------------------------------------------------*/
/* DpTrcRmPattern ()                                      */
/*                                                        */
/* Callbackfunktion fuer ein Pattern deregistrieren.	  */
/*                                                        */
/* INPUT   pattern	 Pattern, fuer das Callback ent-  */
/*                       fernt werden soll                */
/*                                                        */
/* OUTPUT                                                 */
/*                                                        */
/* RETURNS                                                */
/*         SAP_O_K	Callback deregistriert		  */
/*	   sonst	Fehler				  */
/*                                                        */
/*--------------------------------------------------------*/
extern SAPRETURN	DpTrcRmPattern		(SAP_UC		*pattern);

/*--------------------------------------------------------*/
/* DpTrcRmAllPattern ()                                   */
/*                                                        */
/* Alle Callbackfunktionen deregistrieren		  */
/*                                                        */
/* INPUT						  */
/*                                                        */
/* OUTPUT                                                 */
/*                                                        */
/* RETURNS                                                */
/*         SAP_O_K	Alle Callbacks deregistriert	  */
/*	   sonst	Fehler bei der Registrierung	  */
/*                                                        */
/*--------------------------------------------------------*/
extern SAPRETURN	DpTrcRmAllPattern	(void);


/* Macros to set/get the trace level */
#define CTrcGetTraceLevel		(ct_level)
#define CTrcSetTraceLevel(_x)		(ct_level = (_x))

/* Macros to set/get the syslog level */
#define CTrcGetSysLogLevel		(ct_log_level)
#define CTrcSetSyslogLevel(_x)		(ct_log_level = (_x))

/* Macros to set/get the time frequency */
#define CTrcGetTimeRes			(ct_time_res)
#define CTrcSetTimeRes(_x)		(ct_time_res = (_x))


/* Macros um die Einrueckung (indent) zu setzen */
#define CTrcSetIndent(_i)		CTrcSetGlobalIndent(_i)
#define CTrcIcrIndent			CTrcChangeGlobalIndent(1)
#define CTrcDecrIndent			CTrcChangeGlobalIndent(-1)



/*--------------------------------------------------------*/
/* CTrcThrOpen ()                                         */
/*                                                        */
/* Eigenes Tracefile fuer aktuellen Thread oeffnen.       */
/* CTrcInit muss vorher bereits gerufen worden sein!      */
/*                                                        */
/* INPUT   file          Tracefile                        */
/*         mode          Modus in der Datei geoeffnet     */
/*                       werden soll (siehe CTrcInit).    */
/*         level         Tracelevel fuer diese Datei      */
/*                                                        */
/* OUTPUT                                                 */
/*                                                        */
/* RETURNS                                                */
/*         FILE *       Filepointer fuer diese Datei      */
/*                                                        */
/*--------------------------------------------------------*/
extern FILE *CTrcThrOpen        (const SAP_UC      *file,
                                 SAP_RAW           mode,
                                 int               level);


/*--------------------------------------------------------*/
/* CTrcThrGetTraceLevel(), CTrcThrSetTraceLevel(),        */
/* CTrcThrResetTrace()                                    */
/*                                                        */
/* Betreffen den threadspezifischen Trace und muessen     */
/* IMMER innerhalb des Threads gerufen werden fuer dessen */
/* Tracefile fp sie gelten sollen.                        */
/* -CTrcThrSetTraceLevel:                                 */
/*   wird der threadspezifische Tracelevel aktiviert.     */
/*   Damit hat dieser Thread einen eigenen Tracelevel der */
/*   allerdings nur zum tragen kommt, wenn ct_level       */
/*   mindestens den gleichen Wert hat (ansonsten wird nur */
/*   mit ct_level getracet). Dies muss manuell im Programm*/
/*   eingestellt werden und wird nicht in                 */
/*   CTrcThrSetTraceLevel gemacht. Mit einem Wert von     */
/*   SAP_INT_MAX wird das threadspezifische Tracing fuer  */
/*   diesen Thread deaktiviert.                           */
/* -CTrcThrGetTraceLevel:                                 */
/*   Liefert den Tracelevel des Threads. Dabei bedeutet   */
/*   der Returncode -1, dass ein Fehler aufgetreten ist,  */
/*   SAP_INT_MAX bedeutet, dass das threadspezifische     */
/*   Tracing entweder global oder nur fuer diesen Thread  */
/*   nicht aktiviert ist.                                 */
/* -CTrcThrResetTrace:                                    */
/*   deaktiviert den threadspezifischen Trace global, d.h.*/
/*   alle Threads tracen nur noch mit dem Tracelevel der  */
/*   in ct_level angegeben ist.                           */
/*                                                        */
/* INPUT   file Tracefile Handle                          */
/*         level         Tracelevel fuer diese Datei      */
/*                                                        */
/* OUTPUT                                                 */
/*                                                        */
/* RETURNS                                                */
/*         int           Tracelevel fuer diese Datei,     */
/*                       -1 falls ein Fehler auftrat,     */
/*                       SAP_INT_MAX falls kein thread-   */
/*                       spezifisches Tracing vorliegt.   */
/*         SAPRETURN     SAP_O_K oder -1 falls ein Fehler */
/*                       auftrat                          */
/*                                                        */
/*--------------------------------------------------------*/
extern int CTrcThrGetTraceLevel( FILE *fp );
extern SAPRETURN CTrcThrSetTraceLevel( FILE *fp, int level );
extern void CTrcThrResetTrace( void );


/*--------------------------------------------------------*/
/* CTrcThrClose ()                                        */
/*                                                        */
/* Eigenes Tracefile fuer den aktuellen Thread schliessen */
/* Diese Datei muss vorher mit CTrcThrOpen geoeffnet      */
/* worden sein.                                           */
/*                                                        */
/* INPUT   file          Tracefile                        */
/*         mode          Modus in der Datei geoeffnet     */
/*                       werden soll (siehe CTrcInit).    */
/*         level         Tracelevel fuer diese Datei      */
/*                                                        */
/* OUTPUT                                                 */
/*                                                        */
/* RETURNS                                                */
/*         FILE *       Filepointer fuer diese Datei      */
/*                                                        */
/*--------------------------------------------------------*/
extern SAPRETURN CTrcThrClose   (FILE   *fp);

#ifdef RSLG_H_passed

/*
 * CTrcHookRslg bietet die Moeglichkeit Syslog ins Trace zu integrieren
 * dies sollte mit CT_HOOK_RSLG() gerufen werden
 */

typedef void CT_RSLGW1PARAM (SAP_UC* p_param, int p_lg);
typedef void CT_RSLGWRI2    (RSLGTYPE p_ftyp, const SAP_UC* p_tabe,
                             RSLG_DATA*p_data);
typedef void CT_STRFCPY     (SAP_CHAR* p_to,  const SAP_UC* p_from, int p_len);

extern void         CTrcHookRslg   (CT_RSLGW1PARAM *,CT_RSLGWRI2 *, CT_STRFCPY *);

#define CT_HOOK_RSLG() CTrcHookRslg (rslgw1param, rslgwri2, strfcpy)

#endif /* RSLG_H_passed */

/*--------------------------------------------------------*/
/* CTrcTransHdl ()                                        */
/*                                                        */
/* Handle in File-Descriptor uebersetzen                  */
/* Falls vorhanden, noch zusaetzlich Id der               */
/* Komponente und Level zurueckgeben                      */
/*                                                        */
/* INPUT   hdl1          Trace Handle                     */
/*                                                        */
/* OUTPUT  hdl2          File-Pointer fuer Tracefile      */
/*         compid        falls aktiv, Id der Komponente   */
/*                       sonst (SAP_CHAR) 0               */
/*         complev       falls aktiv, Level, bis zu dem   */
/*                       die Komponente tracen soll,      */
/*                       sonst 0                          */
/*                                                        */
/*--------------------------------------------------------*/

extern void CTrcTransHdl (FILE		*hdl1,
			  FILE		**hdl2,
			  SAP_CHAR	*compid,
			  SAP_INT	*complev);

/*---------------------------------------------------------------------*/
/* Makros, um trotz des hysterischen Wachstums unabhaengig von         */
/* sapparam.o zu werden.                                               */
/*---------------------------------------------------------------------*/

#if defined (SAPonWINDOWS) || defined (SAPonApple)
    #define CT_GETLV getenv ("DPTRACE")

    #define  TRC_INIT(file_name, mode)                       \
	 CTrcInit (CT_GETLV == NULL ? NULL : file_name, mode,\
		   CT_GETLV == NULL ? 0 : atoi (CT_GETLV))
#else
    extern SAP_UC * sapgparam(const SAP_UC *p_name); /* Schmutzig... */
    #define CT_GETLV sapgparam(cU("rdisp/TRACE"))

    #define  TRC_INIT(file_name, mode)                       \
	 CTrcInit (CT_GETLV == NULL ? NULL : file_name, mode,\
		   CT_GETLV == NULL ? 0 : atoiU(CT_GETLV))
#endif


#define  trc_init(x)   TRC_INIT (x, CTRC_TRUNC)
#define  trc_append(x) TRC_INIT (x, CTRC_APPEND)

extern void             perrno         	 (FILE *  trace_file);
extern void             DpTrcTime      	 (FILE *  trace_file);
extern void             DpTrcTimeNoThr 	 (FILE *  trace_file);


/*--------------------------------------------------------------------*/
/* IN IMS / AIX / ... werden ANSI-varargs benutzt. Leider laesst      */
/* sich diese Form noch nicht durch unsere Makros darstellen.         */
/*--------------------------------------------------------------------*/

extern void             DpTrc		(FILE *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void             DpTrcNoThr	(FILE *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void             DpTrcVar	(FILE *,const SAP_UC *, va_list arg_ptr);
extern void		DpTrcNoThrVar	(FILE *hdl, const SAP_UC *fmt, va_list arg_ptr);
extern void             DpTrcNoTi	(FILE *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void             DpTrcNoTiNoThr	(FILE *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void		DpTrcNoTiVar	(FILE *hdl, const SAP_UC *fmt, va_list arg_ptr);
extern void		DpTrcNoTiNoThrVar (FILE *hdl, const SAP_UC *fmt, va_list arg_ptr);
extern void             RstrTrc		(FILE *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void             DpTrcErr	(FILE *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void             DpTrcErrVar	(FILE *, const SAP_UC *, va_list arg_ptr);
extern void             DpTrcErrNoThr	(FILE *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void             DpSysErr	(FILE *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void             DpSysErrNoThr	(FILE *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void             DpELogErr	(FILE *, const SAP_UC *, const SAP_UC *, ...) /*PRINTFLIKE3*/;
extern void             DpSLogErr	(FILE *, const SAP_UC *, const SAP_UC *, ...) /*PRINTFLIKE3*/;
extern void             DpELog		(const SAP_UC *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void             DpSLog		(const SAP_UC *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void             DpELogFun	(const SAP_UC *, const SAP_UC *,
					 const SAP_UC *, const SAP_UC *, ...) /*PRINTFLIKE4*/;
extern void             DpSLogFun	(const SAP_UC *, const SAP_UC *,
					 const SAP_UC *, const SAP_UC *, ...) /*PRINTFLIKE4*/;
extern void             DpTrcA7		(FILE *, const SAP_A7 *, ...) /*PRINTFLIKE2*/;
extern void             DpTrcNoThrA7	(FILE *, const SAP_A7 *, ...) /*PRINTFLIKE2*/;
extern void		DpTrcVarA7	(FILE *hdl, const SAP_A7 *fmt, va_list arg_ptr);
extern void		DpTrcNoThrVarA7 (FILE *hdl, const SAP_A7 *fmt, va_list arg_ptr);

/* !! obsolete -> use DpTrcVarA7 !! */
extern void             DpTrcA7Fmt	(FILE *, const SAP_A7 *,
					 va_list arg_ptr);

extern void             DpTrcNoTiA7	(FILE *, const SAP_A7 *, ...) /*PRINTFLIKE2*/;
extern void             DpTrcNoTiNoThrA7(FILE *, const SAP_A7 *, ...) /*PRINTFLIKE2*/;
extern void		DpTrcNoTiVarA7 (FILE *hdl, const SAP_A7 *fmt, va_list arg_ptr);
extern void		DpTrcNoTiNoThrVarA7 (FILE *hdl, const SAP_A7 *fmt, va_list arg_ptr);
extern void             RstrTrcA7	(FILE *, const SAP_A7 *, ...) /*PRINTFLIKE2*/;
extern void             DpTrcErrA7	(FILE *, const SAP_A7 *, ...) /*PRINTFLIKE2*/;
extern void             DpTrcErrNoThrA7	(FILE *, const SAP_A7 *, ...) /*PRINTFLIKE2*/;
extern void             DpSysErrA7	(FILE *, const SAP_A7 *, ...) /*PRINTFLIKE2*/;
extern void             DpSysErrNoThrA7	(FILE *, const SAP_A7 *, ...) /*PRINTFLIKE2*/;

extern void             DpTrcWarn	(FILE *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void             DpTrcWarnVar (FILE *, const SAP_UC *, va_list arg_ptr);
extern void		DpTrcWarnNoThrVar (FILE *hdl, const SAP_UC *fmt, va_list arg_ptr);

extern void             DpTrcWarnNoThr	(FILE *, const SAP_UC *, ...) /*PRINTFLIKE2*/;
extern void             DpTrcWarnA7	(FILE *, const SAP_A7 *, ...) /*PRINTFLIKE2*/;
extern void             DpTrcWarnNoThrA7(FILE *, const SAP_A7 *, ...) /*PRINTFLIKE2*/;



/*--------------------------------------------------------------------*/
/* (2) Macro Definitionen                                             */
/*                                                                    */
/* Initialisierung des Entwickler-Trace:                              */
/*                                                                    */
/* #include "dptrace.h"                                               */
/* FILE	*fp;                   			                      */
/* {                                                                  */
/*     fp = trc_init ("pathname");                                    */
/*     ...                                                            */
/* }                                                                  */
/*--------------------------------------------------------------------*/


/*--------------------------------------------------------*/
/* SAVELOC / SAVELOC2:                                    */
/* intern benutztes Macro, das die momentane Position     */
/* (Modul, Line) in einer globalen Variablen abspeichert  */
/* (nur im Macro moeglich), auf die in den einzelnen      */
/* Funktionen zugegriffen werden kann.                    */
/*--------------------------------------------------------*/

/* offset im Feld savloc zur Zeilennummer */
#define CTRC_SL_OFFSET	12

#define SAVELOC2(fi,li)	CTrcSaveLocation(fi,li)

#define SAVELOC		SAVELOC2 (cU(__FILE__), __LINE__)

/*--------------------------------------------------------*/
/* TRC: freiformatigen Text in Entwicker- und SAP-Trace   */
/*      schreiben. Aufruf aehnlich fprintf (), z.B.       */
/*      TRC ((fp, "values for x: %d y:%d\n", x, y))       */
/*                                                        */
/* Da alle String-Argumente als SAP_UC angegeben werden,  */
/* neue Trace Macros fuer ASCII A7 Werte: A7_....         */
/*--------------------------------------------------------*/
#define TRC(x)          { DPETRC(ct_level, x)  }
#define TRC0(x)         { DPETRC0(ct_level, x) }
#define TRC1(x)         { DPETRC1(ct_level, x) }
#define TRC3(x)         { DPETRC3(ct_level, x) }

#define UTRC(_l,x)      { DPETRC(_l, x)  }
#define UTRC0(_l,x)     { DPETRC0(_l, x) }
#define UTRC1(_l,x)     { DPETRC1(_l, x) }
#define UTRC3(_l,x)     { DPETRC3(_l, x) }

/* variable trace */
/* Mit _l wird der trace level angegeben, ab dem getraced werden soll */
#define VTRC(_l,x)		{ DPVTRC(_l, x)  }


#define TRC_NO_THR(x)   { DPETRC_NO_THR(ct_level, x)  }
#define TRC_NO_THR0(x)  { DPETRC_NO_THR0(ct_level, x) }
#define TRC_NO_THR1(x)  { DPETRC_NO_THR1(ct_level, x) }
#define TRC_NO_THR3(x)  { DPETRC_NO_THR3(ct_level, x) }

#define UTRC_NO_THR(_l,x)	{ DPETRC_NO_THR(_l, x)  }
#define UTRC_NO_THR0(_l,x)	{ DPETRC_NO_THR0(_l, x) }
#define UTRC_NO_THR1(_l,x)	{ DPETRC_NO_THR1(_l, x) }
#define UTRC_NO_THR3(_l,x)	{ DPETRC_NO_THR3(_l, x) }

#define VTRC_NO_THR(_l,x)	{ DPVTRC_NO_THR(_l, x)  }

/* A7 Traces */
#define A7_TRC(x)		{ A7_DPETRC(ct_level, x)  }
#define A7_TRC0(x)		{ A7_DPETRC0(ct_level, x) }
#define A7_TRC1(x)		{ A7_DPETRC1(ct_level, x) }
#define A7_TRC3(x)		{ A7_DPETRC3(ct_level, x) }

#define A7_UTRC(_l,x)		{ A7_DPETRC(_l, x)  }
#define A7_UTRC0(_l,x)		{ A7_DPETRC0(_l, x) }
#define A7_UTRC1(_l,x)		{ A7_DPETRC1(_l, x) }
#define A7_UTRC3(_l,x)		{ A7_DPETRC3(_l, x) }

#define A7_VTRC(_l,x)		{ A7_DPVTRC(_l, x)  }

/* Trace no thread id */
#define A7_TRC_NO_THR(x)	{ A7_DPETRC_NO_THR(ct_level, x)  }
#define A7_TRC_NO_THR0(x)	{ A7_DPETRC_NO_THR0(ct_level, x) }
#define A7_TRC_NO_THR1(x)	{ A7_DPETRC_NO_THR1(ct_level, x) }
#define A7_TRC_NO_THR3(x)	{ A7_DPETRC_NO_THR3(ct_level, x) }

#define A7_UTRC_NO_THR(_l,x)	{ A7_DPETRC_NO_THR(_l, x)  }
#define A7_UTRC_NO_THR0(_l,x)	{ A7_DPETRC_NO_THR0(_l, x) }
#define A7_UTRC_NO_THR1(_l,x)	{ A7_DPETRC_NO_THR1(_l, x) }
#define A7_UTRC_NO_THR3(_l,x)	{ A7_DPETRC_NO_THR3(_l, x) }

#define A7_VTRC_NO_THR(_l,x)	{ A7_DPVTRC_NO_THR(_l, x)  }

/* trace timestamp */
#define TRCTIME(x)		{ DPETRCTIME(ct_level, x) }
#define TRCTIME_NO_THR(x)	{ DPETRCTIME_NO_THR(ct_level, x) }

#define UTRCTIME(_l,x)		{ DPETRCTIME(_l, x) }
#define UTRCTIME_NO_THR(_l,x)	{ DPETRCTIME_NO_THR(_l, x) }

/* trace without timestamp */
#define TRC_NO_TI(x)		{ DPETRC_NO_TI(ct_level, x) }
#define A7_TRC_NO_TI(x)		{ A7_DPETRC_NO_TI(ct_level, x) }

#define TRC_NO_TI_NO_THR(x)     { DPETRC_NO_TI_NO_THR(ct_level, x) }
#define TRC_NO_TI_NO_THR1(x)    { DPETRC_NO_TI_NO_THR1(ct_level, x) }
#define TRC_NO_TI_NO_THR3(x)    { DPETRC_NO_TI_NO_THR3(ct_level, x) }
#define A7_TRC_NO_TI_NO_THR(x)	{ A7_DPETRC_NO_TI_NO_THR(ct_level, x) }
#define A7_TRC_NO_TI_NO_THR1(x)	{ A7_DPETRC_NO_TI_NO_THR1(ct_level, x) }
#define A7_TRC_NO_TI_NO_THR3(x)	{ A7_DPETRC_NO_TI_NO_THR3(ct_level, x) }

#define UTRC_NO_TI(_l,x)	{ DPETRC_NO_TI(_l, x) }
#define A7_UTRC_NO_TI(_l,x)	{ A7_DPETRC_NO_TI(_l, x) }

#define UTRC_NO_TI_NO_THR(_l,x)		{ DPETRC_NO_TI_NO_THR(_l, x) }
#define A7_UTRC_NO_TI_NO_THR(_l,x)	{ A7_DPETRC_NO_TI_NO_THR(_l, x) }

#define VTRC_NO_TI(_l,x)		{ DPVTRC_NO_TI(_l, x)  }
#define A7_VTRC_NO_TI(_l,x)		{ A7_DPVTRC_NO_TI(_l, x)  }

#define VTRC_NO_TI_NO_THR(_l,x)		{ DPVTRC_NO_TI_NO_THR(_l, x)  }
#define A7_VTRC_NO_TI_NO_THR(_l,x)	{ A7_DPVTRC_NO_TI_NO_THR(_l, x)  }

/*--------------------------------------------------------*/
/* TRCERR: Fehlermeldung mit freiformatigem Text in       */
/*      Entwicker- und SAP-Trace schreiben (mit Modul/    */
/*      Zeilen-Angabe). Aufruf wie TRC ()                 */
/* TRCERR_FL: zusaetzlich mit File und Line Angabe        */
/*--------------------------------------------------------*/
#define TRCERR(x)			{ DPETRCERR(ct_level,x) }
#define TRCERR_FL(x,f,l)		{ DPETRCERR_FL(ct_level,x,f,l) }
#define TRCERR_NO_THR(x)		{ DPETRCERR_NO_THR(ct_level,x) }
#define TRCERR_FL_NO_THR(x,f,l)		{ DPETRCERR_FL_NO_THR(ct_level,x,f,l) }

#define UTRCERR(_l,x)			{ DPETRCERR(_l,x) }
#define UTRCERR_FL(_l,x,f,l)		{ DPETRCERR_FL(_l,x,f,l) }
#define UTRCERR_NO_THR(_l,x)		{ DPETRCERR_NO_THR(_l,x) }
#define UTRCERR_FL_NO_THR(_l,x,f,l)	{ DPETRCERR_FL_NO_THR(_l,x,f,l) }

#define A7_TRCERR(x)			{ A7_DPETRCERR(ct_level,x) }
#define A7_TRCERR_FL(x,f,l)		{ A7_DPETRCERR_FL(ct_level,x,f,l) }
#define A7_TRCERR_NO_THR(x)		{ A7_DPETRCERR_NO_THR(ct_level,x) }
#define A7_TRCERR_FL_NO_THR(x,f,l)	{ A7_DPETRCERR_FL_NO_THR(ct_level,x,f,l) }

#define A7_UTRCERR(_l,x)		{ A7_DPETRCERR(_l,x) }
#define A7_UTRCERR_FL(_l,x,f,l)		{ A7_DPETRCERR_FL(_l,x,f,l) }
#define A7_UTRCERR_NO_THR(_l,x)		{ A7_DPETRCERR_NO_THR(_l,x) }
#define A7_UTRCERR_FL_NO_THR(_l,x,f,l)	{ A7_DPETRCERR_FL_NO_THR(_l,x,f,l) }

#define VTRCERR(_l,x)			{ DPVTRCERR(_l, x)  }
#define VTRCERR_NO_THR(_l,x)		{ DPVTRCERR_NO_THR(_l, x)  }
#define A7_VTRCERR(_l,x)		{ A7_DPVTRCERR(_l, x)  }
#define A7_VTRCERR_NO_THR(_l,x)		{ A7_DPVTRCERR_NO_THR(_l, x)  }

/*--------------------------------------------------------*/
/* TRCWARN: Warnung mit freiformatigem Text in            */
/*      Entwicker- und SAP-Trace schreiben (mit Modul/    */
/*      Zeilen-Angabe). Aufruf wie TRCERR ()              */
/* TRCWARN_FL: zusaetzlich mit File und Line Angabe       */
/*--------------------------------------------------------*/

#define TRCWARN(x)			{ DPETRCWARN(ct_level,x) }
#define TRCWARN_FL(x,f,l)		{ DPETRCWARN_FL(ct_level,x,f,l) }
#define TRCWARN_NO_THR(x)		{ DPETRCWARN_NO_THR(ct_level,x) }
#define TRCWARN_FL_NO_THR(x,f,l)	{ DPETRCWARN_FL_NO_THR(ct_level,x,f,l) }

#define UTRCWARN(_l,x)			{ DPETRCWARN(_l,x) }
#define UTRCWARN_FL(_l,x,f,l)		{ DPETRCWARN_FL(_l,x,f,l) }
#define UTRCWARN_NO_THR(_l,x)		{ DPETRCWARN_NO_THR(_l,x) }
#define UTRCWARN_FL_NO_THR(_l,x,f,l)	{ DPETRCWARN_FL_NO_THR(_l,x,f,l) }

#define A7_TRCWARN(x)			{ A7_DPETRCWARN(ct_level,x) }
#define A7_TRCWARN_FL(x,f,l)		{ A7_DPETRCWARN_FL(ct_level,x,f,l) }
#define A7_TRCWARN_NO_THR(x)		{ A7_DPETRCWARN_NO_THR(ct_level,x) }
#define A7_TRCWARN_FL_NO_THR(x,f,l)	{ A7_DPETRCWARN_FL_NO_THR(ct_level,x,f,l) }

#define A7_UTRCWARN(_l,x)		{ A7_DPETRCWARN(_l,x) }
#define A7_UTRCWARN_FL(_l,x,f,l)	{ A7_DPETRCWARN_FL(_l,x,f,l) }
#define A7_UTRCWARN_NO_THR(_l,x)	{ A7_DPETRCWARN_NO_THR(_l,x) }
#define A7_UTRCWARN_FL_NO_THR(_l,x,f,l)	{ A7_DPETRCWARN_FL_NO_THR(_l,x,f,l) }

/*--------------------------------------------------------*/
/* VTRCWARN: Warnung mit freiformatigem Text in           */
/*      Entwicker- und SAP-Trace schreiben (mit Modul/    */
/*      Zeilen-Angabe). Aufruf wie TRCERR (), zusaetzlich */
/*      mit Angabe des Tracelevels bei dem getraced       */
/*      werden soll.                                      */
/*--------------------------------------------------------*/

#define VTRCWARN(_l,x)			{ DPVTRCWARN(_l, x)  }
#define VTRCWARN_NO_THR(_l,x)		{ DPVTRCWARN_NO_THR(_l, x)  }
#define A7_VTRCWARN(_l,x)		{ A7_DPVTRCWARN(_l, x)  }
#define A7_VTRCWARN_NO_THR(_l,x)	{ A7_DPVTRCWARN_NO_THR(_l, x)  }



/*--------------------------------------------------------*/
/* TRCRET: wie TRCERR, aber mit return (val) in           */
/*      naechsthoehere Ebene. Aufruf z.B.                 */
/*      TRCRET ((fp, "Caller: Callee (ret %d)", ret), -1) */
/*--------------------------------------------------------*/
#define TRCRET(x,y)		{ DPETRCERR(ct_level,x) return (y); }
#define A7_TRCRET(x,y)		{ A7_DPETRCERR(ct_level,x) return (y); }

#define TRCRET_NO_THR(x,y)	{ DPETRCERR_NO_THR(ct_level,x) return (y); }
#define A7_TRCRET_NO_THR(x,y)	{ A7_DPETRCERR_NO_THR(ct_level,x) return (y); }

#define UTRCRET(_l,x,y)		{ DPETRCERR(_l,x) return (y); }
#define A7_UTRCRET(_l,x,y)	{ A7_DPETRCERR(_l,x) return (y); }

#define UTRCRET_NO_THR(_l,x,y)		{ DPETRCERR_NO_THR(_l,x) return (y); }
#define A7_UTRCRET_NO_THR(_l,x,y)	{ A7_DPETRCERR_NO_THR(_l,x) return (y); }

#define VTRCRET(_l,x,y)			{ DPVTRCERR(_l, x) return (y); }
#define VTRCRET_NO_THR(_l,x,y)		{ DPVTRCERR_NO_THR(_l, x) return (y); }
#define A7_VTRCRET(_l,x,y)		{ A7_DPVTRCERR(_l, x) return (y); }
#define A7_VTRCRET_NO_THR(_l,x,y)	{ A7_DPVTRCERR_NO_THR(_l, x) return (y); }


/*--------------------------------------------------------*/
/* SYSERR: wie TRCERR, aber mit Information ueber syscall-*/
/*      errno und den dazugehoerigen Text (zur Verwendung */
/*      in Unix-spezifischen Modulen). Aufruf wie TRCERR()*/
/*--------------------------------------------------------*/
#define SYSERR(x)		{ DPESYSERR(ct_level,x) }
#define A7_SYSERR(x)		{ A7_DPESYSERR(ct_level,x) }

#define SYSERR_NO_THR(x)	{ DPESYSERR_NO_THR(ct_level,x) }
#define A7_SYSERR_NO_THR(x)	{ A7_DPESYSERR_NO_THR(ct_level,x) }

#define USYSERR(_l,x)		{ DPESYSERR(_l,x) }
#define A7_USYSERR(_l,x)	{ A7_DPESYSERR(_l,x) }

#define USYSERR_NO_THR(_l,x)	{ DPESYSERR_NO_THR(_l,x) }
#define A7_USYSERR_NO_THR(_l,x)	{ A7_DPESYSERR_NO_THR(_l,x) }

/*--------------------------------------------------------*/
/* SYSRET: wie SYSERR, aber mit return (val) in           */
/*      naechsthoehere Ebene (zur Verwendung in Unix-     */
/*      spezifischen Modulen). Aufruf wie TRCRET ()       */
/*--------------------------------------------------------*/
#define SYSRET(x,y)		{ DPESYSERR(ct_level,x) return(y); }
#define A7_SYSRET(x,y)		{ A7_DPESYSERR(ct_level,x) return(y); }

#define SYSRET_NO_THR(x,y)	{ DPESYSERR_NO_THR(ct_level,x) return(y); }
#define A7_SYSRET_NO_THR(x,y)	{ A7_DPESYSERR_NO_THR(ct_level,x) return(y); }

#define USYSRET(_l,x,y)		{ DPESYSERR(_l,x) return(y); }
#define A7_USYSRET(_l,x,y)	{ A7_DPESYSERR(_l,x) return(y); }

#define USYSRET_NO_THR(_l,x,y)		{ DPESYSERR_NO_THR(_l,x) return(y); }
#define A7_USYSRET_NO_THR(_l,x,y)	{ A7_DPESYSERR_NO_THR(_l,x) return(y); }

/*--------------------------------------------------------*/
/* TRC_LOCK_FILE/TRC_UNLOCK_FILE                          */
/*      expliziter lock des ctrace. Damit koennen         */
/*      in multithreaded Apps mehrere Zeilen aneinander-  */
/*      haengend im Tracefile ausgegeben werden.          */
/*      Lock muss nach der Ausgabe mit TRC_UNLOCK wieder  */
/*      freigegeben werden.                               */
/*      In single-threaded Apps hat dieser Lock keine     */
/*      Auswirkung.                                       */
/*--------------------------------------------------------*/
#define TRC_LOCK_FILE   DpLock()
#define TRC_UNLOCK_FILE DpUnlock()


/*--------------------------------------------------------*/
/* intern                                                 */
/*--------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*                                                                    */
/* Die folgenden 3 Informationen entscheiden darueber, ob ein Trace-  */
/* Eintrag geschrieben werden soll:                                   */
/*                                                                    */
/* . ab welchem Trace-Level soll der Eintrag geschrieben werden ?     */
/*   (entrylev = immer, nur bei 1, ab 2, ab 3)                        */
/*                                                                    */
/* . was ist der globale Trace-Level ?                                */
/*   (Variable TRACE = 0, 1, 2, 3)                                    */
/*                                                                    */
/* . was ist der Trace-Level dieser Komponente ?                      */
/*   (complev = 0, 1, 2, 3)                                           */
/*                                                                    */
/*                                                                    */
/* Der Eintrag wird geschrieben, wenn gilt:                           */
/*                                                                    */
/*   (ct_level   >= entrylev) &&                                      */
/*   (complev >= entrylev)                                            */
/*                                                                    */
/*--------------------------------------------------------------------*/
#ifdef GEN_TRACE
/* general trace macros */
#define DPETRC(_l,x)       { if ((_l) >= 2)                                 \
                             { DpLock(); DpTrc x; DpUnlock(); } }
#define DPETRC0(_l,x)      { DpLock(); EntLev=0; DpTrc x; EntLev=2; DpUnlock(); }
#define DPETRC1(_l,x)      { if ((_l) >= 1)                                 \
                             {DpLock();EntLev=1; DpTrc x; EntLev=2;DpUnlock();}}
#define DPETRC3(_l,x)      { if ((_l) >= 3)                                 \
                             {DpLock();EntLev=3; DpTrc x; EntLev=2;DpUnlock();}}
#define DPETRCERR(_l,x)    { if ((_l) >= 1)                                 \
                             {DpLock(); SAVELOC; DpTrcErr x; DpUnlock();}}
#define DPESYSERR(_l,x)    { if ((_l) >= 1)                                 \
                             {DpLock(); SAVELOC; DpSysErr x; DpUnlock();}}
#define DPETRCTIME(_l,x)   { if ((_l) >= 1)                                 \
                             {DpLock(); DpTrcTime (x); DpUnlock();}}
#define DPETRC_NO_TI(_l,x) { DpLock();EntLev=2; DpTrcNoTi x; EntLev=2;DpUnlock(); }

#define DPETRCERR_FL(_l,x,f,l) { if ((_l) >= 1)                             \
                              {DpLock(); SAVELOC2(f,l); DpTrcErr x;DpUnlock();}}
#define DPESYSERR_FL(_l,x,f,l) { if ((_l) >= 1)                             \
                              {DpLock(); SAVELOC2(f,l); DpSysErr x;DpUnlock();}}


/* warning macros */
#define DPETRCWARN(_l,x)   { if ((_l) >= 1)                                 \
			   {DpLock(); SAVELOC; DpTrcWarn x; DpUnlock();}}
#define DPETRCWARN_FL(_l,x,f,l) { if ((_l) >= 1)                            \
                           {DpLock(); SAVELOC2(f,l); DpTrcWarn x;DpUnlock();}}
#define DPETRCWARN_NO_THR(_l,x) { if ((_l) >= 1)                            \
			   {DpLock(); SAVELOC; DpTrcWarnNoThr x; DpUnlock();}}
#define DPETRCWARN_FL_NO_THR(_l,x,f,l) { if ((_l) >= 1)                     \
                           {DpLock(); SAVELOC2(f,l); DpTrcWarnNoThr x;DpUnlock();}}
#define A7_DPETRCWARN(_l,x)    { if ((_l) >= 1)                             \
			   {DpLock(); SAVELOC; DpTrcWarnA7 x; DpUnlock();}}
#define A7_DPETRCWARN_FL(_l,x,f,l) { if ((_l) >= 1)                         \
			   {DpLock(); SAVELOC2(f,l); DpTrcWarnA7 x;DpUnlock();}}
#define A7_DPETRCWARN_NO_THR(_l,x)    { if ((_l) >= 1)                      \
			   {DpLock(); SAVELOC; DpTrcWarnNoThrA7 x; DpUnlock();}}
#define A7_DPETRCWARN_FL_NO_THR(_l,x,f,l) { if ((_l) >= 1)                  \
			   {DpLock(); SAVELOC2(f,l); DpTrcWarnNoThrA7 x;DpUnlock();}}



/* general trace macros ohne Thread ID */
#define DPETRC_NO_THR(_l,x)    { if ((_l) >= 2)                             \
				{ DpLock(); DpTrcNoThr x; DpUnlock(); } }
#define DPETRC_NO_THR0(_l,x)   { DpLock(); EntLev=0; DpTrcNoThr x; EntLev=2; DpUnlock(); }
#define DPETRC_NO_THR1(_l,x)   { if ((_l) >= 1)                                 \
				{DpLock();EntLev=1; DpTrcNoThr x; EntLev=2;DpUnlock();}}
#define DPETRC_NO_THR3(_l,x)   { if ((_l) >= 3)                                 \
				{DpLock();EntLev=3; DpTrcNoThr x; EntLev=2;DpUnlock();}}
#define DPETRCERR_NO_THR(_l,x) { if ((_l) >= 1)                                 \
				{DpLock(); SAVELOC; DpTrcErrNoThr x; DpUnlock();}}
#define DPESYSERR_NO_THR(_l,x) { if ((_l) >= 1)                                 \
				{DpLock(); SAVELOC; DpSysErrNoThr x; DpUnlock();}}
#define DPETRCTIME_NO_THR(_l,x) { if ((_l) >= 1)                                 \
				{DpLock(); DpTrcTimeNoThr (x); DpUnlock();}}
#define DPETRC_NO_TI_NO_THR(_l,x) { if ((_l) >= 2)                               \
				{ DpLock();DpTrcNoTiNoThr x;DpUnlock(); }}
#define DPETRC_NO_TI_NO_THR1(_l,x) { if ((_l) >= 1)                              \
				{ DpLock();EntLev=1;DpTrcNoTiNoThr x;EntLev=2; DpUnlock(); }}
#define DPETRC_NO_TI_NO_THR3(_l,x) { if ((_l) >= 3)                              \
				{ DpLock();EntLev=3;DpTrcNoTiNoThr x;EntLev=2; DpUnlock(); }}

#define DPETRCERR_FL_NO_THR(_l,x,f,l) { if ((_l) >= 1)                             \
                              {DpLock(); SAVELOC2(f,l); DpTrcErrNoThr x;DpUnlock();}}
#define DPESYSERR_FL_NO_THR(_l,x,f,l) { if ((_l) >= 1)                             \
                              {DpLock(); SAVELOC2(f,l); DpSysErrNoThr x;DpUnlock();}}


/* Tracemacros mit plain Ascii A7 Datentypen */
#define A7_DPETRC(_l,x)       { if ((_l) >= 2)                              \
				{ DpLock(); DpTrcA7 x; DpUnlock(); } }
#define A7_DPETRC0(_l,x)      { DpLock(); EntLev=0; DpTrcA7 x; EntLev=2; DpUnlock(); }
#define A7_DPETRC1(_l,x)      { if ((_l) >= 1)                              \
				{DpLock();EntLev=1; DpTrcA7 x; EntLev=2;DpUnlock();}}
#define A7_DPETRC3(_l,x)      { if ((_l) >= 3)                              \
				{DpLock();EntLev=3; DpTrcA7 x; EntLev=2;DpUnlock();}}
#define A7_DPETRCERR(_l,x)    { if ((_l) >= 1)                              \
				{DpLock(); SAVELOC; DpTrcErrA7 x; DpUnlock();}}
#define A7_DPESYSERR(_l,x)    { if ((_l) >= 1)                              \
				{DpLock(); SAVELOC; DpSysErrA7 x; DpUnlock();}}
#define A7_DPETRC_NO_TI(_l,x) { DpLock();EntLev=0; DpTrcNoTiA7 x; EntLev=2;DpUnlock(); }

#define A7_DPETRCERR_FL(_l,x,f,l) { if ((_l) >= 1)                          \
					{DpLock(); SAVELOC2(f,l); DpTrcErrA7 x;DpUnlock();}}
#define A7_DPESYSERR_FL(_l,x,f,l) { if ((_l) >= 1)                          \
					{DpLock(); SAVELOC2(f,l); DpSysErrA7 x;DpUnlock();}}

/* Tracemacros mit plain Ascii A7 Datentypen ohne Thread Id */
#define A7_DPETRC_NO_THR(_l,x)	{ if ((_l) >= 2)                              \
					{ DpLock(); DpTrcNoThrA7 x; DpUnlock(); } }
#define A7_DPETRC_NO_THR0(_l,x) { DpLock(); EntLev=0; DpTrcNoThrA7 x; EntLev=2; DpUnlock(); }
#define A7_DPETRC_NO_THR1(_l,x) { if ((_l) >= 1)                              \
					{DpLock();EntLev=1; DpTrcNoThrA7 x; EntLev=2;DpUnlock();}}
#define A7_DPETRC_NO_THR3(_l,x) { if ((_l) >= 3)                              \
					{DpLock();EntLev=3; DpTrcNoThrA7 x; EntLev=2;DpUnlock();}}
#define A7_DPETRCERR_NO_THR(_l,x)    { if ((_l) >= 1)                              \
					{DpLock(); SAVELOC; DpTrcErrNoThrA7 x; DpUnlock();}}
#define A7_DPESYSERR_NO_THR(_l,x)    { if ((_l) >= 1)                              \
					{DpLock(); SAVELOC; DpSysErrNoThrA7 x; DpUnlock();}}
#define A7_DPETRC_NO_TI_NO_THR(_l,x) { if ((_l) >= 2)                              \
					{ DpLock();DpTrcNoTiNoThrA7 x;DpUnlock(); }}
#define A7_DPETRC_NO_TI_NO_THR1(_l,x) { if ((_l) >= 1)                              \
					{ DpLock(); EntLev=1; DpTrcNoTiNoThrA7 x; EntLev=2; DpUnlock(); }}
#define A7_DPETRC_NO_TI_NO_THR3(_l,x) { if ((_l) >= 3)                              \
					{ DpLock(); EntLev=3; DpTrcNoTiNoThrA7 x; EntLev=2; DpUnlock(); }}

#define A7_DPETRCERR_FL_NO_THR(_l,x,f,l) { if ((_l) >= 1)                          \
					{DpLock(); SAVELOC2(f,l); DpTrcErrNoThrA7 x;DpUnlock();}}
#define A7_DPESYSERR_FL_NO_THR(_l,x,f,l) { if ((_l) >= 1)                          \
					{DpLock(); SAVELOC2(f,l); DpSysErrNothrA7 x;DpUnlock();}}


/* variable trace */
#define DPVTRC(_l,x)       { if ((ct_level) >= (_l))                                \
                             { DpLock(); DpTrc x; DpUnlock(); } }
#define DPVTRC_NO_THR(_l,x){ if ((ct_level) >= (_l))                                \
                             { DpLock(); DpTrcNoThr x; DpUnlock(); } }
#define A7_DPVTRC(_l,x)	   { if ((ct_level) >= (_l))                                \
                             { DpLock(); DpTrcA7 x; DpUnlock(); } }
#define A7_DPVTRC_NO_THR(_l,x)	   { if ((ct_level) >= (_l))                        \
                             { DpLock(); DpTrcNoThrA7 x; DpUnlock(); } }

#define DPVTRC_NO_TI(_l,x)   { if ((ct_level) >= (_l))                              \
                             { DpLock(); DpTrcNoTi x; DpUnlock(); } }
#define A7_DPVTRC_NO_TI(_l,x)	   { if ((ct_level) >= (_l))                        \
                             { DpLock(); DpTrcNoTiA7 x; DpUnlock(); } }
#define DPVTRC_NO_TI_NO_THR(_l,x)  { if ((ct_level) >= (_l))                        \
                             { DpLock(); DpTrcNoTiNoThr x; DpUnlock(); } }
#define A7_DPVTRC_NO_TI_NO_THR(_l,x) { if ((ct_level) >= (_l))                      \
                             { DpLock(); DpTrcNoTiNoThrA7 x; DpUnlock(); } }
#define DPVTRCERR(_l,x)	     { if ((ct_level) >= (_l))                              \
                             {DpLock(); SAVELOC; DpTrcErr x; DpUnlock();}}
#define DPVTRCERR_NO_THR(_l,x) { if ((ct_level) >= (_l))                            \
                             {DpLock(); SAVELOC; DpTrcErrNoThr x; DpUnlock();}}
#define A7_DPVTRCERR(_l,x)   { if ((ct_level) >= (_l))                              \
                             {DpLock(); SAVELOC; DpTrcErrA7 x; DpUnlock();}}
#define A7_DPVTRCERR_NO_THR(_l,x) { if ((ct_level) >= (_l))                         \
                             {DpLock(); SAVELOC; DpTrcErrNoThrA7 x; DpUnlock();}}

#define DPVTRCWARN(_l,x)     { if ((ct_level) >= (_l))                              \
                             {DpLock(); SAVELOC; DpTrcWarn x; DpUnlock();}}
#define DPVTRCWARN_NO_THR(_l,x)      { if ((ct_level) >= (_l))                      \
                             {DpLock(); SAVELOC; DpTrcWarnNoThr x; DpUnlock();}}
#define A7_DPVTRCWARN(_l,x)  { if ((ct_level) >= (_l))                              \
                             {DpLock(); SAVELOC; DpTrcWarnA7 x; DpUnlock();}}
#define A7_DPVTRCWARN_NO_THR(_l,x)   { if ((ct_level) >= (_l))                      \
                             {DpLock(); SAVELOC; DpTrcWarnNoThrA7 x; DpUnlock();}}


#else
#define DPETRC(_l,x)       { }
#define DPETRC0(_l,x)      { }
#define DPETRC1(_l,x)      { }
#define DPETRC3(_l,x)      { }
#define DPETRCERR(_l,x)    { }
#define DPESYSERR(_l,x)    { }
#define TRCTIME(_l,x)      { }
#define DPETRC_NO_TI(_l,x) { }

#define DPETRCERR_FL(_l,x,f,l) { }
#define DPESYSERR_FL(_l,x,f,l) { }


/* general trace macros ohne Thread ID */
#define DPETRC_NO_THR(_l,x)	{}
#define DPETRC_NO_THR0(_l,x)	{}
#define DPETRC_NO_THR1(_l,x)	{}
#define DPETRC_NO_THR3(_l,x)	{}
#define DPETRCERR_NO_THR(_l,x)	{}
#define DPESYSERR_NO_THR(_l,x)	{}
#define DPETRCTIME_NO_THR(_l,x)	{}
#define DPETRC_NO_TI_NO_THR(_l,x) {}
#define DPETRC_NO_TI_NO_THR1(_l,x) {}
#define DPETRC_NO_TI_NO_THR3(_l,x) {}

#define DPETRCERR_FL_NO_THR(_l,x,f,l) {}
#define DPESYSERR_FL_NO_THR(_l,x,f,l) {}


/* Tracemacros mit plain Ascii A7 Datentypen */
#define A7_DPETRC(_l,x)       { }
#define A7_DPETRC0(_l,x)      { }
#define A7_DPETRC1(_l,x)      { }
#define A7_DPETRC3(_l,x)      { }
#define A7_DPETRCERR(_l,x)    { }
#define A7_DPESYSERR(_l,x)    { }
#define A7_DPETRC_NO_TI(_l,x) { }

#define A7_DPETRCERR_FL(_l,x,f,l) { }
#define A7_DPESYSERR_FL(_l,x,f,l) { }

/* Tracemacros mit plain Ascii A7 Datentypen ohne Thread Id */
#define A7_DPETRC_NO_THR(_l,x)		{}
#define A7_DPETRC_NO_THR0(_l,x)		{}
#define A7_DPETRC_NO_THR1(_l,x)		{}
#define A7_DPETRC_NO_THR3(_l,x)		{}
#define A7_DPETRCERR_NO_THR(_l,x)	{}
#define A7_DPESYSERR_NO_THR(_l,x)	{}
#define A7_DPETRC_NO_TI_NO_THR(_l,x)	{}
#define A7_DPETRC_NO_TI_NO_THR1(_l,x)	{}
#define A7_DPETRC_NO_TI_NO_THR3(_l,x)	{}

#define A7_DPETRCERR_FL_NO_THR(_l,x,f,l) {}
#define A7_DPESYSERR_FL_NO_THR(_l,x,f,l) {}

/* variable trace*/
#define DPVTRC(_l,x)			{}
#define DPVTRC_NO_THR(_l,x)		{}
#define A7_DPVTRC(_l,x)			{}
#define A7_DPVTRC_NO_THR(_l,x)		{}
#define DPVTRC_NO_TI(_l,x)		{}
#define A7_DPVTRC_NO_TI(_l,x)		{}
#define DPVTRC_NO_TI_NO_THR(_l,x)	{}
#define A7_DPVTRC_NO_TI_NO_THR(_l,x)	{}
#define DPVTRCERR(_l,x)			{}
#define DPVTRCERR_NO_THR(_l,x)		{}
#define A7_DPVTRCERR(_l,x)		{}
#define A7_DPVTRCERR_NO_THR(_l,x)	{}
#define DPVTRCWARN(_l,x)		{}
#define DPVTRCWARN_NO_THR(_l,x)		{}
#define A7_DPVTRCWARN(_l,x)		{}
#define A7_DPVTRCWARN_NO_THR(_l,x)	{}

#endif


/*--------------------------------------------------------*/
/* LOGERR: 20 Bytes freiformatige Fehlermeldung (plus 12  */
/*      Bytes Modul/Zeilen-Angabe) in SAP-Syslog, S- und  */
/*      E- Trace schreiben. Aufruf z.B.                   */
/*      LOGERR ((fp, "E01", "Dies sind %d Bytes ", 20))   */
/*--------------------------------------------------------*/
#define LOGERR(x)                                  \
{                                                  \
    DpLock();                                      \
    SAVELOC;                                       \
    DPELOGERR(x)                                   \
    DPSLOGERR(x)                                   \
    DpUnlock();                                    \
}


/*--------------------------------------------------------*/
/* LOGRET: wie LOGERR, aber mit autom. Return. Aufruf z.B.*/
/*      LOGRET ((fp, "E01", "Dies sind %d Bytes ", 20),-1)*/
/*--------------------------------------------------------*/
#define LOGRET(x, y)                               \
{                                                  \
    DpLock();                                      \
    SAVELOC;                                       \
    DPELOGERR(x)                                   \
    DPSLOGERR(x)                                   \
    DpUnlock();                                    \
    return (y);                                    \
}


/*---------------------------------------------------------------*/
/*                                                               */
/*          +---------------------------------+-------+------+   */
/* RSLG     |            Kunden-Text          | Modul | Line |   */
/*          +---------------------------------+-------+------+   */
/*                                                               */
/*          20 Bytes freiformatiger Kunden-Text                  */
/*           8 Bytes Modulname (automatisch generiert)           */
/*           4 Bytes Zeilennummer (automatisch generiert)        */
/*                                                               */
/* Aufruf z.B.                                                   */
/* RSLG (("E17", "%8.8s%4.4d%s", "UserName", 17, "XY"));         */
/*                                                               */
/*---------------------------------------------------------------*/
#define RSLG(x)                                    \
{                                                  \
    DpLock();                                      \
    SAVELOC;                                       \
    DPELOG(x);                                     \
    DPSLOG(x);                                     \
    DpUnlock();                                    \
}


/*---------------------------------------------------------------*/
/*                                                               */
/*          +-------------+----------+--------+-------+------+   */
/* RSLGFUN  | Kunden-Text | FuncName | Reason | Modul | Line |   */
/*          +-------------+----------+--------+-------+------+   */
/*                                                               */
/*          6 Bytes freiformatiger Kunden-Text                   */
/*          7 Bytes Funktionsname (Caller)                       */
/*          7 Bytes Error-Reason (z.B. Callee)                   */
/*          8 Bytes Modulname (automatisch generiert)            */
/*          4 4 Bytes Zeilennummer (automatisch generiert)       */
/*                                                               */
/* Aufruf z.B.                                                   */
/* RSLGFUN (("E17", "Callername", "CalleeName", "%4.4d", 17));   */
/*                                                               */
/*---------------------------------------------------------------*/
#define RSLGFUN(x)                                 \
{                                                  \
    DpLock();                                      \
    SAVELOC;                                       \
    DPELOGFUN(x);                                  \
    DPSLOGFUN(x);                                  \
    DpUnlock();                                    \
}

/*---------------------------------------------------------------*/
/*                                                               */
/*             +--------------------+---+------------+           */
/* RSLGSAVTXT  | variabler Textteil | Laenge oder -1 |           */
/*             +--------------------+----------------+           */
/*                                                               */
/*          bis zu 300 Zeichen langer varaibler Text             */
/*          Laenge des Textes oder -1 (bei -1 bestimmt die       */
/*          rslg-Schicht die Textlaenge durch strlen)            */
/*                                                               */
/* Aufruf z.B.                                                   */
/* RSLGSAVTXT (("ein langer variabler Text", -1))                */
/*                                                               */
/*---------------------------------------------------------------*/
#ifdef SYS_LOG
#define RSLGSAVTXT(x) { CTrcRslgw1param x; }
#else
#define RSLGSAVTXT(x)
#endif

/*--------------------------------------------------------*/
/* intern                                                 */
/*--------------------------------------------------------*/
/* el 1Feb2004 - avoid new ccQ warnings (format string
 *               in variable)
 */
#ifdef GEN_TRACE
#define DPELOGERR(x)    { if (ct_level && ct_log_level) \
	    {DpLock(); \
	     /* CCQ_FORMAT_STRING_OK */ \
	     DpELogErr x; \
	     DpUnlock();} }
#define DPELOG(x)       { if (ct_level && ct_log_level) \
	    {DpLock(); \
	    /* CCQ_FORMAT_STRING_OK */ \
	    DpELog x; \
	    DpUnlock();} }
#define DPELOGFUN(x)    { if (ct_level && ct_log_level) \
	    {DpLock(); \
	    /* CCQ_FORMAT_STRING_OK */ \
	    DpELogFun x; \
	    DpUnlock();} }
#else
#define DPELOGERR(x)    { }
#define DPELOG(x)       { }
#define DPELOGFUN(x)    { }
#endif

#ifdef SYS_LOG
#define DPSLOGERR(x)    { if (ct_log_level) \
	    {DpLock(); \
	    /* CCQ_FORMAT_STRING_OK */ \
	    DpSLogErr x; \
	    DpUnlock();} }
#define DPSLOG(x)       { if (ct_log_level) \
	    {DpLock(); \
	    /* CCQ_FORMAT_STRING_OK */ \
	    DpSLog x; \
	    DpUnlock();} }
#define DPSLOGFUN(x)    { if (ct_log_level) \
	    {DpLock(); \
	    /* CCQ_FORMAT_STRING_OK */ \
	    DpSLogFun x; \
	    DpUnlock();} }
#else
#define DPSLOGERR(x)    { }
#define DPSLOG(x)       { }
#define DPSLOGFUN(x)    { }
#endif


/*--------------------------------------------------------------------*/
/* (3) miscellaneous                                                  */
/*--------------------------------------------------------------------*/

/* SAP-Trace .........................................................*/

#define STR_MOD_IN(modul)       { }
#define STR_MOD_OUT()           { }
#define STR_FCT_IN(func)        { }
#define STR_FCT_OUT(rc)         { }
#define STR_FCT_CALL(func)      { }
#define STR_FCT_RET(rc)         { }
#define STR_FORK_BE()           { }
#define STR_FORK_AF(p)          { }


/*--------------------------------------------------------------------*/
/* (4) Formatstrings fuer Trace-Files                                 */
/*--------------------------------------------------------------------*/

#define TRC_DISP                cU("dev_disp")
#define TRC_TSKH                cU("dev_w%d")
#define TRC_DB                  cU("dev_db%d")
#define TRC_APPC                cU("dev_appc")
#define TRC_MSG                 cU("dev_ms")
#define TRC_GWRD		cU("dev_rd")
#define TRC_GWWR		cU("dev_wr")
#define TRC_GWWP		cU("dev_wp%d")
#define TRC_GWMON		cU("dev_mon")
#define TRC_RFC  		cU("dev_rfc%d")
#define TRC_ICM                 cU("dev_icm")
#define TRC_ICM_SIG 		cU("dev_icm_sig") /* Signalthread     */
#define TRC_ICM_WD 		cU("dev_icm_wd")  /* Watchdog         */
#define TRC_ICM_RQW 		cU("dev_icm_rqw") /* ReqQueueWriter   */
#define TRC_ICM_WEBDISP		cU("dev_webdisp")
#define TRC_ICM_WEBDISP_SIG	cU("dev_webdisp_sig.log")
#define TRC_ICM_WEBDISP_WD	cU("dev_webdisp_wd.log")
#define TRC_ICM_WEBDISP_RQW	cU("dev_webdisp_rqw.log")

#define TRC_LOG_EXTENSION       cU(".old")
#define TRC_LOG_NEW_EXTENSION   cU(".new")


#ifdef SAPonOS2_2x
#define TRPTRCVERSION      3
extern int TrpQueryTRPTRCVersion (void);
#endif

#ifdef SAPonOS400
#define TRCO4ERR(fp, err, called) \
         { \
           SAP_UC  msg[100]; \
           SAP_UC  help[6000]; \
           SAP_UC *s; \
           TRCERR((fp, cU("AS/400 Exception %.7s occured calling %s"), \
                   err.Get_Exception_Id(), called)); \
           PlainMessageText(msg, 100, err); \
           TRC1((fp, cU("---------------------------------------------------") \
                 cU("-----------------------------------------------------\n"))) \
           TRC1((fp, cU("%s\n"), msg)) \
           SecLvlMessageText(help, 6000, err); \
           TRC1((fp, cU("--- More details ---\n"))) \
           TRC1((fp, cU("%s\n"), MessageLine(help, 80))) \
           while ((s = MessageLine(NULL, 80))) \
             TRC1((fp, cU("%s\n"), s)) \
           TRC1((fp, cU("---------------------------------------------------") \
                 cU("-----------------------------------------------------\n"))) \
         }
#endif

/* Set hiding (of session cookie value and ..) in data display */
void CTrcSetHiding (SAP_BOOL hidingActive);

END_externC

#endif /* DPTRACE_H */
