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
#ifndef VPR08TRACE_C
#define VPR08TRACE_C

#include "gpr03.h"
#include "vpr08trace.h"
#include <ctype.h>

#ifdef SQLODBC
/********  ODBC async proceduren vpa09ac  **********/ /* PTS 1120833 */
#define PA09_WAIT_FOR_SEMAPHORE    1
#define PA09_RELEASE_SEMAPHORE     2
#define PA09_SEMAPHORE_SQLCANCEL   10
#define PA09_SEMAPHORE_WRITETRACE  11

extern void pa09Semaphore (int sema, int action);
#endif

/*******************************************************************************

  module:       vpr08trace.c

  ------------------------------------------------------------------------------

  responsible:  


  
  special area: PR layer (precompiler runtime)

  description:  trace routines

  see also:     
  ------------------------------------------------------------------------------


*******************************************************************************/

/*------------------------------------------------------------------------------
    Include files:
  ------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
    Defines, macro definitions
  ------------------------------------------------------------------------------*/

#define VPR08TRACE_CRLF "\x0A"


#define LENGTH_OF_TRACEBUFFER 256

#define VPR08TRACE_CRLF "\x0A"

/*--------------------------------------------------------*/
void pr08EnterFunction(sqlratype * pSqlra)
{
    tsp00_Int2    *strl  = &pSqlra->rasqltap->tastr80l;
    char        *str    = (char *) &pSqlra->rasqltap->tastr80;

    *strl = sprintf (str, "");
    pr08vfwrtrc (pSqlra->rasqltap);
}


/*--------------------------------------------------------*/
/* signed int trace                                       */
void pr08sint2trace (sqlratype * pSqlra, char *szStr, tsp00_Int2 sb2Number, char cWriteTrace)
{
    char        istr [256];
    tsp00_Int2    *strl  = &pSqlra->rasqltap->tastr80l;
    tsp00_Int2    cStrl   = 0;
    char        *str    = (char *) &pSqlra->rasqltap->tastr80;

    cStrl = sprintf (istr, "%s: %d", szStr, sb2Number);
    memcpy ((str+*strl), istr, cStrl);
    (*strl) += cStrl;

    if (cWriteTrace==FLUSHTRACE)
    {
        pr08vfwrtrc (pSqlra->rasqltap);
    }
}

void pr08sint4trace (sqlratype * pSqlra, char *szStr, tsp00_Int4 sb4Number, char cWriteTrace)
{
    char        istr [256];
    tsp00_Int2    *strl  = &pSqlra->rasqltap->tastr80l;
    tsp00_Int2    cStrl   = 0;
    char        *str    = (char *) &pSqlra->rasqltap->tastr80;

    cStrl = sprintf (istr, "%s: %d", szStr, sb4Number);
    memcpy ((str+*strl), istr, cStrl);
    (*strl) += cStrl;

    if (cWriteTrace==FLUSHTRACE)
    {
        pr08vfwrtrc (pSqlra->rasqltap);
    }
}

/*--------------------------------------------------------*/
/* unsigned int trace                                     */
void pr08uint2trace (sqlratype * pSqlra, char *szStr, tsp00_Uint2 ub2Number, char cWriteTrace)
{
    char        istr [256];
    tsp00_Int2    *strl  = &pSqlra->rasqltap->tastr80l;
    tsp00_Int2    cStrl   = 0;
    char        *str    = (char *) &pSqlra->rasqltap->tastr80;

    cStrl = sprintf (istr, "%s: %d", szStr, ub2Number);
    memcpy ((str+*strl), istr, cStrl);
    (*strl) += cStrl;

    if (cWriteTrace==FLUSHTRACE)
    {
        pr08vfwrtrc (pSqlra->rasqltap);
    }
}

void pr08uint4trace (sqlratype * pSqlra, char *szStr, tsp00_Uint4 ub4Number, char cWriteTrace)
{
    char        istr [256];
    tsp00_Int2    *strl  = &pSqlra->rasqltap->tastr80l;
    tsp00_Int2    cStrl   = 0;
    char        *str    = (char *) &pSqlra->rasqltap->tastr80;

    cStrl = sprintf (istr, "%s: %d", szStr, ub4Number);
    memcpy ((str+*strl), istr, cStrl);
    (*strl) += cStrl;

    if (cWriteTrace==FLUSHTRACE)
    {
        pr08vfwrtrc (pSqlra->rasqltap);
    }
}


/*--------------------------------------------------------*/
void pr08longhextrace (sqlratype * pSqlra, char *szComment, tsp00_Int4 * buf, char cWriteTrace)
{
    char        istr [256];
    size_t    *strl  = (size_t*) &pSqlra->rasqltap->tastr80l;
    size_t    cStrl   = 0;
    char        *str    = (char *) &pSqlra->rasqltap->tastr80;

    
    cStrl = (int) sprintf (istr, "%s: %0*x", szComment, 2 * sizeof (long),  *buf);
    memcpy ((str+*strl), istr, cStrl);
    (*strl) += cStrl;
    if (cWriteTrace==FLUSHTRACE)
    {
        pr08vfwrtrc (pSqlra->rasqltap);
    }
}


/*--------------------------------------------------------*/
void pr08TraceString (sqlratype * pSqlra, char *text, char cWriteTrace)
{
    pr08TraceNonTerminatedString(pSqlra, "", text, (tsp00_Int2) strlen(text),cWriteTrace);
}

/*--------------------------------------------------------*/
void pr08TraceZeroTerminatedString (sqlratype * pSqlra, char *szComment, char *text, char cWriteTrace)
{
    pr08TraceNonTerminatedString(pSqlra, szComment, text, (tsp00_Int2) strlen(text),cWriteTrace);
}

/*--------------------------------------------------------*/
void pr08TraceNonTerminatedString (sqlratype * pSqlra, char *szComment, char *text, tsp00_Int4 len, char cWriteTrace)
{
    char                    *str        = pSqlra->rasqltap->tastr80;
    tsp00_Int2                *strl       = &pSqlra->rasqltap->tastr80l;
    tsp00_Int4                iComment    = (int) strlen(szComment),
                            curlen      = 0,
                            i           = 0;

    if (len<=0)          /* strings must be at least 1 char long */
        return;

    if (iComment>0)
    {
        memcpy (str+*strl, (void *) szComment, (int) iComment);
        *strl+=iComment;
        str[*strl]=(char) ':';      /* now append ': ' */
        str[*strl+1]=(char) ' ';
        *strl+=2;
    }

    if (len+iComment>LENGTH_OF_TRACEBUFFER)
    {
        /* trace more than LENGTH_OF_TRACEBUFFER characters */
        len += iComment;

        /* the first LENGTH_OF_TRACEBUFFER chars */
        memcpy (str+*strl, (void *) text, (int) LENGTH_OF_TRACEBUFFER - iComment);
        *strl = LENGTH_OF_TRACEBUFFER;
        pr08vfwrtrc (pSqlra->rasqltap);

        /* now the rest */
        for (i=0; i< (tsp00_Int2) len/LENGTH_OF_TRACEBUFFER; i++)
        {
            memcpy ((void *) str, (void *) (text + (i*LENGTH_OF_TRACEBUFFER)), (int) LENGTH_OF_TRACEBUFFER);
            *strl = LENGTH_OF_TRACEBUFFER;
            pr08vfwrtrc (pSqlra->rasqltap);
        } 

        /* trace the very last characters */
        memcpy ((void *) str, (void *) (text + (i*LENGTH_OF_TRACEBUFFER)), (int) len - (i*LENGTH_OF_TRACEBUFFER));
        *strl = len - (i*LENGTH_OF_TRACEBUFFER);
        if (cWriteTrace==FLUSHTRACE)
        {
            pr08vfwrtrc (pSqlra->rasqltap);
        }
    }
    else
    {
        memcpy (str+*strl, (void *) text, (int) len);
        *strl += len;
        if (cWriteTrace==FLUSHTRACE)
        {
            pr08vfwrtrc (pSqlra->rasqltap);
        }
    }
}

/*--------------------------------------------------------*/
void pr08traceErr (sqlratype * pSqlra, char *szComment, struct SQLERROR *ret)
{
    tsp00_Int4        rcode;
    tsp00_Int4        pre     = (tsp00_Int4) ret->eprerr,
                    lzu     = (tsp00_Int4) ret->elzu;

    if (ret->elzu != net_ok)
        rcode = lzu;
    else if (ret->eprerr != cpr_p_ok)
        rcode = pre;
    else if (ret->ereturncode != 0)
        rcode = (tsp00_Int4) ret->ereturncode;
    else
        rcode = 0;
    if (rcode != 0)
    {
        pr08TraceZeroTerminatedString (pSqlra, szComment, "ERROR while executing that commando", FLUSHTRACE);
        pr08sint4trace (pSqlra, "RETURN CODE OF CDA/LDA(", (tsp00_Int4) rcode, DONTFLUSHTRACE);
        pr08TraceNonTerminatedString (pSqlra, ")ERROR TEXT: ", (char *) ret->etext, ret->etextlen, FLUSHTRACE);
    }
    else
    {
        /* auf Wunsch von Burkhard auskommentiert
        pr08TraceZeroTerminatedString (pSqlra, szComment, "commando successful", FLUSHTRACE); */
    }
}


/*--------------------------------------------------------*/
void pr08vfwrtrc (sqltatype * sqlta)
{
    tsp05_RteFileError      vferr;
    struct                  SQLERROR dret;
    int                     i;

    p03initsqlem (&dret);
    
    for (i=0;i<sqlta->tastr80l;i++)
    {
        if (!isprint(sqlta->tastr80[i]))
        {
            sqlta->tastr80[i]=46;      /* nicht druckbare Zeichen durch . ersetzen */
        }
    }
#ifdef SQLODBC
      pa09Semaphore (PA09_SEMAPHORE_WRITETRACE, PA09_WAIT_FOR_SEMAPHORE);
#endif
    sqlfwritep (sqlta->tatraceno, sqlta->tastr80, sqlta->tastr80l, &vferr);
#ifdef SQLODBC
      pa09Semaphore (PA09_SEMAPHORE_WRITETRACE, PA09_RELEASE_SEMAPHORE);
#endif
    sqlta->tastr80l = 0;
}

/*--------------------------------------------------------*/
void pr08TraceBufferCompleteAsHex (sqlratype * pSqlra, char * buf, tsp00_Int4 len, char cWriteTrace)
/* traces the buffer 'buf' as hex */
{
#define PU01_BUFMAXTRACELEN             100         /* so viele Zeichen eines Buffers werden herausgetract, auf 0 setzen, damit alles herausgetracet wird */
#define VPR08TRACE_MAXCHAR_IN_HEXLINE   60          /* maximale Anzahl von "Hex-Chars"/Zeile für den Hex-Buffer-Output */

    char        tmpBuf[VPR08TRACE_MAXCHAR_IN_HEXLINE],
                PosPrefix[20];
    tsp00_Int4    i               = 0,
                j               = 0,
                iCurrentPos     = 0;

    if (len<0)
    {
        /* dann nach erstem 0-Zeichen suchen */
        for (i=0; (i<10000 && buf[i] !=0);i++);      /* 10.000 ist eine willkuerliche Grenze */
        if (buf[i]==0)
        {
            len=i;
        }
        else
        {
            len=10000;
        }
    }

    if (PU01_BUFMAXTRACELEN==0)
    {
        for (i=0; i< (tsp00_Int4) (len/VPR08TRACE_MAXCHAR_IN_HEXLINE); i++)
        {
            iCurrentPos = i*VPR08TRACE_MAXCHAR_IN_HEXLINE;
            if (iCurrentPos <  1000) 	strcpy(PosPrefix, "Pos:");
            if (iCurrentPos >= 1000) 	strcpy(PosPrefix, "1**:");
            if (iCurrentPos >= 2000) 	strcpy(PosPrefix, "2**:");
            if (iCurrentPos >= 3000) 	strcpy(PosPrefix, "3**:");
            if (iCurrentPos >= 4000) 	strcpy(PosPrefix, "4**:");
            if (iCurrentPos >= 5000) 	strcpy(PosPrefix, "5**:");
            if (iCurrentPos >= 6000) 	strcpy(PosPrefix, "6**:");
            if (iCurrentPos >= 7000) 	strcpy(PosPrefix, "7**:");
            if (iCurrentPos >= 8000) 	strcpy(PosPrefix, "8**:");
            if (iCurrentPos >= 9000) 	strcpy(PosPrefix, "9**:");
            if (iCurrentPos >= 10000)	strcpy(PosPrefix, "X**:");

            memcpy ((void *) tmpBuf, (void *) (buf + iCurrentPos), (int) VPR08TRACE_MAXCHAR_IN_HEXLINE);   /* copy VPR08TRACE_MAXCHAR_IN_HEXLINE bytes from buf to a temporary buf */
            pr08TraceLineAsHex(pSqlra,tmpBuf,VPR08TRACE_MAXCHAR_IN_HEXLINE,PosPrefix,cWriteTrace);
        } 

        /* trace the very last characters */
        iCurrentPos = i*VPR08TRACE_MAXCHAR_IN_HEXLINE;
        if (iCurrentPos <  1000) 	strcpy(PosPrefix, "Pos:");
        if (iCurrentPos >= 1000) 	strcpy(PosPrefix, "1**:");
        if (iCurrentPos >= 2000) 	strcpy(PosPrefix, "2**:");
        if (iCurrentPos >= 3000) 	strcpy(PosPrefix, "3**:");
        if (iCurrentPos >= 4000) 	strcpy(PosPrefix, "4**:");
        if (iCurrentPos >= 5000) 	strcpy(PosPrefix, "5**:");
        if (iCurrentPos >= 6000) 	strcpy(PosPrefix, "6**:");
        if (iCurrentPos >= 7000) 	strcpy(PosPrefix, "7**:");
        if (iCurrentPos >= 8000) 	strcpy(PosPrefix, "8**:");
        if (iCurrentPos >= 9000) 	strcpy(PosPrefix, "9**:");
        if (iCurrentPos >= 10000)	strcpy(PosPrefix, "X**:");

        memcpy ((void *) tmpBuf, (void *) (buf + (i*VPR08TRACE_MAXCHAR_IN_HEXLINE)), (int) len - (i*VPR08TRACE_MAXCHAR_IN_HEXLINE));
        pr08TraceLineAsHex(pSqlra,tmpBuf,(int) len - (i*VPR08TRACE_MAXCHAR_IN_HEXLINE),PosPrefix,cWriteTrace);
    }
    else
    {
        if (len>PU01_BUFMAXTRACELEN)
        {
            len=PU01_BUFMAXTRACELEN;
        }
        pr08TraceLineAsHex(pSqlra,buf,(int) len,"Pos:",cWriteTrace);
    }
}

void pr08TraceLineAsHex(sqlratype * pSqlra, char * buf, tsp00_Int4 len, char *PosPrefix, char cWriteTrace)
{
    char    posline[1024],                          /* actually it doesn't make sense that those buffers shouldn't grow longer than LENGTH_OF_TRACEBUFFER chars */
            hexline[1024],                          /* 1024 should be big enough to avoid memory allocation probs */
            chrline[1024],
            chr,
            cchar[20],
            cpos[20],
            hchar[20],
            *pBuf           = ((char*)buf);

    int     iStrLen         = 0,
            i;

    strcpy(chrline,"chr:");
    strcpy(hexline,"hex:");
    strcpy(posline,PosPrefix);

    for ( i=1; i <= len; i++) 
    {
	     sprintf(cpos, "%3d", i%1000);
	     sprintf(hchar, "%3x", (unsigned char) *pBuf);
	     chr = (char) toascii (*pBuf++);
         if (!isprint(chr))
            chr = '.';
	     sprintf(cchar, "%3c", chr);
	     strcat(posline, cpos);
	     strcat(hexline, hchar);
	     strcat(chrline, cchar);
	}

    /* now flush the characters */

    /* posline */
    memset(pSqlra->rasqltap->tastr80,' ',LENGTH_OF_TRACEBUFFER);                                    /* empty trace output buffer to avoid annoying chars */
    iStrLen = (int) strlen(posline) > LENGTH_OF_TRACEBUFFER ? LENGTH_OF_TRACEBUFFER : (int)strlen(posline);    /* maximum of LENGTH_OF_TRACEBUFFER chars! */
    pSqlra->rasqltap->tastr80l =  iStrLen;                                                           /* set length of trace output buffer to LENGTH_OF_TRACEBUFFER */
    memcpy(&pSqlra->rasqltap->tastr80,&posline,iStrLen);                                            /* copy to trace structure */
    if (cWriteTrace==FLUSHTRACE)
        pr08vfwrtrc (pSqlra->rasqltap);


    /* hexline */
    memset(pSqlra->rasqltap->tastr80,' ',LENGTH_OF_TRACEBUFFER);
    iStrLen = (int) strlen(posline) > LENGTH_OF_TRACEBUFFER ? LENGTH_OF_TRACEBUFFER : (int)strlen(hexline);
    pSqlra->rasqltap->tastr80l = iStrLen;
    memcpy(&pSqlra->rasqltap->tastr80,&hexline,iStrLen);
    if (cWriteTrace==FLUSHTRACE)
        pr08vfwrtrc (pSqlra->rasqltap);

    /* chrline */
    memset(pSqlra->rasqltap->tastr80,' ',LENGTH_OF_TRACEBUFFER);
    iStrLen = (int) strlen(posline) > LENGTH_OF_TRACEBUFFER ? LENGTH_OF_TRACEBUFFER : (int)strlen(chrline);
    pSqlra->rasqltap->tastr80l = iStrLen;
    memcpy(&pSqlra->rasqltap->tastr80,&chrline,iStrLen);
    if (cWriteTrace==FLUSHTRACE)
        pr08vfwrtrc (pSqlra->rasqltap);
}

#endif
