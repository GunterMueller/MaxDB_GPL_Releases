/*!
  @file           RTE_SystemPA.h
  @author         JoergM
  @ingroup        Runtime
  @brief          System Interface

  Needed for Precision architecture specialties.
   aCC (C++ compiler on HPUX) does not support inline assembler

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
#ifndef RTE_SYSTEMPA_H
#define RTE_SYSTEMPA_H

#if defined(PA20W)

#if defined(__cplusplus)
extern "C" 
#endif

/*!
  Own coroutine context used only if a assembly coroutine switch code exists
 */
struct RTE_OwnCoroutineContext
{
   void *sp ;        /*!< Saved stack entry point during switch or pointer to begin of stack area */
   void (*addr)() ;  /*!< Initial entry address */
};

/*!
  @brief Implementation of own context setup interface

   struct RTE_OwnCoroutineContext
   {
       void *sp ;          Stack pointer
       void (*addr)() ;	Return pointer
   };

  pContext must point to an already filled context
     pContext->sp   set to first byte of stack array,
     pContext->addr point to initial function

  @param pContext [in] context to setup
 */
void RTE_OwnMakeContext(struct RTE_OwnCoroutineContext *pContext);

/*
   @brief Implementation of own context switch interface

   struct RTE_OwnCoroutineContext
   {
       void *sp ;          Stack pointer
       void (*addr)() ;	Return pointer
   };

   This call replaces swapcontext() and will always succeed. Stack overflow must be check outside if needed

   @param pCallerContext [out] context of caller (for update)
   @param pNewContext [in] context to resume into
 */
void RTE_OwnSwapContext( struct RTE_OwnCoroutineContext *pCallerContext
                        ,struct RTE_OwnCoroutineContext *pNewContext );

#endif  /* PA20W */
#endif /* RTE_SYSTEMPA_H */
