*   *ID* N24SEL3  DBCMD    changed on 1992-09-28-13.25.43 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT / <distinct spec> / <select list>
 *** INTO <parameter list>
 *** FROM <table name> <reference name>
 *** WHERE ...
 *** Getestet wird <reference name>.
 *** Es existiert genau eine Ergebniszeile. !
file connect ( kern test !
select *
   into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10
   from tab ref
   where spalte1 = 'ILSE' !
select *
   into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10
   from tab ref
   where ref.spalte1 = 'ILSE' !
select *
   into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10
   from tab referencename_5678
   where referencename_5678.spalte1 = 'ILSE' !
select spalte1, ref.spalte2, ref.spalte8, spalte5
   into :a1,:a2,:a3,:a4
   from tab ref
   where spalte1 = 'ILSE' !
select ref.spalte1, spalte2
   into :a1,:a2
   from tab ref
   where spalte1 = 'ILSE' !
select referencename_5678.spalte1, referencename_5678.spalte2,
       spalte3
   into :a1,:a2,:a3
   from tab referencename_5678
   where (referencename_5678.spalte1, referencename_5678.spalte5) in
         (('ILSE', 'BERLIN'), ('ILSE', 'MUENCHEN')) !
select  ref.spalte1 neu1, spalte2 neu2,spalte3, ref.spalte4
   into :a1, :a2 , :a3, :a4
   from  tab  ref
   where spalte1 = 'ILSE'
     and ref.spalte2 <=  120 !
select all  ref.spalte1, ref.spalte2 neu2,
       spalte3 neu3, spalte4
   into :a1,:a2,:a3,:a4
   from tab   ref
   where  ref.spalte1  like '__SE'
     and  spalte8 is null !
select all spalte1  , ref.spalte2, ref.spalte3 neu3,spalte4
   into :a1,:a2,:a3,:a4
   from   tab  ref
   where spalte1 like  '__SE'
     and  ref.spalte8 is null !
select distinct spalte1, ref.spalte2,ref.spalte3 neu3,
                spalte4 neu4
   into :a1,:a2,:a3,:a4
   from tab ref
   where ref.spalte1 = user
     and spalte5  is  null !
select distinct ref.spalte1, spalte2, ref.spalte3 neu3,spalte4
   into :a1,:a2,:a3,:a4
   from tab  ref
   where spalte1 = user
     and ( ref.spalte5 is null ) !
select DISTINCT ref.spalte1 neu1, ref.spalte2 neu2,
                spalte3 neu3, ref.spalte4
   into :a1,:a2, :a3,:a4
   from tab ref
   where spalte1 = user
     and spalte5 is null !
commit work release !
