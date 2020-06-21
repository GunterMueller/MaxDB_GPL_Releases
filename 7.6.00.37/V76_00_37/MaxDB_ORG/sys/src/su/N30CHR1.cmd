*   *ID* N30CHR1  DBCMD    changed on 1992-09-28-13.26.01 by ADMIN     *
select chr(723), CHR ( +123 ), CHR(-2), chr  ( + 15 ), chr ( -  72),
       chr(0020) from tab where spalte1 is null !
fetch !
select chr (  7.32 ), chr(+ 8.), chr ( -  .820  ), chr (-  000.023 ),
       chr (.12), chr (+.12  ) col6, chr ( -.12 )
       from tab  where spalte2 >0 !
fetch !
select chr(2E26 ), chr(+2E+26), chr ( - .2E-12 ), chr( 8.e6 ),
       chr (-8e06  ), chr  (+  0001.2E+6 ), chr ( 0.0e00 )
       from tab where tab.spalte4 = 'Paul' !
fetch !
select chr ( tab.spalte1) col1, chr( spalte2 ) col2,
       chr  (kern.tab.spalte3 ) from kern.tab !
fetch !
select chr (spalte4 ) from tab !
fetch !
select chr(spalte5  ) from tab !
fetch !
select chr ( a ) from tab !
select chr (_) from tab !
select chr ( 'abc' ) from tab !
fetch !
select chr ( 3+5-2*4/2 ) from tab where spalte1 = 0 !
fetch !
select chr ( spalte1 * ( 3) + ((spalte2 + spalte3)/ 3 ) ) from tab !
fetch !
commit work !
