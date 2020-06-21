*   *ID* N33COUN1 DBCMD    changed on 1992-09-28-13.26.15 by ADMIN     *
select  COUNT ( * ), ( COUNT(*) ) from tab !
fetch !
select count(*) from tab where spalte1 is null or  spalte2 is null !
fetch !
select count ( spalte3+ tab.spalte4  ) , count (* ) from tab !
fetch !
select count(5),(count(all 5 ) ) , count (distinct (5) ) from tab !
fetch !
select ( ( count (5) )), count ( spalte2) from tab !
fetch !
select count(spalte1 ),count (tab.spalte2 ), count( kern.tab.spalte3)
    from tab !
fetch !
select count ( tab.spalte1 ), COUNT ( 2*3 ) , count (spalte4)
    from tab where spalte1 <> 'a' !
fetch !
select count(DISTINCT spalte3 + 1E9)
    from tab where spalte1 = 'a' or spalte1 = 'b' !
fetch !
select count ( ALL tab.spalte3 ), count(spalte4)
    from tab where spalte3 is null !
fetch !
select count((*)) from tab !
select count ( count ( spalte2) ) from tab !
select 5 , count ( *) from tab !
select * from tab where count(*) = 9 !
drop table tab !
 * check, if count(x) if len(x)>10 gives correct bufpos for
 * next output column. Error till 23.7.93 !
CREATE TABLE T ( A CHAR (1) , B CHAR (10))!
INSERT T VALUES ('a', 'bbbbbbbbbb')!
INSERT T VALUES ('b', 'cccccccccc')!
SELECT COUNT (DISTINCT A||B), COUNT (A||B), COUNT(DISTINCT A)
INTO :A, :B, :C FROM T !
CREATE TABLE T1 ( C CHAR (1), D CHAR (10))!
INSERT T1 VALUES ('d', 'eeeeeeeeee')!
INSERT T1 VALUES ('e', 'ffffffffff')!
create table t2 ( e char (11))!
insert t2 set e = 'eeeeeeeeeee' !
SELECT COUNT (DISTINCT A||B), COUNT (A||B), COUNT(DISTINCT A)
INTO :A, :B, :C FROM T,T1 !
SELECT COUNT (DISTINCT A||D), COUNT (A||B), COUNT(DISTINCT A)
INTO :A, :B, :C FROM T,T1 !
pars_then_ex !
SELECT COUNT (DISTINCT A||B), COUNT (A||B), COUNT(DISTINCT A)
INTO :A, :B, :C FROM T !
SELECT COUNT (DISTINCT A||B), COUNT (A||B), COUNT(DISTINCT A)
 FROM T !
FETCH INTO :A, :B, :C !
SELECT COUNT (DISTINCT A||B), COUNT (A||B), COUNT(DISTINCT A)
INTO :A, :B, :C FROM T,T1 !
select count (distinct e), count (e) into :a, :b from t2 !
SELECT COUNT (DISTINCT A||D), COUNT (A||B), COUNT(DISTINCT A)
INTO :A, :B, :C FROM T,T1 !
pars_execute !
drop table t !
drop table t1 !
create table kunde ( ort char (25), b char (8))!
insert kunde values ('Berlin', 'aaa')!
insert kunde values ('Koeln', 'bbb')!
insert kunde values ('Berlin', 'ccc')!
select count (ort) from kunde group by ort !
mfetch !
6
select b, count (ort) from kunde group by b !
mfetch !
6
rollback work !
