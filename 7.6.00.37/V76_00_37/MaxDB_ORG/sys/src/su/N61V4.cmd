*   *ID* N61V4    DBCMD    changed on 1992-09-28-13.27.00 by ADMIN     *
file connect ( kern test !
create table t1 ( t1xc1 fixed(5) key,
                  t1xc2 char(5),
                  t1xc3 char(5) ) !
create table t2 ( t2xc1 fixed(5),
                  t2xc2 fixed(5),
                  t2xc3 char(5) ) !
insert t1 values ( 2, 'aaa' , 'sss' ) !
insert t1 values ( 3, 'nnn' , 'nnn' ) !
insert t1 values ( 4, 'qqq' , 'aaa' ) !
insert t1 values ( 9, 'aaa' , 'bbb' ) !
insert t1 values ( 6, 'ccc' , 'bbb' ) !
insert t1 values ( 8, 'aaa' , 'bbb' ) !
insert t1 values ( 5, null  , null  ) !
insert t1 values ( 1, 'aaa' , 'sss' ) !
insert t2 values ( 3, 2, 'aaa' ) !
insert t2 values ( 4, 1, 'nnn' ) !
insert t2 values ( 4, 2, 'nnn' ) !
insert t2 values ( 8, null, 'bbb' ) !
insert t2 values ( 1, 4, 'sss' ) !
insert t2 values ( null, 2, 'bbb' ) !
insert t2 values ( 5, 1, null ) !
insert t2 values ( 1, 8, 'aaa' ) !
insert t2 values ( 1, 4, 'bbb' ) !
insert t2 values ( 3, 2, 'nnn' ) !
insert t2 values ( 2, 1, 'sss' ) !
insert t2 values ( 3, 2, 'bbb' ) !
insert t2 values ( 2, 1, 'bbb' ) !
create view v1 as
  select distinct sum ( distinct 5 + t2xc1 ) + 2 * sum ( t1xc1 ),
                  count ( t2xc1 ),
                  max ( t1xc2 ),
                  t2xc2
  from t1, t2
  where t1xc3 = t2.t2xc3
    and t1xc1 > t2xc1 - 1
  group by t2xc2, t1xc2
  having min ( t2xc1 ) < max ( all t1xc1 )
     and avg ( distinct t1.t1xc1 ) + min ( t1xc1 ) <= kern.t2.t2xc2 !
select * from v1 !
fetch !
select v1.t2xc2, ref.t2xc2, v1.t2xc2 + ref.t2xc2 from v1, v1 ref
   where v1.t2xc2 = ref.t2xc2 !
fetch !
rollback work !
* SELECT INTO pars/execute with COMPLEX VIEW hat CRASH March 95 !
create table t ( a char (8))!
insert t set a = 'aaa'!
create table t1 (b char (8))!
create view v1 as select * from t,t1
   where t.a = t1.b(+) !
select * from v1 !
fetch!
select * into :a, :b from v1 where v1.a = 'aaa'!
pars_then_ex !
select * from v1 !
fetch!
select * into :a, :b from v1 where a = 'aaa'!
pars_execute!
rollback release !
