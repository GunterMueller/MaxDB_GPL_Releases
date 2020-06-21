
/*!**************************************************************************

    module      : hta99.h

    -------------------------------------------------------------------------

    responsible : N.N.

    special area: Tracing
    description : TA_knltrace

    last changed: 2001-03-14

    -------------------------------------------------------------------------

    copyright:    Copyright (c) 2000-2005 SAP AG

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


*/

#ifndef HTA99_H
#define HTA99_H

/* ========================================================================== */
/* ==========              DECLARATION OF TYPES                     ========= */
/* ========================================================================== */

#include "ggg00.h"

#include "gsp00_0.h" // COMPILEMODE_MSP00

#if COMPILEMODE_MSP00 >= SLOW_MSP00	
#include "hta01.h"
#endif


/* ========================================================================== */
/* ==========           REPLACEMENTS BY THE PREPROCESSOR            ========= */
/* ========================================================================== */

#undef  TRACER_CLASS_MSP00
#define TRACER_CLASS_MSP00	tta99_TraceClass
#undef  TRACER_CLASS_MEO00
#define TRACER_CLASS_MEO00	tta99_TraceClass

#define BASIS_ERROR_TRACE(Debug,BasisError)  cta99_BasisErrorTrace localError_cta99 (Debug,BasisError)
#define IS_OK_TRACE(Debug,IsOk)              cta99_IsOkTrace       localIsOk_cta99  (Debug,IsOk)


/* ========================================================================== */
/* ==========              DEFINITION OF GLOBAL FUNCTIONS           ========= */
/* ========================================================================== */


//===================================================================================
/*! class:       tta99_TraceClass
*/
class tta99_TraceClass
{
private:
	const char* RoutineName;
	
public:
    tta99_TraceClass (const char *Param = NULL);
    ~tta99_TraceClass ();
    void trace (tgg00_Debug_Enum, const char *format, ...);
};


//===================================================================================
/*! class:       cta99_BasisErrorTrace

    description: the destructor writes the BasisError into the trace 
*/
class cta99_BasisErrorTrace
{
private:
    const tgg00_Debug_Param  m_Debug;
    const tgg00_BasisError  &m_BasisError;
    
public:
    
    inline cta99_BasisErrorTrace (
        tgg00_Debug_Param       Debug,
        const tgg00_BasisError &BasisError);
    
    ~cta99_BasisErrorTrace ();
};


//===================================================================================
/*! class:       cta99_IsOkTrace

    description: the destructor writes the bool value IsOk into the trace 
*/
class cta99_IsOkTrace
{
private:
    const tgg00_Debug_Param  m_Debug;
    const bool              &m_IsOk;
    
public:
    
    inline cta99_IsOkTrace (
        tgg00_Debug_Param  Debug,
        const bool        &IsOk);
    
    ~cta99_IsOkTrace ();
};


/*==========================================================================*
*  DEFINITION OF INLINE METHODS                                             *
*===========================================================================*/

inline cta99_BasisErrorTrace::cta99_BasisErrorTrace (tgg00_Debug_Param       Debug,
                                                     const tgg00_BasisError &BasisError):
m_Debug      (Debug),
m_BasisError (BasisError)
{
}

//---------------------------------------------------------------------------

inline cta99_IsOkTrace::cta99_IsOkTrace (tgg00_Debug_Param  Debug,
                                         const bool        &IsOk):
m_Debug (Debug),
m_IsOk  (IsOk)
{
}

#endif /*HTA99_H*/