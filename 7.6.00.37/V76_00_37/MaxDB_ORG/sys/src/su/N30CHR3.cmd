*   *ID* N30CHR3  DBCMD    changed on 1992-09-28-13.26.02 by ADMIN     *
create table tab1 ( spalte1 float(3), spalte2 fixed(3,1) ) !
insert tab1 values ( -2.34E-12 , 32.4 ) !
declare erg1 cursor for 
select chr(spalte1), chr(spalte2) from tab1 !
fetch erg1 !
declare colinfo cursor for 
select columnname, mod, datatype, code, len, dec, 
   columnprivileges, "DEFAULT"
   from sysdd.columns where owner = user and tablename = 'ERG1' and 
   columnname not like 'INTERNAL%'!
fetch colinfo !
declare erg2 cursor for 
select chr(spalte1) col1, chr(spalte2) col2 from tab1 !
fetch erg2 !
declare colinfo cursor for 
select columnname, mod, datatype, code, len, dec, 
   columnprivileges, "DEFAULT"
   from sysdd.columns where owner = user and tablename = 'ERG2' and 
   columnname not like 'INTERNAL%'!
fetch colinfo !
declare erg3 cursor for 
select chr(5) col1, chr(5), chr(-5) col2, chr(-5) from tab1 !
fetch erg3 !
declare colinfo cursor for 
select columnname, mod, datatype, code, len, dec, 
   columnprivileges, "DEFAULT"
   from sysdd.columns where owner = user and tablename = 'ERG3' and 
   columnname not like 'INTERNAL%'!
fetch colinfo !
declare erg4 cursor for 
select chr(5E10) col1, chr(5E30) col2,
       chr(-5E30) col3  from tab1 !
fetch erg4 !
declare colinfo cursor for 
select columnname, mod, datatype, code, len, dec, 
   columnprivileges, "DEFAULT"
   from sysdd.columns where owner = user and tablename = 'ERG4' and 
   columnname not like 'INTERNAL%'!
fetch colinfo !
declare erg5 cursor for 
select chr(-5.55555E10) col1 from tab1 !
fetch erg5 !
declare colinfo cursor for 
select columnname, mod, datatype, code, len, dec, 
   columnprivileges, "DEFAULT"
   from sysdd.columns where owner = user and tablename = 'ERG5' and 
   columnname not like 'INTERNAL%'!
fetch colinfo !
select chr(5E10,3) col1 from tab1 !
select chr(0.00005 ,3) from tab1 !
select chr(0.000000005 ,8) from tab1 !
select chr ( 5e9,7 ) from tab1 !
declare erg6 cursor for 
select chr(5E3,5) col1 from tab1 !
fetch erg6 !
declare colinfo cursor for 
select columnname, mod, datatype, code, len, dec, 
   columnprivileges, "DEFAULT"
   from sysdd.columns where owner = user and tablename = 'ERG6' and 
   columnname not like 'INTERNAL%'!
fetch colinfo !
declare erg7 cursor for 
select chr(5E30,5) col1 from tab1 !
fetch erg7 !
declare colinfo cursor for 
select columnname, mod, datatype, code, len, dec, 
   columnprivileges, "DEFAULT"
   from sysdd.columns where owner = user and tablename = 'ERG7' and 
   columnname not like 'INTERNAL%'!
fetch colinfo !
declare erg8 cursor for 
select spalte1 + spalte2 col1 from tab1 !
fetch erg8 !
declare colinfo cursor for 
select columnname, mod, datatype, code, len, dec, 
   columnprivileges, "DEFAULT"
   from sysdd.columns where owner = user and tablename = 'ERG8' and 
   columnname not like 'INTERNAL%'!
fetch colinfo !
declare erg9 cursor for 
select chr (spalte1 + spalte2) col1 from tab1 !
fetch erg9 !
declare colinfo cursor for 
select columnname, mod, datatype, code, len, dec, 
   columnprivileges, "DEFAULT"
   from sysdd.columns where owner = user and tablename = 'ERG9' and 
   columnname not like 'INTERNAL%'!
fetch colinfo !
declare erg10 cursor for 
select chr(spalte2 + 1000) col1 from tab1 !
fetch erg10 !
declare colinfo cursor for 
select columnname, mod, datatype, code, len, dec, 
   columnprivileges, "DEFAULT"
   from sysdd.columns where owner = user and tablename = 'ERG10' and 
   columnname not like 'INTERNAL%'!
fetch colinfo !
commit work !
create table t1 ( c1 float(12) ) !
create table t2 ( c2 float(12) ) !
insert t1 set c1 = 0.123456789012 !
insert t2 set c2 = 0.123456789012 !
select * from t1, t2 where chr(c1) = chr(c2) !
fetch !
rollback work !
commit work release !
