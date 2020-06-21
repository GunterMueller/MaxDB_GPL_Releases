*   *ID* AK24DRIN DBCMD    changed on 1992-09-28-13.22.14 by ADMIN     *
create table t1 (s1 char(5), s2 char(5),
                 s3 char(5) )!
create index i1 on t1 (s1,s2)!
create index i2 on t1 (s2, s3)!
create table t2 (s1 char(5), s2 char(5),
                 s3 char(5) )!
create index i1 on t2 (s1, s2)!
* create user thomas password w dba !
create user thomas password w resource !
grant select on t1 to thomas !
grant select on t2 to thomas !
create table t11 (s1 char(5), s2 char(5) )!
create index i1 on t11 (s1)!
create index i3 on t11 (s2)!
grant all on t11 to thomas !
commit work release !
file connect ( thomas w !
drop index i1 !
drop index i2 !
drop index i3 !
commit work release !
file connect ( kern test !
drop index i3 !
drop index i1 !
drop index i2 !
file sel_index_1tab ( KERN T1 !
file sel_index_1tab ( KERN T2 !
drop index i1 on kern.t1 !
commit work release !
