*   *ID* N31SUBST DBCMD    changed on 1992-09-28-13.26.11 by ADMIN     *
select substr ( 'Paul', 2, 2 ), substr(  'Paul', 1 , 2 ),
       SUBSTR ('Paul' ,  2,3), substr  (  'Paul',2), substr('Paul',4,1)
   from tab where spalte2 is null !
fetch !
select substr ('Paul',3,0) from tab where spalte2 is null !
fetch !
select spalte2, substr(spalte2, 5, 2 ) from tab !
fetch !
select substr ( ' ',1,1 ) from tab where spalte2 is null !
fetch !
select substr ( '   ', 1,3 ) from tab where spalte2 is null !
fetch !
select substr ('Paul',0,3) from tab !
select  substr ( 'Paul', -2 ) from tab !
select substr(  'Paul'  ,2, -1 ) from tab !
select substr ( 'Paul', 5 ) from tab !
select substr ('Paul', 2,4 ) from tab  !
select substr ( 'Paul Mueller', 5, 1 ) from tab !
fetch !
select substr ( ' Paul',1,1 ) from tab !
fetch !
select substr ( 'Paul ',5, 1 ) from tab !
fetch !
select substr ( 'Paul     Mueller',  6  ,  3 ) from tab !
fetch !
select substr(spalte4,1) from tab !
select substr(spalte1) from tab !
select substr  ( spalte5 ,2,1 ) from tab !
select   SUBSTR (= ) from tab !
select   SUBSTR (=,2) from tab !
select substr(a,2,1) from tab !
select substr ( tab.spalte1, 4, 1 ), substr(kern.tab.spalte3,4 )
    from tab !
fetch !
select substr ( spalte4 ,1,1) from tab where spalte4 is null !
select substr (spalte1, 12, 1 ) from  tab where spalte1 is not null !
fetch !
select ( substr('',1 ,1 )) from tab !
select ( substr ('', 1,0 )) from tab !
select ( substr ( ' ', 3,1 ) ) from tab !
select substr ( substr(spalte1,1,10), 5, 3 ) from tab !
fetch !
select (substr ( substr( substr ( 'Paul-Gustav Meier', 5, 10),5,3),1,2))
    from tab where spalte2 is null !
fetch !
select substr ( substr ( ' ' , 1,1), 1,1 )
    from tab where spalte2 is null !
fetch !
select substr ( substr ( 'Hans', 2,2 ), 2,3) from tab
     where spalte2 is null !
select 3* substr( 'Hans',2,1 ) from tab where spalte2 is null !
select substr ( spalte3, 5, 27 ) from tab where spalte3 is null !
select substr ( spalte3, 31 ) from tab where spalte3 is null !
commit work !
