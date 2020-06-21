*   *ID* N23SEL3  DBCMD    changed on 1992-09-28-13.25.38 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT * INTO :a1,:a2, ... ,:a8 FROM tab WHERE ...
 *** Es existieren mindestens zwei Ergebniszeilen. !
file connect ( kern test !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 between 'GISELA' and  'GISELA' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 between 'KERN'  and 'THEODOR' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5 between 'HAMBURG' and 'HAMBURG' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte4 between -3 and +3 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte2 = 299.99 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5 = spalte7 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 = USER   !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte3 <= -3 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 <=  'CHRISTINE' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte4  < 0 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5 <'BONN' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5>= 'MUENCHEN' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte2 >= 100.00 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8 >=1e+5 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5 >= user !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte4 >= spalte3 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 > 'KARIN' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte3>4 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8>01.00E8 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte3<>4.0 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 <> 'PETER' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte2 <> spalte4 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 in (    'HANS'    )  !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5 in (  'AACHEN', 'BONN', 'BERLIN' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte4 in (  81.00,2.92e1, 1.1111e-21,1.2345e4) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 like '_ANS%' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte3 is null !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte6 is null !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8 is  null !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 sounds like 'KRN' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 not between 'ELISABETH' and 'KERN' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 not between 'ELISABETH' and    USER    !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte3 not between -4 and +4 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 not in ( 'BERND','SUSANNE', 'HANS' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 not in ( 'BERND', user    , 'HANS',
   'CHRISTINE', 'FRIEDA', 'GERHARD', 'GISELA' ,
   'HEINRICH', 'HILDE', 'ILSE', 'INGRID' , 'JOACHIM' ,
   'JUERGEN', 'KARIN', 'KARL' , 'KLAUS', 'LOTTE',
   'PABLO', 'PAUL' , 'RITA' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8 not in (-3.e1,-3,-8e-4,1,3,5,1.23E1,
   2.92E1 , 8.880E1, 1.23e3, 2.2E6, 9.9E8, 9.9E10 ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5 not like '%U__CHEN' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte3 is  not  null !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte7 is  not  null !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte5 not sounds 'BLN' !
commit work release !
