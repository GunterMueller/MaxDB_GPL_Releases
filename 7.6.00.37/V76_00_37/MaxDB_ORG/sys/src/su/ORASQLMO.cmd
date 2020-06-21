*   *ID* ORASQLMO DBCMD    changed on 1992-09-28-13.21.48 by ADMIN     *
file connect ( kern test sqlmode &1 !
*
*   Diese vier SELECTs standen frueher im Testfile SQLMODE. Da aber
*   fuer die sprachabhaengige Darstellung der Monate beim Modus
*   ORACLE ab 3.0.1M die Meldung Nr. 18201 in der SYSMESSAGE-Tabelle
*   benoetigt wird, und diese nur ueber einen RESTART benbutzbar ist,
*   sind alle Tests, die diese Meldung erfordern, jetzt im Test ORADATE
*   zusammengefasst.
*
create table t ( a char (8) )!
insert into t values ('aaa')!
*
* oracle !
select sysdate " *++ date" into :a from t !
*
* sql-db !
create table t1 (s0 char(35), a date)!
insert into t1 values (' *++ date + 3 days  ', sysdate)!
insert into t1 values (' ',                   '19870523')!
*
* db2. Aber auch oracle mit Referenzname 'MONTHS' !
declare c cursor for select s0, a + 3 months from t1 !
fetch c into :a, :b !
close c!
*
* oracle !
declare c cursor for select s0, a + 3 from t1 !
fetch c into :a, :b !
close c!
*
* db2 !
declare c cursor for select s0, a + 3 day from t1 !
fetch c into :a, :b !
close c!
*
commit work release !
file connect (kern test !
drop table t !
drop table t1 !
commit work release !
