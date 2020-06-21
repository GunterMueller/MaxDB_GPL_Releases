*   *ID* N23SEL8  DBCMD    changed on 1992-09-28-13.25.40 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT * INTO :a1,:a2, ... ,:a8 FROM tab
 ***    WHERE ... AND ... AND ... !
file connect ( kern test !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
  where  spalte7 is null  and   spalte8 > 0
    and   spalte5 = user !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
  where ((( (spalte7 is null))   and (( spalte8 > 0 ) ))
    and   (( spalte5 = user )) ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
  where (( (spalte7 is null))   and ( (( spalte8 > 0 ) )
    and   (( spalte5 = user )))) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
  where not ( spalte4 between 0 and 1E10 )
    and not ( spalte5 sounds 'BRLIN' )
    and not ( spalte5 in ( 'HAMBURG', user, 'KOELN' )) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
  where ( not ( spalte7 is not null )
    and ( not ( spalte4 = spalte2 ) ) )
    and ( not ( not ( spalte5 like '%RN' ) ) ) !
commit work release !
