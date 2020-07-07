#
# :sapdb/RegistryUpgrade/RegUpd.com
#
# Registry Upgrade and Migration Tool
#
# Created By:	Markus Oezgen
# Created Time:	13.06.2003
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

RegUpd_Main.cpp		inc=SAPDB_Types.h,Reg_Registry.h,RegUpd_Log.h,geo572.h,heo06.h,heo670.h

RegUpd_Log.h		inc=SAPDB_Types.h,geo572.h,heo06.h,heo670.h
RegUpd_Log.c		inc=RegUpd_Log.h


