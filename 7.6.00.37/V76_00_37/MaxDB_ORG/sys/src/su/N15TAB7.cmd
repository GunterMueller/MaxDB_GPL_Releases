*   *ID* N15TAB7  DBCMD    changed on 1992-09-28-13.25.21 by ADMIN     *
file connect ( kern test !
create user d1 password w resource !
create table tab ( spalte1 fixed(5), spalte2 fixed(5) ) !
create synonym syn for tab !
create table newtab like syn !
grant select(spalte1) on tab to d1 !
create view v1 as select * from tab !
create view v2 as select spalte1 from tab !
create table newv1 like v1 !
create table newv2 like v2 !
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
* ADIS 1001449 !
file CONNECT ( KERN TEST !
CREATE TABLE T_like ( A CHAR (8))!
INSERT T_like SET A = 'aaa'!
COMMENT ON COLUMN T_like.a IS 'das soll hier eine lange Spalte sein'!
CREATE TABLE T2_like LIKE T_like !
SELECT * FROM SYSDD.COLUMNS WHERE TABLENAME LIKE 'T%LIKE'!
fetch!
commit release !
