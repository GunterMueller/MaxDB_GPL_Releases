# @(#)   desc/fast/eo.com   6.1.2   09/17/96
#
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#

veo02uc inc=vsp00c,vsp002c,vsp003c,veo00c,vos00spc,vos00c trace=none check=no -DPROTOTYPE
veo11uc inc=vsp00c,vsp002c,vsp003c,vsp005c,veo00c,vos00spc,vos00c,veo002c,veo00mc trace=none check=no -DPROTOTYPE
veo06c  inc=vsp00c,vsp002c,vsp005c,veo00c,vos00spc,vos00c,vos00mc -ansi
veo13uc inc=vsp00,vsp00c,vsp002c,vsp003c,vsp004c,veo00c,vos00spc,vos00c,vos00mc,veo002c,veo004c,veo00mc -DPROTOTYPE

