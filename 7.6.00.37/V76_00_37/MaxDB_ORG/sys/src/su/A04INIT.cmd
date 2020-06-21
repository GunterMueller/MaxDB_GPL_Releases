*   *ID* A04INIT  DBCMD    changed on 1992-09-28-13.22.23 by ADMIN     *
 ***************************
 ***   A 0 4 I N I T     ***
 ***************************!
file connect ( kern test !
create table kern.p1  (k1 char (3) key,
  f1 char(5), f2 char(5), n1 fixed (4), n2 fixed (4,2)) !
commit  work  !
file sel_cols_1tab ( KERN P1 !
insert into kern.p1 set  k1 = 'aaa', n1 = 1000, n2 = 10.2,
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
lock table    kern.p1 in exclusive mode !
delete from kern.p1  key k1 = 'bbb' !
update of  kern.p1  set f1 = 'neu3' key k1 ='ccc' !
commit work release !
