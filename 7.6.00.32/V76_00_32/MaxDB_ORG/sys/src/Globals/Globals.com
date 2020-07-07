####################
# Globals.com
####################
# settings for all layers
#------------------------
#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end


&if $SAPDB_VER >= 80
* vers80=true
&endif

*:variant(nothread) -nothread parentlinkoption=-nothread

# all pascal files deppend from "manipulated" ptoc.h
*p dep=:ptoc/ptoc.h

# special for Purify
*.cpp:variant(pfy) -DSAPDBMem_USE_SYSTEM_ALLOC -DUSE_SYSTEM_ALLOC_CO13
*x:variant(pfy)    -DSAPDBMem_USE_SYSTEM_ALLOC -DUSE_SYSTEM_ALLOC_CO13
*.c:variant(pfy)   -DSAPDBMem_USE_SYSTEM_ALLOC -DUSE_SYSTEM_ALLOC_CO13
*c:variant(pfy)    -DSAPDBMem_USE_SYSTEM_ALLOC -DUSE_SYSTEM_ALLOC_CO13
*p:variant(pfy)    -DSAPDBMem_USE_SYSTEM_ALLOC -DUSE_SYSTEM_ALLOC_CO13

# special buildin for automatic quantify make
&if defined QUANTIFY_MAKE and $OS = WIN32
# special for quantify
*c parentlinkoption=-symbol_dir=$INSTROOT/pgm \
   parentlinkoption=-map_dir=$INSTROOT/pgm \
   -save_sources=$INSTROOT/src/ \
   -pdbfile=$WRK/obj/kernelqfy.pdb \
   -suppress_variant_name

*x parentlinkoption=-symbol_dir=$INSTROOT/pgm \
   parentlinkoption=-map_dir=$INSTROOT/pgm \
   -save_sources=$INSTROOT/src/ \
   -pdbfile=$WRK/obj/kernelqfy.pdb \
   -suppress_variant_name

*p parentlinkoption=-symbol_dir=$INSTROOT/pgm \
   parentlinkoption=-map_dir=$INSTROOT/pgm \
   -save_sources=$INSTROOT/src/ \
   -pdbfile=$WRK/obj/kernelqfy.pdb \
   -suppress_variant_name
&endif


# -Zi as default option
&if $OSSPEC in [ WIN32 ]
*.idl -multi_offset -CFLAGS=-Zi -CFLAGS=-DMsg_List=SAPDBErr_MessageList # suppress_uncond
*.cpp -Zi -Oi -DMsg_List=SAPDBErr_MessageList
*x    -Zi -Oi -DMsg_List=SAPDBErr_MessageList
*.c   -Zi -Oi
*c    -Zi -Oi
*p    -CFLAGS=-Zi
&else
*.idl -multi_offset -DMsg_List=SAPDBErr_MessageList # suppress_uncond
*.cpp -DMsg_List=SAPDBErr_MessageList
*x    -DMsg_List=SAPDBErr_MessageList
&endif


# until we have no compiler that allows us to detect all broken aliasing rules
# we must switch off strict aliasing optimization for gcc3.3 or higher....
&if $OSSPEC in [ LINUX, FREEBSD ]
*.cpp -fno-strict-aliasing
*x -fno-strict-aliasing
*.c -fno-strict-aliasing
*c -fno-strict-aliasing
*p -CFLAGS=-fno-strict-aliasing
&  if $__HAS_NPTL == 1
*.cpp -DHAS_NPTL
*x    -DHAS_NPTL
*.c   -DHAS_NPTL
*c    -DHAS_NPTL
*p    -CFLAGS=-DHAS_NPTL
&  endif
&endif

&if defined FORCED_SYMBOLDIR
* parentlinkoption=-symbol_dir=$FORCED_SYMBOLDIR
&endif
