# @(#)   desc/slow/ls.com   7.3   03/12/97
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

# die beiden folgenden zeilen bedeuten: es gilt fuer alle!!!
* trace=none shadow=yes unpack=no inc=gls00.h

# Compile all modules with exception handling enabled
regex(.*\.cpp$)    -except
regex(.*\.y$)      -except
regex(.*\.l$)      -except


gls00.h             inc=heo02.h,heo03.h,heo06.h,vsp007,vin00,geo47.h,hsp77.h,gsp100.h,hsp100.h,ToolsCommon/Tools_DynamicUTF8String.hpp,hwd07dbc.h,\
                        hls33_backup.h,hsp81.h,ggg00,SAPDBCommon/SAPDB_string.h,RunTime/System/RTESys_MicroTime.h    dep=RTE_Configure.pl

hls01.h             inc=+hls98msg.h
vls01.cpp           inc=+hls01.h,hls03.h,hls05.h,hls11.h,hls99.h,hls98msg.h,gsp09.h

vls03.cpp           inc=+hls03.h,hls04.h,hls99.h,hls98msg.h

vls04.cpp           inc=+hls04.h,hls05.h,hls99.h,hls98msg.h,hsp77.h

vls041.cpp          inc=+hls041.h,hls04.h,hls05.h,hls07.h,hls99.h,hls98msg.h

vls05.cpp           inc=+hls05.h,hls07.h,hls98msg.h,hsp78_0.h,SAPDBCommon/SAPDB_UTF8Basis.hpp

vls07.cpp           inc=+hls07.h,hls98msg.h

vls08_long.cpp      inc=+hls03.h,hls04.h,hls041.h,hls05.h,hls07.h,hls08_long.h,hls30.h,hls98msg.h,hls99.h

vls10.cpp           inc=+heo01.h,hls03.h,hls05.h,hls10.h,hls11.h,hls13.h,hls15.h,hls20_dataload.h,hls21.h,hls26.h,hls27_dataextract.h,hls28_TLoad.h,hls29_dataupdate.h,hls31_catalogextract.h,hls98msg.h,hls99.h,ToolsCommon/Tools_DynamicUTF8String.hpp

vls11.cpp           inc=+hls05.h,hls10.h,hls11.h,hls14_scan.h,hls14_parse.h,hls15.h,hls98msg.h,hls99.h

vls12.cpp           inc=+hls03.h,hls04.h,hls041.h,hls07.h,hls12.h,hls13.h,hls98msg.h,hls99.h

hls13.h             inc=+hls31_odbc_Result.h
vls13.cpp           inc=+hls03.h,hls04.h,hls041.h,hls05.h,hls12.h,hls13.h,hls31_odbc_Result.h,hls99.h,hls98msg.h

# Parser/Scanner:
# Ausgehend von der grammatikdatei wird das parserobjekt erstellt.
# Dazu ist die skelettdatei noetig -> daher das 'demand=<filename>.skl'.
# Beim Scanner ist zusaetzlich zu beachten, dass vor dem aufruf von flex.exe mit den entsprechenden
# scannerdateien der parser fertig sein muss. Es gilt also die erstellungsreihenfolge parser->scanner.
# Daher in der scanner-beschreibung 'dep=<filename>.y'.
#
# Die erstellung der objekte und die gesamte aufrufreihenfolge der tools fuer den parser und den
# scanner ist in der datei comprc.pl in den developmenttools gekapselt!

FlexLexer.h         ->$WRK/incl inc=-gls00.h

vls14_parse.skl     binary
vls14_scan.skl      binary

hls14_FlexDefs.h    inc=-gls00.h
hls14_scan.h        inc=+hls14_global.h,hls14_FlexDefs.h,hls17_BufPars.h,hls98msg.h

vls14_parse.y       inc=+hls14_global.h,hls14_parse.h,hls14_scan.h,hls01.h,hls03.h,hls05.h,hls07.h,hls13.h,hls15.h,hls19_check.h,hls99.h,heo01.h demand=vls14_parse.skl extdep=$WRK/incl/vls14_parse_gen.h -DMsg_List=SAPDBErr_MessageList
vls14_scan.l        inc=+hls14_global.h,hls14_scan.h,hls98msg.h,hls99.h,FlexLexer.h,hls17_BufPars.h demand=vls14_scan.skl dep=vls14_parse.y -DMsg_List=SAPDBErr_MessageList

vls15.cpp           inc=+hls98msg.h,hls03.h,hls04.h,hls041.h,hls05.h,hls07.h,hls12.h,hls13.h,hls15.h,hwd07dbc.h,hls30.h,hls31_odbc_Result.h,hls99.h,heo01.h,heo13.h,RunTime/Security/RTESec_Authentication.hpp,SAPDBCommon/Fields/SAPDBFields_VarData.hpp

vls16_condition.cpp inc=+hls16_condition.h,hls05.h,hls07.h,hls18.h,hls25.h,hls98msg.h,hls99.h

hls17_BufPars.h     inc=-gls00.h
vls17_BufPars.cpp   inc=+hls17_BufPars.h,hls14_FlexDefs.h

hls18.h             inc=+hls98msg.h
vls18.cpp           inc=+hls05.h,hls07.h,hls18.h,hls19_check.h,hls98msg.h,hls99.h,SAPDBCommon/SAPDB_UTF8Basis.hpp

vls19_check.cpp     inc=+hls19_check.h,hls05.h,hls07.h,hls16_condition.h,hls98msg.h,hls99.h

hls20.h             inc=+hls30.h,hls31_odbc_Result.h
vls20.cpp           inc=+hls03.h,hls04.h,hls041.h,hls05.h,hls07.h,hls08_long.h,hls10.h,hls11.h,hls15.h,hls16_condition.h,hls18.h,hls20.h,hls20_dataload.h,hls20_parse_stmt.h,hls24.h,hls25.h,hls30.h,hls99.h,hls98msg.h

hls20_dataload.h    inc=+hls31_odbc_Result.h
vls20_dataload.cpp  inc=+hls04.h,hls11.h,hls041.h,hls05.h,hls15.h,hls20.h,hls20_dataload.h,hls21.h,hls30.h,hls31_catalogextract.h,hls99.h,hls98msg.h,hsp77.h
vls20_parse_stmt.cpp  inc=+hls13.h,hls20_parse_stmt.h,hls31_odbc_Result.h,hls99.h,hls98msg.h

hls14_parse.h

hls21.h             inc=+hls31_odbc_Result.h
vls21.cpp           inc=+hls03.h,hls04.h,hls05.h,hls041.h,hls07.h,hls13.h,hls15.h,hls16_condition.h,hls18.h,hls21.h,hls22.h,hls30.h,hls99.h,hls98msg.h,hsp30.h,ggg00,gbd00

vls22.cpp           inc=+hls03.h,hls04.h,hls07.h,hls13.h,hls16_condition.h,hls18.h,hls20.h,hls22.h,hls23.h,hls30.h,hls98msg.h,hls99.h,ggg00,gbd00

vls23.cpp           inc=+hls05.h,hls07.h,hls18.h,hls20.h,hls23.h,hls24.h,hls25.h,hls30.h,hls98msg.h,hls99.h,hsp30.h,hls98msg.h

vls24.cpp           inc=+hls05.h,hls07.h,hls16_condition.h,hls24.h,hls25.h,hls99.h,hsp30.h,hls98msg.h

vls25.cpp           inc=+hls05.h,hls07.h,hls25.h,hls98msg.h

hls26.h             inc=+hls31_odbc_Result.h
vls26.cpp           inc=+hls04.h,hls041.h,hls05.h,hls07.h,hls12.h,hls13.h,hls15.h,hls26.h,hls27_dataextract.h,hls29_dataupdate.h,hls30.h,hls31_odbc_Result.h,hls31_catalogextract.h,hls98msg.h,hls99.h,hls26_TETable.h

vls26_TETable.cpp   inc=+hls01.h,hls03.h,hls04.h,hls041.h,hls05.h,hls07.h,hls13.h,hls15.h,hls30.h,hls98msg.h,hls99.h,gbd00,hls31_odbc_Result.h,hls26.h,hls26_TETable.h,

hls27_dataextract.h     inc=+hls31_odbc_Result.h
vls27_dataextract.cpp   inc=+hls05.h,hls07.h,hls15.h,hls27_dataextract.h,hls27_DEExec.h,hls31_catalogextract.h,hls30.h,hls98msg.h,hls99.h,hsp77.h

hls27_DEExec.h      inc=+hls31_odbc_Result.h
vls27_DEExec.cpp    inc=+hls01.h,hls03.h,hls04.h,hls041.h,hls05.h,hls07.h,hls08_long.h,hls12.h,hls13.h,hls15.h,hls18.h,hls19_check.h,hls24.h,hls25.h,hls27_DEExec.h,hls27_dataextract.h,hls30.h,hls31_odbc_Result.h,hls98msg.h,hls99.h,hsp77.h

hls28_TLoad.h       inc=+hls51.h
vls28_TLoad.cpp     inc=+hls05.h,hls07.h,hls10.h,hls11.h,hls13.h,hls15.h,hls20_dataload.h,hls26_TETable.h,hls30.h,hls31_catalogextract.h,hls51.h,hls98msg.h,hls99.h,hls28_TLTable.h,hls28_TLoad.h

hls28_TLTable.h     inc=+hls51.h
vls28_TLTable.cpp   inc=+hls01.h,hls04.h,hls041.h,hls05.h,hls07.h,hls10.h,hls13.h,hls15.h,hls18.h,hls26_TETable.h,hls28_TLSwap.h,hls51.h,hls98msg.h,hls99.h,gbd00,hls11.h,hls31_odbc_Result.h,hls28_TLoad.h,hls28_TLTable.h

vls28_TLSwap.cpp    inc=+hls07.h,hls26_TETable.h,hls98msg.h,hls99.h,gbd00,ggg00,hls31_odbc_Result.h,hls28_TLSwap.h,

vls29_dataupdate.cpp     inc=+hls20.h,hls29_dataupdate.h,hls99.h,hls98msg.h

vls30.cpp                inc=+hls07.h,hni00b.h,heo01.h,hls18.h,hls30.h,hls99.h,hls98msg.h

hls31_odbc_Result.h      inc=+hls98msg.h,hwd07dbc.h,Tools/SQLStudio/OAL/StudioOAL_WResult.hpp,SQLStudio/TableDefinition/StudioTD_TableDef.h,gpr00.h,SAPDB/RunTime/RTE_UniqueId.h
vls31_odbc_Result.cpp    inc=+heo01.h,hls05.h,hls10.h,hls11.h,hls15.h,hls30.h,hls99.h,hls27_dataextract.h,hls29_dataupdate.h,hls31_odbc_Result.h

hls31_catalogextract.h   inc=+gls00.h
vls31_catalogextract.cpp inc=+gls00.h,hls03.h,hls15.h,hls30.h,hls99.h,hls31_odbc_Result.h,hls31_catalogextract.h

hls33_backup.h           inc=hcn35a.h,hcn35d.h,ToolsCommon/Tools_DynamicUTF8String.hpp
vls33_backup.cpp         inc=+heo01.h,hls11.h,hls98msg.h,hls33_backup.h

vls51.cpp           inc=+hls51.h,hls07.h,hls30.h,hls98msg.h,hls99.h

vls91.cpp           inc=gsp00,gsp09.h,hls15.h,,RTESys_ProcessID.hpp,RTESys_SystemInfo.h

vls98msg.cpp        inc=+hls01.h,hls98msg.h,heo01.h,heo06.h,gsp09.h,geo47.h,heo95.h,RunTime/RTE_IniFileHandling.h

vls99.cpp           inc=+hls99.h,heo01.h,hls98msg.h

hls40cli.h          inc=-gls00.h
vls40bload.cpp      inc=gsp00,vsp001,gsp09.h,hls05.h,hls30.h,hls40cli.h,hcn13.h,hcn14.h,heo01.h,heo02.h,heo03.h,gsp01.h,geo47.h,ToolsCommon/Tools_DynamicUTF8String.hpp,hls98msg.h,RTESys_ProcessID.hpp,RTESys_SystemInfo.h

# loader scripting
&if $OS in [ WIN32 ]
hls92
vls92-a.c   inc=hsp100.h,hcn14.h -I$ACTIVEPERL_INCLUDE -DWIN32 -DMSWIN32 -DACTIVE_PERL -TP
vls92-c.c   inc=hsp100.h,hcn14.h -I$COREPERL_INCLUDE -DWIN32 -DMSWIN32 -DWIN32IO_IS_STDIO
&else
vls92-c.c   inc=hsp100.h,hcn14.h -I$COREPERL_INCLUDE
&endif

repman.pm ascii ->$INSTMISC/SAP/DBTech/
loader.pm ascii ->$INSTMISC/SAP/DBTech/

vls93.c   inc=hsp100.h,hcn14.h -I$PYTHON_INCLUDE
vls93.cpp inc=hsp100.h,hcn14.h -I$PYTHON_INCLUDE
loader.py ascii ->$INSTMISC/
repman.py ascii ->$INSTMISC/

&if $MACH = SUN
# is this still necessary for Sun Studio 10+?
vls14_parse.y  -OPT=-xO2
vls14_scan.l   -OPT=-xO2
&endif

&if $OSSPEC == AIX
vls31_odbc_Result.cpp -NO
&endif

&if $OSSPEC in [ HPUX ] && $MACH in [PA20W]
vls31_odbc_Result.cpp -NO
&endif


