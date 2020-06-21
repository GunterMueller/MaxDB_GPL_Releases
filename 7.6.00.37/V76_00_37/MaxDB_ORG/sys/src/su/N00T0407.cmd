*   *ID* N00T0407 DBCMD    changed on 1992-09-28-13.24.49 by ADMIN     *
 ***  4.7  <explain statement> !
file connect ( kern test !
create table t1 ( t1xc1 char(5) ascii key,
                  t1xc2 char(5) key,
                  t1xc3 fixed(5) ) !
create table t2 ( t2xc1 char(5) ascii key,
                  t2xc2 fixed(5),
                  t2xc3 char(8),
                  t2xc4 fixed(6,1) not null ) !
create index i1 on t2 ( t2xc4 asc, t2xc3 desc ) !
create index i2 on t2 ( t2xc2 asc ) !
create index "t1.t1xc3"
	on t1( t1xc3 desc ) !
insert t1 values ( 'cc', 'ffff', 2 ) !
insert t1 values ( 'aa', 'ssss', null ) !
insert t2 values ( 'aa', 5, 'dddd', 3 ) !
insert t2 values ( 'dd', 3, 'ffff', 2 ) !
explain select * from t1, t2 where t1xc1 = t2xc1 and t1xc3 < t2xc4 !
explain select * from t2 where t2xc1 > 'aa'
     or t2xc2 between 1 and 5 order by t2xc4 asc, t2xc3 desc !
rollback work release !
