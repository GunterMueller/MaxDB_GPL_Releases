*   *ID* CORR6    DBCMD    changed on 1992-09-28-13.23.03 by ADMIN     *
file connect ( kern test !
* Fehlerprotokoll 173 !
create table t ( a fixed (8)key )!
insert t set a = 1 !
insert t set a = 2 !
insert t set a = 3 !
insert t set a = 5 !
insert t set a = 6 !
insert t set a = 7 !
insert t set a = 8 !
insert t set a = 9 !
insert t set a = 10!
create table t1 ( b fixed (8) key )!
insert t1 set b = 3 !
insert t1 set b = 5 !
insert t1 set b = 7 !
insert t1 set b = 9 !
select a+1 from t x where not exists (select * from t where
          t.a = x.a+1 and t.a > x.a  and a <= 9) and a <= 9 !
fetch !
select a+1 from t x where not exists
          (select a from t,t1
          where
               t.a = x.a+1 and t.a > x.a
           and a = b
           and a <= 9 and b <= 9)
           and a <= 9 !
fetch !
pars_then_ex !
data !
select a+1 from t x where not exists (select * from t where
          t.a = x.a+1 and t.a > x.a  and a <= :a) and a <= :b !
n 8 9 n 8 9

nodata !
fetch into :a !
data !
select a+1 from t x where not exists
          (select a from t,t1
          where
               t.a = x.a+1 and t.a > x.a
           and a = b
           and a <= :a and b <= :a)
           and a <= :a !
n 8 9 n 8 9 n 8 9

nodata !
fetch into :a !
pars_execute !
******** !
select a-1 from t x where not exists (select * from t where
          t.a = x.a-1 and t.a <= x.a) and a <= 9 !
fetch !
select a-1 from t x where not exists
          (select a from t,t1
          where
               t.a = x.a-1 and t.a <= x.a
           and a = b
           )
           and a <= 9 !
fetch !
pars_then_ex !
data !
select a-1 from t x where not exists (select * from t where
          t.a = x.a-1 and t.a <= x.a) and a <= :b !
n 8 9

nodata !
fetch into :a !
data !
select a-1 from t x where not exists
          (select a from <t,t1>
          where
               t.a = x.a-1 and t.a <= x.a
           and a = b
           )
           and a <= :a !
n 8 9

select a-1 from t x where not exists
          (select a from <t1,t>
          where
               t.a = x.a-1 and t.a <= x.a
           and a = b
           )
           and a <= :a !
n 8 9

nodata !
fetch into :a !
pars_execute !
******** !
select a+1 from t x where exists (select * from t where
          t.a = x.a and a <= 9) and a <= 9 !
fetch !
select a+1 from t x where not exists
          (select a from t,t1
          where
               t.a = x.a
           and a = b
           and a <= 9 and b <= 9)
           and a <= 9 !
fetch !
pars_then_ex !
data !
select a+1 from t x where exists (select * from t where
          t.a = x.a and a <= :a) and a <= :b !
n 8 9 n 8 9

nodata !
fetch into :a !
data !
select a+1 from t x where not exists
          (select a from t,t1
          where
               t.a = x.a
           and a = b
           and a <= :a and b <= :a)
           and a <= :a !
n 8 9 n 8 9 n 8 9

nodata !
fetch into :a !
pars_execute !
******** !
create table t2 ( a fixed (8) )!
create index "t2.a"
	on t2( a ) !
insert t2 set a = 3 !
insert t2 set a = 5 !
insert t2 set a = 7 !
insert t2 set a = 9 !
******** !
select a+1 from t x where not exists (select * from t2 where
          t2.a = x.a+1 and t2.a > x.a  and a <= 9) and a <= 9 !
fetch !
select a+1 from t x where not exists
          (select a from <t2,t1>
          where
               t2.a = x.a+1 and t2.a > x.a
           and a = b
           and a <= 9 and b <= 9)
           and a <= 9 !
select a+1 from t x where not exists
          (select a from <t1,t2>
          where
               t2.a = x.a+1 and t2.a > x.a
           and a = b
           and a <= 9 and b <= 9)
           and a <= 9 !
fetch !
pars_then_ex !
data !
select a+1 from t x where not exists (select * from t2 where
          t2.a = x.a+1 and t2.a > x.a  and a <= :a) and a <= :b !
n 8 9 n 8 9

nodata !
fetch into :a !
data !
select a+1 from t x where not exists
          (select a from <t2,t1>
          where
               t2.a = x.a+1 and t2.a > x.a
           and a = b
           and a <= :a and b <= :a)
           and a <= :a !
n 8 9 n 8 9 n 8 9

select a+1 from t x where not exists
          (select a from <t1,t2>
          where
               t2.a = x.a+1 and t2.a > x.a
           and a = b
           and a <= :a and b <= :a)
           and a <= :a !
n 8 9 n 8 9 n 8 9

nodata !
fetch into :a !
pars_execute !
******** !
select a-1 from t x where not exists (select * from t2 where
          t2.a = x.a-1 and t2.a <= x.a) and a <= 9 !
fetch !
select a-1 from t x where not exists
          (select a from <t2,t1>
          where
               t2.a = x.a-1 and t2.a <= x.a
           and a = b
           )
           and a <= 9 !
select a-1 from t x where not exists
          (select a from <t1,t2>
          where
               t2.a = x.a-1 and t2.a <= x.a
           and a = b
           )
           and a <= 9 !
fetch !
pars_then_ex !
data !
select a-1 from t x where not exists (select * from t2 where
          t2.a = x.a-1 and t2.a <= x.a) and a <= :b !
n 8 9

nodata !
fetch into :a !
data !
select a-1 from t x where not exists
          (select a from <t2,t1>
          where
               t2.a = x.a-1 and t2.a <= x.a
           and a = b
           )
           and a <= :a !
n 8 9

select a-1 from t x where not exists
          (select a from <t1,t2>
          where
               t2.a = x.a-1 and t2.a <= x.a
           and a = b
           )
           and a <= :a !
n 8 9

nodata !
fetch into :a !
pars_execute !
******** !
select a+1 from t x where exists (select * from t2 where
          t2.a = x.a and a <= 9) and a <= 9 !
fetch !
select a+1 from t x where not exists
          (select a from t2,t1
          where
               t2.a = x.a
           and a = b
           and a <= 9 and b <= 9)
           and a <= 9 !
fetch !
pars_then_ex !
data !
select a+1 from t x where exists (select * from t2 where
          t2.a = x.a and a <= :a) and a <= :b !
n 8 9 n 8 9

nodata !
fetch into :a !
data !
select a+1 from t x where not exists
          (select a from t2,t1
          where
               t2.a = x.a
           and a = b
           and a <= :a and b <= :a)
           and a <= :a !
n 8 9 n 8 9 n 8 9

nodata !
fetch into :a !
pars_execute !
drop table t !
drop table t1 !
drop table t2 !
commit release !
