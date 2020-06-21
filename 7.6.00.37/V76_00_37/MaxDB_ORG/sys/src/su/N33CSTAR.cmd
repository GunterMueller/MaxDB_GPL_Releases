*   *ID* N33CSTAR DBCMD    changed on 1992-09-28-13.26.15 by ADMIN     *
file CONNECT ( KERN TEST !
create table t (f1 char  (8),
                f2 fixed (4),
                f3 char (20),
                f4 fixed (6))!
insert t values ('first   ',    1, 'one                 ',    4)!
insert t values ('first   ',    2, 'two                 ',    4)!
insert t values ('first   ',    3, 'three               ',    4)!
insert t values ('first   ',    4, 'four                ',    4)!
insert t values ('second  ',    1, 'one                 ',    4)!
insert t values ('second  ',    2, 'two                 ',    4)!
insert t values ('second  ',    3, 'three               ',    4)!
insert t values ('second  ',    4, 'four                ',    4)!
insert t values ('third   ',    1, 'one                 ',    4)!
insert t values ('third   ',    2, 'two                 ',    4)!
insert t values ('third   ',    3, 'three               ',    4)!
insert t values ('third   ',    4, 'four                ',    4)!
insert t values ('fourth  ',    1, 'one                 ',    4)!
insert t values ('fourth  ',    2, 'two                 ',    4)!
insert t values ('fourth  ',    3, 'three               ',    4)!
insert t values ('fourth  ',    4, 'four                ',    4)!
create index "t.f1"
	on t( f1 ) !
create index i1 on t (f2) !
create index "t.f3"
	on t( f3 ) !
* ohne Zusatzbedingungen !
* sequentiell, column_statistics !
select count(*) from t !
fetch !
select count(*) from t
       where syskey > x'0000000000000009'!
fetch !
* in_invlist !
select count(*) from t
       where f1 = 'first' !
fetch !
select count(*) from t
       where f1 in ('first', 'fifth', 'third') !
fetch !
* intersect !
select count(*) from t
       where f1 = 'first' and f2 = 2 !
fetch !
select count(*) from t
       where f1 = 'first' and f2 = 5 !
fetch !
select count(*) from t
       where f1 = 'first' and f2 = 2 and f3 = 'two' !
fetch !
* merge_inter !
select count(*) from t
       where f1 = 'first' and f2 between 1 and 3 !
fetch !
select count(*) from t
       where f1 between 'first' and 'fourth' and f2 = 2 !
fetch !
select count(*) from t
       where f1 between 'first' and 'fourth' and f2 between 1 and 3 !
fetch !
select count(*) from t
       where f1 = 'first' and f2 between 5 and 7 !
fetch !
select count(*) from t
       where f1 = 'nine' and f2 between 1 and 3 !
fetch !
select count(*) from t
       where f1 between 'max' and 'min' and f2 = 2 !
fetch !
* between !
select count(*) from t
       where f1 between 'first' and 'fourth' !
fetch !
select count(*) from t
       where f1 between 'max' and 'min' !
fetch !
* multindex !
drop index "t.f1" !
drop index i1 on t !
drop index "t.f3" !
create index i2 on t (f1, f2, f3) !
* mi_equal !
select count(*) from t
       where f1 = 'first' and f2 = 2 and f3 = 'two' !
fetch !
select count(*) from t
       where f1 = 'first' and f2 = 3 and f3 = 'two' !
fetch !
* not mi_equal !
select count(*) from t
       where f1 = 'first' and f2 = 2 !
fetch !
select count(*) from t
       where f1 = 'first' and f2 = 2 and f3 > 'two' !
fetch !
select count(*) from t
       where f1 = 'first' and f2 = 2 and f3 >= 'two' !
fetch !
select count(*) from t
       where f1 = 'first' and f2 between 2 and 3 !
fetch !
drop index i2 on t !
create index "t.f1"
	on t( f1 ) !
create index i1 on t (f2) !
create index "t.f3"
	on t( f3 ) !
* mit  Zusatzbedingungen ==> KB71 !
* sequentiell !
select count(*) from t where f4 = 4 !
fetch !
select count(*) from t where f4 <> 4 !
fetch !
* in_invlist !
select count(*) from t
       where f1 = 'first' and f4 = 4 !
fetch !
select count(*) from t
       where f1 in ('first', 'fifth', 'third') and f4 > 4 !
fetch !
* intersect !
select count(*) from t
       where f1 = 'first' and f2 = 2 and f4 < 4 !
fetch !
select count(*) from t
       where f1 = 'first' and f2 = 5 and f4 = 4 !
fetch !
select count(*) from t
       where f1 = 'first' and f2 = 2 and f3 = 'two' and f4 = 4 !
fetch !
* merge_inter !
select count(*) from t
       where f1 = 'first' and f2 between 1 and 3 and f4 = 4 !
fetch !
select count(*) from t
       where f1 between 'first' and 'fourth' and f2 = 2 and f4 <> 4 !
fetch !
select count(*) from t
       where f1 between 'first' and 'fourth' and f2 between 1 and 3
         and f4 = 4 !
fetch !
select count(*) from t
       where f1 = 'first' and f2 between 5 and 7 and f4 = 4 !
fetch !
select count(*) from t
       where f1 = 'nine' and f2 between 1 and 3 and f4 = 4 !
fetch !
select count(*) from t
       where f1 between 'max' and 'min' and f2 = 2 and f4 = 4 !
fetch !
* between !
select count(*) from t
       where f1 between 'first' and 'fourth' and f4 = 4 !
fetch !
select count(*) from t
       where f1 between 'max' and 'min' and f4 = 4 !
fetch !
* multindex !
drop index "t.f1" !
drop index i1 on t !
drop index "t.f3" !
create index i2 on t (f1, f2, f3) !
* mi_equal !
select count(*) from t
       where f1 = 'first' and f2 = 2 and f3 = 'two' and f4 = 4 !
fetch !
select count(*) from t
       where f1 = 'first' and f2 = 2 and f3 = 'two' and f4 > 4 !
fetch !
* not mi_equal !
select count(*) from t
       where f1 = 'first' and f2 = 2 and f4 = 4 !
fetch !
select count(*) from t
       where f1 = 'first' and f2 = 2 and f3 > 'two' and f4 <> 4 !
fetch !
select count(*) from t
       where f1 = 'first' and f2 = 2 and f3 >= 'two' and f4 = 4 !
fetch !
select count(*) from t
       where f1 = 'first' and f2 between 2 and 3 and f4 = 4 !
fetch !
drop index i2 on t !
create index "t.f1"
	on t( f1 ) !
create index i1 on t (f2) !
create index "t.f3"
	on t( f3 ) !
drop table t !
commit release !
