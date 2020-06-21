* test some error situations first !
create table &1.t1 (s1 fixed(10,2) default serial) !
create table &1.t1 (s1 float default serial) !
create table &1.t1 (s1 char(20) default serial) !
create table &1.t1 (s1 fixed(10) default serial (10), s2 serial) !
* test basics 
create table &1.t1 (s1 fixed(10) default serial (9999999999)) !
commit !
update &1.t1 set s1 = default !
insert &1.t1 values (0) !
insert &1.t1 values (0) !
insert &1.t1 values (100) !
insert &1.t1 values (0) !
insert &1.t1 values (0) !
select * from &1.t1 !
fetch !
rollback !
* same with parse/execute now !
pars_then_ex !
data !
insert &1.t1 values (:a) !
n 10 0

insert &1.t1 values (:a) !
n 10 100

insert &1.t1 values (:a) !
n 10 0

insert &1.t1 values (:a) !
n 10 0

pars_execute !
select * from &1.t1 !
fetch !
* test insert select !
create table &1.t2 (s1 int default serial, s2 int) !
insert &1.t2 select 0, s1 from &1.t1 !
select * from &1.t2 !
fetch !
create table &1.t3 (s1 int default serial, s2 int, primary key(s1)) !
create view t3_v as select * from &1.t3 where s2 <> 200 !
insert &1.t3 select 0, s1 from &1.t1 !
select * from &1.t3 !
fetch !
delete &1.t3 !
insert &1.t3 values (  0, 5)!
insert &1.t3 values (100, 100)!
insert &1.t3 values (  0, 101)!
select * from &1.t3 !
fetch !
delete &1.t3 !
pars_then_ex !
insert &1.t3 values (  0, 102)!
insert &1.t3 values (200, 200)!
insert &1.t3 values (  0, 201)!
select * from &1.t3 !
fetch !
insert t3_v values (  0, 202)!
insert t3_v values (300, 300)!
insert t3_v values (  0, 301)!
select * from t3_v !
fetch !
delete t3_v !
pars_then_ex !
insert t3_v values (  0, 302)!
insert t3_v values (400, 400)!
insert t3_v values (  0, 401)!
select * from t3_v !
fetch !
pars_execute !
create table &1.t4 like &1.t2 !
insert &1.t4 select * from &1.t2 !
select owner, tablename, columnname, "DEFAULT"
       from sysdd.columns where "DEFAULT" like '%SERIAL%' !
fetch !
parsing !
insert &1.t1 values (:a)!
executing !
data !
insert !
1
n 10 0

nodata !
pars_execute !
select * from &1.t1 !
fetch!
* PTS 1118447 if delete t1 is not there !
delete &1.t1 !
parsing !
minsert &1.t1 values (:a)!
executing !
data !
minsert !
32767
3
1
n 10 0
n 10 0
n 10 0

nodata !
pars_execute !
select * from &1.t1 !
fetch!
drop table &1.t1 !
drop table &1.t2 !
drop table &1.t3 !
drop table &1.t4 !
commit !
