*   *ID* N23SEL12 DBCMD    changed on 1992-09-28-13.25.36 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT * INTO :a1,:a2, ... ,:a8 FROM tab WHERE ...
 *** komplizierte <where clause>
 *** Es existiert genau eine Ergebniszeile. !
file connect ( kern test !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where (((( spalte5 is null )
      or ( spalte5 <> spalte7 ))
     and ( spalte7 is null )
      or ( spalte4 not in ( 0 ) ))
     and ( spalte2 = spalte4 )
     and not (( spalte3 is not null ) )
      or ( spalte8 > 10E10
      or spalte7 like 'H__MEL%'
      or spalte1 sounds like 'JOHN'
     and (( spalte1 not between 'HANS' and 'LINDA' )) )) !
commit work release  !
