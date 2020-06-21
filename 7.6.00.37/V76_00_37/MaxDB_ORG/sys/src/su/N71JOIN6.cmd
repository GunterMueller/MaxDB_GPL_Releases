*   *ID* N71JOIN6 DBCMD    changed on 1992-09-28-13.27.09 by ADMIN     *
file connect ( kern test !
* join-like/-sounds ohne kartesisches Produkt vor der like-Pruefung !
create table t ( a char (8) ascii)!
insert t set a = 'aaa'!
insert t set a = 'bbb'!
insert t set a = 'ccc'!
select a.a, b.a from t a, t b where a.a like b.a !
fetch !
select a.a, b.a from t a, t b where a.a like b.a and b.a like a.a !
fetch !
select a.a, b.a from t a, t b where a.a like b.a and a.a > b.a !
fetch !
select a.a, b.a from t a, t b where a.a sounds b.a !
fetch !
select distinct a.a, b.a from t a, t b where a.a like b.a order by a.a!
fetch !
select distinct a.a, b.a from t a, t b where a.a sounds b.a order by b.a!
fetch !
select a.a, b.a from t a, t b where b.a like a.a !
fetch !
select a.a, b.a from t a, t b where b.a sounds a.a !
fetch !
select distinct a.a, b.a from t a, t b where b.a like a.a order by a.a!
fetch !
select distinct a.a, b.a from t a, t b where b.a sounds a.a order by b.a!
fetch !
rollback release !
