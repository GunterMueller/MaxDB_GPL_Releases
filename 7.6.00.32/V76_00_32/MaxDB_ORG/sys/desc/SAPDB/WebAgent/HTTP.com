#
# :sapdb/webagent/wahttp.com
#
# WebAgent HTTP Port
#
# Created By:	Markus Oezgen
# Created Time:	05.12.2000
#
# Modified By:	
#
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

WAHTTP_Agent.h		inc=WAHTTP_Misc.h
WAHTTP_Agent.c		inc=WAHTTP_Misc.h,WAHTTP_HTTP.h,\
						WAHTTP_Agent.h,WAHTTP_FileAgent.h,WAHTTP_WebAgent.h


WAHTTP_Entries.h	inc=
WAHTTP_Entries.c	inc=SAPDB_string.h,WAHTTP_Misc.h,WAHTTP_Entries.h,WAHTTP_Memory.h,WAHTTP_Log.h


WAHTTP_FileAgent.h	inc=
WAHTTP_FileAgent.c	inc=gwd00.h,hwd29date.h,\
						WAHTTP_Agent.h,WAHTTP_Misc.h,WAHTTP_IniPar.h,WAHTTP_Log.h,\
						WAHTTP_FileAgent.h,WAHTTP_Memory.h

WAHTTP_HTTP.h		inc=WAHTTP_Log.h
WAHTTP_HTTP.c		inc=WAHTTP_Misc.h,WAHTTP_Connect.h,WAHTTP_HTTP.h,WAHTTP_Log.h,\
						WAHTTP_Memory.h

WAHTTP_IniPar.h		inc=hwd01wadef.h,WAHTTP_Misc.h
WAHTTP_IniPar.c		inc=WAHTTP_Misc.h,WAHTTP_Entries.h,WAHTTP_IniPar.h,\
						WAHTTP_Log.h,WAHTTP_Server.h,Reg_Registry.h,WAHTTP_Memory.h,gsp100.h,hsp100.h,hsp77.h,WAHTTP_HTTP.h

WAHTTP_Log.h		inc=gsp00
WAHTTP_Log.c		inc=hsp100.h,gsp05,heo670.h,\
						WAHTTP_Misc.h,WAHTTP_Log.h,WAHTTP_IniPar.h

WAHTTP_Misc.h		inc=hsp77.h
WAHTTP_Misc.c		inc=gwd00.h,heo07.h,\
						WAHTTP_Misc.h,WAHTTP_Log.h,WAHTTP_Memory.h

WAHTTP_Server.h		inc=hwd01wadef.h
WAHTTP_Server.c		inc=heo07.h,\
						WAHTTP_Misc.h,WAHTTP_Log.h,WAHTTP_IniPar.h,WAHTTP_HTTP.h,\
						WAHTTP_Connect.h,WAHTTP_Agent.h,WAHTTP_Server.h,WAHTTP_Workers.h,\
						WAHTTP_Memory.h
						
WAHTTP_WebAgent.h	inc=
WAHTTP_WebAgent.c	inc=gwd00.h,hwd01wadef.h,hwd03waapi.h,hwd20control.h,\
						hwd21req.h,hwd22rep.h,hwd28dict.h,hwd29date.h,\
						WAHTTP_Agent.h,WAHTTP_WebAgent.h,WAHTTP_Memory.h,WAHTTP_HTTP.h

WAHTTP_Connect.h	inc=

WAHTTP_Main.cpp		inc=WAHTTP_Misc.h,WAHTTP_Control.h,WAHTTP_Log.h,WAHTTP_IniPar.h

WAHTTP_Control.h	inc=WAHTTP_IniPar.h,hwd01wadef.h
WAHTTP_Control.c	inc=WAHTTP_Control.h,WAHTTP_Misc.h,WAHTTP_Memory.h,gsp05

WAHTTP_Workers.h	inc=hwd01wadef.h
WAHTTP_Workers.c	inc=heo07.h,\
						WAHTTP_Memory.h,WAHTTP_Misc.h,WAHTTP_Log.h,WAHTTP_Connect.h,\
						WAHTTP_Workers.h

WAHTTP_Memory.h		inc=geo102.h,hwd01wadef.h
WAHTTP_Memory.c		inc=WAHTTP_Misc.h,WAHTTP_Memory.h


WAHTTP_Conf.win					ascii ->$DBROOT/pgm/wahttp.conf
WAHTTP_Conf.unix				ascii ->$DBROOT/pgm/wahttp.conf
WAHTTP_DefaultDocument.html		ascii ->$DBROOT/Documents/index.html


