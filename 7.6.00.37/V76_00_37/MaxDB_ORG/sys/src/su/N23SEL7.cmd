*   *ID* N23SEL7  DBCMD    changed on 1992-09-28-13.25.40 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT * INTO :a1,:a2, ... ,:a8 FROM tab
 ***    WHERE ... OR ... !
file connect ( kern test !
select  *  into  :a1, :a2,:a3,:a4,:a5,:a6,:a7,:a8 from  tab
   where spalte4 is null or spalte8 <-3 !
select  *  into  :a1, :a2,:a3,:a4,:a5,:a6,:a7,:a8 from  tab
   where spalte1 sounds 'JOERGN'
      or spalte7 in ( 'KOELN', 'HAMELN' ) !
select  *  into  :a1, :a2,:a3,:a4,:a5,:a6,:a7,:a8 from  tab
   where ( spalte4 between 2 and 03.5E+00 or spalte4>10e50) !
select  *  into  :a1, :a2,:a3,:a4,:a5,:a6,:a7,:a8 from  tab
   where ( spalte2 = -0.04 ) or ( spalte2 = -0.04 ) !
select  *  into  :a1, :a2,:a3,:a4,:a5,:a6,:a7,:a8 from  tab
   where (( spalte2  = spalte3 ) or ( spalte4 =spalte3 ) ) !
select  *  into  :a1, :a2,:a3,:a4,:a5,:a6,:a7,:a8 from  tab
   where spalte1 not between 'CHRIS' and 'SUSANNE'
      or not ( (spalte3 >= -4 )) !
select  *  into  :a1, :a2,:a3,:a4,:a5,:a6,:a7,:a8 from  tab
   where not ( not ( spalte6 is not null ))
      or ( not ( spalte4 between -1.0 and 99.999E50 ) ) !
commit work release !
