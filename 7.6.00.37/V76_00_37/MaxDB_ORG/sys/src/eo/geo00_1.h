/*!
  @file           geo00_1.h
  @author         RaymondR
  @special area   common include file
  @brief          common include file
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



#ifndef GEO00_1_H
#define GEO00_1_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define ALIGNMENT_VALUE                    16


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define  ALIGN(_val,_alignment)                                               \
           ((((_val) - 1) / (_alignment) + 1 ) * (_alignment))

#define DEVSPACE_PAGE_SHIFT        KGS->ulDBPageShift

#define PAGESIZE_TO_SHIFTVALUE(_pagesize, _shiftvalue)                                      \
                                        { for ( *_shiftvalue = 10;                          \
                                                ((_pagesize) >> ((*_shiftvalue) + 1) ) > 0; \
                                                (*_shiftvalue)++ ); }

#define PAGES_TO_BYTES(_pages)          ((_pages) << DEVSPACE_PAGE_SHIFT)
#define BYTES_TO_PAGES(_bytes)          ((_bytes) >> DEVSPACE_PAGE_SHIFT)


#define PAGES_TO_QWORD(_Low,_High,_Page) {                                      \
                                        _Low =(_Page)<<DEVSPACE_PAGE_SHIFT;     \
                                        _High=(_Page)>>(32-DEVSPACE_PAGE_SHIFT);\
                                        }

#define QWORD_TO_PAGES(_Low,_High)      ((_Low   >> DEVSPACE_PAGE_SHIFT) |     \
                                        (_High  << (32-DEVSPACE_PAGE_SHIFT)))

#define MAX_DEVSPACE_PAGES              (MAX_DWORD_VAL - 1)

#define HIGH_VALUE_VALID(_High)         (!(_High >> DEVSPACE_PAGE_SHIFT)  ?    \
                                         true : false )

#define LOW_VALUE_VALID(_Low)           (!(_Low << (32-DEVSPACE_PAGE_SHIFT)) ? \
                                         true : false )



#define BLOCKS_TO_QWORD(_Low,_High,_Page,_BlockShftOp)                         \
                                        { _Low =(_Page) << _BlockShftOp;       \
                                          _High=(_Page) >> (32-_BlockShftOp);}

#define QWORD_TO_BLOCKS(_Low,_High, _BlockShftOp)                              \
                                        ((_Low >> _BlockShftOp) |              \
                                         (_High << (32 - _BlockShftOp)))

#define BLOCKS_TO_BYTES(_pages, _BlockShftOp)                                  \
                                        ((_pages) << _BlockShftOp)
#define BYTES_TO_BLOCKS(_pages, _BlockShftOp)                                  \
                                        ((_pages) >> _BlockShftOp)



#define  ARRAY_SIZE(_A)                                                        \
           (sizeof(_A) / sizeof((_A)[0]))


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO00_1_H */
