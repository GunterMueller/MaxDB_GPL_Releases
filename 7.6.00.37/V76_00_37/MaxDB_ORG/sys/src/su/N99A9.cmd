*   *ID* N99A9    DBCMD    changed on 1992-09-28-13.27.23 by ADMIN     *
file connect ( kern test !
CREATE TABLE T1 ( a date)!
CREATE TABLE T2( A CHAR (5))!
CREATE TABLE T3( A CHAR (6))!
CREATE TABLE T4 ( A CHAR (12) )!
CREATE TABLE T5 ( A time )!
insert t1 set a = time !
insert t5 set a = date !
PARS_THEN_EX !
INSERT T1 SET A = DATE !
INSERT T2 SET A = DATE !
INSERT T3 SET A = DATE !
INSERT T4 SET A = DATE !
INSERT T5 SET A = DATE !
INSERT T1 SET A = time !
INSERT T2 SET A = TIME !
INSERT T3 SET A = TIME !
INSERT T4 values (TIME ) !
INSERT T5 values (TIME ) !
pars_execute !
select * from t1 !
fetch !
rollback work release !
