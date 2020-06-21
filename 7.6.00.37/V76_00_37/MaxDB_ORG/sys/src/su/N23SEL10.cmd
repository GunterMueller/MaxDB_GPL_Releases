*   *ID* N23SEL10 DBCMD    changed on 1992-09-28-13.25.35 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT * INTO :a1,:a2, ... ,:a8 FROM tab
 *** WHERE ... AND ... OR ...
 *** Es existiert genau eine Ergebniszeile. !
file connect ( kern test !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1  like '___N'
     and spalte2 not in ( 77.18, 0 , 0.12, -88.8 )
      or spalte3 = -5 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where ((spalte1  like '__RN'
     and spalte2 not in ( 77.18, 0 , 0.12, -88.8 )))
      or spalte3 = -5 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1  like '_ERN'
     and ( spalte2 not in ( 77.18, 0 , 0.12, -88.8 )
      or spalte3 = -5 ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte4 between 1E7 and 1E10
     and not ( spalte7 is not null )
      or spalte8 = 9.9E+10 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where ((spalte4 between 1E7 and 1E10 )
     and not ( spalte7 is not null ))
      or spalte8 = 9.9E+10 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte4 between 1E7 and 1E10
     and (  not (( spalte7 is not null ))
      or (spalte8 = 9.9E+10)) !
commit work release !
