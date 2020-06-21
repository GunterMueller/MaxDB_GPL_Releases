*   *ID* A18REN1  DBCMD    changed on 1992-09-28-13.22.50 by ADMIN     *
file connect ( kern test !
create user xx1 password w resource !
create table t1 (s1 integer key, s2 char(5), s3 char(5))!
create table t2 (a1 char(5) ascii key, a2 char(5) ascii, a3 integer)!
create view v1 as select s1, s2, a1, a2 from t1, t2 where
                                           a2 = s2 !
grant select on t1 to xx1 !
grant select on v1 to xx1 !
commit work release !
file connect ( xx1 w !
create table t1 (b1 integer key, b2 char(5) ascii)!
create view v2 as select * from t1, kern.t1 where
b1 = kern.t1.s1 !
rename table t1 to tabelle1 !
file sel_view ( V2 !
select c_definition into :def from sysdd.views
  where owner = usergroup and tablename = 'V2'!
commit work release !
file connect ( kern test !
file sel_view ( V1 !
select c_definition into :def from sysdd.views
  where owner = usergroup and tablename = 'V1'!
rename table t1 to sdbatab1 !
file sel_view ( V1 !
select c_definition into :def from sysdd.views
  where owner = usergroup and tablename = 'V1'!
commit work release !
