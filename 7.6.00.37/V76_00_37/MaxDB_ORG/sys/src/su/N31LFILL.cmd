*   *ID* N31LFILL DBCMD    changed on 1992-09-28-13.26.09 by ADMIN     *
select lfill( 'Hans', 'x', 5 ) from tab where spalte3 is null !
fetch !
select lfill( '    Hans', 'x', 5 ) from tab where spalte3 is null !
fetch !
select lfill( 'Hans    ', 'x', 5 ) from tab where spalte3 is null !
fetch !
select lfill ( 'Hans', 'x', 8 ) , (LFILL(' Hans','x',8))  ,
       lfill('Hans ', 'x', 8 ), lfill  ( '  Hans', '*', 8 ),
       lfill ( 'Hans  ', 'x', 8 ), lfill(' Ha ns ', 'x', 8 )
       from tab where spalte3 is null !
fetch !
select lfill ( 'Hans', ' ', 5 ) from tab where spalte3 is null !
fetch !
select lfill ( 'Hans', 'xx', 9 ) from tab !
fetch !
select lfill ( 'Hans', 'xy', 9 ) from tab !
fetch !
select lfill('Hans', 'x') from tab !
select lfill ( 'Hans', '', 9 ) from tab !
select LFILL( ' ', 'x', 9 ) from tab where spalte3 is null !
fetch !
select lfill( '  ', 'x', 9 ) from tab where spalte3 is null !
fetch !
select  lfill('', '*', 9 ) from tab where spalte3 is null !
fetch !
select lfill( '   Hans', '*', 3 ) from tab where spalte3 is null !
select lfill( spalte1, '*', 19) , lfill( tab.spalte2, '-' ),
       lfill(kern.tab.spalte3, 'a' ) , lfill(spalte1, ' ' )
       from tab !
fetch !
select * from tab where lfill('Hans-Peter Klein', ' ',17 )=tab.spalte1 !
fetch !
select lfill ( lfill(spalte3, '*'), '+') from tab !
select ( lfill(lfill(spalte3, '*', '+', 35 )) from tab !
select ( lfill(lfill(spalte3, '*'), '+', 35 )) from tab !
fetch !
rollback work !
