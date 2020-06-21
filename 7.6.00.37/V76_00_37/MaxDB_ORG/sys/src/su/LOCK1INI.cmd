*   *ID* LOCK1INI DBCMD    changed on 1992-09-28-13.24.34 by ADMIN     *
file connect ( kern test !
create table t (a char (18) ascii key, b char (5) )!
insert t set a = 'first ', b = 'b1'!
insert t set a = 'second', b = 'b2'!
insert t set a = 'third ', b = 'b3'!
create table t1 ( b1 char (8) ascii key, a1 char (8)) !
commit release !
