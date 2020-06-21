*   *ID* N31RPAD  DBCMD    changed on 1992-09-28-13.26.11 by ADMIN     *
select rpad ( 'Hans', 3, 'x', 8 ), (rpad( ' Hans', 3, 'x', 8 ) ),
       RPAD ( '  Hans', (3),'x',10) , rpad ('Hans ',+3.0,'x',10),
       rpad ( 'Hans  ',3,'x', 10 ), rpad(  ' Ha ns ',  3,  'x' , 15 )
       from tab where spalte3 is null !
fetch !
select rpad ('Hans', 3 , ' ' , 8 ) from tab where spalte3 is null !
fetch !
select rpad (  'Hans', 3 ,'  ', 9 ) from tab !
select rpad (  'Hans', 3 ,'  ', 10) from tab !
fetch!
select rpad ('Hans', 3, 'xx', 9 ) from tab !
select rpad ('Hans', 3, 'xx', 10) from tab !
fetch!
select  rpad  ( 'Hans',1,'x', 4 ) from tab !
select rpad ( 'Hans', 5 , 'x', 8 ) from  tab !
select rpad ( ' ',5,'x', 8 ) from tab where spalte3 is null !
fetch !
select rpad ( '    ', 5 , 'x',   10 ) from tab where spalte3 is null !
fetch !
select rpad('' , 5, 'x', 7 ) from tab where spalte3 is null !
fetch !
select rpad ( 'Hans', 5, '' ,10 ) from tab !
select rpad ( 'Hans', 2* 2,'x', 8 ) from tab where spalte3 is null !
fetch !
select rpad('Hans', 3, 'x') from tab !
select RPAD ( a, 3 , 'x' ) from tab !
select rpad ( 'Hans', '3', 'x', 8 ) from tab !
select rpad ( 'Hans', 0, 'x', 8 ) from tab !
fetch !
select rpad ( 'Hans', -3, 'x', 10 ) from tab !
select ( rpad(spalte1, 3 , '2' )) from tab !
fetch !
select rpad (tab.spalte1,1, 'x', 19),rpad(kern.tab.spalte2,10,'x',30)
       from tab !
fetch !
select rpad (spalte4, 1, 'x',20 ) from tab !
select rpad(spalte1, spalte4+tab.spalte4- 60, 'x', 25)
    from tab where spalte4 =32 !
fetch !
select * from tab where rpad(spalte3, 3 , 'x') >= 'G' !
fetch !
select * from tab where ( rpad(tab.spalte1,5,'*', 30 )) is not null !
fetch !
select * from tab
    where   rpad(rpad (kern.tab.spalte2,3,'-'),3, '+', 19)
          > rpad ('aaa',5,'b',8 ) !
fetch !
rollback work !
