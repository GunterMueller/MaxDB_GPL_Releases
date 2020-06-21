*   *ID* N00T0408 DBCMD    changed on 1992-09-28-13.24.49 by ADMIN     *
 ***       <select direct statement: positioned> !
file connect ( kern test !
create table tab ( spalte1 fixed(5), spalte2 fixed(3) ) !
insert tab values ( 3, 3 ) !
insert tab values ( 5, 5 ) !
declare erg cursor for
select 10 + spalte1 col from tab for update !
fetch1 erg !
rollback work release !
