*   *ID* N30ASCII DBCMD    changed on 1992-09-28-13.26.00 by ADMIN     *
select ascii ( 'abc' ), ascii('+-?_6a'), ASCII(  ' 23 7' ),
       ascii ('TRUNC')   from tab !
fetch !
select ascii ( 7) from tab !
select ascii ( NULL ) from tab where spalte1 = 0 !
fetch!
select ascii (spalte4 ), ASCII( spalte5  ) from tab order by 1 asc !
fetch !
select ascii (spalte4 ), ASCII( spalte5  ) from tab order by 2 !
fetch !
select ascii (spalte4 ) col1,  ASCII( spalte5  ) col2
    from tab order by col1 desc !
fetch !
select ascii ( spalte1 ) from tab !
select ascii ( spalte3 ) from tab !
commit work !
