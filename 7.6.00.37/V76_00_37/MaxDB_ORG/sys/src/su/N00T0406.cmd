*   *ID* N00T0406 DBCMD    changed on 1992-09-28-13.24.48 by ADMIN     *
 ***  4.8.2  Numerierung mittels ROWNO !
file connect ( kern test !
create table tab ( spalte fixed(5) ) !
insert tab values ( 3 ) !
insert tab values ( 4 ) !
insert tab values ( 6 ) !
select spalte, rowno from tab where spalte < 5 !
fetch !
rollback work release !
