#  @(#)ox.com	6.1.1.13F	1995-07-05	N.Steingraeber
#
#
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
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

&if $OS = OS2
*                       trace=none check=no -OPT="-O -Op"
&else
*                       trace=none check=no
&endif

# XBACKUP

vox10c	inc=vox00c 

vox11c	inc=vox00c 
vox12c	inc=vox00c 
vox13c	inc=vox00c 
vox14c	inc=vox00c 

# mutrun

vox20c	inc=vox00c

vox11c	inc=vox00c 
vox12c	inc=vox00c 
vox14c	inc=vox00c 
vox15c	inc=vox00c 

#xtcpupd 
vox33x	-YX inc=vox30x,vox31x,vox32x,vox34x
vox35x	-YX inc=vox30x,gos00.h,vox31x,vox32x,vox34x,heo01.h
