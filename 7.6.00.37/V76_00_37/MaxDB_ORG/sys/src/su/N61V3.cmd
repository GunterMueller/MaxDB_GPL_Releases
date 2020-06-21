*   *ID* N61V3    DBCMD    changed on 1992-09-28-13.26.59 by ADMIN     *
file connect ( kern test !
create table t1 ( t1xc1 char(20),
                  t1xc2 fixed(10),
                  t1xc3 char(10) ascii,
                  t1xc4 fixed(10,5),
                  t1xc5 float(10),
                  t1xc6 fixed(18) ) !
insert t1 values ( 'Hans', 100, 'Muenchen', 100.100, null,
                    500000000000000000 ) !
insert t1 values ( null, null, 'Hamburg', 0.00005, 8E-30, -2000 ) !
insert t1 values ( 'Karin', -600000, null, null, -1E12,
                    800000000000000000 ) !
insert t1 values ( 'Karin', 5, 'Bonn', -700, 1000, null ) !
insert t1 values ( 'Hans', -500000, 'Bremen', 700.00005, null, 300 ) !
insert t1 values ( ' Marianne', -80, 'Koeln 1', -300, 10e-10, null ) !
create table t2 ( t2xc1 fixed(5) key,
                  t2xc2 char(10) ascii,
                  t2xc3 char(15) ascii,
                  t2xc4 float(5),
                  t2xc5 fixed(5) range between -50000 and +50000 ) !
insert t2 values ( 10, 'Klaus', 'Mueller', 1E20, 20000 ) !
insert t2 values ( 20, 'Elena', 'Meier', -5E10, -100 ) !
insert t2 values ( 30, null, 'Lehmann', null, null ) !
insert t2 values ( 40, 'Paul', 'Anse Rauch', 8 , -50000 ) !
create table t3 ( t3xc1 fixed(2,1 ) key,
                  t3xc2 char(20) key ,
                  t3xc3 float( 5) ,
                  t3xc4 char(20) ascii range between 'A' and 'G' ,
                  t3xc5 fixed(18) ) !
insert t3 values ( 0.1, 'Frankreich', 80E50, 'Amsel',
                   700000000000000000 ) !
insert t3 values ( 2.5, 'Belgien', 1E-30, 'Fink', null ) !
insert t3 values ( 3.0, 'Italien', -5E30, 'Amsel', -1000 ) !
insert t3 values ( 4.1, 'VR Polen', null, 'Fink', +200 ) !
create view v1 as
   select sum ( t1.t1xc2 ) , sum ( t2.t2xc1 ) from t1, t2 !
select * from v1 !
fetch !
create view v2 as
select count (*), count ( distinct kern.t2.t2xc2 ),
       count ( all ( t3xc5)) from t1,t2,t3 !
select * from v2 !
fetch !
create view v3 as
select avg ( distinct t2.t2xc1 ) ,
       count ( distinct t2.t2xc1 ) ,
       avg ( all kern.t1.t1xc5 ) ,
       sum ( all t1xc5 ) ,
       count ( all ( t1xc5 ) ) from t1,t2 !
select * from v3 !
fetch !
create view v4 as
select sum ( ref.t1xc6 ) , AVG ( ref.t1xc4) from t1,t1 ref !
select * from v4 !
fetch !
create view v5 as
select sum ( t1xc4 ), avg ( t1xc4), t1xc1, count(t1xc1)
    from t1,t2 group by t1xc1 !
select expression1, expression2, t1xc1 || ' aa', expression3 + 1
   from v5 !
fetch !
create view v6 as
select sum ( t1xc4 ), sum ( t2xc4)
    from t1, t2 ref
    group by ref.t2xc1
    having ref.t2xc1 > 10
       and sum ( ref.t2xc4) is not null !
declare erg1 cursor for 
select expression1, max ( expression2 ) exp1 from v6
   where expression1 <> v6.expression2
   group by expression1
   having count(*) = 2 !
fetch erg1 !
create view v7 as
select avg ( t1xc2), avg ( distinct t1xc5 ) , t1.t1xc5
    from t1, t2
    group by t1xc1 , kern.t1.t1xc5
    having sum ( t1xc5 ) > 0 !
select * from v7 !
fetch !
create view v8 as
select sum( t1xc2) , count ( t1xc2) , avg (t1xc2)
    from t1,t3 group by t1xc1, t3.t3xc4 !
select * from v8 where expression2 < 3 order by 1 !
fetch !
create view v9 as
select sum ( t1.t1xc2 + t2.t2xc1 ),
       avg ( t1xc4 * 5 * t2xc5 ),
       ( sum ( distinct kern.t1.t1xc4 / t2xc5 ) )
    from t1, t2 group by t2xc1 !
select * from v9 !
fetch !
create table i (i1 float (14), i2 float (14), i3 fixed(18))!
create view v10 as
select sum ( all t1.t1xc5 - t2xc4*t3xc1),
       avg ( all  (t1.t1xc5  )- t2xc4 * t3xc1 ) ,
       count (all ( t2xc4 * t3xc1 *((-1))+ t1.t1xc5 )) col3
    from t1,t2, t3
    group by t3xc4 !
select * from v10 !
fetch !
insert i select * from v10 !
select * from i !
fetch !
declare erg3 cursor for
select min (v9.expression1) n1, v10.expression2 n2
   from v9, v10
   where v9.expression2 is not null or v9.expression3 < 0
   group by v10.expression1, v10.expression2
   having sum (v9.expression2) - sum (v10.expression2) > 0
   order by 2 desc !
select * from erg3 !
fetch erg3 !
declare erg4 cursor for select n1 from erg3 !
fetch erg4 !
select * from v10 !
fetch!
select * from t1 !
fetch !
select v10.expression1, t1.t1xc1, t1.t1xc2 from v10, t1 !
fetch !
pars_then_ex !
select v10.expression1, t1.t1xc1, t1.t1xc2 from v10, t1 !
fetch into :a, :b, :c!
pars_execute !
rollback work release !
