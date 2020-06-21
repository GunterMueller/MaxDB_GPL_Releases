*   *ID* N99A14   DBCMD    changed on 1992-09-28-13.27.12 by ADMIN     *
file connect ( kern test !
create table tab ( spalte1 fixed(5) ) !
insert tab values ( 1 ) !
create synonym syn for tab !
alter table syn add ( spalte2 char(5) ) !
alter table tab add ( spalte2 char(5) ) !
select * from tab !
fetch !
rename table syn to tab2 !
rename column syn.spalte1 to col1 !
rollback work release !
