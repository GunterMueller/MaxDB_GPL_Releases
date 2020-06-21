*   *ID* P03TEST  DBCMD    changed on 1992-09-28-13.27.43 by ADMIN     *
 ***************************
 ***   P 0 3 T E S T     ***
 ***************************!
alter table kern.abt add  neu char (4) !
alter table kern.abt add  neu char (4) !
alter table kern.abt drop  neu !
alter table kern.abt add  neu1 fixed (3),
                      neu2 char (5),
                      neu3 char (1) !
commit work  !
file sel_tab_owner ( KERN !
file sel_cols_1tab ( KERN ABT !
file sel_cols_1tab ( KERN LIEFERANT !
file sel_cols_1tab ( KERN LNUM !
alter table kern.abt alter constraint
 num2 check num2 between 1000 and 8000!
alter table kern.abt alter constraint num2 check
 num2 in (1000, 8000) !
commit work !
insert into kern.abt set name = 'ABC', num2 = 8000 !
commit work !
alter table kern.abt alter constraint num2
      check num2 in (1000, 5300) !
alter table kern.abt alter constraint num2
      check num2 between 1000 and 7000 !
commit work  !
