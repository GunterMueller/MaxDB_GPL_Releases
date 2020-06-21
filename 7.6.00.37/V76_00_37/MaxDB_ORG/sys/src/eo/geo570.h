/*!
  @file           geo570.h
  @author         RaymondR
  @special area   Memory management functions.
  @brief          Memory management functions.
  @see            

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



#ifndef GEO570_H
#define GEO570_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo200.h"

#if defined ( USE_POOL_HELPER_CLASS )
# include    "geo00_0.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define NO_HINT_EO570               (void const *)NULL
#define MEM_UNKNOWN_EO570           (size_t)-1



#if !defined(externC) 
# if defined (__cplusplus)
#  define  externC      extern "C"
# else
#  define  externC
# endif
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*! @name Macros */
 /*@(*/
/*!
   @Description    ALLOCA_EO570, ALLOCATE_EO570, C_ALLOCATE_EO570 ...

 */


#if defined(_WIN32)
# define  ALLOCA_EO570(_s)             _alloca(_s)
#else 
# define  ALLOCA_EO570(_s)             alloca(_s)
#endif

#if defined (__cplusplus)
# define  ALLOCATE_EO570(_s,_e)        teo570_GlobalAlloc::                            \
                                             eo570_Allocate  ( _s, __FILE__,__LINE__,  \
                                                               NO_HINT_EO570, _e )
# define  DEALLOCATE_EO570(_p,_e)      teo570_GlobalAlloc::                            \
                                             eo570_Deallocate( _p, __FILE__,__LINE__,  \
                                                               NO_HINT_EO570, _e )
#endif 

#define  C_ALLOCATE_EO570(_p,_s)       eo570_Allocate  ( _p, _s, __FILE__,__LINE__ )
#define  C_DEALLOCATE_EO570(_p)        eo570_Deallocate( _p, __FILE__,__LINE__ )


/*@)*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#if defined (__cplusplus)

class teo570_AllocPool;  /* - forward declaration  */


/*---------------------------------------------------------------------------*/
/*------------------------ class teo570_GlobalAlloc -------------------------*/
/*---------------------------------------------------------------------------*/

class teo570_GlobalAlloc
{
public:
  static void * eo570_Allocate  ( size_t                    Size,
                                  char const * const        FileName,
                                  unsigned long             LineNo,
                                  void const *              PoolHint   = NO_HINT_EO570,
                                  teo200_EventList *        pEventList = NULL );
  static void * eo570_Reallocate( void *                    OldBuffer,
                                  size_t                    NewSize,
                                  char const * const        FileName,
                                  unsigned long             LineNo,
                                  void const *              PoolHint   = NO_HINT_EO570,
                                  teo200_EventList *        pEventList = NULL );
  static void   eo570_Deallocate( void *                    Buffer,
                                  char const * const        FileName,
                                  unsigned long             LineNo,
                                  void const *              PoolHint   = NO_HINT_EO570,
                                  teo200_EventList *        pEventList = NULL );

  static size_t eo570_MemAvail  ( void const *              PoolHint   = NO_HINT_EO570,
                                  teo200_EventList *        pEventList = NULL );
  
  static bool   eo570_Validate  ( void *                    Buffer     = NULL,
                                  void const *              PoolHint   = NO_HINT_EO570 );

  void *operator new    ( size_t    Size )  { return eo570_Allocate(Size,__FILE__,__LINE__); };
  void operator  delete ( void *    Buffer, 
                          size_t    Size )  { if ( Buffer != NULL) eo570_Deallocate(Buffer,__FILE__,__LINE__); };

# if defined(_WIN32) && _MSC_VER > 1100
    void operator  delete( void *    Buffer ){ if ( Buffer != NULL) eo570_Deallocate(Buffer,__FILE__,__LINE__); };
# endif 

# if !defined ( _IBMR2 ) && !(defined ( SUN ) || defined ( SOLARIS ))
   void *operator new[] ( size_t    Size )  { return eo570_Allocate(Size,__FILE__,__LINE__); };
# endif


private:
  static teo570_AllocPool *   m_Pool;

public :
  static void   eo570_SetPool   ( teo570_AllocPool *        Pool );
};



/*---------------------------------------------------------------------------*/
/*------------------------- class teo570_AllocPool --------------------------*/
/*---------------------------------------------------------------------------*/

class teo570_AllocPool
{
public:
  teo570_AllocPool()  { teo570_GlobalAlloc::eo570_SetPool( this ); };

  virtual ~teo570_AllocPool();

  virtual void * eo570_Allocate  ( size_t              Size,
                                   char const * const  FileName,
                                   unsigned long       LineNo,
                                   void const *        PoolHint,
                                   teo200_EventList *  pEventList ) = 0;

  virtual void * eo570_Reallocate( void *              Buffer,
                                   size_t              NewSize,
                                   char const * const  FileName,
                                   unsigned long       LineNo,
                                   void const *        PoolHint,
                                   teo200_EventList *  pEventList)  = 0;

  virtual void   eo570_Deallocate( void *              Buffer,
                                   char const * const  FileName,
                                   unsigned long       LineNo,
                                   void const *        PoolHint,
                                   teo200_EventList *  pEventList)  = 0;

  virtual size_t eo570_MemAvail  ( void const *        PoolHint,
                                   teo200_EventList *  pEventList = NULL );

  virtual  bool  eo570_Validate  ( void *              Buffer,
                                   void const *        PoolHint );
};


/*---------------------------------------------------------------------------*/
/*-------------------------- class teo570_PoolInfo --------------------------*/
/*---------------------------------------------------------------------------*/

#if defined ( USE_POOL_HELPER_CLASS )

 class teo570_PoolHelper
 {
 public:
   teo570_PoolHelper ()                                           { memset (&m_Excl, 0, sizeof(m_Excl));  \
                                                                    m_Used      = 0;                      \
                                                                    m_Cnt       = 0;
                                                                    m_Available = MEM_UNKNOWN_EO570; };

#  if defined(_WIN32)
    void          eo570_WaitUntilAlone  ()                        { WAIT_UNTIL_ALONE(&m_Excl); };
    void          eo570_Clearlock       ()                        { CLEARLOCK(&m_Excl); };
#  else /* UNIX */
    void          eo570_WaitUntilAlone  ()                        { WAIT_UNTIL_ALONE(m_Excl); };
    void          eo570_Clearlock       ()                        { CLEARLOCK(m_Excl); };
#  endif

   void           eo570_AddUsedSize     ( size_t      Size )      { m_Used += Size; m_Cnt++; };
   void           eo570_SubtractUsedSize( size_t      Size )      { m_Used -= Size; m_Cnt--; };

   size_t         eo570_MemUsed         ()                        { return m_Used; }
   unsigned long  eo570_MemAllocCnt     ()                        { return m_Cnt; };

   void           eo570_SetMemAvailable ( size_t      Available ) { m_Available = Available; };
   size_t         eo570_MemAvailable    ()                        { return m_Available; };

 private:
   size_t                          m_Available;
   size_t                          m_Used;
   unsigned long                   m_Cnt;

#  if defined(_WIN32)
    long                           m_Excl;    /* - used to grant exclusive exccess */
#  else /* UNIX */
    ALONE_IN_CACHE(dum_before) 
    ALONE_UKP_STRUCT               m_Excl;    /* - used to grant exclusive exccess */
    ALONE_IN_CACHE(dum_behind) 
#  endif
 };
#endif /* USE_POOL_HELPER_CLASS */
 
#endif /* __cplusplus */
 
 /*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/*----------------------------- Old C-Interface -----------------------------*/
/*---------------------------------------------------------------------------*/

externC  teo001_SystemRc eo570_Allocate  ( void **                  Buffer,
                                           size_t                   Size,
                                           char const * const       FileName,
                                           unsigned long            LineNo );

externC  void            eo570_Deallocate( void *                   Buffer,
                                           char const * const       FileName,
                                           unsigned long            LineNo );


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

#endif  /* GEO570_H */
