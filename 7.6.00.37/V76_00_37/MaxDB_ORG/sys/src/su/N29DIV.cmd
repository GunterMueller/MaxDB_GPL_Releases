*   *ID* N29DIV   DBCMD    changed on 1992-09-28-13.25.57 by ADMIN     *
file connect ( kern test !
create table tab ( spalte1 fixed(5,2),
                   spalte2 float(7),
                   spalte3 float(16),
                   spalte4 char(10),
                   spalte5 char(10) ) !
insert tab values ( 32, 12, null, 'Paul', null ) !
select 17 div 5, 15 div 5, 8 div 5, 5 div 5, 3 div 5, 0 div 5,
       -2 div 5, - 5 DIV 5, -9. DIV 5, -  10 div (+5), -11 div 5
       from tab !
fetch !
select 17 div (-5), 15 div (-5 ), 8 div ((  - 5 )),
       5 div (-5), 3 div (-5), 0 div (- 5),
       -2 div (-5), - 5 DIV (-  5 ), -9. DIV (-5.00) ,
       -  10 div (-005), -11 div (-5)
       from tab !
fetch !
select 8 div -5 from tab !
fetch !
select (1E18-1) div 5, ( - 1e18 +1 ) div 5 from tab !
fetch !
select 1E36 div 5 from tab !
select 5 div 1E36 from tab !
select 999999999999999999 div 5, -999999999999999999 div 5 from tab !
fetch !
select spalte1 div spalte2 from tab !
fetch !
select 6 div spalte3, spalte3 div 5, spalte3 div spalte3 from tab !
fetch !
select 8 div ( _ ) from tab !
select NULL div 5 from tab !
fetch!
select 8 div NULL from tab !
fetch!
select 7.2 div 5 from tab !
select -12.3 div 5 from tab !
select 7 div 5.2 from tab !
select 7 div (-5.2) from tab !
select 7 div 0 from tab !
select 'Otto' div 5 from tab !
select 8 div 'Otto' from tab !
select spalte4 div 5 from tab !
select  4 div spalte4 from tab !
select 3 div spalte5 from tab !
select spalte5 div 5 from tab !
select 3-5 div 5, 3 * 6 div 5, 12 /6 div 5 from tab !
fetch !
select 12 div 5 - 18, 12 div 5 + 18.2, 12 div 5 * 3,12 div 5/3
   from tab !
fetch !
select 18 div 10 div 3 , -18 div 10 div 3 from tab !
fetch !
select 6 - 18 div 5 * 3 + 5 *7 div 12 - 5 from tab !
fetch !
select 7div 5 from tab !
fetch !
select 7 div5 from tab !
fetch !
select spalte2div 5 from tab !
select 26 divspalte2 from tab !
rollback  work release !
