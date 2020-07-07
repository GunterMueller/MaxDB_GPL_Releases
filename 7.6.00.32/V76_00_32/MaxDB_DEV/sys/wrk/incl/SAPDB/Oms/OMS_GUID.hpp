/*!
 * \file    OMS_GUID.hpp
 * \author  MarkusSi, Roterring
 * \brief   Class Guid.
 */

/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#ifndef __OMS_GUID_HPP
#define __OMS_GUID_HPP

#include <stddef.h>  // size_t

typedef int			ClassID;

#define	KERNEL_GUID(kernelGuid, omsGuid)					\
  GUID kernelGuid;									              \
  memset(&kernelGuid, 0, sizeof(kernelGuid));		  \
  kernelGuid.Data1 = omsGuid;						        


#define LIBOMS_GUID(omsGuid,kernelGuid) \
  ClassID omsGuid;                      \
  omsGuid = kernelGuid.Data1;           



// Array-Objects are persistent objects which might have different lengths. As
// in the kernel the container size must be fixed, different containers must be
// constructed in the kernel, but in the liboms objects belonging to the different
// containers should belong to the same logical class. 
// This is achieved by using different class guids in the liboms and the kernel.
// The liboms is only interpreting the last bits of the guid, while the kernel is
// interpreting all bits.
// The liboms-guid is coded in the 25 last bits. The first bit (sign) is used for
// the standard var-object guid (-1) and the bits 2 to 7 bits are used to code 
// the category of the array.

// Mask to extract the liboms-guid (last 25 bits)
#define GUID_MASK_ARRAY_BASE ((1<<24) - 1)
#define GUID_MASK_ARRAY_HEAD ((1<<25) - 1)

#define ARRAY_HEAD_GUID( BASE_GUID ) ( (BASE_GUID & GUID_MASK_ARRAY_BASE) | (1<<24) )

// Creates a kernel-guid from the liboms-guid and the arraysize.
inline ClassID omsMakeArrayGuid(ClassID guid, short category)
{
  return (ClassID)((guid & GUID_MASK_ARRAY_BASE) | (category << 25));
}

//// Extract the liboms-guid from the kernel-guid
inline ClassID omsMaskGuid(ClassID guid)
{
  return (ClassID)(guid & GUID_MASK_ARRAY_HEAD);
}

// Check whether two different kernel-guids correspond to the same liboms-guid
inline bool omsIsCompatibleGuid(ClassID guid1, ClassID guid2)
{
  return ((guid1 ^ guid2) & GUID_MASK_ARRAY_HEAD) == 0;
}

// Guid's should normally be positive. Only -1 is still used because this is
// the standard guid of the variable sized objects
inline bool omsIsArrayGuid(ClassID guid)
{
  return ((guid != -1) && (guid & (~GUID_MASK_ARRAY_HEAD)) != 0);
}

inline void omsSplitArrayGuid(ClassID guid, ClassID &baseGuid, short &category)
{
  category  = (short)((guid & (~GUID_MASK_ARRAY_HEAD)) >> 25); 
  baseGuid  = (guid & GUID_MASK_ARRAY_BASE);
}

#endif  // __OMS_GUID_HPP
