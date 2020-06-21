*   *ID* N24SEL6  DBCMD    changed on 1992-09-28-13.25.44 by ADMIN     *
 *** <single select statement> wird getestet.
 *** Fehlerfaelle. !
file connect ( kern test !
select * , spalte2
   into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10,:a11
   from tab where spalte1 = 'ILSE' !
select spalte2, *
   into :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10,:a11
   from tab where spalte1 = 'ILSE' !
select spalte1 into :a1 from tab1 where spalte1 = 'ILSE' !
select spalte1 into :a1 from tab
   where tab1.spalte1 = 'ILSE' !
select dir spalte1 into :a1 from tab
   where spalte1 = 'ILSE' !
select  spalte1 into :a1 from tab
   where key spalte1 = 'ILSE' !
select spalte2 into :a1 from tab where spalte2 = -33
   order by spalte2 !
select spalte2 into :a1 from tab where spalte2 = -33
   for update !
select spalte2 into :a1 from tab order by spalte2 !
select spalte2 into :a1 from tab for update !
declare result cursor for select spalte2 into :a1 from tab !
select spalte2 into :a1 from tab where and spalte2 = -33 !
select spalte2 into :a1 from tab where or  spalte2 = -33 !
select spalte2 into :a1 from tab where spalte2 = -33 and !
select spalte2 into :a1 from tab
   where ( and spalte2 = -33 ) !
select spalte2 into :a1 from tab
   where and and spalte2 = -33 !
select spalte2 into :a1 from tab where (  !
select spalte2 into :a1 from tab where  ) !
select spalte2 into :a1 from tab where () !
select spalte2 into :a1 from tab where ( ) !
select ref1.spalte2, ref2.spalte3 into :a1,:a2
   from tab ref1, tab ref2
   where and ref1.spalte2 = ref2.spalte3 !
commit work release !
