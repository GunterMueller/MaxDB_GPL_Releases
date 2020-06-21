*   *ID* C00E265  DBCMD    changed on 1992-09-28-13.21.31 by ADMIN     *
* Fehlerprotokoll 265 !
file connect ( kern test !
create table t ( a fixed (7) key,  b fixed (8), c fixed (3))!
insert t set a = 1, c = 80, b = 3 !
insert t set a = 2, c = 80, b = 5 !
create view tt as select * from kern.t !
explain select * from tt x where c = 80 and
b = (select max (b) from tt y where x.a = y.a )!
rollback release !
