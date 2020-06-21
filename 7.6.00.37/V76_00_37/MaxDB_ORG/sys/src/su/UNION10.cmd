*   *ID* UNION10  DBCMD    changed on 1992-09-28-13.28.44 by ADMIN     *
file connect ( kern test !
create table t1 ( t1xc1 char(10) ascii key,
                  t1xc2 char(20) ,
                  t1xc3 fixed(5,2) ) !
create table t2 ( t2xc1 fixed(7,2) key,
                  t2xc2 char(10)  key,
                  t2xc3 char(20) ascii ) !
create table t3 ( t3xc1 char(15) ascii,
                  t3xc2 char(20) ,
                  t3xc3 fixed(6,3) ) !
insert t1 values ( 'Hans', 'Bremen', 1 ) !
insert t1 values ( 'Karl', 'Bremen', 100 ) !
insert t1 values ( 'Karin', 'Hamburg', null ) !
insert t1 values ( 'Lisa', null, null ) !
insert t2 values ( 20, 'Paul', 'Muenchen' ) !
insert t2 values ( 100, 'Emil', 'Hannover' ) !
insert t2 values ( 100, 'Emma', null ) !
insert t3 values ( 'Karl', 'Hannover', 1 ) !
insert t3 values ( 'Otto', 'Dortmund', 300 ) !
insert t3 values ( null, null, null ) !
insert t3 values ( 'Ilse', 'Hamm', 200 ) !
select * from t1 !
fetch !
select * from t2 !
fetch !
select * from t3 !
fetch !
commit work !
select * from t1 where
   t1xc1 = all ( select t2xc2 from t2 where t2xc2 < 'E' ) !
fetch !
select * from t1 where not
   t1xc1 <> any ( select t2xc2 from t2 where t2xc2 < 'E' ) !
fetch !
select * from t1 where
   t1xc1 = all ( select t2xc2 from t2 where t2xc2 < 'E' )
union all
select * from t1 where not
   t1xc1 <> any ( select t2xc2 from t2 where t2xc2 < 'E' ) !
fetch !
select * from t1 where not t1xc1 <  all ( select t3xc1 from t3 ) !
fetch !
select * from t1 where     t1xc1 >= any ( select t3xc1 from t3 ) !
fetch !
select * from t1 where not t1xc1 <  all ( select t3xc1 from t3 )
intersect all
select * from t1 where     t1xc1 >= any ( select t3xc1 from t3 ) !
fetch !
select * from t1 where
   t1xc3 > any ( select sum(t3xc3) from t3 group by t3xc3
                 having sum  ( t3xc3 ) >1 ) !
select * from t1 where
   t1xc3 <= all ( select count(*) from t3 ) !
fetch !
select * from t1 where
   t1xc3 <= all ( select count(*) from t3 )
except all
select * from t1 where
   t1xc3 > any ( select sum(t3xc3) from t3 group by t3xc3
                 having sum  ( t3xc3 ) >1 ) !
fetch !
select * from t1 where t1xc2 < any ( select t3xc2 from t3
                 where t3xc2 < all ( select t2xc3 from t2
                 where t2xc3 is not null )) !
fetch !
select * from t1 where not t1xc2 >= all ( select t3xc2 from t3
                 where not t3xc2 >= any ( select t2xc3 from t2
                 where t2xc3 is not null )) !
fetch !
select * from t1 where t1xc2 < any ( select t3xc2 from t3
                 where t3xc2 < all ( select t2xc3 from t2
                 where t2xc3 is not null ))
intersect
select * from t1 where not t1xc2 >= all ( select t3xc2 from t3
                 where not t3xc2 >= any ( select t2xc3 from t2
                 where t2xc3 is not null )) !
fetch !
select * from t3 where
    t3xc3 <= ( select sum(t2xc1 ) from t2
    group by t2xc1 having t2xc1 >= 100 )!
fetch !
select * from t3 where
    t3xc3 >= any ( select sum (t2xc1) from t2 group by t2xc1 ) !
fetch !
select * from t3 where
    t3xc3 <= ( select sum(t2xc1 ) from t2
    group by t2xc1 having t2xc1 >= 100 )
union
select * from t3 where
    t3xc3 >= any ( select sum (t2xc1) from t2 group by t2xc1 ) !
fetch !
 **********************************************************!
select * from t1 where     t1xc1 in ( select t2xc2 from t2 ) !
select * from t1 where not t1xc1 in ( select t2xc2 from t2 ) !
fetch !
select * from t1 where     t1xc1 in ( select t2xc2 from t2 )
intersect
select * from t1 where not t1xc1 in ( select t2xc2 from t2 ) !
fetch !
select * from t1 where
   t1xc3 >= all ( select avg (t2xc1 ) from t2 group by t2xc1 ) !
fetch !
select * from t1 where
   t1xc3 < any ( select t2xc1 from t2 group by t2xc1 ) !
fetch !
select * from t1 where
   t1xc3 >= all ( select avg (t2xc1 ) from t2 group by t2xc1 )
union
select * from t1 where
   t1xc3 < any ( select t2xc1 from t2 group by t2xc1 ) !
fetch !
select /*+ORDERED */ * from t1, t3
  where t1xc2 < all ( select t3xc2 from t3 where t3xc2 is not null )
    and ( t3xc2 in ( select t2xc3 from t2 where t2xc3 <> 'Muenchen' )
     or t3xc2 < 'F' ) !
fetch !
select /*+ORDERED */ * from t1, t3
  where t1xc2 < all ( select t3xc2 from t3 where t3xc2 is not null )
    and ( t3xc2 in ( select t2xc3 from t2 where t2xc3 <> 'Muenchen' )
     or t3xc2 < 'F' )
union all
select /*+ORDERED */ * from t1, t3
  where t1xc2 < all ( select t3xc2 from t3 where t3xc2 is not null )
    and ( t3xc2 in ( select t2xc3 from t2 where t2xc3 <> 'Muenchen' )
     or t3xc2 < 'F' ) !
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
commit work release !
