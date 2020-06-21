*   *ID* N00T0501 DBCMD    changed on 1992-09-28-13.24.50 by ADMIN     *
 *** 5.1  <insert statement>  !
file connect ( sut sut !
create user superdbi password super dba !
commit release !
file connect ( superdbi super !
create table superdbi.t3
   ( t3xc1 char(64) key,
     t3xc2 fixed(5,2) range between -150.00 and +150.00 ,
     t3xc3 float(3) not null ) !
create table t7 ( t7xc1 char(64) key, t7xc2 fixed(4) ) !
alter table t7 foreign key t3lt7 (t7xc1) references superdbi.t3 !
insert superdbi.t3 values ( 'Hans', 2.27, 55e-33 ) !
insert into superdbi.t3 values ( user, null , 003E+8 ) !
insert superdbi.t3  (t3xc1,t3xc3) values ('Otto',-33 ) !
insert into  superdbi.t3 ( t3xc1, t3xc3 )
   values ( 'Hans-Bernd', 0.017 ) !
insert superdbi.t3 set t3xc2 = null,
                     t3xc1 = 'Paul',
                     t3xc3 = -.27e12 !
select substr(t3xc1,1,20) t3xc1, t3xc2, t3xc3 from superdbi.t3 !
fetch !
create table superdbi.t4 ( t4xc1 char(5) byte,
                         t4xc2 char(10) not null,
                         t4xc3 char(10) ) !
insert superdbi.t4 values ( x'd7c1e4d3', 'Joachim', 'Paul' ) !
create table superdbi.t5 ( t5xc1 float(5) default 100,
                         t5xc2 char(10) not null ) !
insert superdbi.t5 set t5xc2 = 'Paul' !
insert superdbi.t5 values ( 88e5, 'Susanne' ) !
insert superdbi.t4 (t4xc2) select t3.t3xc1 from superdbi.t3
                         where t3.t3xc2 is null !
select * from superdbi.t4 !
fetch !
create table superdbi.t6 ( t6xc1 char(10),
                         t6xc2 fixed(5,2),
                         t6xc3 float(3)  ) !
insert into superdbi.t6 select distinct *
                      from superdbi.t3 where t3xc1<>user !
select * from superdbi.t6 order by 1,3!
fetch !
insert superdbi.t6 (t6xc1,t6xc3 ) select all t4.t4xc2, t3.t3xc3
   from superdbi.t4,superdbi.t3
   where t3.t3xc1 <> t4.t4xc2
     and t3.t3xc3 = -33 !
select * from superdbi.t6 order by 1,3 !
fetch !
create synonym synt6 for superdbi.t6 !
insert synt6 ( t6xc1, t6xc3 )
   select distinct t4.t4xc3 neu1, t3.t3xc3
     from superdbi.t4, superdbi.t3
     where t3.t3xc1 <> t4.t4xc2
       and t4.t4xc3 is  not null
       and t3.t3xc3 > 0.0 !
select * from superdbi.t6 order by 1,3!
fetch !
create view superdbi.view_t3 as select *
                              from superdbi.t3 where t3xc1 <> user !
insert superdbi.view_t3 values ( 'Karin', 100, -333 ) !
insert superdbi.view_t3 ( t3xc1, t3xc3 )
   select t6xc1 , synt6.t6xc3 from synt6
    where t6xc1 like 'J%' !
select substr(t3xc1,1,20) t3xc1, t3xc2, t3xc3 from superdbi.t3 !
fetch !
insert superdbi.t5 select t3xc3, t3xc1
                   from superdbi.t3
                   where superdbi.t3.t3xc3 < -1E10
   reject duplicates !
select * from superdbi.t5 !
fetch !
insert superdbi.t3 select t5xc2, superdbi.t6.t6xc2, t6xc3
                 from superdbi.t6, superdbi.t5
                 where not t6xc3 is null
   ignore duplicates !
select substr(t3xc1,1,20) t3xc1, t3xc2, t3xc3 from superdbi.t3 !
fetch !
insert superdbi.t3 ( t3xc3, t3xc1 ) select  superdbi.t6.t6xc3, ref.t5xc2
                                    from superdbi.t5  ref, superdbi.t6
                                   where ref.t5xc2 >= 'R'
                                     and t6.t6xc3 > 1e5
   update duplicates !
select substr(t3xc1,1,20) t3xc1, t3xc2, t3xc3 from superdbi.t3 !
fetch !
insert superdbi.t3 select t6.t6xc1, max (synt6.t6xc2), count(ref.t6xc3)
                   from superdbi.t6, superdbi.t6  ref,synt6
                  where superdbi.t6.t6xc1 like '%Bernd'
                    and synt6.t6xc2 between -150 and +150
                    and ref.t6xc3 < 0
                  group by t6.t6xc1, ref.t6xc3
   update duplicates !
select substr(t3xc1,1,20) t3xc1, t3xc2, t3xc3 from superdbi.t3 !
fetch !
* commit work !
create view v1 as select t3xc1, t3xc3, t7xc2 from t7, superdbi.t3
   where superdbi.t3.t3xc1 = t7xc1 with check option !
insert v1 values ( 'Ilse', -8E12, 32 ) !
select substr(t3xc1,1,20) t3xc1, t3xc2, t3xc3 from superdbi.t3 !
fetch !
select * from t7 !
fetch !
rollback work release !
