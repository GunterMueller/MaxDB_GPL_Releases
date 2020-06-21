file connect ( kern test !
create table t ( a fixed (7))!
insert t set a = 3!
insert t set a = 4 !
select sum(a) into :a from t where sum(a) > 1 !
select sum(a) into :a from t having sum(a) > 10!
select sum(a) into :a from t where a < 6 having sum(a) > 1 !
*Mengenselects als Gegenprobe !
select sum(a) from t where sum(a) > 1 !
fetch!
select sum(a) from t having sum(a) > 10!
fetch!
select sum(a) from t where a < 6 having sum(a) > 1 !
fetch!
*Having mit Subquery, speziell having exist !
select sum(a) from t where a < 6 having sum(a) > (select max(a) from t)!
fetch!
select sum(a) from t
    where  a <= (select max(a) from t)
    having exists (select min(a) from t)!
select sum(a) from t
    where  a <= (select max(a) from t)
    having exists (select a from t where a < 1)!
select sum(a) from t where a < 6 having exists (select max(a) from t)!
fetch!
rollback release !
