*   *ID* C00E262  DBCMD    changed on 1992-09-28-13.23.08 by ADMIN     *
file connect ( kern test !
create table t (
  k1 fixed (1) key,
  k2 fixed (2) key,
  k3 fixed (5) key) !
insert t values (1, 12, 1)!
insert t values (1, 12, 2)!
insert t values (1, 12, 8)!
insert t values (1, 12,10)!
insert t values (1, 12,23)!
insert t values (1, 12,91)!
insert t values (2, 12, 1)!
declare erg cursor for 
select * from t where
   k1 < 5 and k2 < 99 and
  (k1 = 1 and k2 = 12 and k3 = 1) !
fetch erg !
declare erg cursor for 
select * from t where
   k1 < 5 and k2 < 99 and
  (k1 = 1 and k2 = 12 and k3 = 1) for reuse!
fetch erg !
drop table t !
commit release !
