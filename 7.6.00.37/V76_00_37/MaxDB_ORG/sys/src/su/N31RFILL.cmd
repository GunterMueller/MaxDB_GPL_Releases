*   *ID* N31RFILL DBCMD    changed on 1992-09-28-13.26.10 by ADMIN     *
select rfill( 'Hans', 'x', 5 ) from tab where spalte3 is null !
fetch !
select rfill( '    Hans', 'x', 5 ) from tab where spalte3 is null !
fetch !
select rfill( 'Hans    ', 'x', 5 ) from tab where spalte3 is null !
fetch !
select rfill ( 'Hans', 'x', 8 ) , (RFILL(' Hans','x',8))  ,
       rfill('Hans ', 'x', 8 ), rfill  ( '  Hans', '*', 8 ),
       rfill ( 'Hans  ', 'x', 8 ), rfill(' Ha ns ', 'x', 8 )
       from tab where spalte3 is null !
fetch !
select rfill ( 'Hans', ' ', 5 ) from tab where spalte3 is null !
fetch !
select rfill ( 'Hans', 'xx', 9 ) from tab !
fetch !
select rfill ( 'Hans', 'xy', 9 ) from tab !
fetch !
select rfill('Hans', 'x') from tab !
select rfill ( 'Hans', '', 9 ) from tab !
select RFILL( ' ', 'x', 9 ) from tab where spalte3 is null !
fetch !
select rfill( '  ', 'x', 9 ) from tab where spalte3 is null !
fetch !
select  rfill('', '*', 9 ) from tab where spalte3 is null !
fetch !
select rfill( '   Hans', '*', 3 ) from tab where spalte3 is null !
select rfill( spalte1, '*', 19) , rfill( tab.spalte2, '-' ),
       rfill(kern.tab.spalte3, 'a' ) , rfill(spalte1, ' ' )
       from tab !
fetch !
select * from tab where rfill('Hans-Peter Klein', ' ',17 )=tab.spalte1 !
fetch !
select * from tab where rfill( spalte1, ' ' ) = tab.spalte1 !
fetch !
select rfill ( rfill(spalte3, '*'), '+') from tab !
select ( rfill(rfill(spalte3, '*'), '+', 35 )) from tab !
fetch !
rollback work !
