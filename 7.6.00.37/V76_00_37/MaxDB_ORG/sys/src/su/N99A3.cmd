*   *ID* N99A3    DBCMD    changed on 1992-09-28-13.27.20 by ADMIN     *
file connect ( kern test !
create table tab ( spalte char(10) ) !
select * from tab where spalte = 'ALL' !
select * from tab where spalte = ALL !
rollback work release !
