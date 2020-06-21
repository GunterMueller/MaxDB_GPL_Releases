*   *ID* N99K3    DBCMD    changed on 1992-09-28-13.27.28 by ADMIN     *
file connect ( kern test !
create table tab ( spalte1 fixed(5), spalte2 fixed(5) ) !
insert into tab values ( 1, 1 ) !
insert into tab values ( 1, 1 ) !
create unique index "tab.spalte2"
	on tab( spalte2 ) !
rollback work release !
