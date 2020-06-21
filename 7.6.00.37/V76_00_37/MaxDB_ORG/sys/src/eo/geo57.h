/*!
  @file           geo57.h
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



#ifndef GEO57_H
#define GEO57_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(_WIN32)
# include <gos00.h> /* nocheck */ /* prototypes (sql57c_malloc, sql57c_free) only */
# if defined (_DEBUG)
#  include <crtdbg.h>
# endif
#else
#include "gen57.h" /* nocheck */ /* prototypes should be relocated ... see above */
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*! @name Macros */
 /*@(*/

/*!
   @Description    ALLOC_MEM, FREE_MEM, ALLOCA ...

 */

#if defined (_WIN32)

# define  ALLOCA_EO57(_s)                _alloca(_s)

# if defined (KERNEL)
#  define  ALLOC_MEM_EO57(_p,_s)         sql57k_pmalloc(__LINE__,__FILE__,_p,_s)
#  define  FREE_MEM_EO57(_p)             sql57k_pfree(__LINE__,__FILE__,_p)
# else
#  define  ALLOC_MEM_EO57(_p,_s)         sql57c_malloc(__LINE__,__FILE__,_p,_s)
#  define  FREE_MEM_EO57(_p)             sql57c_free(__LINE__,__FILE__,_p)
# endif

#else
# define  ALLOCA_EO57(_s)               alloca(_s)
# define  ALLOC_MEM_EO57(_p,_s)         sql57k_pmalloc(__LINE__,__FILE__,_p,_s)
# define  FREE_MEM_EO57(_p)             sql57k_pfree(__LINE__,__FILE__,_p)
#endif


/*@)*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#if defined (__cplusplus)

/*---------------------------------------------------------------------------*/
/*------------------------ class teo57_MemoryState --------------------------*/
/*---------------------------------------------------------------------------*/

#if defined(_WIN32) && defined (_DEBUG)
  /*!
  @class          teo57_MemoryState
     @Description    Memory state snapshots and leak detection.


                The teo57_MemoryState class provides a functionality to 
                make memory state snapshots and detect memory leaks.
                teo57_MemoryState does not have a base class.

   */

  class teo57_MemoryState 
  {
  public:
    /*!
       @Description    Create a new event memory state object.

     */

    teo57_MemoryState() { 
      memset(this, 0, sizeof(*this)); };
    
    ~teo57_MemoryState() {};
  
    /*!
       @Description    Make a snapshot of the current memory state
       @Return value   none

     */

    void            eo57_SnapShot() {   
                      _CrtMemCheckpoint( &State_eo57 ); };

    /*!
       @Description    Build the difference between two memory snapshots.
       @param          PrevState [in] - previous snapshots
       @param          CurrState [in] - current snapshots
       @Return value   Returns true if there are differences.

     */

    bool            eo57_Difference( const teo57_MemoryState& PrevState,
                                     const teo57_MemoryState& CurrState ) {
                      return _CrtMemDifference( &State_eo57, 
                                                &PrevState.State_eo57, 
                                                &CurrState.State_eo57 ) != 0; };
    /*!
       @Description    Dump memory information to the 'debugger output window'.


                Dump objects created after this memory state was checkpointed 
                Will dump all objects if this memory state wasn't checkpointed
                Dump all objects, report about non-objects also List request
                number in {}
       @Return value   none.

     */

    void            eo57_DumpObjects() const { 
                      _CrtMemDumpAllObjectsSince( &State_eo57 ); };

    void            eo57_DumpStatistics() const { 
                      _CrtMemDumpStatistics( &State_eo57 ); };

  private:
    _CrtMemState  State_eo57;
  };
  
#endif

#endif /* __cplusplus */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/


/*===========================================================================*
 *  EXAMPES                                                                  *
 *===========================================================================*/

/*!
   @code           
    
    // Declare the objects needed
    #ifdef _DEBUG
      teo57_MemoryState FirstMemState;
      teo57_MemoryState NextMemState;
      teo57_MemoryState DiffMemState;
    
      FirstMemState.eo57_SnapShot();
    #endif
    
    
    MyLeakyFunc();
    
    
    #ifdef _DEBUG    
      NextMemState.eo57_SnapShot();
    
      if( DiffMemState.eo57_Difference(FirstMemState, NextMemState) )
      {
        DiffMemState.eo57_DumpStatistics();
        DiffMemState.eo57_DumpObjects();      
      }
    #endif
    
    @endcode        
 */



#endif  /* GEO57_H */
