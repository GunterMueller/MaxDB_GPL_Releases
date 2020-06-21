*   *ID* A14SYNS  DBCMD    changed on 1992-09-28-13.22.35 by ADMIN     *
 ***************************
 ***   A 1 4 S Y N S     ***
 ***************************!
file connect ( kern test !
create user  fell password w !
create user  meier password w resource !
create user  freier password w resource !
create table t1 (f1 fixed (4)) !
create view v1 as select * from t1 where f1 > 9 !
create view s5 as select * from t1 where f1 > 11 !
grant  select  on t1 to public !
grant  all     on t1 to meier  !
grant  select,insert   on v1 to public !
grant  select,insert   on s5 to public !
commit work release !
 ***************************!
file connect ( meier w !
create table t2 (f1 fixed (6)) !
create view v2 as select * from t2 where f1 > 9 !
create view mv1 as select * from kern.t1 where f1 > 12 !
grant select on t2 to public !
grant  select,insert   on v2 to public !
grant  select,insert   on mv1 to public !
grant  select,insert   on public.mv1 to public !
file sel_table !
file sel_priv_on !
file sel_priv_granted ( PUBLIC !
commit work release !
 ***************************!
file connect ( fell w !
select * from kern.t1 !
select * from t2 !
select * from meier.t2 !
select * from v1 !
select * from mv1 !
select * from v2 !
select * from meier.v2 !
file sel_cols_tab ( T1 !
file sel_cols_tab ( T2 !
file sel_cols_1tab ( MEIER T2 !
file sel_view ( V1 !
file sel_view ( MV1 !
file sel_view1 ( MEIER V2 !
file sel_table !
commit work release !
 ***************************!
file connect ( freier w !
create synonym s1 for kern.t1 !
create synonym s2 for kern.v1 !
create synonym s6 for meier.mv1 !
create synonym s3 for meier.t2 !
create synonym s4 for meier.v2 !
select * from s1 !
select * from s2 !
select * from s3 !
select * from s4 !
file sel_table !
file sel_view1 ( MEIER V1 !
file sel_view1 ( MEIER MV1 !
file sel_view1 ( PUBLIC S5 !
file sel_view1 ( PUBLIC V1 !
insert s1  values (11)!
insert s2  values (12)!
insert s3  values (13)!
insert s4  values (14)!
insert s5  values (15)!
insert s6  values (15)!
commit work release !
