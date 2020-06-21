/*!================================================================
 module:    vpr01Trace.h

 responsible: BurkhardD, MarcoP
   
 special area:   CPCDrv | Client | Trace SQL Commands 
 description: Verwaltung des Trace Descriptors
              Ausgabe in das Tracefile
              Funktionalitaeten fuer das Ein-/Aus-/Umschalten des 
              Precompiler-Trace zur Laufzeit

 
 irt trace status:  short int           momentane Traceart der IRT
                                        IRT beendet              = -1
                                        nicht initialisiert      =  0
                                        trace aus                =  1
                                        kurzer trace an          =  2
                                        langer trace an          =  3
                                        kurzer moduletrace an    =  4
                                        langer moduletrace an    =  5
                                        traceoptions aus profile =  6

 see:  
 




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




 ===================================================================*/

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "vpr01Trace.h"
#include "hsp77.h"
#include "hsp78_0.h"
#include "gpr03.h"
#include "gpr08.h"
#include "vpr08SharedMem.h"
#include "vpr100.h"
#include "vpr07c.h"
#include "vpr07Macro.h"
#include "vpr01SQL.h"
#include "vpr01Con.h"
#include "vpr03Part.h"
#include "vpr03Packet.h"
#include "hsp26.h"
#include <stdio.h>
/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/
 static tpr08_sharedMem ashared_mem;
 static tpr08_sharedMem *myshared_mem = &ashared_mem;
/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

void p05inttochr12 (int num, tsp00_C12 VAR_ARRAY_REF chr12);
extern tsp00_Int2 s30lnr (tsp00_C18, char val, int pos, int cnt);

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/
/*!
  Function:     pr01TraceIsTrace

  see also: 

  Description:  checks if trace is on 
  
  Arguments:
    sqlrap [in] sqlra area

  return value: BOOLEAN
    <UL>
        <LI>TRUE: trace is on
        <LI>FALSE: trace is off
    </UL>
 */

externC boolean pr01TraceIsTrace(sqlratype *sqlrap)
{
  sqltatype *tap = (sqlrap) ? sqlrap->rasqltap : NULL;
  if (!tap)
    return false;
  if (tap->tatrout != CPR_TRACE_LONG && tap->tatrout != CPR_TRACE_MODLONG)
    return false;
  return true;
}
/*!
  Function:     pr01TracePrintf

  see also: 

  Description:  Prints a given string to the tracefile 
  
  Arguments:
    sqlrap [in|out] sqlra area
    szFmt  [in] format string (sprintf like)
    ...
    
  return value: void
 */
externC void pr01TracePrintf(sqlratype * sqlrap, const char* szFmt, ...)
{
  va_list ap;
  sqltatype *tap = sqlrap->rasqltap;
  tsp00_Int2 *ll = &tap->tastr80l;
  va_start (ap, szFmt);
  *ll = sp77vsprintf(tap->tastr80, sizeof(tap->tastr80)-1, szFmt, ap);
  if (*ll > 0) {
    p08vfwritetrace (sqlrap);
  }
}
/*!
  Function:     pr01CloseTraceFile

  see also: 

  Description:  Close the tracefile 
  
  Arguments:
    sqlca [in|out] sqlca area
    sqlxa [in|out] sqlxa area

  return value: void
 */
void pr01CloseTraceFile (sqlcatype *my_sqlca, sqlxatype *my_sqlxa)
{
   struct SQLERROR sqlemp;   
   /*close tracefile*/
   p03tvfclosetrace (my_sqlca->sqlrap, &sqlemp);
   if (sqlemp.eprerr == cpr_p_ok)
   {
      /*change trace to not initialized*/
      my_sqlca->sqlrap->rasqltap->tatracety = 0;
      if(myshared_mem->getPart(myshared_mem)!=0)
      {
        myshared_mem->part_set_irt_tracestatus(myshared_mem->part,0);
      }
   }
   else
   {
     /*error during close tracefile -> write message to trace*/
     if (my_sqlca->sqlrap->rasqltap->tatraceno != 0)
        pr01TracePrintf(my_sqlca->sqlrap, "ERROR CLOSING TRACEFILE: %s",sqlemp.etext);
   }    
}

/*!
  Function:     pr01OpenTraceFile

  see also: 

  Description:  Open the tracefile 
  
  Arguments:
    sqlca [in|out] sqlca area
    sqlxa [in|out] sqlxa area

  return value: tsp00_Int2
    <UL>
        <LI>1: Processing succeeds
        <LI>0: Error in occurs
    </UL>
 */
tsp00_Int2 pr01OpenTraceFile (sqlcatype *my_sqlca, sqlxatype *my_sqlxa)
{
/* tsp00_VFileOpCodes openType;
   struct SQLERROR sqlemp;
   
   if (myshared_mem->getopen_for_append(myshared_mem))
   {
     openType = SP5VF_APPEND;
   }  
   else 
   {
     openType = SP5VF_WRITE;
     myshared_mem->setopen_for_append(myshared_mem,TRUE);
   }*/ 
      
   /*open tracefile*/
   if ((*my_sqlxa).sqlkap){
     p01xtracefilecheck  (my_sqlca, my_sqlxa);
   }
   return 1;}

/*!
  Function:     pr01traceParameterRefresh

  see also: 

  Description: ON OFF and Switch between different trace types 
               of interface runtime durring running process

 
 irt trace status:  short int           momentane Traceart der IRT
                                        IRT beendet              = -1
                                        nicht initialisiert      =  0
                                        trace aus                =  1
                                        kurzer trace an          =  2
                                        langer trace an          =  3
                                        kurzer moduletrace an    =  4
                                        langer moduletrace an    =  5
                                        traceoptions aus profile =  6

  Arguments:
    sqlca [in|out] sqlca area
    sqlxa [in|out] sqlxa area

  return value: void
*/
externC void pr01traceParameterRefresh(sqlcatype *my_sqlca, sqlxatype *my_sqlxa)
{ 
   /*no dynamic trace on/off switch for ODBC because shared memory errors*/
   if (my_sqlxa->xalang == CPR_LA_CALL)
     return;    
/* if shared memory not initialize -> initialize */
   if ( ! myshared_mem   
     || !myshared_mem->header )
   {
 		if(! tpr08_sharedMem__init_sharedMem(myshared_mem, SHM_NOTERASE, NULL))
   		{
   		    /* error alloc shared mem               */
   		    /* only if tracefile open (PTS 1106543) */
   		    if (my_sqlca->sqlrap->rasqltap->tatraceno != 0)
   		    { 
                my_sqlca->sqlemp->elzu = sp1ce_notok;
                memset(my_sqlca->sqlemp->etext, ' ', sizeof(my_sqlca->sqlemp->elzu));
                if (strlen(myshared_mem->getErrortxt(myshared_mem)) > 0)
                    memcpy((char*)my_sqlca->sqlemp->etext, myshared_mem->getErrortxt(myshared_mem) , strlen(myshared_mem->getErrortxt(myshared_mem)));
 
        	    p08runtimeerror (my_sqlca,my_sqlxa,cpr_trace_error); 
        	}
            return;
    	} 
    	/*initialize lastTracetyp with current Tracetyp*/
    	myshared_mem->setlastTracetyp(myshared_mem, my_sqlca->sqlrap->rasqltap->tatracety);
   }
   
   /* if traceswitchcount changed */
   if (myshared_mem->traceswitchcount_changed(myshared_mem)) 
   {  /* if trace_all switched on */
      if (myshared_mem->header_get_trace_all(myshared_mem->header) == 'y')
      {
         tsp00_Int2 erg = 1;
         /* save current trace typ */
      	 myshared_mem->setlastTracetyp(myshared_mem, my_sqlca->sqlrap->rasqltap->tatracety);
     	 if ( my_sqlca->sqlrap->rasqltap->tatracety == 0 )
         /*open tracefile if nessesary*/
         { 
            /* long trace on */
      	    my_sqlca->sqlrap->rasqltap->tatracety = 3;
    	    erg = pr01OpenTraceFile (my_sqlca, my_sqlxa);
         }
         else
         {
           /* long trace on */
      	   my_sqlca->sqlrap->rasqltap->tatracety = 3;
    	 }
      }
      else 
      {  
         if (myshared_mem->getPart(myshared_mem)==0)
      	 /* if adress of part unknown -> find part */
         {
           if ( myshared_mem->findPart(myshared_mem,myshared_mem->getProcessid(myshared_mem)) )
           {  
           		myshared_mem->part_set_version(myshared_mem->part, atoi(pr07RelNr())); 
           } 
           else
           {       	   
             /*set to last tracetype because maybe it was switch from traceall == 'y' to 'n'*/
         	 if (    myshared_mem->getlastTracetyp(myshared_mem) == 0
         	      && my_sqlca->sqlrap->rasqltap->tatracety != 0)
         	 /*switch from on to off -> close the tracefile*/
         	 {
               pr01CloseTraceFile (my_sqlca, my_sqlxa);
         	 }    
             my_sqlca->sqlrap->rasqltap->tatracety = myshared_mem->getlastTracetyp(myshared_mem);        	
             return;
           }
         }
          
         if (   my_sqlca->sqlrap->rasqltap->tatrout 
             != myshared_mem->part_get_tracetyp(myshared_mem->part))
         /*someone has changed for this process*/
         {  
         	if (myshared_mem->part_get_tracetyp(myshared_mem->part)== -2)
         	/* only info about trace status */
         	{
         		myshared_mem->part_set_irt_tracestatus(myshared_mem->part, my_sqlca->sqlrap->rasqltap->tatrout);
         	}
         	else
         	/* change trace */
         	{	
         	    tsp00_Int2 erg = 1;
         	    if (   myshared_mem->part_get_irt_tracestatus(myshared_mem->part) == 0
         	        && myshared_mem->part_get_tracetyp(myshared_mem->part) > 1)
         	    /*switch from off to on -> open tracefile*/
         	    {   
         	        /* change trace */
                    my_sqlca->sqlrap->rasqltap->tatracety = myshared_mem->part_get_tracetyp(myshared_mem->part);
                    myshared_mem->part_set_irt_tracestatus(myshared_mem->part, myshared_mem->part_get_tracetyp(myshared_mem->part));
                    erg = pr01OpenTraceFile (my_sqlca, my_sqlxa);
                    my_sqlca->sqlrap->rasqltap->tatrout = my_sqlca->sqlrap->rasqltap->tatracety;
           	    }    
         	    else if (   myshared_mem->part_get_irt_tracestatus(myshared_mem->part) > 1
         	             && myshared_mem->part_get_tracetyp(myshared_mem->part) == 0)
         	    /*switch from on to off -> close the tracefile*/
         	    {
                    my_sqlca->sqlrap->rasqltap->tatracety = myshared_mem->part_get_tracetyp(myshared_mem->part);
                    myshared_mem->part_set_irt_tracestatus(myshared_mem->part, myshared_mem->part_get_tracetyp(myshared_mem->part));
                    pr01CloseTraceFile (my_sqlca, my_sqlxa);
                    my_sqlca->sqlrap->rasqltap->tatrout = my_sqlca->sqlrap->rasqltap->tatracety;
         	    }    
         	    else
         	    {
             	 	/* switch from one trace type to another trace type*/
                	my_sqlca->sqlrap->rasqltap->tatracety = myshared_mem->part_get_tracetyp(myshared_mem->part);
     				myshared_mem->part_set_irt_tracestatus(myshared_mem->part, myshared_mem->part_get_tracetyp(myshared_mem->part));
                }
            	
            	/* comment to tracefile */ 
            	if (my_sqlca->sqlrap->rasqltap->tatraceno !=	0)
            	{  
            		pr01TracePrintf(my_sqlca->sqlrap, "");
            		pr01TracePrintf(my_sqlca->sqlrap, "TRACE TYPE HAS BEEN CHANGED TO: %s",myshared_mem->tracetyp_tochar(myshared_mem, myshared_mem->part_get_tracetyp(myshared_mem->part)));
         		}
         	}
         	/*update traceswitch*/
            myshared_mem->part_set_lasttraceswitch(myshared_mem->part, myshared_mem->header_get_traceswitchcount(myshared_mem->header));
         }
      }	  
   }
   return;
}   


/*!
  Function:     pr01WriteToTrace

  see also: 
  
  Description:  Puts a string to tracefile.

  Arguments:
    sqlrap  [in]  pointer to sqlra 
    pString [out] pointer to a tpr05String containing command for tracing

  return value: void
 */
void pr01WriteToTrace (sqltatype *sqltap, char *pString, tsp00_Uint4 cBLenString, struct SQLERROR *sqlemp)
{
    tsp05_RteFileError   vferr;

    M90TRACE (M90_TR_STRING, "pr01WriteToTrace", "entry");
    vferr.sp5fe_result = vf_ok;
    sqlfwritep (sqltap->tatraceno, pString, cBLenString, &vferr);
    if (vferr.sp5fe_result != vf_ok)
        {
        sqlemp->elzu = sp1ce_notok;
        memcpy (sqlemp->etext, vferr.sp5fe_text, 40);
        sqlemp->eprerr = cpr_tracefile_write_error;
        p03cseterror (sqlemp, sqlemp->eprerr);
        sqltap->tatrout = CPR_TRACE_OFF;
        sqltap->tatracety = CPR_TRACE_OFF;
        sqltap->tatraceno = 0;
        sqlresult (CPR_RESULT_TRACE_ERROR);
        }
}     

/*!
  Function:     pr01TraceCmd

  see also: 
  
  Description:  puts a command to tracefile. If the command is longer
                than 80 characters a suitable position for linefeed
                will be searched 

  NOTE that sqlrap->rasqltap->tastr80 can contain data, which first must 
  write to trace.    
  
  Arguments:
    sqlrap  [in]  pointer to sqlra 
    pString [out] pointer to a tpr05String containing command for tracing

  return value: void
 */
externC void pr01TraceCmd(sqlratype *sqlrap, tpr05_String *pString)
{
  sqltatype *tap = sqlrap->rasqltap;
  if (tap->tatrout != CPR_TRACE_OFF) {
  /*only if trace has been switched on*/
      if (pString->rawString != NULL) {
        tpr05_String *TraceString = pr05IfCom_String_NewDynString(4*(pString->cbLen + tap->tastr80l), sp77encodingUTF8);
        tsp78ConversionResult ConvRes;
        tsp00_Uint4 destBytesWritten, srcBytesParsed;

	tsp00_Uint4  currentEndPos = 0;     /*current upper bound */
	tsp00_Uint4  currentBegPos = 0;     /*current lower bound*/
	tsp00_Uint4 currentBufLen = 0;     /*current length of buffer*/
	tsp00_Int2   Delimiter_Found;            
	struct SQLERROR errmsg;
	
        /*copy tap->tastr80 into TraceString, asuming data in tap->tastr80 are encoded in UTF8*/
        if (tap->tastr80l > 0) {
          memcpy (TraceString->rawString, tap->tastr80, tap->tastr80l);
          TraceString->cbLen = tap->tastr80l; 
	  tap->tastr80l = 0;
        }

        /*convert String into UTF8 to TraceString*/
        ConvRes = sp78convertString( TraceString->encodingType,
                                     &TraceString->rawString[TraceString->cbLen],
                                     TraceString->cbMaxLen,
                                     &destBytesWritten,
                                     FALSE,
                                     pString->encodingType,
                                     pString->rawString,
                                     pString->cbLen,
                                     &srcBytesParsed);
        if (ConvRes != sp78_Ok ) {
          /*error convert to UTF8*/
          pr01TracePrintf(sqlrap, "Conversion of command into UTF8 for trace output failed because %s", 
                                   sp78errString (ConvRes));
          return;
        }
        /*set current used length of TraceString*/
        TraceString->cbLen += destBytesWritten;
 
	while ( currentBegPos < TraceString->cbLen ) {
	  if ((TraceString->cbLen - currentBegPos ) > (MXPR_C80)) {
	    /*if command longer than 80 bytes searching delimiter*/
	    currentEndPos = (tsp00_Uint4)(currentBegPos + MXPR_C80 - 1);
	    Delimiter_Found = 0;
	    while ((currentEndPos > currentBegPos) && (!Delimiter_Found)) {
	      if ((TraceString->rawString[currentEndPos] == ')') ||
		  (TraceString->rawString[currentEndPos] == '.') ||
		  (TraceString->rawString[currentEndPos] == ' ') ||
		  (TraceString->rawString[currentEndPos] == ',') ||  
		  (TraceString->rawString[currentEndPos] == '>') ||
		  (TraceString->rawString[currentEndPos] == '=') ||
		  (TraceString->rawString[currentEndPos] == '*')) {
		Delimiter_Found = 1;
	      }
	      else
		currentEndPos --;
	    }
	  }
	  else {
	    currentEndPos = TraceString->cbLen-1;
	  }
	  currentBufLen = (tsp00_Uint4) (currentEndPos - currentBegPos + 1);
	  if (currentBufLen > 0) {
	    pr01WriteToTrace (tap, &TraceString->rawString[currentBegPos], currentBufLen, &errmsg);
          }            	 
	  currentBegPos += currentBufLen;
	}/*end while*/
        pr05IfCom_String_DeleteString(TraceString);	
      }/*end pString->rawString*/
  }  
}

/*!
  Function:     pr01TraceRuntimeError

  see also: 

  Description:  Writes a program error to the sqlca area.
                If trace output is activated, it is logged there.
                In addition, the command is written to the trace file.

  Arguments:
    sqlca [in|out] sqlca area
    sqlxa [in|out] sqlxa area
    error [in] error message 

  return value: void
 */
void pr01TraceRuntimeError (sqlcatype *sqlca, sqlxatype *sqlxa,  tpr_runtime_errors_epr00 error)
{
  sqlgaentry *gae = (sqlxa->xaSQLDesc && sqlxa->xaSQLDesc->ConDesc) ? sqlxa->xaSQLDesc->ConDesc->ga : NULL; 
  p01xtracefilecheck(sqlca, sqlxa);
  p03cmdtrace (sqlca->sqlrap, gae, 1, cpr_com_empty, NULL);
  p08runtimeerror (sqlca, sqlxa, error);
}

/*!
  Function:     pr01TraceSQLResultName

  see also: 

  Description:  Writes a resultname to the trace.

  Arguments:
    sqlca [in|out] sqlca area
    sqlxa [in|out] sqlxa area
    error [in] error message 

  return value: void
 */
void pr01TraceSQLResultName (sqlratype *sqlrap, tsp00_KnlIdentifier * sqlresn)
{
  sqltatype *tap = sqlrap->rasqltap;
  tsp00_Int2 *ll = &tap->tastr80l;
  const tsp77encoding *encoding = sp77encodingUTF8;
  tsp00_Int4 padLength = encoding->countPadChars( sqlresn, sizeof(*sqlresn), ' ');
  tsp00_Int4 cbNameLen = sizeof(*sqlresn)-padLength;
  *ll = sp77sprintfUnicode(sp77encodingUTF8, tap->tastr80, sizeof(tap->tastr80), "SQLRESULTNAME  : %=.*S", encoding, cbNameLen, sqlresn);
  p08vfwritetrace (sqlrap);
}

void pr01TraceErrorPos (sqlcatype *sqlca, struct tpr01_ConDesc *ConDesc)
{
  sqlratype *sqlrap = sqlca->sqlrap;
  sqltatype *tap = sqlrap->rasqltap;
  tsp00_Int2 *ll = &tap->tastr80l;
  tsp00_C12 chr12;
  
  tsp1_part *partPtr = pr03PartFind(sqlrap, sp1pk_command);
  if (!partPtr) {
    /* try to find the part in the request segment */
    tsp1_segment *segp = &((tsp1_packet*)ConDesc->ga->gareqptr)->variant.C_2.sp1_segm_F;
    s26find_part (segp, sp1pk_command, &partPtr);
  }
  p05inttochr12(sqlca->sqlerrd[5], chr12);
  *ll = sp77sprintfUnicode(sp77encodingUTF8, tap->tastr80, sizeof(tap->tastr80), "SQLERRD(INDEX_6) :%.12s", chr12);
  if (partPtr) {    
    char *rawPtr = pr03PartGetRawPtr(partPtr);
    tsp00_Int4 ErrorPos; /* byteposition of the error in part */
    tsp00_Int4 BegPos; /* startposition of printing the SQL-Cmd */ 
    tsp00_Int4 BegLen; /* number of characters to be written up to the ErrorPos */ 
    tsp00_Int4 EndPos; /* endposition up to printing the SQL-Cmd */
    tsp00_Int4 EndLen; /* number of characters to be written after the ErrorPos */ 
    tsp00_Int4 cbPartLen = pr03PartGetPartLength(partPtr)
;
    /* Since the kernel modified the requestbuffer for local databases.
    But in Version 7.3.05 this bug was fixed and the requestbuffer have
    the same encoding like the packet*/
    tpr05_StringEncoding PacketEncoding = pr03PacketGetEncoding(ConDesc->ga->gareqptr);
    
    /* number of bytes to trace before/after the error */
    tsp00_Int2 nBytes = (PacketEncoding == sp77encodingAscii) ? 10 : 20;
    
    /* calculate absolute position in part */
    ErrorPos = (sqlca->sqlerrd[5]-1);
    ErrorPos = (PacketEncoding == sp77encodingAscii) ? ErrorPos : ErrorPos*2;
    
    /* start trace nBytes before Error occured or at begining */
    BegPos = (ErrorPos > nBytes) ? ErrorPos - nBytes : 0;
    BegLen = ErrorPos-BegPos;
    BegLen = (PacketEncoding == sp77encodingAscii) ? BegLen : BegLen / 2;

    /* stop trace nBytes after Error occured or at the end */
    EndPos = ErrorPos;
    EndLen = (EndPos+nBytes > cbPartLen) ? cbPartLen-EndPos : nBytes;
    EndLen = (PacketEncoding == sp77encodingAscii) ? EndLen : EndLen / 2;
    
    /* trace first part */
    *ll += sp77sprintfUnicode(sp77encodingUTF8, &tap->tastr80[*ll], sizeof(tap->tastr80)-*ll, "ERROR NEAR ^ : %=.*S", PacketEncoding, BegLen, &rawPtr[BegPos]);
    /* mark the position */
    *ll += sp77sprintfUnicode(sp77encodingUTF8, &tap->tastr80[*ll], sizeof(tap->tastr80)-*ll, "^");
    /* trace last part */
    *ll += sp77sprintfUnicode(sp77encodingUTF8, &tap->tastr80[*ll], sizeof(tap->tastr80)-*ll, "%=.*S", PacketEncoding, EndLen, &rawPtr[EndPos]);
  }
  p08vfwritetrace (sqlca->sqlrap);  
}

/*!
  Function:     pr01TraceSession

  see also: 

  Description:  Builds a tracestring which contains the Sessionnumber or a 
                databasename of the current session in the tracebuffer 
                ta80str and updates the length of tastr80.                

  Arguments:
    gae [in|out] sqlca area
    sqlra [in|out] sqlxa area

  return value: void

  comments:     This function doesn't flush the tracebuffer to a file.
             
  change history : 
        
        D025086 03-13-2001 Initial version.

 */
void pr01TraceSession(sqlgaentry *gae, sqlratype *sqlra)
{
  sqltatype *tap = sqlra->rasqltap;
  short *t80l = &tap->tastr80l;
  *t80l = 0;
  if (strncmp((char*)gae->gaatdbname, bsp_c64, 
              sizeof(gae->gaatdbname)) != 0) {
    tsp00_Int2  len;
    *t80l = 0;
    len = s30lnr (gae->gaatdbname, bsp_c1, 1, sizeof(gae->gaatdbname));
    len = PR07MIN(len, sizeof(tap->tastr80));
    memcpy (&tap->tastr80[*t80l], &gae->gaatdbname, len);
    *t80l += len;
    memcpy (&tap->tastr80[*t80l], ": ", 2);      
    *t80l += 2;
  }
  else 
    if (sqlra->raactsession >= CPR_SE_SECOND)	{
      tap->tastr80[0] = 'S';
      tap->tastr80[1] = (char) (sqlra->raactsession + (short) '0');
      tap->tastr80[2] = ':';
      tap->tastr80[3] = ' ';
      *t80l = 4;
    }
}
