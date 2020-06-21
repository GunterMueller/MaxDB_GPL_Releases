*   *ID* C00E263  DBCMD    changed on 1992-09-28-13.23.09 by ADMIN     *
file connect ( kern test !
create table t ( a char (8),
                 b char (8))!
insert t set a = 'aaa', b = 'bbb'!
insert t set a = 'aaa', b = 'ccc'!
create table t1 ( a1 fixed(8), b1 char (8))!
insert t1 set a1 = 2, b1 = 'aaa'!
insert t1 set a1 = 2, b1 = 'bbb'!
create view v1 as select * from t, t1
    where b = b1 !
select * from v1
  where b = 'bbb'
    and a = (select min (a) from v1 where b||b1 > 'b')!
fetch!
pars_then_ex !
data !
select * into :a, :b, :c, :d from v1
  where b = :b1
    and a = (select min (a) from v1 where b||b1 > :b2 and a <> :a)!
c 8 'bbb'
c 255 'b'
c 8 'z'

nodata !
pars_execute !
rollback release !
