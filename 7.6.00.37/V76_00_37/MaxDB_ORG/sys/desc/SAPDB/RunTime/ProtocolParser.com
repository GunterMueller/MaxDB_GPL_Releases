#
#	RunTime Parser
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

* -except
#
# No exception handling code variant
#
*-noex -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
#


RTEPPars_SAPDBProt.h      inc=SAPDB_Singleton.hpp,geo003.h
RTEPPars_DLLMain.cpp      inc=RTEPPars_SAPDBProt.h
RTEPPars_SAPDBProt.cpp    inc=RTEPPars_SAPDBProt.h,SAPDBTrace_Usage.hpp,SAPDBMem_NewDestroy.hpp
