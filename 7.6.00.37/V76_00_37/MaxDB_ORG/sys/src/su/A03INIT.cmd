*   *ID* A03INIT  DBCMD    changed on 1992-09-28-13.22.18 by ADMIN     *
 ***************************
 ***   A 0 3 I N I T     ***
 ***************************!
file connect ( kern test !
create table kern.p1  (k1 char (3) key,
   f1 char(5), f2 char(5), n1 fixed (4), n2 fixed (4,2)) !
create table kern.t1
  (f1 char(5) , f2 char(5), n1 fixed (4), n2 fixed (4,2)) !
file sel_cols_1tab ( KERN P1 !
commit work release !
