*   *ID* N99A16   DBCMD    changed on 1992-09-28-13.27.13 by ADMIN     *
file connect ( sut sut !
create user d1 password w resource !
commit release !
file connect ( kern test !
create table tab ( spalte1 fixed(5), spalte2 fixed(5) ) !
create synonym syn for tab !
create table newtab like syn !
grant select(spalte1) on tab to d1 !
create view v1 as select * from tab !
create view v2 as select spalte1 from tab !
create table newv1 like v1 !
create table newv2 like v2 !
file sel_cols_tab ( NEWV1 !
file sel_cols_tab ( NEWV2 !
file sel_table !
commit work release !
file connect ( d1 w !
create synonym syn for kern.tab !
file sel_cols_1tab ( KERN TAB !
create table t1 like kern.tab !
create table t2 like syn !
file sel_table !
file sel_cols_tab ( T1 !
file sel_cols_tab ( T2 !
rollback work release !
file connect ( sut sut !
drop user d1 !
commit release !
file connect ( kern test !
drop table tab !
drop table newtab!
drop table newv1 !
drop table newv2 !
commit release !
