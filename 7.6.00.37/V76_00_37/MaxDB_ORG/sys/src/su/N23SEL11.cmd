*   *ID* N23SEL11 DBCMD    changed on 1992-09-28-13.25.36 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT * INTO :a1,:a2, ... ,:a8 FROM tab
 *** WHERE ... OR ... AND ...
 *** Es existiert genau eine Ergebniszeile. !
file connect ( kern test !
 * gibt mit user KERN einen Fehler !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( spalte2 in (-123.17, -88.8, -88.09,
         -70.00, - 58.76, - 55,-36,-33,-3.00, -0.04,
         0.12, 3, 4, +7.07, 12.3, +22, 33 , 48.12,
         112.28, 120.0,  123.17, 256.54, 299.99 ))
      or spalte3 is not null
     and not ( spalte7 between 'AACHEN' and USER ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( spalte2 in (-123.17, -88.8, -88.09,
         -70.00, - 58.76, - 55,-36,-33,-3.00, -0.04,
         0.12, 3, 4, +7.07, 12.3, +22, 33 , 48.12,
         112.28, 120.0,  123.17, 256.54, 299.99 ))
      or spalte3 is not null
     and not ( spalte7 between 'AACHEN' and 'ZOPPOT' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where ( not ( spalte2 in (-123.17, -88.8, -88.09,
         -70.00, - 58.76, - 55,-36,-33,-3.00, -0.04,
         0.12, 3, 4, +7.07, 12.3, +22, 33 , 48.12,
         112.28, 120.0,  123.17, 256.54, 299.99 ))
      or spalte3 is not null )
     and (not ( spalte1 <= 'KARL' ))
     and (spalte1 < 'KLAUS' ) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where not ( spalte2 in (-123.17, -88.8, -88.09,
         -70.00, - 58.76, - 55,-36,-33,-3.00, -0.04,
         0.12, 3, 4, +7.07, 12.3, +22, 33 , 48.12,
         112.28, 120.0,  123.17, 256.54, 299.99 ))
      or ( spalte3 is not null
     and (   not ( spalte7 between 'AACHEN' and 'ZOPPOT' )
         and not ( spalte7 between 'AACHEN' and user     ))) !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte7 = user
      or not ( spalte1 <= 'HANS' )
     and spalte1 sounds 'JURGN' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where ((spalte7 = user
      or not ( spalte1 <= 'HANS' )))
     and spalte1 sounds 'JURGN' !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where spalte7 = user
      or ( not ( spalte1 <= 'HANS' )
     and (spalte1 sounds 'JURGN'))  !
select * into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8 from tab
   where  not ( spalte4 not between 3 and 3
      or spalte5 in   ( 'HAMBURG' , 'BERLIN' )
     and not ( spalte1>=user )) !
commit work release !
