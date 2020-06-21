*   *ID* N00T0807 DBCMD    changed on 1992-09-28-13.24.56 by ADMIN     *
 *** 8.11 <create synonym statement>
 *** 8.12 <drop synonym statement>  !
file connect ( kern test !
create table tabelle ( spalte1 char(5) key ,
                       spalte2 char(5) byte key ) !
create synonym syntab1 for  kern.tabelle !
create view viewtab as select spalte1 from tabelle where spalte1 < 'B' !
create synonym syntab_2 for viewtab !
drop synonym syntab1 !
drop synonym syntab_2 !
rollback work release !
