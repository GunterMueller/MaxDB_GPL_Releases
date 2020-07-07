/*!
  @file           heo05.h
  @author         JoergM
  @special area   
  @brief          RTE - common_rte_include_module
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



\endif
*/



#ifndef HEO05_H
#define HEO05_H

#include "gsp09.h"

#define USEVAR_LINT(var)        var

#define OSError (-1)

typedef struct RTEHeaderT {
    tsp00_Int4 dataLen;
} RTEHeaderT;

typedef struct PacketT {
    RTEHeaderT  header;
    char        data [1];
} PacketT;

#define packetSizeC (4 * 4096)

void            eo44initError (tsp9_rte_xerror* xerror);
void            eo44sysError (tsp9_rte_xerror* xerror,
                              int              rc);
void            eo44anyError (tsp9_rte_xerror* xerror, tsp9_cstr msg);
void            eo44eoshowError (tsp9_rte_xerror* xerror);
void            eo44printfError (tsp9_rte_xerror* xerror,
                    tsp00_Int4 msg_no, signed char msg_type,
                    char *msg_label, char *msg_format, ...);
void            eo44printfSimpleError (tsp9_rte_xerror* xerror,
                    tsp00_Int4  msg_no, char *msg_format, ...);

#define MSG_ERR(x)      eo44printfError x
#define MSG_6(x)      eo44printfSimpleError x
#define MSG_7(x)      eo44printfSimpleError x
#define MSG_8(x)      eo44printfSimpleError x
#define MSG_9(x)      eo44printfSimpleError x

tsp9_cstr       eo44skipSpace (tsp9_cstr str);
tsp9_cstr       eo44skipIdentifier (tsp9_cstr str);
int             eo44parseInt (tsp9_cstr str);
int             eo44strcont (tsp9_cstr string1, tsp9_cstr string2, int minlgt);

#define ceo5_quoted      TRUE
#define ceo5_plain      FALSE

void            eo44printString (char** outbuf, int* bufSize,
                            tsp9_cstr str, tsp9_bool quoted);
void            eo44printLong (char** outbuf, int* bufSize, long val);
void            eo44printChar (char** outbuf, int* bufSize, int ch);


typedef void teo5_cntl_function (tsp9_cstr cmd, tsp9_rte_xerror* xerror);
typedef void teo5_monitor_function (void* handle, tsp9_rte_readdirection direction,
    char* outbuf, int bufSize, tsp9_rte_xerror* xerror);

#define ceo5_compressed_format          1

typedef struct teo5_monitor_object {
    teo5_monitor_function *nextRecord;
    tsp9_cstr description;
    int flags;
    BOOLEAN started;
} teo5_monitor_object;

typedef teo5_monitor_object* teo5_monitor_constructor (tsp9_cstr, int flags,
    tsp9_rte_xerror* xerror);

#if defined (_WIN32)
void os92getRemoteInfo (teo5_monitor_object* monitor, tsp9_rte_xerror* xerror);

teo5_monitor_constructor os92allTasks;
teo5_monitor_constructor os92state;
#else
teo5_cntl_function en902cancelTask;
teo5_cntl_function en902coreProcess;
teo5_cntl_function en902changeDebugLevel;
teo5_cntl_function en902killTask;
teo5_cntl_function en902resetCounter;
teo5_cntl_function en902errioRead;
teo5_cntl_function en902errioWrite;
teo5_cntl_function en902time;

teo5_monitor_constructor en902allProcesses;
teo5_monitor_constructor en902activeProcesses;
teo5_monitor_constructor en902state;
#endif

#endif  /* HEO05_H */
