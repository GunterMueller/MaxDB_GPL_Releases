*   *ID* N30CHR2  DBCMD    changed on 1992-09-28-13.26.01 by ADMIN     *
select chr ( 'abc', 4 ) from tab where spalte4 = 'Paul' !
fetch !
select chr ( 'abc', 3 ) from tab where spalte4 = 'Paul' !
fetch !
select chr ( 'abc', 2 ) from tab where spalte4 = 'Paul' !
fetch !
select chr ( '   abc', 2 ) from tab where spalte4 = 'Paul' !
fetch !
select chr ( 123, 255 ) from tab where spalte4 = 'Paul' !
select chr ( 123, 254 ) from tab where spalte4 = 'Paul' !
fetch !
select chr ( 123, 4   ) from tab where spalte4 = 'Paul' !
fetch !
select chr ( 123, 3   ) from tab where spalte4 = 'Paul' !
fetch !
select chr ( 123, 2   ) from tab where spalte4 = 'Paul' !
select chr ( 123, 0   ) from tab where spalte4 = 'Paul' !
select chr ( 123, -1  ) from tab where spalte4 = 'Paul' !
select chr ( 123, *   ) from tab where spalte4 = 'Paul' !
select chr ( 123, 4,1 ) from tab where spalte4 = 'Paul' !
select chr ( 2.34E12, 14 ) from tab where spalte4 = 'Paul' !
fetch !
select chr ( 2.34E12, 13 ) from tab where spalte4 = 'Paul' !
fetch !
select chr ( 2.34E12,  9 ) from tab where spalte4 = 'Paul' !
select chr ( 2.34E12,  4 ) from tab where spalte4 = 'Paul' !
select chr ( spalte1,10 ) from tab !
fetch !
select chr ( spalte1, 6 ) from tab !
fetch !
select chr ( spalte1, 5 ) from tab !
fetch !
select chr ( spalte1, 4 ) from tab !
select chr ( spalte1, 2 ) from tab !
select chr ( spalte2, 8 ) from tab !
select chr ( spalte2, 15 ) from tab !
fetch !
select chr ( spalte2, 8 ) from tab where spalte4 = 'Paul' !
fetch !
select chr ( spalte2, 8 ) from tab where spalte4 = '123' !
fetch !
select chr ( spalte4, 15 ) from tab !
fetch !
select chr ( spalte4, 5 ) from tab !
fetch !
commit work !
