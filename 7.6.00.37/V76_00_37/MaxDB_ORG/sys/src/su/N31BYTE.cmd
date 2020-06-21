*   *ID* N31BYTE  DBCMD    changed on 1992-09-28-13.26.05 by ADMIN     *
create  table tab ( spalte1 char(10) byte,
                    spalte2 char(15) byte,
                    spalte3 char(10) byte ) !
insert tab values ( X'814181', X'824282', null ) !
select trunc ( spalte1 ) from tab !
select abs ( spalte1 ) from tab !
select index ( spalte1, X'41' ) from tab !
fetch !
select index ( spalte2, X'82', 2 ) from tab !
fetch !
select index ( spalte1, 'A' ) from tab !
fetch !
select length ( spalte1 ) , length ( spalte2 ) from tab !
fetch !
select length ( spalte3 ) from tab !
fetch !
select num ( spalte1 ) from tab !
select substr( spalte1, 2, 2 ), substr ( spalte2, 3 ) from tab !
fetch !
select substr ( spalte3, 3 ) from tab !
fetch !
select spalte1 & spalte2 from tab !
fetch !
select spalte1 & X'838383' from tab !
fetch !
select spalte1 & 'ccc' from tab !
fetch !
select lpad ( spalte1, 3, X'83' ),
       rpad ( spalte2, 3, X'84' ) from tab !
fetch !
select rpad ( spalte3, 3, X'83' ) from tab !
fetch !
select lfill ( spalte3, X'83' ) from tab !
fetch !
select lfill ( spalte1, X'83' ) , rfill ( spalte2, X'83', 5 ) from tab !
fetch !
select trim ( spalte1 ), trim ( X'8383830000000000' ) from tab !
fetch !
select chr ( spalte1 ) from tab !
select upper ( spalte1 ) from tab !
select lower ( spalte2 ) from tab !
select ascii ( spalte1 ) from tab !
rollback work !
