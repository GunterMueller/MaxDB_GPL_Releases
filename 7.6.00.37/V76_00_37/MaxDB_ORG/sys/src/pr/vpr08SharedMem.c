/*!================================================================
 module:    vpr08SharedMem.c

 responsible:  MarcoP

 special area: CPCDrv | Client | Trace SQL Commands
 description:  access to the shared memory for Interface Runtime (IRT)
               and the tool pctrace (dynamic trace switch)


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

#include "vpr08SharedMem.h"
#include "gpr03.h"
#include "heo01.h"
#include "heo06.h"
#include "geo900.h"
#include "gip00.h"
#include "SAPDB/RunTime/RTE_IniFileHandling.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/
void               tpr08_header__set_traceswitchcount(tpr08_header *this);
unsigned short int tpr08_header__get_traceswitchcount(tpr08_header *this);
void               tpr08_header__set_trace_all_on (tpr08_header *this);
void               tpr08_header__set_trace_all_off(tpr08_header *this);
char               tpr08_header__get_trace_all    (tpr08_header *this);
void               tpr08_header__set_sharedmemsize(tpr08_header *this, unsigned int asize);
unsigned int       tpr08_header__get_sharedmemsize(tpr08_header *this);


void               tpr08_part__set_pid            (tpr08_part *this, int apid);
int                tpr08_part__get_pid            (tpr08_part *this);
void               tpr08_part__set_version        (tpr08_part *this, unsigned int aversion);
unsigned int       tpr08_part__get_version        (tpr08_part *this);
void               tpr08_part__set_irt_tracestatus(tpr08_part *this, short int astatus);
short int          tpr08_part__get_irt_tracestatus(tpr08_part *this);
void               tpr08_part__set_tracetyp       (tpr08_part *this, short int astatus);
short int          tpr08_part__get_tracetyp       (tpr08_part *this);
void               tpr08_part__set_profile        (tpr08_part *this, const char * aname);
char *             tpr08_part__get_profile        (tpr08_part *this);
void               tpr08_part__set_lasttraceswitch(tpr08_part *this, unsigned short int atraceswitch);
unsigned short int tpr08_part__get_lasttraceswitch(tpr08_part *this);


void               tpr08_sharedMem__delete_sharedMem        (tpr08_sharedMem *this);
tsp00_Int2         tpr08_sharedMem__init_sharedMem          (tpr08_sharedMem *this, ShMInitTyp aInitTyp, char* ShMpath);
tsp00_Int2         tpr08_sharedMem__newPart                 (tpr08_sharedMem *this, int apid);
tsp00_Int2         tpr08_sharedMem__findPart                (tpr08_sharedMem *this, int apid);
tsp00_Int2         tpr08_sharedMem__nextPart                (tpr08_sharedMem *this);
tpr08_header *     tpr08_sharedMem__getHeader               (tpr08_sharedMem *this);
tpr08_part *       tpr08_sharedMem__getPart                 (tpr08_sharedMem *this);
void               tpr08_sharedMem__setlastTracetyp         (tpr08_sharedMem *this, short int aTracetyp);
short int          tpr08_sharedMem__getlastTracetyp         (tpr08_sharedMem *this);
void               tpr08_sharedMem__setopen_for_append      (struct tpr08_sharedMem *this, tsp00_Int2 yes);
tsp00_Int2         tpr08_sharedMem__getopen_for_append      (struct tpr08_sharedMem *this);
int                tpr08_sharedMem__getProcessid            (tpr08_sharedMem *this);
char*              tpr08_sharedMem__getPath                 (tpr08_sharedMem *this);
char*              tpr08_sharedMem__getErrortxt             (tpr08_sharedMem *this);
int                tpr08_sharedMem__getErrornr              (tpr08_sharedMem *this);
tsp00_Int2         tpr08_sharedMem__traceswitchcount_changed(tpr08_sharedMem *this);
tsp00_Int2         tpr08_sharedMem__firstPart               (tpr08_sharedMem *this);
const char*        tpr08_sharedMem__tracetyp_tochar         (tpr08_sharedMem *this, short int atracetyp);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/*---------------------------------------------------------------*/
/* Methoden der header Klasse
    class tpr08_header {
        unsigned short int traceswitchcount;
        char               trace_all;
        unsigned short int partsize;
        unsigned int       sharedmemsize;  */



void tpr08_header__set_traceswitchcount(tpr08_header *this)
{
    if (this->traceswitchcount > 15000 )
       this->traceswitchcount = 42;
    else
	  this->traceswitchcount++;
}

unsigned short int tpr08_header__get_traceswitchcount(tpr08_header *this)
{
	return this->traceswitchcount;
}

void tpr08_header__set_trace_all_on(tpr08_header *this)
{
	this->trace_all ='y';
}

void tpr08_header__set_trace_all_off(tpr08_header *this)
{
	this->trace_all ='n';
}

char tpr08_header__get_trace_all(tpr08_header *this)
{
	return this->trace_all;
}

void tpr08_header__set_sharedmemsize(tpr08_header *this, unsigned int asize)
{
	this->sharedmemsize = asize;
}

unsigned int tpr08_header__get_sharedmemsize(tpr08_header *this)
{
	return this->sharedmemsize;
}


/*---------------------------------------------------------------*/
/* Methoden der part Klasse
    class tpr08_part {
        int                pid;
        unsigned int       version;
        short int          trace_status;
        unsigned short int lasttraceswitch; */


void tpr08_part__set_pid(tpr08_part *this, int apid)
{
	this->pid =apid;
}

int  tpr08_part__get_pid(tpr08_part *this)
{
	return this->pid;
}

void tpr08_part__set_version(tpr08_part *this, unsigned int aversion)
{
	this->version =aversion;
}

unsigned int tpr08_part__get_version(tpr08_part *this)
{
	return this->version;
}

void tpr08_part__set_irt_tracestatus(tpr08_part *this, short int astatus)
{
	this->irt_tracestatus =astatus;
}

short int tpr08_part__get_irt_tracestatus(tpr08_part *this)
{
	return this->irt_tracestatus;
}

void tpr08_part__set_tracetyp(tpr08_part *this, short int astatus)
{
	this->tracetyp =astatus;
}

short int tpr08_part__get_tracetyp(tpr08_part *this)
{
	return this->tracetyp;
}

void tpr08_part__set_profile(tpr08_part *this, const char * aname)
{
	strcpy(this->profile,aname);
}

char *  tpr08_part__get_profile(tpr08_part *this)
{
	return this->profile;
}

void tpr08_part__set_lasttraceswitch(tpr08_part *this, unsigned short int atraceswitch)
{
	this->lasttraceswitch = atraceswitch;
}

unsigned short int tpr08_part__get_lasttraceswitch(tpr08_part *this)
{
	return this->lasttraceswitch;
}


/*---------------------------------------------------------------*/
/* Methoden der tpr08_sharedMem Klasse */
/*
*/

/* Destruktor */
void tpr08_sharedMem__delete_sharedMem(tpr08_sharedMem *this)
    {
     if (this->header != NULL)
     {
     	/*Shared Memory freigeben*/
     	sqlFreeSharedMem((void*)this->header, this->header->sharedmemsize);
     	/*printf ("Shared Memory was released"); */
     }
    }

/* Initialisierung */
tsp00_Int2 tpr08_sharedMem__init_sharedMem(tpr08_sharedMem *this, ShMInitTyp aInitTyp, char* ShMPath)
    {
    	tsp01_RteError      pfad_error;
        tsp00_Pathc         pfad;

        /* linking of member methods */
        this->delete_sharedMem = tpr08_sharedMem__delete_sharedMem;
        this->getHeader        = tpr08_sharedMem__getHeader;
        this->getPart          = tpr08_sharedMem__getPart;
        this->setlastTracetyp  = tpr08_sharedMem__setlastTracetyp;
        this->getlastTracetyp  = tpr08_sharedMem__getlastTracetyp;
        this->getProcessid     = tpr08_sharedMem__getProcessid;
        this->setopen_for_append  = tpr08_sharedMem__setopen_for_append;
        this->getopen_for_append  = tpr08_sharedMem__getopen_for_append;
        this->getPath          = tpr08_sharedMem__getPath;
        this->getErrortxt      = tpr08_sharedMem__getErrortxt;
        this->getErrornr       = tpr08_sharedMem__getErrornr;
        this->init_sharedMem   = tpr08_sharedMem__init_sharedMem;
        this->traceswitchcount_changed = tpr08_sharedMem__traceswitchcount_changed;
        this->newPart          = tpr08_sharedMem__newPart;
        this->findPart         = tpr08_sharedMem__findPart;
        this->firstPart        = tpr08_sharedMem__firstPart;
        this->nextPart         = tpr08_sharedMem__nextPart;
        this->tracetyp_tochar  = tpr08_sharedMem__tracetyp_tochar;

        this->header_set_traceswitchcount = tpr08_header__set_traceswitchcount;
        this->header_get_traceswitchcount = tpr08_header__get_traceswitchcount;
        this->header_set_trace_all_on     = tpr08_header__set_trace_all_on;
        this->header_set_trace_all_off    = tpr08_header__set_trace_all_off;
        this->header_get_trace_all        = tpr08_header__get_trace_all;
        this->header_set_sharedmemsize    = tpr08_header__set_sharedmemsize;
        this->header_get_sharedmemsize    = tpr08_header__get_sharedmemsize;

        this->part_set_pid              = tpr08_part__set_pid;
        this->part_get_pid              = tpr08_part__get_pid;
        this->part_set_version          = tpr08_part__set_version;
        this->part_get_version          = tpr08_part__get_version;
        this->part_set_irt_tracestatus  = tpr08_part__set_irt_tracestatus;
        this->part_get_irt_tracestatus  = tpr08_part__get_irt_tracestatus;
        this->part_set_tracetyp         = tpr08_part__set_tracetyp;
        this->part_get_tracetyp         = tpr08_part__get_tracetyp;
        this->part_set_profile          = tpr08_part__set_profile;
        this->part_get_profile          = tpr08_part__get_profile;
        this->part_set_lasttraceswitch  = tpr08_part__set_lasttraceswitch;
        this->part_get_lasttraceswitch  = tpr08_part__get_lasttraceswitch;

        /*Pfad für die Synchronisationsdatei des Shared Memory setzen*/
        if(ShMPath)
        {
            strcpy(this->path,ShMPath);
            strcat(this->path, PATHSEP_IP00);
            strcat(this->path, "irtrace.shm");
        }
        else
        {
          if ( RTE_GetUserSpecificConfigPath ( pfad, TERM_WITH_DELIMITER_EO01, &pfad_error ))
          {
                strcpy(this->path,pfad);
                strcat(this->path, "irtrace.shm");
          }
          else
          {
                this->errornr   = -803;
                sprintf(this->errortxt, "%d ",pfad_error.RteErrCode);
                strcat (this->errortxt,pfad_error.RteErrText);
            	return 0;
          }
        }
        /* The following part must be removed because problems on linux*/

        /* if the shared memory is not use, try delete shared memory file
           minimize size, but only if trace all is not set.
        if (aInitTyp == SHM_ERASE)
        {
        	sqlferasec (this->getPath(this),&file_error);
        }*/

     	/* Header des Shared Memory auslesen */
     	if ((this->header = (tpr08_header*) sqlAllocSharedMem(this->path,sizeof(tpr08_header))) == 0)
     	{
        	this->errornr=-803;
        	strcpy(this->errortxt,"sqlAllocSharedMem failed");
        	return 0;
        }

     	/*Falls Shared Memory neu angelegt wurde -> initialisieren*/
     	if (this->header->sharedmemsize <= sizeof(tpr08_header))
     	{
     		this->header->sharedmemsize = sizeof(tpr08_header);
     	}
     	this->current_sharedmemsize = this->header->sharedmemsize;
     	sqlFreeSharedMem((tpr08_header*)this->header, sizeof(tpr08_header));

     	/*gesamten Shared Memory allokieren */
     	if((this->header = (tpr08_header*) sqlAllocSharedMem(this->path,this->current_sharedmemsize)) ==0)
     	{
		    this->errornr=-803;
        	strcpy(this->errortxt,"sqlAllocSharedMem failed");
        	return 0;
        }

        /*delete inactive parts from ShM*/
        if (this->firstPart(this)) {
        	do {
        	    RTEProc_Handle  myHandle;
            	    bool            erg;

    		    erg = RTEProc_Open (this->getPart(this)->pid, &myHandle);

            	    if(RTEProc_GetState( myHandle ) == Died) {
            	      this->part_set_irt_tracestatus (this->part, -1);
            	    }
                    erg = RTEProc_Close   ( myHandle );
        	} while (this->nextPart(this));
	}

     	/*Objekt initialisieren */
     	this->part   = NULL;
     	this->current_traceswitchcount = 0;
     	this->open_for_append = 0;
     	this->lastTracetyp = 0;
     	sqlgetpid (&(this->processid));
        return 1;

  } /* init_sharedMem() */


tsp00_Int2 tpr08_sharedMem__newPart(tpr08_sharedMem *this, int apid)
    {
       unsigned int i, anzahl_parts;

       /* Kontrolle, ob der Pid schon ein Part zugeordnet ist*/
       if (this->findPart(this,apid))
        {
       	  return 1;
       	}

       /* unbenutzten Part suchen (trace_status	== -1) */
       anzahl_parts = (this->current_sharedmemsize - sizeof(tpr08_header))/sizeof(tpr08_part);
       for( i= 0;i<anzahl_parts;i++)
       {   this->part   = (tpr08_part*) ( (this->header )+1);
           this->part   = (tpr08_part*) ( (this->part   )+i);
    	   if (this->part_get_irt_tracestatus(this->part)== -1 || this->part_get_pid(this->part)== 0)
    	    {
               /* initialisieren des Part */
               this->part_set_version (this->part, 0);
               this->part_set_irt_tracestatus (this->part, 0);
               this->part_set_tracetyp(this->part, 0);
               this->part_set_lasttraceswitch (this->part, 0);
     	       this->part_set_profile(this->part, "        ");
               this->part_set_pid (this->part, apid);
               return 1;
            }
        }

       /* da zur Pid noch kein Part existiert und kein unbenutzter Part
          gefunden wurde, wird neuer Part erzeugt und am Ende des Shared
          Memory angefuegt */

       /* Shared Memory um einen Part vergroeßern und neu allokieren */
       sqlFreeSharedMem((tpr08_header*)this->header, this->header_get_sharedmemsize(this->header));
       this->current_sharedmemsize += sizeof(tpr08_part);
       if ((this->header = (tpr08_header*) sqlAllocSharedMem(this->path,this->current_sharedmemsize))==0 )
     	{
			this->errornr  = -803;
			strcpy (this->errortxt, "sqlAllocSharedMem failed");
        	return 0;
        }

       this->header_set_sharedmemsize(this->header,this->current_sharedmemsize);

       /* neuen Part als aktuellen Part setzen */
       this->part   = (tpr08_part*) ( (this->header ) + 1);
       this->part   = (tpr08_part*) ( (this->part   ) + anzahl_parts);

       /* initialisieren des Part */
       this->part_set_version (this->part, 0);
       this->part_set_irt_tracestatus (this->part,0);
       this->part_set_tracetyp(this->part,0);
       this->part_set_lasttraceswitch (this->part,0);
       this->part_set_pid (this->part,apid);
       return 1;
    }

tsp00_Int2 tpr08_sharedMem__findPart(tpr08_sharedMem *this, int apid)
    {
    	unsigned int i, old_sm_size, anzahl_parts;
    	/* Falls Shared Memory Groesse geaendert wurde, muss der Shared Memory
    	   mit seiner aktuellen Groesse neu allokiert werden */
    	if ((old_sm_size = this->current_sharedmemsize) != this->header_get_sharedmemsize(this->header))
    	{
    	     this->current_sharedmemsize = this->header_get_sharedmemsize(this->header);
             sqlFreeSharedMem((tpr08_header*)this->header, old_sm_size);

             this->header = (tpr08_header*) sqlAllocSharedMem(this->path,this->current_sharedmemsize);
      	}

        /* Part mit apid suche */
        anzahl_parts = (this->current_sharedmemsize - sizeof(tpr08_header))/sizeof(tpr08_part);
        for( i= 0;i<anzahl_parts;i++)
        {   this->part   = (tpr08_part*) ( (this->getHeader(this) )+1);
            this->part   = (tpr08_part*) ( (this->getPart(this)   )+i);
    	    if (this->part_get_pid(this->part)== apid)
    	    {
    	       /* Part mit apid gefunden und gesetzt */
               return 1;
            }
        }
        /* kein Part gefunden*/
        this->part = 0;
        return 0;
    }


tsp00_Int2 tpr08_sharedMem__nextPart(tpr08_sharedMem *this)
	{
	   unsigned int old_sm_size;
	   tsp00_Longuint begin, end;
    	   /* Falls Shared Memory Groesse geaendert wurde, muss der Shared Memory
    	      mit seiner aktuellen Groesse neu allokiert werden */
    	   if ((old_sm_size = this->current_sharedmemsize) != this->header_get_sharedmemsize(this->header))
    	     {
    	       this->current_sharedmemsize = this->header_get_sharedmemsize(this->header);
               sqlFreeSharedMem((tpr08_header*)this->header, old_sm_size);
               this->header = (tpr08_header*) sqlAllocSharedMem(this->path,this->current_sharedmemsize);
      	     }
          begin = (tsp00_Longuint)this->header;
          end   = (tsp00_Longuint)this->part;
	  if ((this->current_sharedmemsize-sizeof(tpr08_part)) > (end-begin))
	  {
	     this->part   = (tpr08_part*) ( (this->part   )+1);
	     return 1;
	  }
	  else return 0;
	}


/*  get/set Methods of tpr08_sharedMem    */
tpr08_header * tpr08_sharedMem__getHeader(tpr08_sharedMem *this)
    {
     	return this->header;
    }

tpr08_part * tpr08_sharedMem__getPart(tpr08_sharedMem *this)
    {
     return this->part;
    }

void tpr08_sharedMem__setlastTracetyp(tpr08_sharedMem *this, short int aTracetyp)
    {
     this->lastTracetyp = aTracetyp;
    }

short int tpr08_sharedMem__getlastTracetyp(tpr08_sharedMem *this)
	{
	 return this->lastTracetyp;
	}

int tpr08_sharedMem__getProcessid(tpr08_sharedMem *this)
    {
     return this->processid;
    }

void tpr08_sharedMem__setopen_for_append (struct tpr08_sharedMem *this, tsp00_Int2 yes)
{
    this->open_for_append = yes;
}

tsp00_Int2 tpr08_sharedMem__getopen_for_append (struct tpr08_sharedMem *this)
{
    return this->open_for_append;
}

char* tpr08_sharedMem__getPath(tpr08_sharedMem *this)
    {
     return this->path;
    }

char* tpr08_sharedMem__getErrortxt(tpr08_sharedMem *this)
    {
     return this->errortxt;
    }

int  tpr08_sharedMem__getErrornr(tpr08_sharedMem *this)
    {
     return this->errornr;
    }

tsp00_Int2 tpr08_sharedMem__traceswitchcount_changed(tpr08_sharedMem *this)
	{
		if(this->current_traceswitchcount != this->header_get_traceswitchcount(this->header))
		{
		  this->current_traceswitchcount = this->header_get_traceswitchcount(this->header);
		  return 1;
		}
		else return 0;
	}

tsp00_Int2 tpr08_sharedMem__firstPart(tpr08_sharedMem *this)
	{
          if (this->current_sharedmemsize > sizeof(tpr08_header))
          {
	   this->part   = (tpr08_part*) ( (this->header)+1);
	   return 1;
	  }
	  else return 0;
	}

const char* tpr08_sharedMem__tracetyp_tochar(tpr08_sharedMem *this, short int atracetyp)
   {
    switch (atracetyp)
   	{
   	    case 0 :
   	      return "not init";
   	    case 1 :
   	      return "off";
   	    case 2 :
   	      return "short";
   	    case 3 :
   	      return "long";
   	    case 4 :
   	      return "short modul";
   	    case 5 :
   	      return "long modul";
   	    default :
   	      return "";
   	}
   }

/*-----------------------------------------------------------------*/
	
