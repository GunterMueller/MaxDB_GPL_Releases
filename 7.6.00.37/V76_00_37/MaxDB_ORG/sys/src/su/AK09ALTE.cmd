*   *ID* AK09ALTE DBCMD    changed on 1992-11-11-17.01.53 by KERN      *
file connect ( sut sut !
create user d1 password w dba !
create user r1 password w resource !
commit release!
file connect ( kern test !
*   *ID* AK09ALTE DBCMD    changed on 1990-09-18-09.33.14 by PINO      *
 ***  7.13 <create view statement> !
create table kern.t1
   ( t1xc1 char(10) key,
     t1xc2 fixed(3) not null range between -100 and +100,
     t1xc3 float(3),
     t1xc4 char(10) byte ) !
create table kern.t3
   ( t1xc1 char(10) key,
     t1xc2 fixed(3) not null range between -100 and +100,
     t1xc3 float(3),
     t1xc4 char(10) byte not null) !
create view kern.v1xt3  as
   select t1xc4, t1xc1, t1xc2  from kern.t3!
insert v1xt3 values ('aaaa', 'key1', 1 ) !
file sel_table !
file sel_cols_1tab ( KERN T3 !
file sel_cols_1tab ( KERN V1XT3 !
file sel_view ( V1XT3 !
alter table kern.t3 drop t1xc3 !
insert v1xt3 values ('aaaa', 'key2', 2 ) !
file sel_table !
file sel_cols_1tab ( KERN T3 !
file sel_cols_1tab ( KERN V1XT3 !
file sel_view ( V1XT3 !
alter table kern.t3 drop t1xc4 !
insert v1xt3 values ('aaaa', 'key3', 3 ) !
file sel_table !
file sel_cols_1tab ( KERN T3 !
file sel_cols_1tab ( KERN V1XT3 !
file sel_view ( V1XT3 !
create view kern.v1xt1 as
   select *  from kern.t1
   where t1xc1 > 'HAMELN' or ( t1.t1xc4 is  null ) !
create view kern.v2xt1 as select t1.t1xc1, t1xc2  from kern.t1 !
create view  v3xt1  as select  t1xc3 from kern.t1
   where kern.t1.t1xc3 >= 0 !
create view kern.v4xt1 ( v4xc1, v4xc2 )  as
   select all t1xc1, k1xt1.t1xc2 from kern.t1 k1xt1
   where ( k1xt1.t1xc2 between -50 and +50 )
   and k1xt1.t1xc4 < 'f'
   and k1xt1.t1xc1 < 'K' with check option !
create view kern.v5xt1 (  v5xc1 ) as
   select 3 * t1xc3
   from kern.t1
   where t1xc3*3 <3E20 !
commit work !
insert t1 values ( 'MUENCHEN', 32, 2E10,  X'D6E3E3D6' ) !
insert v1xt1 values ( 'BERLIN', 12, 8E-7, X'D7C1E4D3' ) !
insert kern.v1xt1 set t1xc1 = 'HAMBURG',
                        t1xc2 = 38,
                        t1xc3 = 80E-08 !
update v2xt1 set t1xc2 = 64 where t1xc2 = 32 !
select v3xt1.*, t1xc3  from kern.v3xt1 !
fetch !
insert v4xt1 set v4xc1 = 'BONN',
                 v4xc2 = -20 !
commit !
delete kern.v5xt1 where v5xt1.v5xc1 > 1.E10 !
rollback !
select * from t1 !
fetch !
create view kern.v1xv1xt1 as
   select t1xc1, kern.v1xt1.t1xc2, t1xc3
   from kern.v1xt1
   where v1xt1.t1xc1 > 'KOELN'
   with check option !
create index "kern.t1.t1xc2"
	on kern.t1( t1xc2 ) !
create view kern.v1xv1xv1xt1 as
   select *
   from kern.v1xv1xt1 syn1
   where syn1.t1xc3 <1E30 !
insert v1xv1xv1xt1 values ( 'MANNHEIM', 18, -5.e12 ) !
declare erg1 cursor for 
select  t1xc1, kern.v1xv1xt1.t1xc2,v1xv1xt1.t1xc3
   from kern.v1xv1xt1 !
fetch erg1 !
declare erg2 cursor for 
select distinct t1.t1xc1, kern.v1xv1xv1xt1.t1xc3, syn2.t1xc2
   from kern.t1, kern.v2xt1 syn2, kern.v1xv1xv1xt1
   where syn2.t1xc2 + 32 <= v1xv1xv1xt1.t1xc3
   order by t1.t1xc1, syn2.t1xc2 !
fetch erg2 !
commit work !
create table kern.t2 ( t2xc1 float(5) , t2xc2 char(20)) !
create synonym synxt2 for kern.t2 !
create synonym synxv1xv1xt1 for kern.v1xv1xt1 !
create view kern.synxview
   (spalte1,spalte2,spalte3,spalte4,spalte5,spalte6,spalte7 ) as
   select synxt2.t2xc1, synxv1xv1xt1.t1xc1, t2xc2, v1xv1xt1.*,
          synxv1xv1xt1.t1xc2
   from synxt2, synxv1xv1xt1 , kern.v1xv1xt1
   where synxt2.t2xc2 < 'K' and  ( v1xv1xt1.t1xc1 < 'NUERNBERG' ) !
insert synxt2 values ( 38, 'MUENCHEN' ) !
create synonym synxsynxview for kern.synxview !
select * from synxsynxview where ( spalte1 in ( 37, 38, 39 )
                             and synxsynxview.spalte2 < 'MOELLN' ) !
fetch !
commit work !
grant select, insert on kern.v1xt1 to public !
grant select on kern.v3xt1 to d1 !
commit release !
file connect ( d1 w !
create table t1 (f1 char (10)) !
create view d1v1xt1 as select * from kern.v1xt1
   where  t1xc1 = 'sss' !
create view d1v3xt1 as select * from kern.v3xt1 !
file sel_cols_1tab ( D1 T1 !
file sel_cols_1tab ( KERN T1 !
file sel_cols_1tab ( D1 D1V1XT1 !
file sel_cols_1tab ( D1 D1V3XT1 !
file sel_cols_1tab ( KERN V1XT1 !
file sel_view ( V1XT1 !
file sel_view ( D1V1XT1 !
file sel_view ( V2XT1 !
file sel_view ( D1V3XT1 !
file sel_view ( V3XT1 !
file sel_view ( V4XT1 !
file sel_view ( V1XV1XT1 !
file sel_view ( V1XV1XV1XT1 !
file sel_table !
commit work release !
file connect ( kern test !
drop table kern.t1 !
commit release !
file connect ( d1 w !
file sel_table !
commit release !
file connect ( kern test !
create table t1 (s1 int, s2 char(254)) !
create view v1 as select * from t1 with check option !
insert t1 values (1, 'a') !
insert v1 values (1, 'va') !
alter table t1 add s3 char (40) !
insert t1 values (2, 'b', 'c') !
insert v1 values (2, 'vb', 'vc') !
alter table t1 add s4 char (254) !
insert t1 values (3, 'd', 'e', 'f') !
insert v1 values (3, 'vd', 've', 'vf') !
alter table t1 add s5 char (40) !
insert t1 values (4, 'g', 'h', 'i', 'j') !
insert v1 values (4, 'vg', 'vh', 'vi', 'vj') !
alter table t1 add s6 char (254) !
insert t1 values (5, 'k', 'l', 'm', 'n', 'o') !
insert v1 values (5, 'vk', 'vl', 'vm', 'vn', 'vo') !
select * from t1 !
fetch !
select * from v1 !
fetch !
file sel_cols_tab ( T1 !
file sel_cols_tab ( V1 !
rollback release !
