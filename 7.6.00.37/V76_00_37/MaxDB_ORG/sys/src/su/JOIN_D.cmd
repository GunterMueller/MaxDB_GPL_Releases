senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* D01SEL
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect (kern test !
create table tab ( s1 fixed(5), s2 fixed(3),s3 fixed(3),s4 char(5))!
insert into tab values (5,1,5,'bla')!
insert into tab values (0,2,5,'arg')!
insert into tab values (3,3,5,'ist')!
insert into tab values (4,4,5,'soll')!
insert into tab values (2,5,5,'wer')!
insert into tab values (1,6,5,'mein')!
create table tab1 (s1 fixed (5),s2 char(5))!
insert into tab1 values (5,'ente')!
commit work release!
file connect ( kern test !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1 order by 1 !
fetch !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1
order by 1 DESC!
fetch !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1 
order by tab.s1 !
fetch !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1 
order by tab.s1 DESC!
fetch !
select tab.s1,tab.s4,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1
order by tab1.s2 !
fetch !
select tab.s1,tab.s4,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1
order by tab1.s2 DESC!
fetch !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1
order by substr(tab.s4,2,2) !
fetch !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1
order by substr(tab.s4,2,2) DESC!
fetch !
commit work release !

file connect ( kern test !
select min(tab.s1),max(tab.s4),tab1.s2 from tab,tab1 where tab.s3 = tab1.s1
group by tab1.s2 !
fetch !
select substr(tab.s4,2,2),min(tab1.s2) from tab,tab1 where tab.s3 = tab1.s1
group by substr(tab.s4,2,2) !
fetch !
commit work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* DML.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t ( a char (8) key )!
insert t set a = 'aaa'!
create table t1 (a char (8) key, b char (8) key, c char (8))!
insert t1 set a = 'aaa', b = 'bbb', c = 'ccc' !
create view v1 as select t.a x, t1.* from t, t1 where t.a = t1.a!
select * into :a from t where a = 'aaa'!
pars_then_ex !
select t.a, t1.a b into :a, :b from t, t t1 where t1.a = 'aaa'!
select * into :a, :b, :c from v1 where a = 'aaa' !
pars_execute !
drop table t !
drop table t1 !
commit release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* some catalog join tests 
file connect ( sut sut !
select taskname, statedesc from sysdd.SYSMON_TASK_DETAIL td 
WHERE 
  td.TASKNAME in ('User', 'Timer') AND 
  NOT td.STATEDESC LIKE 'Connect wait' AND 
  NOT td.STATEDESC LIKE 'Inactive' 
  order by statedesc !
fetch !
select * from sysdd.RUNNING_commands!

explain select /*+ordered*/ taskname, statedesc from 
 sysdd.SYSMON_TASK_DETAIL td, 
 sysdd.RUNNING_commands rp
WHERE 
  rp.TASKID (+) = td.DBPID AND 
  td.TASKNAME in ('User', 'Timer') AND 
  NOT td.STATEDESC LIKE 'Connect wait' AND 
  NOT td.STATEDESC LIKE 'Inactive' 
  order by statedesc !
select /*+ordered*/ taskname, statedesc from 
 sysdd.SYSMON_TASK_DETAIL td, 
 sysdd.RUNNING_commands rp
WHERE 
  rp.TASKID (+) = td.DBPID AND 
  td.TASKNAME in ('User', 'Timer') AND 
  NOT td.STATEDESC LIKE 'Connect wait' AND 
  NOT td.STATEDESC LIKE 'Inactive' 
  order by statedesc !
fetch!

* reverse join order
explain select /*+ordered*/ taskname, statedesc from 
 sysdd.RUNNING_commands rp,
 sysdd.SYSMON_TASK_DETAIL td
WHERE 
  rp.TASKID (+) = td.DBPID AND 
  td.TASKNAME in ('User', 'Timer') AND 
  NOT td.STATEDESC LIKE 'Connect wait' AND 
  NOT td.STATEDESC LIKE 'Inactive' 
  order by statedesc !
select /*+ordered*/ taskname, statedesc from 
 sysdd.RUNNING_commands rp,
 sysdd.SYSMON_TASK_DETAIL td
WHERE 
  rp.TASKID (+) = td.DBPID AND 
  td.TASKNAME in ('User', 'Timer') AND 
  NOT td.STATEDESC LIKE 'Connect wait' AND 
  NOT td.STATEDESC LIKE 'Inactive' 
  order by statedesc !
fetch!
commit work release !


*****************************************************************************
* test for PTS 1132276
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect (kern test !

create table tab( c1 int, c2 int, c3 int,
primary key(c3))!
create unique index i1 on tab(c2)!
insert into tab values(0,0,0)!
insert into tab values(1,1,1)!
insert into tab values(2,2,2)!
insert into tab values(3,3,3)!
insert into tab values(3,4,4)!
insert into tab values(4,5,5)!
insert into tab values(4,6,6)!
insert into tab values(0,7,7)!

explain
select /*+ordered,join(access=L2_more_strategies,
  L2_strat_no_optim,indexaccess)*/ 
 t1.c3 from tab t1, tab t2
where
t1.c3=0 or t1.c3=1 or t1.c3=2 or t1.c3=2 !

select /*+ordered,join(access=L2_more_strategies,
  L2_strat_no_optim,indexaccess)*/ 
 t1.c3 from tab t1, tab t2
where
t1.c3=0 or t1.c3=1 or t1.c3=2 or t1.c3=2 !
fetch!

explain
select /*+ordered,join(;access=L2_more_strategies,
  L2_strat_no_optim,indexaccess)*/ 
 t1.c3 from tab t2, tab t1
where
t1.c3=0 or t1.c3=1 or t1.c3=2 or t1.c3=2 !

select /*+ordered,join(;access=L2_more_strategies,
  L2_strat_no_optim,indexaccess)*/ 
 t1.c3 from tab t2, tab t1
where
t1.c3=0 or t1.c3=1 or t1.c3=2 or t1.c3=2 !
fetch!

commit work release !
*****************************************************************************
* tests for aggregation
file connect ( sut sut !
CREATE TABLE "TP2_DEFINITIONS"
(
    "ID"               Integer    NOT NULL,
    "USERCOUNT"        Integer    NOT NULL,
    PRIMARY KEY ("ID")
)!
insert into "TP2_DEFINITIONS" values (112910, 32)!
insert into "TP2_DEFINITIONS" values (112911, 32)!
insert into "TP2_DEFINITIONS" values (112938, 32)!
insert into "TP2_DEFINITIONS" values (112939, 32)!

CREATE TABLE "TP2_RESULTS"
(
    "DEF_ID"           Integer    NOT NULL,
    "TPS"              Fixed (12,4)    NOT NULL,
    PRIMARY KEY ("DEF_ID")
)!
insert into "TP2_RESULTS" values (112910, 2324.5166)!
insert into "TP2_RESULTS" values (112911, 1801.15)!
insert into "TP2_RESULTS" values (112938, 1952.8334)!
insert into "TP2_RESULTS" values (112939, 2109.4666)!


CREATE TABLE "TP2_SESSIONS_RESULTS"
(
    "DEF_ID"           Integer    NOT NULL,
    "SESSION_ID"       Integer    NOT NULL,
    PRIMARY KEY ("DEF_ID")
)!
insert into "TP2_SESSIONS_RESULTS" values (112910, 2503)!
insert into "TP2_SESSIONS_RESULTS" values (112911, 2503)!
insert into "TP2_SESSIONS_RESULTS" values (112938, 2503)!
insert into "TP2_SESSIONS_RESULTS" values (112939, 2503)!

EXPLAIN SELECT 
AVG(res.tps) AS Tps_avg,
STDDEV(res.tps) AS tps_dev,
COUNT(res.def_id) AS test_count
FROM 
tp2_definitions def, tp2_results res, tp2_sessions_results lnk
WHERE lnk.def_id = def.id AND res.def_id = def.id 
GROUP BY def.usercount !
SELECT 
AVG(res.tps) AS Tps_avg,
STDDEV(res.tps) AS tps_dev,
COUNT(res.def_id) AS test_count
FROM 
tp2_definitions def, tp2_results res, tp2_sessions_results lnk
WHERE lnk.def_id = def.id AND res.def_id = def.id 
GROUP BY def.usercount !
fetch!

rollback work release !

*****************************************************************************
* PTS 1138754
file connect ( kern test !

create table "historico" (
h1 char (8), hts1 timestamp, hts2 timestamp, h2 char (40))!
insert "historico" values ('h1h1h1h1', '20051028114023000000', 
 '20051028114023000000', 
 'h2h2h2h2h2h2h2h2h2h2h2h2h2h2h2h2h2h2h2h2')!
create table "operadores" ( o1 int, ots1 timestamp, o2 char (12))!
insert "operadores" values (1234, '20051028114023000000', 'o2o2o2o2o2o2')!
create view "base_histo1" as 
select * from "historico", "operadores" where hts1 = ots1 !

select * from "historico", "operadores" where hts1 = ots1 !
fetch!
select * from "base_histo1" !
fetch!

rollback work release !
*****************************************************************************

file connect ( sut sut !
drop user kern !
commit work release !
*****************************************************************************
