*   *ID* CORR3    DBCMD    changed on 1992-09-28-13.23.01 by ADMIN     *
file connect ( kern test !
* corr subquery mit or !
create table rel(bky fixed (6), name char (12))!
create table att(key_opt char (1), bky fixed (6), name char (12))!
create table sch(name char (16), bky fixed (6))!
insert rel values (3, 'mmmmm')!
insert rel values (4, 'aaaaa')!
insert att values ('Y', 4, 'attname')!
insert att values ('N', 3, 'attname')!
insert sch values ('test', 3)!
select fixed (rel.bky, 5) bk,
    rel.name nam,
    att.name att,
    att.key_opt p
from rel, att, sch
where sch.name = 'test'
  and rel.bky = sch.bky
  and ((rel.name not in
            (select name from rel where bky in
                  ( select bky from att
                    where key_opt = 'Y'))
        and att.bky = rel.bky )
       or ( not exists
           (select rel.bky
              from att, rel, att ref
             where rel.bky = att.bky)))
order by rel.bky !
fetch !
drop table att !
drop table rel !
drop table sch !
commit release !
file connect ( kern test !
* corr subquery mit or und nor exists !
create table test1 ( a fixed (4), b fixed (4))!
insert test1 values (1, null) !
insert test1 values (2, null) !
insert test1 values (3, null) !
insert test1 values (4, null) !
insert test1 values (5, 5) !
insert test1 values (6, 6) !
insert test1 values (7, 7) !
insert test1 values (8, 8) !
create table test2 ( a fixed (5), b fixed (5))!
insert test2 values (5, 5)!
insert test2 values (5, 6)!
select * from test1 !
fetch !
select * from test2 !
fetch !
select a, b  from test1 where b is null !
fetch !
select a, b from test1 where not exists
    (select a, b from test2 where test1.b = test2.b)!
fetch !
select a, b from test1 where not exists
    (select a, b from test2 where test1.b = test2.b)
                           or b is null !
fetch !
drop table test1 !
drop table test2 !
commit release !
file connect ( kern test !
* corr subquery mit joinview !
create table csd_domain
(
DM_BKY                   FLOAT        (  15 ) key,
DM_VERS              FIXED        (  10 ) key,
ENTY_VERS            FLOAT        (  15 ) key)!
create table csd_enty
(
enty_BKY                   FLOAT        (  15 ) key)!
create table csd_attribute
(
AT_BKY                     FLOAT        (  15 ) key,
AT_VERSION_NO              FIXED        (  10 ) key,
ENTY_VERS            FLOAT        (  15 ) key,
DM_BKY                     FLOAT        (  15 ) ) !
insert csd_domain values (   10, 1, 1 )!
insert csd_domain values (   11, 1, 1 )!
insert csd_domain values (   12, 1, 1 )!
insert csd_domain values (   13, 1, 1 )!
insert csd_domain values (   14, 1, 1 )!
insert csd_enty values (10)!
insert csd_enty values (11)!
insert csd_enty values (12)!
insert csd_enty values (13)!
insert csd_enty values (14)!
create view msv_domain as select * from csd_enty, csd_domain
where dm_bky = enty_bky !
insert csd_attribute values (   10, 3, 4,  10)!
insert csd_attribute values (   11, 3, 4,  11)!
insert csd_attribute values (   13, 3, 4,  13)!
insert csd_attribute values (   14, 3, 4,  14)!
insert csd_attribute values (   20, 3, 4,null)!
select * from csd_enty, csd_domain domn
where  not exists
       (select * from csd_attribute
        where csd_attribute.dm_bky = domn.dm_bky)
  and dm_bky = enty_bky !
fetch !
select * from msv_domain domn
where  not exists
       (select * from csd_attribute
        where csd_attribute.dm_bky = domn.dm_bky)!
fetch !
drop table csd_enty !
drop table csd_attribute !
drop table csd_domain !
commit release !
file connect ( kern test !
* corr subquery mit set-function und weiteren Bedingungen !
create table ac1010 (
   acc_no    char (16) ascii key,
   acc_descr char(30) ascii,
   dirp_stat fixed(1,0),
   acc_typ   fixed(2,0),
   trns_stat fixed (1,0),
   subl_stat fixed(2,0),
   fc_status fixed(3,0))!
insert ac1010 values
   ('10000', 'after destruction database', 1, 2, 1, 1, 999)!
insert ac1010 values
   ('10001', 'after destruction database', 1, 3, 1, 1, -1 )!
insert ac1010 values
   ('10002', 'after destruction database', 1, 1, 1, 1, 3  )!
insert ac1010 values
   ('10003', 'after destruction database', 1, 1, 1, 1, 4)!
insert ac1010 values
   ('10004', 'after destruction database', 1, 3, 1, 1, 999)!
insert ac1010 values
   ('11111', 'fc_code 1                 ', 1, 3, 1, 1, 1  )!
insert ac1010 values
   ('20000', 'after destruction database', 1, 3, 1, 1, 10)!
insert ac1010 values
   ('zu10000', 'niet na destruction database', 1, 1, 0, 99, 3)!
insert ac1010 values
   ('zu20000', 'niet na destruction database', 1, 1, 0, 99, 3)!
insert ac1010 values
   ('zu30000', 'niet na destruction database', 1, 2, 0, 99, 3)!
insert ac1010 values
   ('zu40000', 'niet na destruction database', 1, 3, 0, 99, -1)!
insert ac1010 values
   ('zu50000', 'niet na destruction database', 1, 2, 0, 99, 999)!
insert ac1010 values
   ('zu60000', 'niet na destruction database', 1, 2, 0, 99, 999)!
insert ac1010 values
   ('zu70000', 'niet na destruction database', 1, 1, 0, 99, 999)!
insert ac1010 values
   ('zu80000', 'niet na destruction database', 1, 2, 0, 99, 999)!
commit !
create table ac1021 (
   acc_year    fixed(4,0) key,
   acc_no     char (16) ascii key,
   pos_p_no     fixed(3,0) key,
   pos_p_a_d float(18),
   pos_p_a_c float(18))!
insert ac1021 values
   (1988, '10004', 0, 12, 0)!
insert ac1021 values
   (1988, 'zu50000', 0, -123, 0)!
commit !
select * from ac1010 !
fetch!
select * from ac1021 !
fetch !
commit !
pars_then_ex !
select sum (pos_p_a_d), sum(pos_p_a_c)
    into :a, :b
    from ac1021
   where acc_year = 1988
     and pos_p_no = 0
     and exists
         (select *
              from ac1010
             where ac1010.acc_no = ac1021.acc_no
               and acc_typ <= 3
               and (subl_stat >= 99 and subl_stat <= 99)
               and dirp_stat = 1) !
select sum (pos_p_a_d), sum(pos_p_a_c)
    into :a, :b
    from ac1021
   where acc_year = 1988
     and pos_p_no = 0
     and exists
         (select *
              from ac1010
             where ac1010.acc_no = ac1021.acc_no
               and acc_typ <= 3
               and (subl_stat >= 99 and subl_stat <= 99)
               and dirp_stat = 1) !
pars_execute !
drop table ac1010 !
drop table ac1021 !
commit release !
