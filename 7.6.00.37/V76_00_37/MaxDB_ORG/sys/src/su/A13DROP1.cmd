*   *ID* A13DROP1 DBCMD    changed on 1992-09-28-13.22.30 by ADMIN     *
file connect ( kern test !
create table t1 (s1 char(5), s2 char(5) ascii,
s3 char(5) ascii, s4 char(4), s5 char(5) ascii)!
create user x1 password w resource !
create user x2 password w resource !
grant selupd (s3) on t1 to x1 !
grant selupd (s3, s5) on t1 to x2 !
grant select on t1 to public !
create view v1 (a,b,c,d,e) as select s2, s3, s4, s5, s1 from t1 !
grant selupd (b) on v1 to x1 !
grant selupd (b,c) on v1 to x2 !
grant select on t1 to public !
file sel_priv_grant_on ( T1 !
file sel_priv_grant_on ( V1 !
alter table t1 drop (s3)!
file sel_priv_grant_on ( T1 !
file sel_priv_grant_on ( V1 !
drop table t1 !
commit work release !
