*   *ID* A03STERN DBCMD    changed on 1992-09-28-13.22.22 by ADMIN     *
 ***************************
 ***   A 0 3 S T E R N   ***
 ***************************!
file connect ( kern test !
drop table kern.t1 !
commit work !
create table kern.t1 (f1 fixed(4) key,f2 fixed(4) key,f3 float(6) key,
            f4 char (6), f5 char (12)) !
create view kern.s1 as select * from kern.t1
       where f2 >= 20  and f3 <= 1.e6 and f4 > 'f' with check option !
create synonym  syns1 for kern.s1 !
commit work !
insert kern.t1 values (1, 10, 1.1e1,'satz1', 'feld5' )!
insert kern.t1 values (2, 22, 1.1e2,'satz2', 'feld5' )!
insert kern.t1 values (3, 33, 1.1e3,'satz3', 'feld5' )!
insert kern.t1 values (4, 44, 1.1e4,'satz4', 'feld5' )!
insert kern.t1 values (5, 55, 1.1e5,'satz5', 'feld5' )!
insert kern.t1 values (6, 66, 1.1e6,'satz6', 'feld5' )!
commit work !
declare erg cursor for 
select * from kern.t1 where f1 > 2 and f1 < 5 !
fetch erg !
update kern.t1 set f4 = 'feld4' !
commit work !
declare erg1 cursor for 
select * from kern.t1 where f1 > 2 and f1 < 5 for reuse !
fetch erg1 !
declare erg2 cursor for 
select syn.f1 s1f1, f2 s1f2, syn.f3 s1f3, f4 s1f4,
       syn.f5 s1f5 from syns1  syn for reuse !
fetch erg2 !
declare erg3 cursor for 
select * from erg2 !
fetch erg3 !
declare erg4 cursor for 
select * from erg1, erg2
     where  erg1.f1 = erg2.s1f1  !
fetch erg4 !
commit work release !
