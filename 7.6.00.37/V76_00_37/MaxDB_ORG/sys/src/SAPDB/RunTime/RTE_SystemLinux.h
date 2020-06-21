/*!
  @file           RTE_SystemLinux.h
  @author         IvanS
  @ingroup        Runtime
  @brief          Coroutine support for Linux

  Needed for coroutines on Linux.

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
#ifndef RTE_SYSTEMLINUX_H
#define RTE_SYSTEMLINUX_H

#if defined(LINUX) && defined (I386)

/*!
   Own coroutine context used only if a assembly coroutine switch code exists
 */
struct RTE_OwnCoroutineContext
{
    void *sp ;        /*!< Saved stack entry point during switch or pointer to begin of stack area */
    void (*addr)() ;  /*!< Initial entry address */
};

/*
 * moved after struct because gcc 3.3 complains
 * about storage class for sp -- DanielD
 */
#if defined(__cplusplus)
extern "C" {
#endif

/*!
  @brief Implementation of own context setup interface

   pContext must point to an already filled context
     pContext->sp   set to first byte of stack array,
     pContext->addr point to initial function

  @param pContext [in] context to setup
 */
void RTE_OwnMakeContext(struct RTE_OwnCoroutineContext *pContext);

/*
   @brief Implementation of own context switch interface

   This call replaces swapcontext() and will always succeed. Stack over must be check outside if needed

   @param pCallerContext [out] context of caller (for update)
   @param pNewContext [in] context to resume into
 */
void RTE_OwnSwapContext( struct RTE_OwnCoroutineContext *pCallerContext
                        ,struct RTE_OwnCoroutineContext *pNewContext );

#if defined(__cplusplus)
}
#endif

#endif

#endif  /* RTE_SYSTEMLINUX_H */
