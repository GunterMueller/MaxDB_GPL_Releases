*   *ID* A03UPD   DBCMD    changed on 1992-09-28-13.22.22 by ADMIN     *
 ***************************
 ***   A 0 3 U P D       ***
 ***************************!
file connect ( kern test !
drop table kern.t1 !
commit work !
create table kern.t2 (f1 fixed (4)key,f2 fixed(4)key,f3 float(6) key,
           f4 char (6), f5 char (12)) !
create view kern.s1 as select * from kern.t2
       where f2 >= 20  and f3 <= 1.e6 and f4 > 'f' with check option !
commit work !
insert kern.t2 values (1, 10, 1.1e1,'satz1', 'feld5' )!
insert kern.t2 values (2, 22, 1.1e2,'satz2', 'feld5' )!
insert kern.t2 values (3, 33, 1.1e3,'satz3', 'feld5' )!
insert kern.t2 values (4, 44, 1.1e4,'satz4', 'feld5' )!
insert kern.t2 values (5, 55, 1.1e5,'satz5', 'feld5' )!
insert kern.t2 values (6, 66, 1.1e6,'satz6', 'feld5' )!
commit work !
declare erg cursor for 
select * from kern.t2 where f1 > 2 and f1 < 5 !
fetch erg !
update kern.t2 set f4 = 'feld4' !
commit work !
declare erg1 cursor for 
select * from kern.t2 where f1 > 2 and f1 < 5 !
fetch erg1 !
declare erg2 cursor for 
select * from kern.s1 !
fetch erg2 !
 *****  current of update test  ***** !
again erg1 !
fetch first erg1 !
fetch1 erg1 !
update kern.t2  set f4  = 'curr1'  where current of erg1 !
fetch1 erg1 !
update kern.t2  set f4  = 'curr2'  where current of erg1 !
fetch1 erg1 !
update kern.t2  set f4  = 'curr3'  where current of erg1 !
commit work !
declare ergc cursor for 
select * from kern.t2 !
fetch ergc !
fetch first erg2 !
fetch1 erg2 !
update kern.s1  set f4  = 'fes11'  where current of erg2 !
fetch1 erg2 !
update kern.s1  set f4  = 'cus12'  where current of erg2 !
fetch1 erg2 !
update kern.s1  set f4  = 'cus13'  where current of erg2 !
commit work !
declare ergs cursor for 
select * from kern.t2 !
fetch ergs !
delete kern.t2 !
commit work !
declare erg3 cursor for 
select * from kern.t2 !
fetch erg3 !
commit work release !
