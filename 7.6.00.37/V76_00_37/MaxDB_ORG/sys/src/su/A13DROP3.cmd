*   *ID* A13DROP3 DBCMD    changed on 1992-09-28-13.22.31 by ADMIN     *
file connect ( kern test !
create table t1 (s1 char(5) key , s2 char(5) ascii,
s3 char(5) ascii, s4 char(5))!
insert t1 values ('aaaaa','aaaa1','aaaa2','aaaa3')!
create view v1 (a) as select s2 from t1 !
create view v2 (a,b,c,d,e,f,g,h)
   as select t1.*, reft1.* from t1, t1 reft1 !
create view v3 (a,b,c,d) as select s2, s1, s4, s3 from t1 !
create view v4 (a1,a2,a3,a4) as select d,c,b,a from v3 !
create view v5 as select s2 a1, s4 a2 from t1 where s1 = 'aaaaa' !
alter table t1 drop (s2) !
file sel_view ( V1 !
file sel_view ( V2 !
file sel_view ( V3 !
file sel_view ( V4 !
file sel_view ( V5 !
select * from v3 !
fetch !
select * from v4 !
fetch !
select * from v5 !
fetch !
alter table t1 drop (s4) !
file sel_view ( V2 !
file sel_view ( V3 !
file sel_view ( V4 !
select * from v3 !
fetch !
select * from v4 !
fetch !
file sel_view ( V5 !
commit work release !
