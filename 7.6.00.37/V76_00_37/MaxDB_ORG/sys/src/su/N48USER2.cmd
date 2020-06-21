*   *ID* N48USER2 DBCMD    changed on 1992-09-28-13.26.40 by ADMIN     *
file connect ( kern test !
create user p1 password w !
create usergroup gr !
create user grp1 password w usergroup gr !
commit work release !
file connect ( kern test !
create table tab ( spalte char(8) ) !
insert tab values ( 'P1' ) !
insert tab values ( 'GR' ) !
insert tab values ( 'GRP1' ) !
grant all on tab to p1, gr !
commit work release !
file connect ( p1 w !
select * from kern.tab where spalte = USER !
fetch !
select * from kern.tab where spalte = USERGROUP !
fetch !
commit work release !
file connect ( grp1 w !
select * from kern.tab where spalte = USER !
fetch !
select * from kern.tab where spalte = USERGROUP !
fetch !
rollback work release !
