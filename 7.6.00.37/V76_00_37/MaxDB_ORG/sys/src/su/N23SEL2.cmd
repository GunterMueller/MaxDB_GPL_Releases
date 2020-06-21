*   *ID* N23SEL2  DBCMD    changed on 1992-09-28-13.25.37 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT * INTO :a1,:a2, ... ,:a8 FROM tab WHERE ...
 *** Es existiert genau eine Ergebniszeile.!
file connect ( kern test !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1  between 'BERND' and 'BERND' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 between  'BALDUIN' and 'CHRIS'  !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte2  between  - 88.8  and - 88.8 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte4 between 1.E+4 and 1.E+6  !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8 between 29.2  and 29.200e00 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 = 'BERND' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 ='HEINRICH'  !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1  = 'RITA' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte2  =  123.17 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte3  =-4.0 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte4=  1.1228e+02 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8 = 12.00 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 <= 'BERND' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1  <='BERT' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte2  <=  -  123.17 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte4 <= -0.25 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 < 'BERT' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 < 'CHRISTINE' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8 <-3 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8 >= 0.099E12 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8 >=  0.099e11 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8>  0.099e11 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte2 = spalte3 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte3 = spalte2 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte3 = spalte4 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8 = spalte4 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 in ( 'RITA' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte2  in  (7.07) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8  in  (0.099E+12, 123E18, -45.6E-24 ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 like '%E_N_' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 like '%NN%'  !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1  sounds 'ELSB'  !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1  not between 'BERT' and 'THEO' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1  not between 'BERT' and user !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1  NOT between 'BERT' and 'SANDRA' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte8 not  between - 50 and 1E9 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte2 not in ( -123.17, -88.8, -88.09, -58.76,
   - 55, -33., -3, - 0.04 ,0, 0.12, 3,4, 7.07, 12.3, 22,
   33, 48.12, 112.28, 123.17, 256.54, 299.99 ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 not in ( 'THEODOR', user, 'RITA', 'PETER',
   'PABLO', 'LOTTE', 'KLAUS', 'KARL', 'KARIN', 'JUPP',
   'JUERGEN', 'JOACHIM', 'INGRID', 'ILSE', 'HILDE',
   'HEINRICH', 'HANS' , 'HANNELORE', 'GISELA', 'GISBERT',
   'GERHARD', 'FRIEDRICH', 'FRIEDA' , 'ELISABETH',
   'CHRISTINE' , 'BERND' )  !
commit work release !
