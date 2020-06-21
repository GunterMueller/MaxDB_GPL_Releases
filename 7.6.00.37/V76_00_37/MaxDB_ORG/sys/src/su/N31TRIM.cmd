*   *ID* N31TRIM  DBCMD    changed on 1992-09-28-13.26.12 by ADMIN     *
declare erg1 cursor for 
select trim(spalte1), trim(spalte2), trim(spalte3) from tab
   for reuse !
select * from erg1 !
fetch !
select columnname, mod, datatype, code, len, dec, columnprivileges, "DEFAULT"
from sysdd.columns where owner = user and tablename = 'ERG1'!
fetch!
declare erg2 cursor for 
select trim ( 'Marie        ' ) from tab where spalte2 is null
   for reuse !
select * from erg2 !
fetch !
file sel_cols_tab ( ERG2 !
select trim ( ' Marie        ' ) from tab where spalte2 is null !
fetch !
select length (trim ( ' Marie     ' )) from tab where spalte2 is null !
fetch !
select  trim ( ' Marie        ' ) a from tab
   where spalte2 is null !
fetch !
select trim ( ' Marie        ' ) || 'b'  from tab
   where spalte2 is null !
fetch !
select trim ( '' ) from tab where spalte2 is null !
fetch !
select trim ( ' ' ) from tab where spalte2 is null !
fetch !
select trim ( ' ' ) || 'b' from tab where spalte2 is null for reuse !
fetch !
declare erg6 cursor for 
select trim ( '  ' ) from tab where spalte2 is null for reuse !
select * from erg6 !
fetch !
select trim ( NULL ) from tab where spalte2 is null for reuse !
fetch !
declare erg8 cursor for 
select trim ( user ) from tab where spalte2 is null for reuse !
select * from erg8 !
fetch !
file sel_cols_tab ( ERG8 !
select trim ( substr('aaa',1,0) ) from tab where spalte2 is null !
fetch !
select trim ( substr('aaa',1,0) ) || 'b' from tab where spalte2 is null !
fetch !
rollback work !
