*   *ID* N00T0401 DBCMD    changed on 1992-09-28-13.24.46 by ADMIN     *
 *** 4.5  <select direct statement> !
file connect ( kern test !
create table t1 ( spalte1 char(10) key,
                  spalte2 fixed(5) key,
                  spalte3 fixed(5) ) !
create synonym syn for t1 !
insert t1 values ( 'Hans', 5, 1 ) !
insert t1 values ( 'Paul', 4, null ) !
insert t1 values ( 'Hans', 2, 7 ) !
rollback work !
create table t2 ( col1 char(10), col2 fixed(5) ) !
insert t2 values ( 'Hamburg', 7 ) !
insert t2 values ( null, 5 ) !
insert t2 values ( 'Bonn', 1 ) !
* select syskey, * from t2 !
* fetch !
rollback work release !
