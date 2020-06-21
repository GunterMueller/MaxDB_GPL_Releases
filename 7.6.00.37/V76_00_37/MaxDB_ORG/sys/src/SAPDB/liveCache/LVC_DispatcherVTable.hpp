/*!
 * \file    LVC_DispatcherVTable.cpp
 * \author  IvanS, ThomasA, FerdiF
 * \brief   VTable definition for dispatcher.
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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


#ifndef __LVC_DISPATCHERVTABLE_HPP
#define __LVC_DISPATCHERVTABLE_HPP

#if defined(_IBMR2)

#if defined(BIT64)
/* 
 *  with xlC Compiler on AIX an interface pointer points to the following      
 *  structure:
 *
 *
 *
 *  Interface - pointer  
 *   
 *  | 
 *  |
 *  v
 *   ----------   0  -----------      
 *  |virt.Meth.|--->! Method 1  !    Method 1 is a 'normal' Function- 
 *   ----------   8  -----------     pointer ( see AIX 32 BIT )
 *                  ! Method 2  !    
 *               16  -----------                       
 *                  !     .     !                    
 *                        .                             
 *                  !     .     !                        
 *              n*8  -----------
 *                  ! Method n  !    
 *                   -----------     
 *                  !  0xdead   !    
 *                   -----------     
 *                  !  0xdead   !     
 *                   -----------     
 *                                          
 *                                   
 *                                           
 */
typedef long *t_lpptr;
#else  /* AIX 32 BIT */
/* 
 *  with xlC Compiler on AIX an interface pointer points to the following      
 *  structure:
 *
 *
 *
 *  Interface - pointer  
 *   
 *  | 
 *  |
 *  v
 *   ----------   0  -----------     each Method-Pointer is made up of 
 *  |virt.Meth.|--->!     0     !    2 words:
 *   ----------      -----------        
 *                  !     0     !      1-word: address (Method-Desc)
 *                8  -----------       2-word: pInterface + 2-word = 
 *                  ! Method 1  !              this-pointer of C++ Class
 *                   -         -               of the member function
 *                  ! pointer   !              
 *               16  -----------
 *                  ! Method 2  !    Method-Desc-i -> 0  -----------
 *                   -         -                        ! method i  !
 *                  ! pointer   !                     4  -----------
 *               24  -----------                        !   TOC     ! r2
 *                  !     .     !                     8  -----------
 *                        .                             !environment! r11
 *                  !     .     !                        ----------
 *              n*8  -----------
 *                  ! Method n  !    the Method-Desc-i has the 'normal'
 *                   -         -     structure for a function pointer
 *                  ! pointer   !    parameter
 *                   -----------     1. word: address of the entry point of
 *                  !  0xdead   !             the procedure
 *                   -         -     2. word: address of TOC of module in which 
 *                  !  0xbeaf   !             the procedure is bound
 *                   -----------     3. word: environment-pointer for languages 
 *                                            as Pascal. If used by C++ ????
*/
typedef struct t_tabentry
{
  void*     addr;
  long      this_displ;
} t_tabentry;
typedef t_tabentry *t_lpptr;
#endif /* AIX 32 BIT */

#elif defined(NMP)  /* Reliant Unix  -  32 & 64 Bit */

/* 
 *  with CC Compiler on NMP an interface pointer points to the following      
 *  structure:
 *
 *
 *
 *  Interface - pointer  
 *   
 *  | 
 *  |
 *  v
 *   ----------   0  -----------     each Method-Entry made up of 
 *  |virt.Meth.|--->!   RTTI    !    2 short fields +
 *   ----------      --       --     1 pointer (4 Byte aligned 32 Bit/
 *                  !   Entry   !               8 Byte aligned 64 Bit )
 *                i  -----------       1.st short: is the delta value added 
 *                  ! Method 1  !                  to the this pointer  
 *                   -         -       2.nd short: isn't used (cfront compatability)
 *                  !  Entry    !      3 pointer:  point to the virtual function
 *               2*i -----------
 *                  ! Method 2  !                                            
 *                   -         -                                       
 *                  !  Entry    !                                    
 *               3*i -----------                                          
 *                  !     .     !    i = sizeof(short)*2 + alignment + sizeof(pointer);
 *                        .                                               
 *                  !     .     !        32 Bit ==> alignment = 0; 
 *              n*i  -----------         64 Bit ==> alignment = 4;
 *                  ! Method n  !                                      
 *                   -         -                                      
 *                  !  Entry    !               
 *                   -----------                                            
 *                                                         
 *                                                                              
*/
typedef struct t_tabentry
{
  short     this_displ;
  short     not_used;
#if defined(BIT64)
  int       filler;
#endif
  void*     addr;
} t_tabentry;

typedef t_tabentry *t_lpptr;

//#end Reliant Unix NMP */
#elif defined(SUN) || defined(SOLARIS)
#if (__SUNPRO_CC < 0x0500)     /* PTS 1105381 */
/* 
 *
 *  the SUN C++ Compiler interface pointer points to the following      
 *  structure:
 *
 *  pInterface -> 0  ----------        0  -----------  each Method-Pointer is  
 *                  |virt.Meth.|-------->!     0     !   made up of 2 words:
 *                   ----------           -----------        
 *                  |          |         !     0     ! 1-word:  
 *                                      8  -----------  1. half-word: pInterface +       
 *                                       ! Method 1  !     half-word = this-pointer 
 *                                        -         -                  of Class
 *                                       ! pointer   !  2. half-word: not used      
 *                                    16  -----------       
 *                                       ! Method 2  ! 2-word: address of method
 *                                        -         -         
 *                                       ! pointer   !        
 *                                    24  -----------         
 *                                       !     .     !        
 *                        .              
 *                                       !     .     !        
 *                                   n*8  -----------
 *                                       ! Method n  !    
 *                                        -         -     
 *                                       ! pointer   !    
 *                                        -----------
*/
typedef struct t_tabentry
{
  short     this_displ;
  short     filler;
  void*     addr;
} t_tabentry;

typedef t_tabentry *t_lpptr;
#else
/* 
 *
 *  pInterface -> 0  -----------        0  ------------
 *                  ! virt.Meth ! ------->|      0     |
 *                   -----------           ------------
 *                  !     :     !         |      0     |
 *                                      8  ------------
 *                                        | Method 1   |
 *                                     16  ------------
 *                                        | Method 2   |
 *                                         ------------
 *                                        |      :     |
 *                                               :
 *                                        |      :     |
 *                                         ------------
 *                                (n+1)*4 | Method n   |
 *                                         ------------
 *                                        
*/
typedef long *t_lpptr;
#endif
#elif defined(PA11) || defined(PA20W)
/* 
 *  the HP 32Bit aCC Compiler interface pointer points to the following      
 *  structure:
 *
 *  pInterface -> 0  -----------        0  ------------
 *                  ! virt.Meth ! ------->|      0     |
 *                   -----------           ------------
 *                  !     :     !         |      0     |
 *                                         ------------
 *                                        |      x     |
 *                                         ------------
 *                                        |      0     |
 *                                      16 ------------
 *                                        | Method 1   |
 *                                         ------------
 *                                        | Method 2   |
 *                                         ------------
 *                                        |      :     |
 *                                               :
 *                                        |      :     |
 *                                         ------------
 *                                        | Method n   |
 *                                         ------------
 *                                        
*/
typedef long *t_lpptr;
#elif defined(ALPHA)
/* 
 *  the ALPHA 64Bit cxx Compiler interface pointer points to the following      
 *  structure:
 *
 *  pInterface -> 0  -----------        0  ------------
 *                  ! virt.Meth ! ------->| Method 1   |
 *                   -----------           ------------
 *                  !     :     !       8 | Method 2   |
 *                                         ------------
 *                                        |      :     |
 *                                               :
 *                                        |      :     |
 *                                         ------------
 *                               (n-1)*8  | Method n   |
 *                                         ------------
 *                                        
 * on the ALPHA machine the C++ type 'long' and pointers are 64-bit long.
*/
typedef long *t_lpptr;
#elif defined(LINUX)
/* 
 *  gcc 2.95/2.96 Compiler interface pointer points to the following      
 *  structure:
 *
 *  pInterface -> 0  -----------        0  ------------
 *                  ! virt.Meth ! ------->|      0     |
 *                   -----------           ------------
 *                  !     :     !       4 | Type-Info  |
 *                                         ------------
 *                                      8 | Method 1   |
 *                                         ------------
 *                                        |      :     |
 *                                               :
 *                                        |      :     |
 *                                         ------------
 *                           8 + (n-1)*4  | Method n   |
 *                                         ------------
 *
 *
 *  gcc 3.2 for i386 interface pointer points to the following      
 *  structure:
 *
 *  pInterface -> 0  -----------        0  ------------
 *                  ! virt.Meth ! ------->| Method 1   |
 *                   -----------           ------------
 *                  !     :     !       4 | Method 2   |
 *                                         ------------
 *                                      8 | Method 3   |
 *                                         ------------
 *                                        |      :     |
 *                                               :
 *                                        |      :     |
 *                                         ------------
 *                              (n-1) * 4 | Method n   |
 *                                         ------------
 *
 *
 *  gcc 3.1 on ia64 uses new c++ abi (without runtime type info):
 *
 *  pInterface -> 0  -----------        0  -----------------------
 *                  ! virt.Meth ! ------->| gp0        | Method 0 |
 *                   -----------          |------------|          |
 *                  !     :     !       8 | offset0    |          |
 *                                         ------------+----------|
 *                                     16 | gp1        | Method 1 |
 *                                         ------------|          |
 *                                     24 | offset1    |          |
 *                                         ------------+----------|
 *                                        |      :     |    :     |
 *                                               :       
 *                                        |      :     |    :     |
 *                                         ------------+----------|
 *                             16*(n-1)+0 | gp n       | Method n |
 *                                         ------------|          |
 *                             16*(n-1)+8 | offset n   |          |
 *                                         ------------+----------
 */
typedef long *t_lpptr;
#elif defined(_WIN32)

#if defined(_WIN64)
/* 
 *  MS VC++ Compiler interface pointer points to the following      
 *  structure:
 *
 *  pInterface -> 0  -----------        0  ------------
 *                  ! virt.Meth ! ------->| Method 1   |
 *                   -----------           ------------
 *                  !     :     !       8 | Method 2   |
 *                                         ------------
 *                                        |      :     |
 *                                               :
 *                                        |      :     |
 *                                         ------------
 *                               (n-1)*8  | Method n   |
 *                                         ------------
*/
typedef tsp00_Longint *t_lpptr;
#else
/* 
 *  MS VC++ Compiler interface pointer points to the following      
 *  structure:
 *
 *  pInterface -> 0  -----------        0  ------------
 *                  ! virt.Meth ! ------->| Method 1   |
 *                   -----------           ------------
 *                  !     :     !       4 | Method 2   |
 *                                         ------------
 *                                        |      :     |
 *                                               :
 *                                        |      :     |
 *                                         ------------
 *                               (n-1)*4  | Method n   |
 *                                         ------------
*/
typedef long *t_lpptr;
#endif

#else
typedef long *t_lpptr;
#endif

/// Parameter type union for COM class method.
typedef struct
{
  union
  {
    char i1;
    short i2;
    long i4;
    int intVal;
    FLOAT flt;
    FLOAT dbl;              
    PVOID ptr;
  } t;
} t_Parm;

/// Dummy parameters for COM class method.
typedef struct
{
  t_Parm p[256];
} t_Parms;

/// Pointer to COM class function.
typedef HRESULT (__stdcall * t_lpIFunc) (LPVOID lpIID, const t_Parms p);

/*!
 * \brief Compute entry pointer from VTable.
 *
 * \param dispid method ID to dispatch,
 * \param lpClassPtr reference to the pointer to store this pointer of the class,
 * \param lpIUnknown pointer to the COM object (needed to calculate VTable).
 */
static t_lpIFunc LVC_DispatcherVTableCompute(int dispid,
  IUnknown* &lpClassPtr, IUnknown *lpIUnknown)
{
  /* calculate entry address of method to call      */
  t_lpptr lpVTable = (t_lpptr) * (tsp00_Longint *) lpIUnknown;
  t_lpIFunc lp_IFunc;
  lpClassPtr = lpIUnknown;

#if defined(PA11) || defined(PA20W)
  lp_IFunc    = (t_lpIFunc) lpVTable[dispid+4];
#elif defined(HP_IA64)
  lp_IFunc    = (t_lpIFunc) &(lpVTable[2 * dispid]);
#elif defined(ALPHA) || ( defined(_IBMR2) && defined(BIT64) )
  lp_IFunc    = (t_lpIFunc) lpVTable[dispid];
#elif (defined(SUN) || defined(SOLARIS)) && (__SUNPRO_CC >= 0x0500)
  /* PTS 1105381 *//* 15-Nov-2000 FF chg == to >= */ 
  lp_IFunc    = (t_lpIFunc) lpVTable[dispid+2];
#elif defined(LINUX) && (__GNUC__ < 3)
  /* gcc 2.95/2.96 on i386 and ia64 */
  lp_IFunc = (t_lpIFunc) lpVTable[dispid+2];
#elif defined(LINUX) && (( __GNUC__ >= 4 ) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 2)) 
  /* default for gcc >= 3.2 */
#if defined(IA64)
 /* ia64-c++-abi ( aka abi-version 1 and abi-version 2 ) */
  lp_IFunc = (t_lpIFunc) &lpVTable[2 * dispid];
#else
  /* default gcc >= 3.2 */
  lp_IFunc = (t_lpIFunc) lpVTable[dispid];
#endif
#elif defined(_WIN32) 
  lp_IFunc = (t_lpIFunc) lpVTable[dispid];
#else
  lp_IFunc    = (t_lpIFunc) lpVTable[dispid+1].addr;
  /* calculate this pointer of COM-Object           */
  lpClassPtr = (IUnknown*)((long)lpIUnknown + lpVTable[dispid+1].this_displ);
#endif  // platform switch

  return lp_IFunc;
}


#endif  // __LVC_DISPATCHERVTABLE_HPP
