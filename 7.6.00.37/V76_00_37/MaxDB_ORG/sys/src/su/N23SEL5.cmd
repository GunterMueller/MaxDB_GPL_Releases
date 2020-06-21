*   *ID* N23SEL5  DBCMD    changed on 1992-09-28-13.25.39 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT * INTO :a1,:a2, ... ,:a8 FROM tab WHERE ...
 *** wobei die <where clause> ein oder mehrere "NOT" enthaelt.
 *** Es existiert genau eine Ergebniszeile. !
file connect ( kern test !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( spalte1 between 'BERT' and 'THEO' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not (( spalte1  between  'BERT'  and  'THEO' )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not (spalte4 not between -1E-30 and +1E-30 ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( (spalte4 not between -1E-30 and +1E-30 ) ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( not ( spalte2 between 5 and 10 ) ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not (not ((spalte2 between 5 and 10))) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ((not ( spalte2 between 5 and 10 ) )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ((not((spalte2 between 5 and 10)) )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( not ( spalte3 not between -3 and 5 )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( not ( ( spalte3 not between -3 and 5 )))!
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not (( not (spalte3 not between -3 and 5 ) ) ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not( ( not( ( spalte3 not between -3 and 5 )) )) !
 ***************************************************** !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( spalte2 <> -0.04 ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not(( (( spalte2 <> - 0.040 )) )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( not (spalte4 = 50      )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ((( not ((( not (( spalte3>-4)))))))) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( not (not(not(  spalte8>=9.9E9 )) ) ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( spalte3<>spalte2 ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( not ( spalte8 = spalte4 ) ) !
 ***************************************************** !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( spalte3 in (-5,-4,-3,-1,1,3,5 ) ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not (spalte2 not in (256.47,-178.12,22,7.12))!
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not((not (spalte2 in (256.47,-178.12,22,7.12))))!
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not  ( not (( not ( spalte4 in (-3,  -8E-4 ,
   -2.2e-13,0, 3.4e-29, 1,3, 2.92E+01, 5E+01,
   1.1228E2, 1.2345E+04, 2.2e+06, 9.900E+08, 1.E13,
   7E13, 3.34300E+29, 5.8E+37, 1.234E+44 ) ) ) )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ((not(((spalte4 not in ( -3, -8e-04 ,
   -2.2e-13,0, 3.4e-29, 1,3, 2.92E+01, 5E+01,
   1.1228E2, 1.2345E+04, 2.2e+06, 9.900E+08, 1.E13,
   7E13, 3.34300E+29, 5.8E+37, 1.234E+44 ) ) ) ))) !
 ***************************************************** !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not (spalte1 not like '%J__RG%' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not (( not (spalte1 like '%J__RG%') )) !
 ***************************************************** !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( spalte1 not sounds like 'HNRICH' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ((spalte1 not sounds 'HNRICH' )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not(   not (spalte1 sounds 'HNRICH' )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not(not(not(not(spalte1 sounds like 'HNRICH'))))!
commit work release !
* PTS 1130088-91!
file connect ( kern test !
create table purchase_order (partno int, supplierno int, 
"date" date, price int)!
insert into purchase_order values (1,1,'20030311',20030311)!
insert into purchase_order values (1,1,'20040111',20040111)!
insert into purchase_order values (1,1,'20040505',20040505)!
insert into purchase_order values (1,2,'20011231',20011231)!
insert into purchase_order values (1,2,'20030311',20030311)!
insert into purchase_order values (2,1,'20030311',20030311)!
insert into purchase_order values (2,1,'20040111',20040111)!
insert into purchase_order values (2,1,'20040505',20040505)!
select * from purchase_order 
where not (select max (partno) from purchase_order) = partno !
fetch!
declare first_cursor cursor for select partno,supplierno, max("date") 
from purchase_order 
 group by partno,supplierno!
 fetch first_cursor!
   select * from purchase_order 
       where (partno,supplierno, "date") in (select * from first_cursor)
   union all
   select * from purchase_order where 
      (partno,supplierno, "date") in 
         (select partno,supplierno, max("date") from purchase_order
         where (partno,supplierno, "date") not in (select * from first_cursor)
                                    group by partno,supplierno)
   order by 1,2!  
   fetch!
   select * from purchase_order 
       where (partno,supplierno, "date") in (select * from first_cursor)
   union all
   select * from purchase_order where 
      (partno,supplierno, "date") in 
         (select partno,supplierno, max("date") from purchase_order
         where not (partno,supplierno, "date") in (select * from first_cursor)
                                    group by partno,supplierno)
   order by 1,2!  
fetch!
rollback release !