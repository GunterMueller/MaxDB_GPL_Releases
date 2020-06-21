*   *ID* N31LPAD  DBCMD    changed on 1992-09-28-13.26.10 by ADMIN     *
select lpad ( 'Hans',5,'x',9), LPAD('Ha ns' , 1,  'x' , 9 ),
       lpad ( 'Hans',  ( 5), 'x', 9 ), lpad( 'Hans' ,5,'x',10 ),
       lpad (  'Hans', 1 , 'x', 8 )
       from tab where spalte1 is NULL !
fetch !
select lpad ( 'Hans', 5, 'x', 8 ) from tab where spalte1 is null !
select lpad ( 'Hans', 5, ' ', 8 )
       from tab where spalte1 is null !
fetch !
select lpad ( 'Hans', 5, ' ', 9 )
       from tab where spalte1 is null !
fetch !
select lpad(' Hans ', 3, 'a', 10 ) from tab where spalte1 is null !
fetch !
select lpad ( ' Hans', 3 ,'  ', 12 ) from tab where spalte1 is null !
fetch !
select lpad ( ' Hans', 3 ,'^-', 12 ) from tab where spalte1 is null !
fetch !
select lpad ( 'Hans ' , 5,'*', 8 ) from tab where spalte1 is null !
fetch !
select lpad ( '     ' , 5, '_' , 8 ) from tab where spalte1 is null !
fetch !
select   lpad  ('Hans',0,'x',8 ) from tab where spalte1 is null !
fetch !
select lpad ( '', 5,'x' , 8 ) from tab where spalte1 is null !
fetch !
select lpad (('Hans') , 5, 'x', 15 ) from tab !
select lpad ( 'Hans' ) from tab !
select lpad ( 'Hans' , 5 ) from tab !
select lpad ( 'Hans', 5 , 'x' ) from tab !
select lpad ( 'Hans' , 5, 'xx', 9 ) from tab !
select lpad ( 'Hans' , 5, 'xx', 19) from tab !
fetch !
select lpad ( 'Hans', 5, 'x ', 9 ) from tab !
select lpad ( 'Hans', 5, 'x ', 19) from tab !
fetch !
select lpad ( null, 5, 'x', 9 ) from tab !
fetch !
select lpad ( a , 5 , 'x', 6 ) from tab !
select lpad ( 'Hans', 5 , x, 9 ) from tab !
select lpad ( 'Hans', '5', 'x', 9 ) from tab !
select lpad ( spaLTE4, 3 , '.', 10 ),
       lpad ( tab.spalte5, 5 , '+' ,15 ),
       lpad ( kern.tab.spalte4, 1 , 'x', 8 ) from tab !
fetch !
select lpad (  spalte5  , 10, 'y' ) from tab !
fetch !
select lpad ( spalte5, 20, 'y' ) from tab !
fetch !
select lpad ( spalte5,1, 'y' ) from tab !
fetch !
select lpad ( 'Hans', -1, 'x', 8 ) from tab !
select lpad ( 'Hans', 5, 'x', 0 ) from tab !
select lpad ( 'Hans', 5 , 'x', -2 ) from tab !
select lpad ( spalte1, 5 ,'x', 10 ) from tab !
select lpad ( spalte2 , 5 , 'x' ) from tab !
select lpad ( 'Hans', 2*3, 'x', 15 ) from tab !
fetch !
select lpad ( 'Hans', +01.00, 'x', 9 ) from tab where spalte1 is null !
fetch !
select lpad ( 'Hans' , 2.3, 'x', 9 ) from tab !
fetch !
select lpad ( user, 3, '0', 15 ) from tab where spalte1 is null !
fetch !
select lpad ( user, 3, '0', 5 ) from tab !
select lpad ( user, 3, '*' ) from tab !
fetch !
select lpad ( date , 5, '*' ,10 ) from tab where spalte1 is null !
fetch !
select lpad ( DATE , 5 , '*', 15) from tab !
fetch !
select lpad ( DATE, 5 , '*' ) from tab where spalte1 is null !
fetch !
select lpad ( TIME, 5, '*',11 ) from tab where spalte1 is null !
fetch !
select lpad ( TIME, 5, '*', 8 ) from tab where spalte1 is null !
select lpad ( spalte4, 5, '*', spalte2  )
       from tab where spalte2 = 12 !
select lpad ( spalte4, spalte2, '*' ,15 ) from tab where spalte2 =12 !
select lpad ( spalte4, spalte2 * 0 +1, '*', 15 ) from tab !
fetch !
select * from tab where lpad ( 'Hans', 3, '*', 8 ) = '***Hans'
                    and spalte2 is null !
fetch !
select * from tab where lpad ( 'Hans', 3, '*', 8 ) = ' ***Hans' !
select * from tab where '***Hans ' = lpad ( 'Hans', 3 , '*', 8 )
                    and spalte1 is null !
fetch !
select * from tab where (lpad ( spalte5, 3, 'b' )) > 'bbb' !
fetch !
select lpad ( lpad ( 'Hans', 3, '*', 8 ), 3, '+', 15 )
      from tab where spalte1 is null !
fetch !
commit work !
