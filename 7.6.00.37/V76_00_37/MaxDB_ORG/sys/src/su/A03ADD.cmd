*   *ID* A03ADD   DBCMD    changed on 1992-09-28-13.22.16 by ADMIN     *
 ***************************
 ***   A 0 3 A D D       ***
 ***************************!
connect kern IDENTIFIED BY :A !
TEST
insert into kern.p1 set  k1 = 'aaa', n1 = 1000, n2 = 10.2,
         f1 = 'satz1', f2 = 'feld2' !
insert into kern.t1 set   n1 = 1000, n2 = 10.2,
         f1 = 'satz1', f2 = 'feld2' !
commit work  !
insert into kern.p1 set  k1 = 'bbb', n1 = 1000, n2 = 10.2,
         f1 = 'satz2', f2 = 'feld2' !
insert into kern.p1 set  k1 = 'ccc', n1 = 1000, n2 = 10.2,
         f1 = 'satz3', f2 = 'feld2' !
insert into kern.p1 set  k1 = 'eee', n1 = 1000, n2 = 33.2,
         f1 = 'satz4', f2 = 'feld2' !
insert into kern.p1 set  k1 = 'fff', n1 = 1000, n2 = 33.2,
         f1 = 'satz5', f2 = 'feld2' !
commit work  !
lock table    kern.p1, kern.t1 in exclusive mode !
delete from kern.p1 key k1 = 'bbb' !
delete      kern.t1 where (  f1  = 'bbb')!
delete from kern.p1 where (  n2  = 10.2)!
update of  kern.p1  set f1 = 'neu3' key k1 ='ccc' !
update     kern.p1  set f1 = 'qual' where  k1 ='ccc' !
update of  kern.t1  set f1 = 't1qu' where  f2 <> 'ccc' !
commit work release !
