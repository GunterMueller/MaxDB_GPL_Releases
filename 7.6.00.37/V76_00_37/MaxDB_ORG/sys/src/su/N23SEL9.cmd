*   *ID* N23SEL9  DBCMD    changed on 1992-09-28-13.25.41 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT * INTO :a1,:a2, ... ,:a8 FROM tab
 *** WHERE ... OR ... OR ...
 *** Es existiert genau eine Ergebniszeile.!
file connect ( kern test !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte1 ='JOE' OR spalte4 IS NULL OR spalte3<-3 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where ( ( spalte7 between 'HAMELN' and 'HAMM' )
      or (  spalte8 in  ( 2.27e12, 3e-29, 0.065E+2 )) )
      or ( spalte1 like 'H%NR%'  ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where   ( spalte7 between 'HAMELN' and 'HAMM' )
      or (( spalte8 in  ( 2.27e12, 3e-29, 0.065E+2 )  )
      or ( spalte1 like 'H%NR%' )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( spalte7 not between 'ZELL' and 'ZUERICH' )
      or not ( spalte2>  -123.17 )
      or not ( not ( spalte1  sounds  like 'MONIKA' )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where ((not (( spalte7 not between 'ZELL' and 'ZUERICH'))
      or not ( ( spalte2>  -123.17 )))
      or (not ( not (spalte1 sounds  like 'MONIKA' )))) !
commit work release !
