/*!**************************************************************************

  module      : SAPDBMem_Alloca.h

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: stack memory allocation
  description : allocate memory on stack via alloca()

                This include file can be used to hide compiler specific stuff necessary to
                use alloca().

  last changed: 2000-06-16  15:06

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




//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

*****************************************************************************/


#ifndef SAPDBMEM_ALLOCA_H
#define SAPDBMEM_ALLOCA_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  Declaration:  alloca()
  Description:  Function that allocates memory on the stack of the caller.

                The alloca routine allocates size bytes from the program stack. The allocated
                space is automatically freed when the calling function exits. Therefore, do
                not pass the pointer value returned by alloca as an argument to free.

                Alloca returns a void pointer to the allocated space, which is
                guaranteed to be suitably aligned for storage of any type of object.
                To get a pointer to a type other than char, use a type cast on the return value.
                A stack overflow exception is generated if the space cannot be allocated.
                <br>

                Function:    void* alloca(size)<br>

                <br>
                Remarks: <br>

                There are restrictions to explicitly calling alloca in an exception handler (EH).
                EH routines that run on x86-class processors operate in their own memory frame: <br>
                They perform their tasks in memory space that is not based on the current
                location of the stack pointer of the enclosing function. The most common
                implementations include Windows NT structured exception handling (SEH) and C++ catch
                clause expressions. Therefore, explicitly calling alloca in any of the following
                scenarios results in program failure during the return to the calling EH routine:
*/

#if defined(WIN32)

#  include <malloc.h>

#elif defined(AIX)

#  include <malloc.h>
#  include <stdlib.h>

#elif defined(NMP)

#  if defined(__cplusplus)
     extern "C" void *alloca(size_t);
#  else
     extern     void *alloca(size_t);
#  endif

#else /* All other UNIX platforms should have this include file... */
#ifndef FREEBSD /* FreeBSD has not ! definition is found in stdlib.h */
#  include <alloca.h>
#endif

#endif

/*! EndDeclaration */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/



#endif  /* SAPDBMEM_ALLOCA_H */

