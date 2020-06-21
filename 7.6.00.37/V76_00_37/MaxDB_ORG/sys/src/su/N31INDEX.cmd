*   *ID* N31INDEX DBCMD    changed on 1992-09-28-13.26.07 by ADMIN     *
select index ( 'Hans', 'a' ) , INDEX ('Hans','H'),
       (index('Hans', 's' ) ) , index  (  'Hans', 'Ha' ) ,
       index('Hans', 'an' ), ((index ('Hans', 'ns' ) )),
       index ( 'Hans', 'Hans' ) , index ('HansHans', 'an' )
       from tab where spalte1 is null !
fetch !
select index ( ' Hans', 'an' ) from tab where spalte1 is null !
fetch !
select  index  ( ' Hans' , ' ' ) from tab where spalte1 is null !
fetch !
select  index  ( ' Hans' , '' ) from tab where spalte1 is null !
fetch !
select  index  ( '' , '' ) from tab where spalte1 is null !
fetch !
select  index  ( '' , 'Hans' ) from tab where spalte1 is null !
fetch !
select index('Hans Peter', ' ' ) from tab where spalte1 is null !
fetch !
select index ( 'Hans Peter', ' Pe' ) from tab where spalte1 is null !
fetch !
select index ( 'Hans Peter', 'ns ' ) from tab where spalte1 is null !
fetch !
select index ('Hans  Peter', '  ') from tab where spalte1 is null !
fetch !
select index ( 'Hans  Peter', '  P' ) from tab where spalte1 is null !
fetch !
select index ('Hans  Peter', 's  ') from tab where spalte1 is null !
fetch !
select  INDEX ( 'Hans ', ' ' ) from tab where spalte1 is null !
fetch !
select index ( 'Hans  ', '  ' )   from tab where spalte1 is null !
fetch !
select  index('  Hans', '  ' ) from tab where spalte1 is null !
fetch !
* --------- jedes Jahr aendern ----------------+
select (index (USER, 'ERN' )), index ( DATE, '00') , index (time, '0')
       from tab where spalte1 is null !
fetch !
select index ( spalte4, '12') from tab where spalte1 is null
                                         or  spalte1 < 0 !
fetch !
select index ( 'Hans', 'b' ) from tab where spalte1 is null !
fetch !
select index ('Hans' , 'ns ' ) from tab where spalte1 is null !
fetch !
select index ( 'Hans' , 'aN' ) from tab where spalte1 is null !
fetch !
select index ( spalte1, '2' ) from tab !
select index ( spalte4, 'a' ) from tab where spalte3 is null !
fetch !
select index ( spalte4, ' ' ) from tab where spalte3 IS NULL !
fetch !
select index (tab.spalte5, '-' ) , index ( kern.tab.spalte5, '-')
       from tab where spalte5 is not null !
fetch !
select index ( 'Hans', 'n' , -1 ) from tab !
select index ('Hans', 'n', 0 ) from tab !
select index ('Hans', 'n', 1 ), index ( 'Hans', 'n', 2 ),
       index ( 'Hans', 'n', 3 ) , index ('Hans', 'n', 4 )
       from tab where spalte1 is null !
fetch !
select index ( 'Hans', 'n', 5 ) from tab !
select index ( spalte4, '2'  ,3  ) from tab !
fetch !
select index ( spalte4, '2', 11 ) from tab !
select index ( spalte4, ' ' , 1 ) from tab !
fetch !
select index ( 'St. Paul', spalte4 ) , index ( 'St. Paul', tab.spalte4 ),
       index ( 'St. Paul', kern.tab.spalte4 ),
       index ( 'St. Paul', spalte4 , 3 ), index('St. Paul',spalte4, 6)
       from tab !
fetch !
select index ( spalte5 , spalte4 ), index ( spalte5,spalte4,1 ),
       index ( spalte5, spalte4, 2 ), index ( spaLTE5, spalte4,3)
       from tab !
fetch !
select index ( 'Hans-Peter', 'ete', 2*2 )
       from tab where spalte1 is null !
fetch !
select index('Hans Peter' , 'ete', (-1) *5 ) from tab !
select index ('Hans-Peter', 'ete', spalte2 ) from tab !
fetch !
select index ('Hans-Peter Kleinschmidt', 'ete' , spalte2 )
       from tab where spaLTE2 = 12 !
fetch !
select * from tab where index ( 'Hans', 'an' ) = 1+1 !
fetch !
select  * from tab where index ( 'Hans' , 'an' ) = '2' !
select * from tab where index(spalte4, '2E' , 2 ) =
                        index ( spalte5, '7', 2 ) !
fetch !
select * from tab where (index('Hans', 'an')+2 )<index (spaLTE5,'-') !
fetch !
select index (tab.spalte4, '2' ) * index (spalte5,'3' ) from tab !
fetch !
select index ( 'Hans', 'n', index('Paul', 'u', 1 )) from tab
       where spalte1 is null !
fetch !
select index ('Hans', 'a', spalte1 ) from tab !
fetch !
commit !
