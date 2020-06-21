*   *ID* UNION8   DBCMD    changed on 1992-09-28-13.28.49 by ADMIN     *
file connect ( kern test !
select * from tab1 where
   t1c1 in (select t1c1 from tab1 union select t2c2 from tab2 ) !
fetch !
select * from tab1 where
   t1c1 = any (select t1c1 from tab1 union select t2c2 from tab2 ) !
fetch !
select * from tab1 where
   t1c1 in (select t1c1 from tab1 union select t2c2 from tab2 ) OR
   t1c1 = any (select t1c1 from tab1 union select t2c2 from tab2 ) !
fetch !
select t1c2 from tab1 where
   t1c1 = ( select t1c1 from tab1 where t1c3 < 300
                   except select t1c1 from tab1 where t1c3 > 200 ) !
fetch !
select * from tab1 where
   t1c3 = ( select t1c4 from tab1 union select t1c3 from tab1 )!
select * from tab1 where
   exists ( select t1c1 from tab1 where t1c3 < 100
              union select t2c2 from tab2 ) !
fetch !
select * from tab1 where
   exists ( select t1c1 from tab1
              union select t2c2 from tab2 where t2c3 = 0 ) !
fetch !
select * from tab1 where
   exists (select t1c3 from tab1 union select t2c3 from tab2 ) AND
   t1c2 = ( select t1c2 from tab1
               except select t1c2 from tab1 where t1c3 > 200 ) !
fetch !
select * from tab1 where
   exists (select t1c3 from tab1 union select t2c3 from tab2 ) AND
   t1c2 = ( select t1c2 from tab1
               except select t1c2 from tab1 where t1c3 > 300 ) !
select * from tab1 where
   exists ( select t1c3 from tab1 union
              select t1c4 from tab1 where t1c1 in
                 ( select t1c1 from tab1 ) )!
fetch !
select * from tab1 where
   t1c1 in ( select t1c1 from tab1 where
      t1c1 in ( select t1c1 from tab1 intersect select t1c1 from tab1 ))!
fetch !
select * from tab1 where
   exists ( select t1c3 from tab1 union
              select t1c4 from tab1 where t1c1 in
                 ( select t1c1 from tab1 ) )
union all
select * from tab1 where
   t1c1 in ( select t1c1 from tab1 where
      t1c1 in ( select t1c1 from tab1 intersect select t1c1 from tab1 ))!
fetch !
select * from tab1 where
     t1c1 in ( select t2c2 from tab2 where t2c3 > tab1.t1c3
                 union select t1c1 from tab1 )!
fetch !
select * from tab1 where
     t1c1 > any( select t2c2 from tab2 where t2c3 > tab1.t1c3 ) OR
     t1c2 in ( select t1c2 from tab1
                 intersect select t1c2 from tab1 where t1c3 > 400 )!
fetch !
select * from tab1 where
     t1c1 > any( select t2c2 from tab2 where t2c3 > tab1.t1c3 ) OR
     t1c2 in ( select t1c2 from tab1 where t1c3 > 400 )
intersect all select * from tab1 !
fetch !
select * from tab1 where
     t1c1 > any( select t2c2 from tab2 where t2c3 > tab1.t1c3 )
intersect all select * from tab1 !
fetch !
select * from tab1 where
     t1c1 > any( select min (t2c2) from tab2 where t2c3 > tab1.t1c3 )
intersect all select * from tab1 !
fetch !
select * from tab1 where
     t1c1 > any( select min (t2c2) from tab2 where t2c3 > tab1.t1c3 
                                        group by t2c3 )
intersect all select * from tab1 !
fetch !
select * from tab1 intersect all
select * from tab1 where
     t1c1 > any( select t2c2 from tab2 where t2c3 > tab1.t1c3 )!
fetch !
drop table tab1 !
commit release !
