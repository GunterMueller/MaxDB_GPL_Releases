/*!***************************************************************************

  module       : SAPDB_C_Cplusplus.h

  -------------------------------------------------------------------------

  responsible : JoegM

  special area: C/C++ mixup
  description : Definitions to allow header files used in C and C++ code

  If a header file is not explicitely 

  last changed: 2000-07-17  21:41
  first created:2000-07-17  14:02

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




*****************************************************************************/


#ifndef SAPDB_C_CPLUSPLUS_H
#define SAPDB_C_CPLUSPLUS_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  Declaration:  externPascal
  Description:  Used for declaration of extern functions written in pascal
*/
#if defined(__cplusplus)
#  define externPascal    extern "C"
#else
#  define externPascal
#endif

/*!---------------------------------------------------------------------------
  Declaration:  externC
  Description:  Used for declaration of extern functions written in C
  NOTE global is a synonym for externC
*/
#if defined(__cplusplus)
#  define externC         extern "C"
#else
#  define externC
#endif

/*!---------------------------------------------------------------------------
  Declaration:  externCpp
  Description:  Used for declaration of extern functions written in C++
*/
#if defined(__cplusplus)
#  define externCpp     
#else
#  define externCpp     
#endif

/*!---------------------------------------------------------------------------
  Declaration:  VAR_ARRAY_REF
  Description:  Used for declaration of reference to arrays of characters

  This is used to declare parameters that are defined as 'PASCAL_STRINGS', which
  is a class if C++ is used and a simple array if C is used. So parameter must
  be declared per reference in C++ and without reference in C!

  For example void vdattime(tsp00_Date VAR_ARRAY_REF Date, tsp00_Time VAR_ARRAY_REF Time)
*/
#if defined(__cplusplus)
#  define VAR_ARRAY_REF	&
#else
#  define VAR_ARRAY_REF
#endif

/*!---------------------------------------------------------------------------
  Declaration:  VAR_VALUE_REF
  Description:  Used for declaration of reference to arrays of characters

  This is used to declare parameters that are defined as pointer in C coding, 
  but as references in C++ coding. To allow common header files, the parameters
  are marked with VAR_VALUE_REF. Common rule is that all parameters that are
  references should be declared as references in C++ coding!

  For example void vwait(RTE_TaskId Tid, SAPDB_Bool1 VAR_VALUE_REF Ok)
*/
#if defined(__cplusplus)
#  define VAR_VALUE_REF	&
#else
#  define VAR_VALUE_REF *
#endif

#endif  /* SAPDB_C_CPLUSPLUS_H */
