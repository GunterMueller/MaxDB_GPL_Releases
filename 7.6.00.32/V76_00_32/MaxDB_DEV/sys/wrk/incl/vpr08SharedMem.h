/*!================================================================
 module:    vpr08SharedMem.h

 responsible: MarcoP
 special area:   CPCDrv | Client | Trace SQL Commands
 description: access to the shared memory for Interface Runtime (IRT)
              and the tool pctrace (dynamic trace switch)

 path of shared memory: IndependendProtPath/pc_dynamic_trace

 Layout of shared memory for dynamischen on/off switch of trace :

 The shared memory (SM) consists of a header (9 bytes) und
 n parts (each part 12 bytes). Every Interface Runtime, for which
 trace options are seted during runtime, will get a part in shared
 memory. The process id is the key for the corresponding part.

    2 bytes     1 bytes  4 bytes
 -----------------------------------
 | traceswitch | trace | sharedmem |
 |    count    |  all  |   size    |
 -----------------------------------
 |------------  Header  -----------|

   4 bytes   4 bytes  2 bytes     2 bytes      2 bytes
 ---------------------------------------------------------
 | process | version | IRT trace |  trace  | last trace- |
 |   id    |   irt   |   status  |   typ   | switch count|...
 ---------------------------------------------------------
 |---------------------  Part  --------------------------|



 traceswitch count: unsigned short int  will be incremmented for every
                                        write operation of irtrace in SM

 trace all:         char                Flag, that means every running
                                        processes of interface runtime
                                        should be trace.
                                        trace all on = 'y'
                                        trace all off= 'n'

 sharedmem size:    unsigned int        length off the whole Shared Memory

 process id:        tsp00_Int4            Process-ID of IRT

 version irt:       unsigned int        Version of Interface Runtime (IRT)

 irt trace status:  short int           actual trace typ der IRT
                                        IRT was terminated       = -1
                                        not initialized          =  0
                                        trace off                =  1
                                        short trace on           =  2
                                        long  trace on           =  3
                                        short modul trace on     =  4
                                        long  modul trace on     =  5
                                        get traceoptions from profile =  6

 trace typ:         short int           wanted trace type (set by irtrace)
                                        possible values are like
                                        irt trace status

 last traceswitch count: unsigned short int  traceswitch count of IRT
                                             at last update of trace status                                            I

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

#ifndef __VPR08_SharedMem_H__
#define __VPR08_SharedMem_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "heo41.h"
#include "geo00.h"
#include "gsp00.h"
/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

    /*typ initialization of Shared Memory */
     typedef enum       { SHM_ERASE = 1,
                          SHM_NOTERASE } ShMInitTyp;

    /*Header of Share Memory*/
    typedef struct tpr08_header {
    /*private:*/
        unsigned short int traceswitchcount;
        char               trace_all;
        unsigned int       sharedmemsize;
    /*public:
          void               (*new_tpr08_header)     (struct tpr08_header *this);

          void               (*set_traceswitchcount) (struct tpr08_header *this);
          unsigned short int (*get_traceswitchcount) (struct tpr08_header *this);

          void               (*set_trace_all_on)     (struct tpr08_header *this);
          void               (*set_trace_all_off)    (struct tpr08_header *this);
          char               (*get_trace_all)        (struct tpr08_header *this);

          void               (*set_sharedmemsize)    (struct tpr08_header *this, unsigned int asize);
          unsigned int       (*get_sharedmemsize)    (struct tpr08_header *this); */
    } tpr08_header;

    /* Part of Shared Memory*/
    typedef struct tpr08_part {
    /*private:*/
        int                pid;
        unsigned int       version;
        short int          irt_tracestatus;
        short int          tracetyp;
        char               profile[9];
        unsigned short int lasttraceswitch;

    /*public:
        void               (*new_tpr08_part)      (struct tpr08_part *this);

        void               (*set_pid)             (struct tpr08_part *this, int apid);
        int                (*get_pid)             (struct tpr08_part *this);

        void               (*set_version)         (struct tpr08_part *this,unsigned int aversion);
        unsigned int       (*get_version)         (struct tpr08_part *this);

        void               (*set_irt_tracestatus) (struct tpr08_part *this,short int astatus);
        short int          (*get_irt_tracestatus) (struct tpr08_part *this);

        void               (*set_tracetyp)        (struct tpr08_part *this,short int astatus);
        short int          (*get_tracetyp)        (struct tpr08_part *this);

        void               (*set_profile)         (struct tpr08_part *this, const char * aname);
        char *             (*get_profile)         (struct tpr08_part *this);

        void               (*set_lasttraceswitch) (struct tpr08_part *this, unsigned short int atraceswitch);
        unsigned short int (*get_lasttraceswitch) (struct tpr08_part *this); */
    } tpr08_part;

   typedef struct tpr08_sharedMem
   {
   /*private:*/
     tpr08_header       *header;
     tpr08_part         *part;
     unsigned int       current_sharedmemsize;
     unsigned short int current_traceswitchcount;
     short int          lastTracetyp;    /*Art des Traces vor dem letzten Umschalten*/
     tsp00_Int2         open_for_append; /*if true new entries in existing tracefile will be appended.
                                           if false existing tracefile will be overwritten */
     int                processid;
     char               path[269];       /*256 bytes for path and sizeof("pc_shared_mem") bytes for filename*/
     char               errortxt[100];   /*tsp01_RteErrString size + 19 bytes reserve*/
     int                errornr;
   /*public:*/

     /*!
     Function:    ~tpr08_sharedMem
     Description: Destructor des Shared Memory

                  Gibt den Shared Memory der Interface Runtime (IRT) frei.
     Parameter:
     Return value:
     */
     void          (*delete_sharedMem)  (struct tpr08_sharedMem *this);

     /* get/set Methoden */
     tpr08_header* (*getHeader)         (struct tpr08_sharedMem *this);
     tpr08_part*   (*getPart)           (struct tpr08_sharedMem *this);
     void          (*setlastTracetyp)   (struct tpr08_sharedMem *this, short int aTracetyp);
     short int     (*getlastTracetyp)   (struct tpr08_sharedMem *this);
     int           (*getProcessid)      (struct tpr08_sharedMem *this);
     void          (*setopen_for_append)(struct tpr08_sharedMem *this, tsp00_Int2 yes);
     tsp00_Int2    (*getopen_for_append)(struct tpr08_sharedMem *this);
     char*         (*getPath)           (struct tpr08_sharedMem *this);
     char*         (*getErrortxt)       (struct tpr08_sharedMem *this);
     int           (*getErrornr)        (struct tpr08_sharedMem *this);

     /*!
     Function:    init_sharedMem()
     Description: Initialisiert die Objektinstance tpr08_sharedMem

                  allokiert den gesamten Shared Memory der Interface
                  Runtime (IRT). Initialisiert den Header der Klasse
                  und setzt den aktuellen Part auf 0.
     Parameter:
     Return value: true  -> wenn Initialisierung erfolgreich
                   false -> wenn Initialisierung gescheitert
     */

     tsp00_Int2  (*init_sharedMem)(struct tpr08_sharedMem *this, ShMInitTyp aInitTyp, char* ShMPath);

     /*!
     Function:    traceswitchcount_changed
     Description: stellt fest ob sich der Aenderungszaehler (traceswitchcount)
                  geaendert hat.

     Parameter:
             benutzt den Objektparameter current_traceswitchcount zum
             speichern des aktuellen Zaehlers und zum Vergleich
     Return value:
     */
     tsp00_Int2         (*traceswitchcount_changed)(struct tpr08_sharedMem *this);

     /*!
     Function:    newPart
     Description: liefert einen Part im Shared Memory der Interface Runtime (IRT)
                  zur angegebenen Prozess-ID

                  Existiert ein Part mit der angegebenen Prozess-ID wird dieser als
                  aktueller Part gesetzt. Ansonsten wird ein unbenutzter Part
                  (trace_status == 0) als aktueller Part fuer diese PID gesetzt.
                  Wird auch kein unbenutzer Part gefunden, so wird ein neuer Part
                  am Ende des Shared Memory erzeugt und als aktueller Part gesetzt.
     Parameter:
             apid [in] Prozess-ID fuer die ein erzeugt werden soll
     Return value: true  -> wenn Part zugewiesen
                   false -> Fehler
     */
     tsp00_Int2         (*newPart)(struct tpr08_sharedMem *this, int apid);

     /*!
     Function:    findPart
     Description: sucht zur uebergebenen Prozess-ID den entsprechenden Part
                  im Shared Memory der Interface Runtime (IRT)

     Parameter:
             apid [in] Prozess-ID nach der gesucht wird
     Return value: true  -> Part gefunden und als aktueller Part gesetzt
                   false -> kein Part zur PID gfunden
     */
       tsp00_Int2          (*findPart)(struct tpr08_sharedMem *this, int apid);

     /*!
     Function:    firstPart
     Description: setzt den ersten Part als aktuellen Part

     Parameter:

     Return value: true  -> aktueller Part auf den ersten Part des Shared
                            Memories gesetzt
                   false -> kein Part vorhanden
     */
       tsp00_Int2          (*firstPart)(struct tpr08_sharedMem *this);

     /*!
     Function:    nextPart
     Description: setzt den naechsten Part als aktuellen Part

     Parameter:

     Return value: true  -> falls Part gesetzt
                   false -> falls kein weitere Part mehr vorhanden
     */
       tsp00_Int2          (*nextPart)(struct tpr08_sharedMem *this);

     /*!
     Function:    tracetyp_tochar
     Description: liefert eine String-Konstante zum Tracetyp

     Parameter:    short int Tracetyp

     Return value: string Darstellung der Traceart
     */
       const char*         (*tracetyp_tochar)(struct tpr08_sharedMem *this, short int atracetyp);


     /* Memberfunktionen der EX-Klasse Header werden an das Sharedmem-Strukt gehangen,
        da Header auf den Shared Memory gecastet werden und Funktionspointer unnötig
        Platz verschwenden würden*/

       void               (*header_set_traceswitchcount) (struct tpr08_header *this);
       unsigned short int (*header_get_traceswitchcount) (struct tpr08_header *this);
       void               (*header_set_trace_all_on)     (struct tpr08_header *this);
       void               (*header_set_trace_all_off)    (struct tpr08_header *this);
       char               (*header_get_trace_all)        (struct tpr08_header *this);
       void               (*header_set_sharedmemsize)    (struct tpr08_header *this, unsigned int asize);
       unsigned int       (*header_get_sharedmemsize)    (struct tpr08_header *this);

     /* Memberfunktionen der EX-Klasse Part werden an das Sharedmem-Strukt gehangen,
        da Part auf den Shared Memory gecastet wird und Funktionspointer unnötig
        Platz verschwenden würden*/

        void               (*part_set_pid)             (struct tpr08_part *this, int apid);
        int                (*part_get_pid)             (struct tpr08_part *this);
        void               (*part_set_version)         (struct tpr08_part *this,unsigned int aversion);
        unsigned int       (*part_get_version)         (struct tpr08_part *this);
        void               (*part_set_irt_tracestatus) (struct tpr08_part *this,short int astatus);
        short int          (*part_get_irt_tracestatus) (struct tpr08_part *this);
        void               (*part_set_tracetyp)        (struct tpr08_part *this,short int astatus);
        short int          (*part_get_tracetyp)        (struct tpr08_part *this);
        void               (*part_set_profile)         (struct tpr08_part *this, const char * aname);
        char *             (*part_get_profile)         (struct tpr08_part *this);
        void               (*part_set_lasttraceswitch) (struct tpr08_part *this, unsigned short int atraceswitch);
        unsigned short int (*part_get_lasttraceswitch) (struct tpr08_part *this);

   } tpr08_sharedMem;

extern tsp00_Int2 tpr08_sharedMem__init_sharedMem (tpr08_sharedMem *this, ShMInitTyp aInitTyp, char* ShMPath);


/*Beispiel: externC void pr01TracePrintf(sqlratype * sqlrap, char* szFmt, ...);*/

#endif /*__VPR01_SharedMem_H__*/
