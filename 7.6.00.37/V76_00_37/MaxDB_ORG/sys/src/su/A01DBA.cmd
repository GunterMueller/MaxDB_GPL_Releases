*   *ID* A01DBA   DBCMD    changed on 1992-09-28-13.22.14 by ADMIN     *
 ***************************
 ***   A 0 1 D B A       ***
 ***************************!
file connect ( kern test !
monitor init !
create table  kern.p1      (k1 char (7) key ,
  f1 fixed (5,2) not null, f2 char (3)) !
create table  freier.p1 (  k1 char (7) key ,
   f1 fixed (5,2) not null, f2 char (3) ) !
create table     kern.p11      (k1 char (7) key ,
   f1 fixed (5,2) key,  f2 char (3) not null ) !
create table     kern.p12     (k1 char (7) key,
   f1 fixed (5,2)key ,f2 char (3) key  ) !
create table kern.t1 (
   f1 char(7) range not between 'a' and 'e',
   f2 fixed (5,2) not null, f3 char (3)
   range  between  'AA' and 'ZZ' ) !
create table kern. t2 (
   f1 char(7) range not between 'a' and 'e',
   f2 fixed (5,2) not null, f3 char (3) not null
   range  between  'AA' and 'ZZ' ) !
COMMIT WORK  !
file sel_tab_owner ( FREIER !
file sel_tab_owner ( KERN !
alter table kern. t1  add
    f4 character (12) , f5 fixed (12,3)    !
COMMIT WORK  !
alter table kern.t1 foreign key l1 ( f1 ) references kern.p1 !
COMMIT WORK  !
file sel_references ( KERN P1 !
create index "kern.p1.k1"
	on kern.p1( k1 ) !
create index "kern.t1.f2"
	on kern.t1( f2 ) !
COMMIT WORK  !
file sel_index_1tab ( KERN T1 !
alter table freier.t1 add   f7 char (12) , f8 fixed (12,3)  !
drop   index        "kern.t1.f2" !
COMMIT WORK  !
alter table kern. t1  drop
    f4 , f2   !
alter table kern. t1
    add constraint f5 check f5 between  20.0 and  100.0 !
alter table kern. t1 drop constraint f3 !
drop table frank.p1!
drop table  kern.p11 !
drop table  kern.p1 !
COMMIT WORK !
file sel_tab_owner ( KERN !
create view  kern.viewt2  as select *
    from kern.t2 where  (f2 = 1.0  and f3 > 'KK') or
       (f1 >'ea' ) !
create view  kern.viewt22 as select *
    from kern.t2 where  (f2 = 1.0  and f3 > 'KK') or
       (f1 >'c' ) !
COMMIT WORK  !
file sel_view1 ( KERN VIEWT2 !
file sel_view1 ( KERN VIEWT22 !
file sel_monitor_all ( CACHES PAGES !
commit work release !
