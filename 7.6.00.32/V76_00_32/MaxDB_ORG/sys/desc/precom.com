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

abaphndl.h dep=lcstream.h ->$WRK/incl/abaphndl.h concom=yes
sqlinfo.h inc= ->$WRK/incl/sqlinfo.h concom=yes
sqlcancel.h inc= ->$WRK/incl/sqlcancel.h concom=yes
lcstream.h dep=OMS_LiveCacheTypes.xml ->$WRK/incl/lcstream.h concom=yes

sqlwhat.c   inc=:precom/what.h
what.c      inc=:precom/what.h
sqlver.c    inc=:precom/what.h,gsp100.h
irtrace.c   inc=vpr08SharedMem.h,hsp100.h,vps99c,geo900.h,heo102.h

&if $OS = WIN32
&if defined HAS_MSC_SECURITY_CHECKS
sqllist     ->$SAPDBSDK/bin/sqllist.bat concom=yes ascii OVERFLOW=1
&else
sqllist     inc= concom=yes unpack=no ascii exec ->$SAPDBSDK/bin/sqllist.bat
&endif
&else
sqllist     inc= concom=yes unpack=no ascii exec ->$SAPDBSDK/bin/
&endif
sqllist.rte inc= concom=yes unpack=no ascii exec ->$SAPDBSDK/bin/
precopt.hlf inc= concom=no unpack=no ascii ->$SAPDBSDK/env/
