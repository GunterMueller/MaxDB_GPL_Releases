*   *ID* C00E110  DBCMD    changed on 1992-09-28-13.23.04 by ADMIN     *
*
* insert fails after alter table alter
*
file cleark !
file connect ( kern test !
create table fehler (
   eins fixed (4) default 10 range in (10, 20, 30, 40),
   zwei fixed (4)
)!
insert fehler set eins = 20, zwei = 10!
insert fehler set zwei = 15!
select * from fehler!
fetch!
alter table fehler column eins fixed(6)!
insert fehler set eins = 30, zwei = 10!
insert fehler set zwei = 35!
select * from fehler!
fetch!
rollback release!
