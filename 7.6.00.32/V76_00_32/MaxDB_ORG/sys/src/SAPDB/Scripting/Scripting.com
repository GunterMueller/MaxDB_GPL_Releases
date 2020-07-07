# @(#)Scripting.com
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2002-2005 SAP AG
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

*.pm ascii ->$INSTROOT/runtime/perl/SAP/DBTech/
instperl.pl ascii ->$INSTMISC/

dbapi.py ascii ->$INSTROOT/runtime/python/sdb/
dbapi-test.py ascii ->$INSTROOT/lib/python2.3/sdb/dbapi.py

&if $OS in [ WIN32 ]
regex(Script_.*Perl-a.c) -I$ACTIVEPERL_INCLUDE -DWIN32 -DMSWIN32 -DACTIVE_PERL -TP
regex(Script_.*Perl-c.c) -I$COREPERL_INCLUDE -DWIN32 -DMSWIN32 -DWIN32IO_IS_STDIO
&else
regex(Script_.*Perl-c.c) -I$COREPERL_INCLUDE
&endif

&if defined PYTHON_INCLUDE
regex(Script_.*Python.c) -I$PYTHON_INCLUDE
&endif

Script_dbmPython*.c    -I$TOOL/incl/extern/python/sdk%-
Script_loaderPython*.c    -I$TOOL/incl/extern/python/sdk%-
Script_sqlPython*.c    -I$TOOL/incl/extern/python/sdk%-

Script_dbmPerl*.c
Script_loaderPerl*.c

#
# include dependencies (do not edit)
#
Script_SQLPacket.c inc=heo102.h,heo03.h,hsp02.h,hsp26.h,hsp30.h,hsp40.h,\
    hsp41.h,hsp45.h,hsp77.h,hsp78_0.h,hsp100.h,\
    :SAPDB/RunTime/Security/RTESec_Authentication.h,\
    :SAPDB/Scripting/Script_SQLPacket.h
Script_SQLPacket.h inc=gsp00,vsp001
Script_dbmInclude.c inc=hsp100.h,hcn14.h,hsp77.h,hsp78_0.h,hsp100.h,\
    hcn14.h,hsp77.h,hsp78_0.h
Script_dbmPerl.c inc=hsp100.h,hcn14.h,hsp77.h,hsp78_0.h
Script_dbmPerl*.c inc=hsp100.h,hcn14.h,hsp77.h,hsp78_0.h
Script_dbmPython.c inc=hsp100.h,hcn14.h,hsp77.h,hsp78_0.h
Script_dbmPython*.c inc=hsp100.h,hcn14.h,hsp77.h,hsp78_0.h
Script_loaderInclude.c inc=hsp100.h,hcn14.h,\
    :SAPDB/RunTime/RTE_UniqueId.h,hsp77.h,hsp78_0.h,hsp100.h,hsp77.h,\
    hsp78_0.h,hcn14.h,:SAPDB/RunTime/RTE_UniqueId.h
Script_loaderIncludea.c inc=hsp100.h,hcn14.h,heo02.h,\
    :SAPDB/RunTime/RTE_UniqueId.h,hsp77.h,hsp78_0.h,hsp100.h,hsp77.h,\
    hsp78_0.h,hcn14.h,heo02.h,:SAPDB/RunTime/RTE_UniqueId.h
Script_loaderIncludec.c inc=hsp100.h,hcn14.h,heo102.h,\
    :SAPDB/RunTime/RTE_UniqueId.h,hsp77.h,hsp78_0.h,hsp100.h,hsp77.h,\
    hsp78_0.h,hcn14.h,heo102.h,:SAPDB/RunTime/RTE_UniqueId.h
Script_loaderPerl.c inc=hsp100.h,hsp77.h,hsp78_0.h,hcn14.h,\
    :SAPDB/RunTime/RTE_UniqueId.h
Script_loaderPerl*.c inc=hsp100.h,hsp77.h,hsp78_0.h,hcn14.h,\
    :SAPDB/RunTime/RTE_UniqueId.h
Script_loaderPerl-a.c inc=hsp100.h,hsp77.h,hsp78_0.h,hcn14.h,heo02.h,\
    :SAPDB/RunTime/RTE_UniqueId.h
Script_loaderPerl-c.c inc=hsp100.h,hsp77.h,hsp78_0.h,hcn14.h,heo102.h,\
    :SAPDB/RunTime/RTE_UniqueId.h
Script_loaderPython.c inc=hsp100.h,hcn14.h,:SAPDB/RunTime/RTE_UniqueId.h,\
    hsp77.h,hsp78_0.h
Script_loaderPython*.c inc=hsp100.h,hcn14.h,\
    :SAPDB/RunTime/RTE_UniqueId.h,hsp77.h,hsp78_0.h
Script_loaderPythona.c inc=hsp100.h,hcn14.h,heo02.h,\
    :SAPDB/RunTime/RTE_UniqueId.h,hsp77.h,hsp78_0.h
Script_loaderPythonc.c inc=hsp100.h,hcn14.h,heo102.h,\
    :SAPDB/RunTime/RTE_UniqueId.h,hsp77.h,hsp78_0.h
Script_sqlInclude.c inc=heo02.h,heo102.h,hsp02.h,hsp30.h,hsp40.h,hsp41.h,\
    hsp42.h,hsp77.h,hsp78_0.h,hsp100.h,\
    :SAPDB/Scripting/Script_SQLPacket.h
Script_sqlPerl.c inc=
Script_sqlPerl-a.c inc=heo02.h,heo102.h,hsp02.h,hsp30.h,hsp40.h,hsp41.h,\
    hsp42.h,hsp77.h,hsp78_0.h,hsp100.h,hin28.h
Script_sqlPerl-c.c inc=heo02.h,heo102.h,hsp02.h,hsp30.h,hsp40.h,hsp41.h,\
    hsp42.h,hsp77.h,hsp78_0.h,hsp100.h,hin28.h
Script_sqlPython.c inc=heo02.h,heo102.h,hsp02.h,hsp30.h,hsp40.h,hsp41.h,\
    hsp42.h,hsp77.h,hsp78_0.h,hsp100.h,\
    :SAPDB/Scripting/Script_SQLPacket.h
Script_sqlPython*.c inc=heo02.h,heo102.h,hsp02.h,hsp30.h,hsp40.h,hsp41.h,\
    hsp42.h,hsp77.h,hsp78_0.h,hsp100.h,\
    :SAPDB/Scripting/Script_SQLPacket.h

