*   *ID* N23SEL4  DBCMD    changed on 1992-09-28-13.25.38 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT * INTO :a1,:a2, ... ,:a8 FROM tab WHERE ...
 *** Es existiert keine Ergebniszeile. !
file connect ( kern test !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 between 'ANNA' and 'ANNE' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte4 between -1e-10 and -1e-12 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5 < 'BERLIN' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte6 <= spalte7 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 > 'SUSANNE' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte2 = 241.260 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte7  in ('BONN'  ,    'HAMELN'    ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8 in (   43E12,  8.23E-23 ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 like  '_TO%' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte4 is null !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5 sounds like 'BNN' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5 not between 'BERLIN' and 'ULM' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 not  between 'BERND' and user
     and spalte1 <  'KERN'
     and spalte1 <> 'RITA' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte3 not between -5.0 and +5.0 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5 not in ( 'BONN', 'BERLIN', 'MUENCHEN',
   'HAMBURG', 'KERN' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5  not  in ( user, 'BONN', 'BERLIN',
   'MUENCHEN', 'HAMBURG' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte3 not in ( -5,-4,-3,-1,0,1,3,4,5 ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte6 is not null !
commit work release !
