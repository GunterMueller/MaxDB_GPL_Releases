/*!================================================================
 module:        gsp00_0.h

 responsible:   DanielD

 special area:  Common | Tracing and Assertions

 description:   tracing and assertions

 see:           http://pwww.bea.sap-ag.de/Entwicklung/cpp/C-Style/commonDeclarations.html

 Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#ifndef GSP00_0_H
#define GSP00_0_H

#include "geo00.h"

/* prototypes used by various inline functions */

externC void sqlk_error (                        /* ALLIN */
                 bool           fatal,
                 const char   * format,
                 ...);

externC void sqlk_assert (                        /* ALLIN */
                 int            kind,
                 int            lineno,
                 const char   * filename,
                 const char   * condition);

externC teo00_Longint sqlk_rangeviolation (               /* ALLIN */
                 teo00_Longint	val,
                 teo00_Longint  minVal,
                 teo00_Longint  maxVal,
                 int            lineno,
                 const char   * fileName);

/* assertions */

enum {
    assertionFailed_esp00,
    preconditionFailed_esp00,
    postconditionFailed_esp00,
    notimplemented_esp00,
    switchError_esp00
};


#if defined (__cplusplus)

inline teo00_Longint
fsp00rangeCheck (                                /* ALLIN */
      teo00_Longint     val,
      teo00_Longint     minVal,
      teo00_Longint     maxVal,
      int               lineno,
      const char       *fileName)
{
    return (((minVal <= val) && (val <= maxVal))
            ? val
            : sqlk_rangeviolation (val, minVal, maxVal, lineno, fileName));
}

#endif          /* defined (__cplusplus) */

#define FAST_MSP00      0
#define DIAGNOSE_MSP00  2
#define QUICK_MSP00     5
#define SLOW_MSP00     10

#if !defined (COMPILEMODE_MSP00)
    #if defined (SAPDB_SLOW)
        #define COMPILEMODE_MSP00 SLOW_MSP00
    #elif defined (SAPDB_QUICK)
        #define COMPILEMODE_MSP00 QUICK_MSP00
    #elif defined (SAPDB_DIAGNOSE)
        #define COMPILEMODE_MSP00 DIAGNOSE_MSP00
    #else
        #define COMPILEMODE_MSP00 FAST_MSP00
    #endif
#endif

#define COMPILEMODE_MEO00 COMPILEMODE_MSP00
#define FAST_MEO00 FAST_MSP00
#define DIAGNOSE_MEO00 DIAGNOSE_MSP00
#define QUICK_MEO00 QUICK_MSP00
#define SLOW_MEO00 SLOW_MSP00

#if COMPILEMODE_MSP00 >= QUICK_MSP00

#define RANGE_CHECK_DBG_MSP00(val, minval, maxval) fsp00rangeCheck (\
            val, minval, maxval, __LINE__, __FILE__)
#define ASSERT_DBG_MSP00(cond)        ((cond) ? (void)0 : \
            sqlk_assert (assertionFailed_esp00, __LINE__, __FILE__, #cond))
#define PRECONDITION_DBG_MSP00(cond)  ((cond) ? (void)0 : \
            sqlk_assert (preconditionFailed_esp00, __LINE__, __FILE__, #cond))
#define POSTCONDITION_DBG_MSP00(cond) ((cond) ? (void)0 : \
            sqlk_assert (postconditionFailed_esp00, __LINE__, __FILE__, #cond))
#define SWITCH_ERROR_DBG_MSP00(val)  sqlk_assert (switchError_esp00, \
            __LINE__, __FILE__, #val)
#define IFQUICK_MSP00(something)      (something)

#else

#define RANGE_CHECK_DBG_MSP00(val, minval, maxval) (val)
#define ASSERT_DBG_MSP00(cond)        0
#define PRECONDITION_DBG_MSP00(cond)  0
#define POSTCONDITION_DBG_MSP00(cond) 0
#define SWITCH_ERROR_DBG_MSP00(val)   0
#define IFQUICK_MSP00(something)      0

#endif

#define NOT_IMPLEMENTED_MSP00(text, stmt)   sqlk_assert (\
notImplemented_esp00, __LINE__, __FILE__, stmt)

/* tracing */
#if defined (__cplusplus)
class tsp00_Tracer
{
  public:
    tsp00_Tracer () {};
    tsp00_Tracer (const char *nameparm, int levelparm = 1);
    ~tsp00_Tracer () {};
    void trace (int layer, const char *format, ...);
};

class tsp00_NullTracer
{
  public:
    tsp00_NullTracer (const char *nameparm, int levelparm = 1) {};
    ~tsp00_NullTracer () {};
    void trace (int layer, const char *format, ...) {};
};

#define TRACER_CLASS_MSP00 tsp00_NullTracer
#define TRACER_CLASS_MEO00 tsp00_NullTracer

#endif

#if defined (__cplusplus) && (COMPILEMODE_MSP00 >= SLOW_MSP00)

#define ROUTINE_DBG_MSP00(routname)     TRACER_CLASS_MSP00 localTrace (routname)
#define TRACEF_MSP00(args)              localTrace.trace args
#define IFSLOW_MSP00(something)         (something)
#define ROUTINE_DBG_MEO00(routname)     TRACER_CLASS_MEO00 localTrace (routname)
#define TRACEF_MEO00(args)              localTrace.trace args
#define IFSLOW_MEO00(something)         (something)

#else

#define ROUTINE_DBG_MSP00(routname)
#define TRACEF_MSP00(args)
#define IFSLOW_MSP00(something)
#define ROUTINE_DBG_MEO00(routname)
#define TRACEF_MEO00(args)
#define IFSLOW_MEO00(something)

#endif

#endif
