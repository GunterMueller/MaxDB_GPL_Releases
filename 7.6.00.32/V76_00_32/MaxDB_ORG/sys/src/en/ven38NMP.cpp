/*@lastChanged: "1999-04-21  15:03"
* @filename:    ven38NMP.cpp
* @purpose:     "COM_Interface for NMP" 
* @release:     7.2.0.0
* @see:         ""
*
* @Copyright (c) 1999-2005 SAP AG"



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




*/

#if defined(NMP) || defined(S390X)

#if defined (BIT64) 
#  define MIN_PARM_NUM  8
#else
#  define MIN_PARM_NUM  4
#endif

/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/
#include <stdio.h>
#include "Oms/OMS_Defines.h"
#include "geo00d.h"
#include "vak001.h"

EXTERN_C void* alloca ( size_t size );

typedef HRESULT (*fct)(...);

EXTERN_C LONG WINAPI sql38aVdcomCall(
    IUnknown *lpIUnknown,
    PROC  pProc,
    void* pBufAddr,
    long  BufLen,
    long  ParmStackSize
    ) {

   int    ix;
   int    ox;
   long   p[MIN_PARM_NUM];
   int    param_num = BufLen / sizeof(tak_dbproc_stack_param_debug);
   tak_dbproc_stack_param_debug *pDebugStack = REINTERPRET_CAST(tak_dbproc_stack_param_debug*,pBufAddr);

   p[0] = (long) lpIUnknown;

   for ( ix=0, ox=1; (ox < MIN_PARM_NUM) && (ix < param_num); ix++, ox++ ) {
     p[ox] = 0;
#if !defined (BIT64)
     if ( !(pDebugStack + ix)->dbg_is_ptr &&
       ((pDebugStack + ix)->dbg_vt == csp1_vt_r8 || 
       (pDebugStack + ix)->dbg_vt == csp1_vt_i8 ||
       (pDebugStack + ix)->dbg_vt == csp1_vt_ui8)) {
       if ( 1 == ox % 2 ) ox++;   /* align on 8 byte boundary */
       if ( ox < MIN_PARM_NUM ) {
         SAPDB_memcpy(&p[ox], &(pDebugStack + ix)->dbg_value.r8(), 4 );
         ox++;
         SAPDB_memcpy(&p[ox], (char*)(&(pDebugStack + ix)->dbg_value.r8())+4, 4 );
       }
       else
         break;
     }
     else
#endif       
     {
       if ( !(pDebugStack + ix)->dbg_is_ptr && ((pDebugStack + ix)->dbg_length) < 8) {
         switch ((pDebugStack + ix)->dbg_length) {
         case 1:
           SAPDB_memcpy((char*)&p[ox] + sizeof(long) - 1, &(pDebugStack + ix)->dbg_value.i1(), 1 );
           break;
         case 2:
           SAPDB_memcpy((char*)&p[ox] + sizeof(long) - 2, &(pDebugStack + ix)->dbg_value.i2(), 2 );
           break;
         case 4:
             SAPDB_memcpy((char*)&p[ox] + sizeof(long) - 4, &(pDebugStack + ix)->dbg_value.i4(), 4 );
           break;
         }
       }
       else
         SAPDB_memcpy(&p[ox], &(pDebugStack + ix)->dbg_value.ptr(), sizeof(long) );
     }
   }
   
   if ( ix < param_num ) {
     long* extra_build_area = (long*) alloca( ParmStackSize - ( MIN_PARM_NUM * sizeof(long)) );
     for ( ox=0; ix < param_num; ix++, ox++ ) {
#if !defined (BIT64)
       if ( !(pDebugStack + ix)->dbg_is_ptr &&
         ((pDebugStack + ix)->dbg_vt == csp1_vt_r8 || 
         (pDebugStack + ix)->dbg_vt == csp1_vt_i8 ||
         (pDebugStack + ix)->dbg_vt == csp1_vt_ui8)) {
         if ( 1 == ox % 2 ) 
           ox++;   /* align on 8 byte boundary */
         SAPDB_memcpy(&extra_build_area[ox], &(pDebugStack + ix)->dbg_value.r8(), 4 );
         ox++;
         SAPDB_memcpy(&extra_build_area[ox], (char*)(&(pDebugStack + ix)->dbg_value.r8())+4, 4 );
       }
       else
#endif       
       {
         if ( !(pDebugStack + ix)->dbg_is_ptr && ((pDebugStack + ix)->dbg_length) < 8) {
           switch ((pDebugStack + ix)->dbg_length) {
           case 1:
             SAPDB_memcpy((char*)&extra_build_area[ox] + sizeof(long) - 1, &(pDebugStack + ix)->dbg_value.i1(), 1 );
             break;
           case 2:
             SAPDB_memcpy((char*)&extra_build_area[ox] + sizeof(long) - 2, &(pDebugStack + ix)->dbg_value.i2(), 2 );
             break;
           case 4:
               SAPDB_memcpy((char*)&extra_build_area[ox] + sizeof(long) - 4, &(pDebugStack + ix)->dbg_value.i4(), 4 );
             break;
           }
         }
         else
           SAPDB_memcpy(&extra_build_area[ox], &(pDebugStack + ix)->dbg_value.ptr(), sizeof(long));
       }
     }
   }

#if defined (BIT64) 
   ((fct)pProc)( p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7] );
#else
   ((fct)pProc)( p[0], p[1], p[2], p[3] );
#endif
   return 0;
 };

#else
#error("This Module is NMP (Reliant Unix) only")
#endif