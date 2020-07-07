/*!
  @file           geo07.h
  @author         JoergM
  @special area   Multithreading Runtine
  @brief          Multithreading Runtine Structures and Typedefs
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



\endif
*/



#ifndef GEO07_H
#define GEO07_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/* tsp_thread_err... */
#define THR_OK_EO07              (0)
#define THR_NOT_OK_EO07          (1)
#define THR_NOT_IMPLEMENTED_EO07 (2)
#define THR_TIMEOUT_EO07         (3)
#define THR_NO_DATA_FOUND_EO07   (100)

/* flags for sqlbeginthread */
#define THR_CREATE_NORMAL_EO07        (0)
#define THR_CREATE_SUSPENDED_EO07     (1)
#define THR_CREATE_DETACHED_EO07      (2)
#define THR_CREATE_PROCESS_SCOPE_EO07 (4)

/* idle priority value for sqlsetthreadpriority */
#if defined(_WIN32)
#define THR_PRIO_IDLE_EO07 ((tsp00_Int4)THREAD_PRIORITY_IDLE)
#else
#define THR_PRIO_IDLE_EO07 ((tsp00_Int4)1)
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


typedef unsigned char                 teo07_ThreadErr;
typedef struct _teo07_Thread          *teo07_Thread;
typedef struct _teo07_Mutex           *teo07_Mutex;
typedef struct _teo07_ThreadSemaphore *teo07_ThreadSemaphore;
typedef int                           teo07_ThreadId;
#if defined(__cplusplus)
extern "C" {
#endif
typedef void *      (*teo07_ThreadCallback) (void *arg);
typedef void        teo07_MutexFunction(teo07_Mutex *mutex);
#if defined(__cplusplus)
}
#endif

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO07_H */
