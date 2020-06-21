*   *ID* N34STDDE DBCMD    changed on 1992-09-28-13.26.17 by ADMIN     *
file connect ( kern test !
create table t ( a fixed (5), b char (245), c char (40))!
select variance (a), stddev (a),
variance (distinct a), stddev (distinct a) from t !
fetch !
create table t1 ( a1 float (18) )!
insert t1 select variance(a) from t !
insert t set a = null !
select avg(a) from t!
select variance (a), stddev (a),
variance (distinct a), stddev (distinct a) from t !
fetch !
insert t set a = null !
select variance (a), stddev (a),
variance (distinct a), stddev (distinct a) from t !
fetch !
insert t set a = 10 !
select variance (a), stddev (a),
variance (distinct a), stddev (distinct a) from t !
fetch !
insert t set a = 20 !
insert t set a = 30 !
insert t set a = 15 !
insert t set a = 27 !
select variance (a), stddev (a),
variance (distinct a), stddev (distinct a) from t !
fetch !
insert t set a = 30 !
select variance (a), stddev (a),
variance (distinct a), stddev (distinct a) from t !
fetch !
select variance (a), stddev (a),
variance (distinct a), stddev (distinct a) from t where a = 30 !
fetch !
select variance (a), variance (distinct a) from t !
fetch !
select variance (a) from t group by b !
update t set c = 'abc' where a <= 20 !
update t set c = 'def' where a >  20 !
select variance (a) from t group by c !
fetch !
select variance (a / (a-15)) from t group by c !
fetch !
commit !
create table t2 (a float (3) )!
insert t2 set a = 0 !
insert t2 set a = 2e32 !
select avg(a), stddev(a) from t2 !
fetch !
delete t2 !
insert t2 set a = 0 !
insert t2 set a = 0 !
insert t2 set a = 4e31 !
insert t2 set a = 4e31 !
select avg(a), stddev(a) from t2 !
select avg(a), stddev(a) from t2 where a = 0 !
fetch!
drop table t !
drop table t1 !
drop table t2 !
commit release !
