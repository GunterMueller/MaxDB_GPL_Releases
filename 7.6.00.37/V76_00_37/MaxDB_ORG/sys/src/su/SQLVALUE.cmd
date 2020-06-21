*   *ID* SQLVALUE DBCMD    changed on 1992-08-11-10.36.57 by KERN      *
file connect ( kern test sqlmode &1 !
create table t ( a char (8) )!
insert into t values ('aaa')!
* all !
select user              into :a from t !
* sql-db !
select usergroup         into :a from t !
select sysdba            into :a from t !
select date              into :a from t !
select time              into :a from t !
select timezone          into :a from t !
* db2 !
select current sqlid     into :a from t !
select current date      into :a from t !
select current time      into :a from t !
select current timestamp into :a from t !
select current timezone  into :a from t !
* oracle !
*
*   Die vier SELECTs, die mit dem aktuellen Datum arbeiten, stehen
*   jetzt im ORASQLMO DBCMD, aufgerufen von ORADATE VDNTEST
*   Fuer die sprachabhaengige Darstellung der Monate beim Modus ORACLE
*   wird ab 3.0.1M die Meldung Nr. 18201 in der SYSMESSAGE-Tabelle
*   benoetigt, und diese ist nur ueber einen RESTART benbutzbar. Deshalb
*   sind alle Tests, die diese Meldung erfordern, jetzt im Test ORADATE
*   zusammengefasst.
*
* select sysdate           into :a from t !
*
* sql-db !
update t set a = default !
update t set a = stamp !
create table t1 ( a date)!
insert into t1 values (sysdate)!
insert into t1 values ('19870523')!
* * db2. Aber auch oracle mit Referenzname 'MONTHS' !
* declare c cursor for select a + 3 months from t1 !
* fetch c into :a !
* close c!
* * oracle !
* declare c cursor for select a + 3 from t1 !
* fetch c into :a !
* close c!
* * db2 !
* declare c cursor for select a + 3 day from t1 !
* fetch c into :a !
* close c!
commit work release !
file connect (kern test !
drop table t !
drop table t1 !
commit work release !
