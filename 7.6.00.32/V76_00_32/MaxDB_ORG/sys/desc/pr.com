
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

*       inc=geo00.h,gsp00,vsp001,vsp004,vsp008,vpr00,vpi00c,vpr00c -DDYNAHASH check=no trace=m90print concom=yes
*-o     inc=geo00.h,gsp00,vsp00c,vsp001,vsp001cc,vsp004,vsp008,vpr00,vpi00c,vpr00c check=no trace=m90print SQLOCI
*-w     inc=geo00.h,gsp00,vsp00c,vsp001,vsp001cc,vsp004,vsp008,vpr00,vpi00c,vpr00c check=no trace=m90print SQLODBC
*-c     inc=geo00.h,gsp00,vpr00,vsp00c,vsp004,vpi00c-c,vpr00c check=no trace=m90print

&if $MACH=NMP
* -Kschar
&endif
& if $MACH in [ HP9 ]
 -OPT=+O2
& endif

vpr01e inc=+gsp05
vpr00 inc=gsp00,vsp001,vsp004,vsp008
vpr10-w -DSQLODBC
vpr10c.h inc=gpr00.h,vpr05IfCom_String.h,vpa40DBC.h,vpr01Con.h
vpr10c*.c -DSQLODBC -Dsql_sqldb inc=vpr10c.h,vpa40DBC.h,vpr01Env.h,vpr01SQL.h,vpr01StmtName.h,vpr03Packet.h check=no
vpr10c-unicode.c -D_UNICODE_ODBC
vpr11c.h inc=gpr00.h,vpr01Con.h
vpr11c.c -DSQLODBC -Dsql_sqldb -Dsql_oracle inc=vpr11c.h,vpr01Con.h,vpr05c.h,vpr08c.h,hsp78_0.h,vpr03Part.h,vpr05AnalyzeSQL.h,vpr01SQL.h check=no
vpr12c.h -DSQLODBC -Dsql_sqldb inc= check=no
vpr12c.c -DSQLODBC -Dsql_sqldb inc=gpr00.h,gpr05.h,vpr12c.h,vpr08c.h,vpr01Con.h,vpr03Part.h,vpr03Packet.h,vpr05c.h check=no
vpr01sc inc=geo00.h,vsp001,gpr00.h,gmt90.h,gpr01s.h,vpr01SQL.h,vpr01Cursor.h check=no
vpr01c.c inc=hsp26.h,gpr00.h,gpr01.h,gpr03.h,gpr04.h,gpr05.h,\
gpr08.h,vpr08c.h,vpr07Macro.h,vpr01Env.h,vpr01Cursor.h,vpr01Precom.h,vpr01StmtName.h,\
vpr01Con.h,vpr01Module.h,vpr01SQL.h,vpr03Segment.h,vpr03Packet.h,\
vpr06ParseId.h,vpr01Trace.h,vpr04Long.h,vpr03Part.h,hsp78_0.h,vpr01ParseInfo.h check=no concom=yes unpack=no -ansi
vpr05c  inc=geo00.h,gpr00.h,gmt90.h
vpr05c.h  inc=gpr01.h -ansi
vpr05c.c  inc=vpr05c.h,vpr07Macro.h,vpr05IfCom_String.h -ansi
vpr03c*.c  inc=heo07.h,\
        gsp05,\
        vsp008,\
        hsp26.h,\
        gpr03.h,\
        gpr08.h,\
        vpr07c,\
        vpr01Env.h,\
        vpr01SQL.h,\
        vpr03Part.h,\
        vpr03Packet.h,\
        vpr05IfCom_String.h,\
        vpr01Trace.h,\
        gmt90.h,\
        hsp78_0.h,\
        vpr01Module.h,\
        vpr01Precom.h,\
        vpr01Con.h,\
        vpr03Con.h concom=yes -ansi
vpr03c-w.c -DSQLODBC
vpr03cac* inc=geo00.h,gpr00.h,vpr01Env.h,vpr01Con.h,gmt90.h,vpr01Module.h,gpr03m.h,vpr03cac
vpr03cac-w -DSQLODBC
vpr03m*.c inc=gpr03m.h,hsp77.h -ansi
vpr03m-m.c -DUSE_MALLOC
vpr03m-d.c -DUSE_MALLOC -MD
vpr03sc*  -Dsql_oracle -Dsql_sqldb inc=geo00.h,gpr00.h,gpr04.h,vpr07c,vpr01SQL.h,vpr07Macro.h,vpr04Param.h,vpr03Packet.h,vpr00c,gmt90.h,gpr03.h,vpr03Part.h,vpr03sc check=no trace=m90print dep=OMS_LiveCacheTypes.xml -ansi \
           dep=OMS_LiveCacheTypes.xml

vpr03sc-w -DSQLODBC
vpr07c  inc=vpr100.h,geo00.h,gpr00.h,vpi00fc,gmt90.h,vpr07Macro.h,vpr07c check=no trace=m90print setbuildinfo 
vpr100.h inc=hsp100.h
vpr04c.c -Dsql_sqldb inc=gpr00.h,gpr04.h,vpr01SQL.h,gmt90.h,hsp45.h,vsp46c,gpr03.h,gpr08.h,vpr03sc,hsp78_0.h,vpr03Packet.h,vpr04Long.h \
        dep=OMS_LiveCacheTypes.xml
vpr04c-w.c -DSQLODBC -Dsql_sqldb inc=gpr00.h,gpr04.h,vpr01SQL.h,gmt90.h,hsp45.h,vsp46c,gpr03.h,gpr08.h,vpr03sc,hsp78_0.h,vpr03Packet.h,vpr04Long.h \
        dep=OMS_LiveCacheTypes.xml

vpr04d2c inc=geo00.h,vsp001,gpr04.h,gmt90.h,gpr03.h,gpr08.h
vpr04orc.c inc=gpr00.h,hsp78_0.h,gpr04.h,gpr03.h,vpr01SQL.h,gpr03.h,gpr08.h,vpr04Long.h,gpr01.h,vpr01Module.h,vpr07Macro.h -ansi \
            dep=OMS_LiveCacheTypes.xml

vpr16c -Dsql_sqldb inc=geo00.h,gpr00.h,gmt90.h,gpr03.h,vpr16c
vpr50c inc=vpr50c -DMAXENT=500000
vpr30wc inc=vpr100.h,geo00.h,gpr00.h,vpr07c,gmt90.h,vpa09mc check=no

&if $MACH == SUN || $OSSPEC == SOLARIS
vpr20 TR_SMALL=Y 
& endif
& if $MACH in [ T35 ]
vpr50 MFPNAME=Y 
& endif

vpr01Abort.c inc=gpr00.h,gpr03.h,vpr01SQL.h,vpi10ConnSync.h

vpr01Con.h inc=gpr00.h,gpr01.h,vpr01StmtName.h,vpr03Segment.h,vpr06ParseId.h,vpr09DynaDesc.h,vpr01ParseInfo.h -ansi
vpr01Con.c inc=vpr01Con.h,gpr00.h,gpr01.h,gmt90.h,gpr03.h,gpr04.h,gpr08.h,vpr02Con.h,vpr03Con.h,vpr01Env.h,vpr01Error.h,vpr01Cursor.h,vpr01StmtName.h,vpr01SQL.h,gpr03.h,vpr03Packet.h,vpr03Segment.h,vpr06ParseId.h,vpr07Macro.h -ansi

vpr01Cursor.h inc=gpr00.h,gpr01.h,vpr09DynaHash.h,vpr09DynaDesc.h,vpr01StmtName.h,vpr01SQL.h
vpr01Cursor.c inc=gmt90.h,vpr07Macro.h,vpr01Cursor.h,vpr01Con.h,vpr01Error.h,vpr01StmtName.h,gpr03.h,gpr03m.h -ansi

vpr03Cancel.c inc=hsp26.h,gpr00.h,:precom/sqlcancel.h -except

vpr09Config.h inc=gsp00,RTE_IniFileHandling.h
vpr09Config*.c inc=vpr09Config.h,SAPDB_Names.h -ansi
vpr09Config-d.c -MD

vpr09DynaDesc.h inc=
vpr09DynaDesc*.c inc=gpr00.h,vpr09DynaDesc.h,gpr03m.h -ansi
vpr09DynaDesc-d.c -MD

vpr01Env.h inc=gpr00.h,gpr01.h,vpr09DynaDesc.h
vpr01Env.c inc=vpr01Env.h,vpr01Module.h,vpr01Con.h,gpr03m.h -ansi

vpr01Error.h inc=gpr00.h,gpr01.h,vpr09DynaDesc.h
vpr01Error.c inc=vsp001,gpr03.h,vpr07Macro.h,vpr01Error.h,gpr03m.h -ansi

vpr01EXECSQL.h inc=gpr00.h
vpr01EXECSQL.c inc=gpr03.h,gpr01.h,gpr08.h,vpr07c,vpr01SQL.h,vpr01Con.h,vpr03CheckPoint.h,vpr01Trace.h,vpr01EXECSQL.h -ansi

vpr01Init.c  inc=gpr00.h,vpr01Env.h,vpr01Module.h,vpr01Trace.h

vpr01Module.h inc=gpr00.h,gpr01.h,vpr09DynaDesc.h
vpr01Module.c inc=gmt90.h,vpr01Module.h,vpr01Env.h,vpr01Con.h,vpr01SQL.h,vpr01Precom.h,vpr01Cursor.h,vpr01StmtName.h,gpr03m.h -ansi

vpr01Precom.h inc=gpr00.h,gpr01.h,vpr09DynaDesc.h,vpr09DynaHash.h
vpr01Precom.c inc=vpr01Precom.h,gpr03.h,gpr08.h,vpr01Con.h,vpr01Cursor.h,vpr01Error.h,vpr01Module.h,vpr01Env.h,vpr01SQL.h,vpr01StmtName.h,vpr07Macro.h,vpr01EXECSQL.h,vpr02Con.h,vpr01Trace.h -ansi

vpr01Seg.h inc=gpr00.h,gpr01.h,vpr09DynaDesc.h
vpr01Seg.c inc=gmt90.h,vpr01Seg.h,vpr01Con.h -ansi

vpr01ParseInfo.h inc=gpr01.h,vpr09DynaDesc.h,vpr05AnalyzeSQL.h,vpr09LRUList.h,vpr09HashTable.h,vpr09LinkedList.h
vpr01ParseInfo.c inc=vpr07Macro.h,gpr03.h,gpr04.h,vpr01ParseInfo.h,vpr01SQL.h,vpr01Trace.h -ansi

vpr01SQL.h inc=gpr00.h,gpr01.h,vpr09DynaDesc.h
vpr01SQL.c inc=gmt90.h,vpr01SQL.h,vpr01Env.h,vpr01StmtName.h,vpr01Error.h,gpr03.h,gpr08.h,vpr03Segment.h,vpr01Cursor.h,vpr04Long.h -ansi

vpr01StmtName.h inc=gpr00.h,gpr01.h,vpr09DynaHash.h,vpr09DynaDesc.h,vpr05AnalyzeSQL.h,vpr01ParseInfo.h
vpr01StmtName.c inc=gpr03.h,vpr07Macro.h,vpr01StmtName.h,vpr01Con.h,vpr01SQL.h,vpr06ParseId.h,vpr01ParseInfo.h -ansi

vpr01Trace.h   inc=gpr00.h
vpr01Trace.c inc=vpr01Trace.h,vpr08SharedMem.h,gpr08.h,vpr100.h,vpr07c,hsp77.h,hsp78_0.h,gpr03.h,vpr07Macro.h,vpr01SQL.h,vpr01Con.h,vpr03Part.h,vpr03Packet.h,hsp26.h -ansi

vpr02Con.h inc=gpr00.h
vpr02Con.c inc=gmt90.h,vpr02Con.h,vpr01Con.h,vpr03Con.h,vpr01SQL.h,vpr01StmtName.h,gpr08.h,vpr07Macro.h,gpr03.h,gpr05.h,hsp78_0.h,vpr01Trace.h,gpr04.h -ansi
vpr02Con-t.c -DTEST inc=gmt90.h,vpr02Con.h,vpr01Con.h,vpr01SQL.h,vpr01StmtName.h,gpr08.h -ansi

vpr03Abap.c inc=gpr00.h -except \
            dep=OMS_LiveCacheTypes.xml

vpr03AbapInfo.c inc=hsp26.h,gpr00.h,:precom/sqlinfo.h,vpr03sc,gpr03.h,gpr08.h -except
vpr03CheckPoint.h inc=:precom/sqlinfo.h
vpr03CheckPoint.c inc=hsp26.h,gpr00.h,vpr03CheckPoint.h,vpr03sc,gpr03.h,gpr08.h

vpr03Con.h inc=gpr00.h,vpr01Con.h
vpr03Con.c inc=gpr03.h,hsp78_0.h,gpr01e.h,vpr03Con.h,vpr03Packet.h,vpr03Segment.h,vpr03Part.h,vpr01Con.h,:SAPDB/RunTime/Security/RTESec_Authentication.h
gpr01e.h,gpr03.h,vpr03Packet.h,vpr03Con.h -ansi

vpr03Packet.h inc=gpr01.h,vpr01Con.h,gpr03.h
vpr03Packet.c inc=vpr03Packet.h,vpr01Env.h,vpr01Con.h,vpr01Trace.h,hsp26.h -ansi

vpr03Part.h inc=gpr00.h
vpr03Part.c inc=vpr07Macro.h,vpr03Part.h,hsp26.h,hsp78_0.h -ansi

vpr03Segment.h inc=vsp001,gpr00.h
vpr03Segment.c inc=vpr03Segment.h,gpr03m.h,hsp26.h -ansi

vpr04Long.h inc=gpr00.h,vpr01Cursor.h

vpr04Long*.c -Dsql_oracle -Dsql_sqldb inc=vpr01Con.h,vpr03Packet.h,gsp05,hsp26.h,hsp30.h,vpr04Long.h,gpr03.h,gpr04.h,gpr08.h,vpr01SQL.h,vpr01Module.h,hsp78_0.h,vpr01Trace.h,vpr07Macro.h -ansi
vpr04Long-w.c -DSQLODBC
vpr04Long-o.c -DSQLOCI

vpr04Param.h inc=gpr00.h,vpr09DynaDesc.h
vpr04Param.c inc=vpr04Param.h,vpr01SQL.h,gpr03m.h -ansi

vpr05AnalyzeSQL.h inc=gpr01.h,vpr05c.h -ansi
vpr05AnalyzeSQL.c inc=vpr05c.h,hsp81.h,vpr07Macro.h,vpr05AnalyzeSQL.h,gpr03m.h -ansi

vpr05IfCom_String.h inc=hsp77.h,hsp81.h
vpr05IfCom_String.c inc=vpr05IfCom_String.h,gpr03m.h,gpr05.h,hsp78_0.h,hsp83.h -ansi

vpr06ParseId.h inc=gpr00.h,vpr09DynaDesc.h
vpr06ParseId.c inc=gpr01.h,vpr06ParseId.h,vpr01Con.h,gpr03m.h,gpr03.h,vpr03Segment.h,vpr03Part.h,vpr03Packet.h -ansi

vpr07Check.h inc=
vpr07Check.c inc=gpr00.h,vpr07Check.h -ansi

vpr07Macro.h inc=
vpr07Macro.c inc=vpr07Macro.h -ansi

vpr08SharedMem.h inc=heo41.h,geo00.h,gsp00
vpr08SharedMem.c inc=vpr08SharedMem.h,gpr03.h,heo01.h,heo06.h,geo900.h,\
        gip00.h,\
        :SAPDB/RunTime/RTE_IniFileHandling.h -ansi

vpr08TraceProfile.h   inc=gos00.h,geo00.h
vpr08TraceProfile.cpp inc=vpr08TraceProfile.h,gos00.h -GX

vpr08trace.h inc=gsp00,gpr00.h
vpr08trace*.c inc=vpr08trace.h,gpr03.h -ansi
vpr08trace-w.c -DSQLODBC

vpr09HashTable.h inc=gpr01.h,vpr09LinkedList.h
vpr09HashTable.c inc=vpr09HashTable.h,gpr03m.h -ansi

vpr09LRUList.h inc=vpr09LinkedList.h
vpr09LRUList.c inc=vpr09LRUList.h,gpr01.h -ansi

vpr09LinkedList.h inc=gpr01.h
vpr09LinkedList.c inc=vpr09LinkedList.h -ansi

vpr08c.c -Dsql_oracle -Dsql_sqldb inc=gpr00.h,gpr03.h,gpr04.h,gpr05.h,gpr08.h,vpr08c.h,vpr05c.h,vpr01Con.h,vpr02Con.h,vpr01SQL.h,vpr01Cursor.h,vpr03Part.h,vpr07Macro.h,vpr01Trace.h -ansi

vpr00c   inc=
gpr00.h  inc=gsp00,vsp001,vpr05IfCom_String.h,heo102.h,heo02.h,gmt90.h,vpr00c,vpi00c,vpr07Check.h concom=yes
gpr01.h  inc=gpr00.h,gpr01b.h,gpr01e.h,gpr01s.h,gpr01s.h
gpr01b.h inc=gpr00.h
gpr01e.h inc=gpr00.h
gpr01l.h inc=gpr00.h
gpr01s.h inc=gpr00.h
gpr03.h inc=gpr00.h,gpr03m.h,gsp05,vpr01Con.h
gpr03m.h inc=gsp00,geo572.h
gpr04.h inc=gpr00.h,gpr01.h
gpr05.h inc=gpr00.h
gpr08.h inc=gpr00.h
vpr08c.h inc=gpr00.h,vpr01Con.h,vpr05AnalyzeSQL.h
gpr11.h inc=gpr00.h

vpr09DynaHash.h inc=vpr09HashTable.h
vpr09DynaHash.c inc=vpr09DynaHash.h,gpr00.h,gpr03m.h -ansi
