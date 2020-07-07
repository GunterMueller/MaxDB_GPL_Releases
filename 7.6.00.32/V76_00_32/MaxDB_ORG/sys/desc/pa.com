
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

*        -DSQLODBC inc=vpa00enc,vpa00wic,vpa00sql.h,vpa00sqltypes.h,vpa00sqlext.h,vpa00dlc,geo00.h,gsp00,vsp001,gpr00.h,vpa00odc check=no

&if $MACH=NMP
* -Kschar
&endif
vpa00wic inc= concom=yes
vpa00sql.h inc=vpa00sqltypes.h concom=yes
vpa00sqltypes.h inc= concom=yes
vpa00sqlext.h inc=vpa00sql.h,vpa00sqlucode.h concom=yes
vpa00sqlucode.h inc= concom=yes
vpa00sqlod32.h inc= concom=yes
vpa00global.h inc=vpr100.h,heo07.h,vpa00vrc,vpa00wic,vpa00enc,vpa00sqlext.h,vpa00txc,vpa00dlc,geo00.h,gsp00,vsp001,gpr00.h,vpa00odc,vpa00sqlod32.h,vpa00odstream.h
hpa05num.h inc=vpa00global.h
vpa05num.c inc=hpa05num.h
vpa06.h inc=vpa50Env.h,vpa40DBC.h,vpa60Stmt.h
vpa09.h inc=vpa40DBC.h,vpa60Stmt.h
vpa10Handle.h inc=vpa00global.h
vpa10Handle*.c inc=vpa10Handle.h,vpa20Desc.h,vpa31DiagRec.h,vpa30DiagArea.h,vpa30.h,vpa60Stmt.h,vpa40DBC.h,vpa50Env.h,vpa80String.h,vpa06.h,vpa09.h,hsp78_0.h -ansi
vpa10Handle-unicode.c  -D_UNICODE_ODBC

vpa20Desc.h inc=hsp77.h,hsp78_0.h,vpa00global.h,vpa21DescRec.h,vpa62DBCSpecial.h,vpa30DiagArea.h -DSQLODBC -DDLL
vpa20Desc*.c inc=vpa09.h,vpa10Handle.h,vpa20Desc.h,vpa50Env.h,vpa40DBC.h,vpa60Stmt.h,vpa70SQLDA.h,vpa80String.h, -DSQLODBC -DDLL -ansi \
            dep=OMS_LiveCacheTypes.xml
vpa20Desc-unicode.c  -D_UNICODE_ODBC

vpa21DescRec.h inc=vpa00global.h
vpa21DescRec.c inc=vpa21DescRec.h,vpa20Desc.h -ansi
vpa30DiagArea.h inc=hsp77.h,vpa00global.h,vpr09DynaDesc.h,vpa31DiagRec.h,vpa41ESQBlock.h
vpa30DiagArea.c inc=vpa30DiagArea.h -ansi
vpa31DiagRec.h inc=hsp77.h,vpa00global.h
vpa31DiagRec*.c inc=vpa31DiagRec.h,vpa80String.h,hsp78_0.h -ansi
vpa31DiagRec-static.c -DODBC_STATIC 
vpa40DBC.h inc=vpa00global.h,vpr09DynaDesc.h,vpa20Desc.h,vpa30DiagArea.h,vpa41ESQBlock.h,vpa61StmtAttr.h,vpr01Env.h,vpr01Con.h

vpa40DBC*.c inc=vpa50Env.h,vpa40DBC.h,vpa60Stmt.h,vpa80String.h,vpa30.h,vpa06.h,vpa09.h,vpr05IfCom_String.h,RTE_IniFileHandling.h,vpr10c.h -ansi
vpa40DBC-unicode.c -D_UNICODE_ODBC 

vpa41ESQBlock.h inc=vpa00global.h
vpa41ESQBlock.c inc=vpa41ESQBlock.h,vpa30.h, -ansi
vpa42URI.cpp inc=:SAPDB/RunTime/Communication/RTEComm_URIBuilder.hpp,vpa40DBC.h
vpa50Env.h inc=vpa00global.h,vpa30DiagArea.h,vpr01Env.h
vpa50Env*.c inc=vpa50Env.h,vpa06.h,vpa09.h -ansi
vpa50Env-unicode.c  -D_UNICODE_ODBC 

vpa60Stmt.h inc=vpa00global.h,vpa61StmtAttr.h,vpa62DBCSpecial.h,vpa30DiagArea.h,vpa20Desc.h,vpa40DBC.h,vpa41ESQBlock.h

vpa60Stmt*.c inc=vpa30.h,vpa40DBC.h,vpa50Env.h,vpa60Stmt.h,vpa70SQLDA.h,vpa06.h,vpa09.h,gpr03.h -ansi
vpa60Stmt-unicode.c -D_UNICODE_ODBC 

vpa61StmtAttr.h inc=vpa00global.h
vpa61StmtAttr.c inc=vpa61StmtAttr.h,vpa09.h -ansi
vpa62DBCSpecial.h inc=vpa00global.h
vpa70SQLDA.h inc=vpa00global.h
vpa70SQLDA.c inc=vpa70SQLDA.h -ansi
vpa80String.h inc=vpa00global.h,vpr05IfCom_String.h
vpa80String.c inc=hsp78_0.h,vpa80String.h -ansi
hpa81odext.h inc=vpa00global.h
vpa81odext.c inc=hpa81odext.h -ansi
vpa82unix*.c inc=vpa00global.h,vpa40DBC.h,vpa50Env.h,vpa60Stmt.h,hpa83odbcinstext.h -ansi
vpa82unix-unicode.c -D_UNICODE_ODBC

vpa83staticNT.c inc=vpa00global.h -ansi
hpa83odbcinstext.h inc=
vpa30.h inc=vpa00global.h,vpa40DBC.h,vpa41ESQBlock.h,vpa60Stmt.h,vpr05IfCom_String.h
vpa30*.c -DSQLODBC -DDLL inc=vpa30.h,vpa20Desc.h,vpa40DBC.h,vpa60Stmt.h,\
	vpa70SQLDA.h,vpa09.h,vpa06.h,vpr03Packet.h,vpr01Con.h,vpr10c.h,\
        gpr03.h,gpr11.h,vpr11c.h -ansi
vpa30-unicode.c  -D_UNICODE_ODBC

vpa01c   inc=vpa00global.h,vpa40DBC.h,vpa60Stmt.h,gpr03.h,gpr11.h,vpa09tc,vpr05IfCom_String.h,vpr01SQL.h,vpr03Part.h,vpr03Packet.h -ansi
vpa03wc  inc=vpr100.h,heo07.h,vpa00global.h,vpa00vrc,gos00.h,heo00.h,heo102.h,vpa03wc -ansi
vpa03tc  inc=vpa00enc,geo00.h,gsp05.h,vpa03tc -ansi
vpa04gc*  -DSQLODBC -DDLL inc=vpa00global.h,vpa60Stmt.h,vpa50Env.h,vpa40DBC.h,vsp46c,hsp77.h,hsp78_0.h,vpa04gc -ansi \
            dep=OMS_LiveCacheTypes.xml
vpa04gc-t  -DTESTMAIN
vpa04tc  inc=hsp78_0.h,vpa00global.h,hpa05num.h,vpa04tc -ansi
vpa04fc  -DSQLODBC -DDLL inc=hsp78_0.h,vpa00global.h,hpa05num.h,vpa04fc -ansi
vpa06c   -DSQLODBC -DDLL inc=vpa00global.h,vpa50Env.h,vpa40DBC.h,vpa60Stmt.h,gpr03.h,vpa06.h,vpa09.h,vpa06c -ansi
#    vpa00odstream.h,livecachetypes.h,
vpa07wc*  -DSQLODBC -DDLL inc=RTE_IniFileHandling.h,SAPDB_Names.h,vpa00global.h,vpa08msc,vpa09tc,vpa30wc,hpa90info.h -ansi
vpa07wc-unicode  -D_UNICODE_ODBC
vpa07wc-setup -D_SETUP_ODBC_ONLY

vpa08msc -DSQLODBC inc=vpa00global.h,vpa40DBC.h,vpa08msc check=no -ansi
vpa08msc-i -DSQLODBC -DSQLSP32 inc=vpa00global.h,vpa40DBC.h,vpa08msc check=no -ansi
vpa09ac  -DSQLODBC inc=vpa00global.h,vpa40DBC.h,vpa60Stmt.h,vpa30wc,vpa09tc,vpa09.h check=no -ansi
vpa09tc  -DSQLODBC inc=vpa00global.h,vpa40DBC.h,vpa60Stmt.h,vpa09tc check=no -ansi
vpa09mc  inc=vpa09mc -ansi
vpa07c   -DSQLODBC inc=vpa00enc,vpa00wic,vpa00sql.h,vpa00sqltypes.h,vpa00sqlext.h,vpa00txc,vpa00dlc,geo00.h,gsp00,vsp001,vpr00c,vpi00c,vpa00odc,vpa08msc check=no -ansi

vpa10ac*  -DSQLODBC -DDLL inc=vpa00global.h,vpa10Handle.h,vpa50Env.h,vpa40DBC.h,vpa60Stmt.h,vpa80String.h,vpa30.h,vpa08msc,vpa30wc,vpa06.h,vpa09.h check=no -ansi
vpa10ac-unicode  -D_UNICODE_ODBC

vpa10ec*  -DSQLODBC -DDLL inc=vpa00global.h,vpa10Handle.h,vpa20Desc.h,vpa50Env.h,vpa40DBC.h,vpa60Stmt.h,vpa70SQLDA.h,vpr10c.h,vpa30.h,vpa30wc,vpa06.h,vpa09.h,gpr11.h,vpr04Long.h -ansi
vpa10ec-unicode  -D_UNICODE_ODBC

vpa10gc*  inc=vpa00global.h,vpa10Handle.h,vpa50Env.h,vpa40DBC.h,vpa60Stmt.h,vpa70SQLDA.h,vpa80String.h,vpa30.h,vpa06.h,vpa09.h check=no -ansi
vpa10gc-unicode  -D_UNICODE_ODBC

vpa11ac*  -DSQLODBC -DDLL inc=vpa00global.h,vpa10Handle.h,vpa50Env.h,vpa40DBC.h,vpa60Stmt.h,vpa30.h,vpa00odstream.h,vpa06.h,vpa09.h,SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h -ansi
vpa11ac-unicode  -D_UNICODE_ODBC

vpa11pc*  -DSQLODBC -DDLL inc=vpa00global.h,vpa20Desc.h,vpa50Env.h,vpa40DBC.h,vpa60Stmt.h,vpa80String.h,vpa30.h,vpa06.h,vpa09.h,SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h
vpa11pc-unicode  -D_UNICODE_ODBC

vpa12ac*  inc=vpa00global.h,vpa10Handle.h,vpa20Desc.h,vpa50Env.h,vpa40DBC.h,vpa60Stmt.h,vpa80String.h,vpa30.h,vpa06.h,vpa09.h,SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h check=no -ansi
vpa12ac-unicode  -D_UNICODE_ODBC

vpa12pc*  inc=vpa00global.h,vpa10Handle.h,vpa50Env.h,vpa40DBC.h,vpa60Stmt.h,vpa80String.h,vpa30.h,vpa06.h,vpa09.h,SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h check=no -ansi
vpa12pc-unicode  -D_UNICODE_ODBC

vpa30wc  inc=vpa00global.h,vpa09.h,vpa60Stmt.h,vpa03wc,vpa09mc,vpa30wc,vpa09.h -ansi
vpa50wc  -DSQLODBC -DDLL inc=vpa00global.h,vpa03wc,vpa08msc check=no -ansi
#vpa60wc  inc=vpa00txc,vpa00dlc check=no 
#vpa70wc  inc=vpa00enc,vpa00wic,vpa00dlc,geo00.h,gsp00,vpi00c,vpa00odc check=no
#vpa81wc  -DSQLODBC -DDLL inc=vpa00enc,vpa00wic,vpa00sql.h,vpa00sqltypes.h,vpa00sqlext.h,vpa00dlc,geo00.h,gsp00,vsp001,vpr00c,vpi00c,vpa00odc, check=no
hpa90info.h inc=vpa00global.h
vpa90info.c inc=hpa90info.h -ansi
#vpa90sc  concom=no
#vpa90hc  concom=no
#vpa90wc  inc=vpa90sc,vpa90hc concom=no
vpa00odstream.h inc=
vpa00vrc inc=vpr100.h

# Compression
hpa101saptype.h inc=
hpa102odcompr.h inc=gsp00
vpa102odcompr.cpp inc=hpa102odcompr.h,hpa111ODCompr2.h
hpa103odcompr.h inc=gsp00,hpa101saptype.h,hpa104CsObject.h
vpa103odcompr.cpp inc=hpa103odcompr.h -ansi
hpa104CsObject.h inc=
vpa104CsObject.cpp inc=hpa101saptype.h,hpa106cslzc.h,hpa107cslzh.h,hpa104CsObject.h,hpa105CsObjInt.h -ansi
hpa105CsObjInt.h inc=
vpa105CsObjInt.cpp inc=hpa101saptype.h,hpa106cslzc.h,hpa107cslzh.h,hpa104CsObject.h,hpa105CsObjInt.h -ansi
hpa106cslzc.h inc=
vpa106cslzc.cpp inc=hpa101saptype.h,hpa106cslzc.h,hpa107cslzh.h,hpa104CsObject.h,hpa105CsObjInt.h -ansi
hpa107cslzh.h inc=
vpa107cslzh.cpp inc=hpa101saptype.h,hpa106cslzc.h,hpa107cslzh.h,hpa104CsObject.h,hpa105CsObjInt.h -ansi
vpa108csulzh.cpp inc=hpa101saptype.h,hpa106cslzc.h,hpa107cslzh.h,hpa104CsObject.h,hpa105CsObjInt.h -ansi
hpa110CmprFlt.h inc=gsp00,hpa101saptype.h,hpa104CsObject.h
vpa110CmprFlt.cpp inc=hpa110CmprFlt.h -ansi
hpa111ODCompr2.h inc=vpa00sql.h,vpa00sqlext.h,hpa110CmprFlt.h,vpa00sql.h,vpa00sqlext.h
vpa111ODCompr2.cpp inc=hpa111ODCompr2.h -ansi
hpa112FileCompr.h inc=hpa110CmprFlt.h
vpa112FileCompr.cpp inc=hpa112FileCompr.h -ansi
#vpa113comprtest.cpp inc=hpa112FileCompr.h 
vpa114decompr.cpp inc=hpa112FileCompr.h -ansi
