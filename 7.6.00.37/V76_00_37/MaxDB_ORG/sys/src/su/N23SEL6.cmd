*   *ID* N23SEL6  DBCMD    changed on 1992-09-28-13.25.39 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT * INTO :a1,:a2, ... ,:a8 FROM tab WHERE ...
 *** wobei die <where clause> genau ein boolesches 'AND'
 *** enthaelt. !
file connect ( kern test !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
    where (spalte1, spalte5) in (('KARIN', 'KOELN'), ('KARIN', 'BERLIN')) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
    where ( spalte7 between 'MOELLN' and  'ZUERICH' )
      and ( spalte8 between 1E7 and +1e30 ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
    where ((spalte4 in ( 27.86E-12, 9.9E8, 2.92E1,
          027E+26 )))
      and ( ( spalte7 like 'M__N%' ) ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
    where spalte8 is  null and not ( spalte4<=3E10 ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
    where spalte8  is null and ( not ( spalte4<= 3E10)) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
    where not (spalte1 between 'CHRISTA' and 'ROMY')
      and not ( spalte5 <> 'BERND' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
    where not (spalte1 between 'CHRISTA' and 'ROMY')
      and not ( spalte5 <> user ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
    where ( not ( spalte1 sounds like 'KRN'))
      and ( spalte8 not in ( -30, -3, -8e-4, 1.2e-1, 1,
          3, 1.2E1, 1.23E1, 2.92E1, 88.8, 1230,
          2.2E+6, 9.9E+8, 9.9E10 )  )
      AND spalte8 > 3.333E20 !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
    where ( not ( spalte7=spalte5)
      and spalte2 between -88.7 and 10) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
    where ((spalte7 is null )
      and not ( not ( spalte8 is null ) ) ) !
commit work release !
