*   *ID* RENCOL   DBCMD    changed on 1992-09-28-13.27.56 by ADMIN     *
file cleark !
file connect ( kern test !
create table t1 (s1 int) !
create table t2 (a1 int, a2 int, a3 int) !
insert t1 values (1) !
insert t2 values (1, 1, 1) !
create view v1 as select s1 from t1 where s1 = 1 !
create view v2 as select s1, a3, a2, a1 from t1, t2 where s1 = 1 and
                  s1 = a1 and a3 = 1 and a2 = 1 !
commit !
rename column t1.s1 to "spalte1" !
rename column t1."spalte1" to "column1" !
commit !
rename column t2.a1 to "aaa1" !
rename column t2.a2 to "aaa2" !
rename column t2.a3 to "aaa3" !
commit !
rename column t2."aaa1" to "a,a,a,a,1" !
rename column t2."aaa2" to "a,a,a,a,2" !
rename column t2."aaa3" to "a,a,a,a,3" !
rename column t1."column1" to "spalt___e" !
commit !
select * from v1 !
fetch !
select * from v2 !
fetch !
save catalog of t1 into save_t1 !
create table t3 like t1 !
insert t3 select * from t1 !
drop table t1 !
rename table t3 to t1 !
commit !
restore catalog from save_t1 !
create table tab1 (s1 int, s2 int) !
create view view1 as select s1, s2 from tab1 where s2 = 1 !
rename column view1.s2 to spalte2 !
file sel_view ( VIEW1 !
commit release !
