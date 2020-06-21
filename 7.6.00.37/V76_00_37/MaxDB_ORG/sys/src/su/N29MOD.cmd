*   *ID* N29MOD   DBCMD    changed on 1992-09-28-13.25.59 by ADMIN     *
file connect ( kern test !
create table tab ( spalte1 fixed(5,2),
                   spalte2 float(7),
                   spalte3 float(16),
                   spalte4 char(10),
                   spalte5 char(10) ) !
insert tab values ( 32, 12, null, 'Paul', null ) !
select 17 mod 5, 15 mod 5, 8 mod 5, 5 mod 5, 3 mod 5, 0 mod 5,
       -2 mod 5, - 5 MOD 5, -9. MOD 5, -  10 mod (+5), -11 mod 5
       from tab !
fetch !
select 17 mod (5), (17) mod (5), ((17)) mod (( 5 )) from tab !
fetch !
select (1E18-1) mod 5, ( - 1e18 +1 ) mod 5 from tab !
fetch !
select 1E36 mod 5 from tab !
select 5 mod 1E36 from tab !
select 999999999999999999 mod 5, -999999999999999999 mod 5 from tab !
fetch !
select spalte1 mod spalte2 from tab !
fetch !
select 6 mod spalte3, spalte3 mod 5, spalte3 mod spalte3 from tab !
fetch !
select NULL mod 5 from tab !
fetch!
select 8 mod NULL from tab !
fetch!
select 7.2 mod 5 from tab !
select -12.3 mod 5 from tab !
select 7 mod 5.2 from tab !
select 7 mod 0 from tab !
select 7 mod (-5) from tab !
select 'Otto' mod 5 from tab !
select 8 mod 'Otto' from tab !
select spalte4 mod 5 from tab !
select  4 mod spalte4 from tab !
select 3 mod spalte5 from tab !
select spalte5 mod 5 from tab !
select 3-5 mod 5, 3 * 6 mod 5, 12 /6 mod 5 from tab !
fetch !
select 12 mod 5 - 18, 12 mod 5 + 18.2, 12 mod 5 * 3,12 mod 5/3
   from tab !
fetch !
select 18 mod 10 mod 3 , -18 mod 10 mod 3 from tab !
fetch !
select 6 - 18 mod 5 * 3 + 5 *7 mod 12 - 5 from tab !
fetch !
select 7mod 5 from tab !
fetch !
select 7 mod5 from tab !
fetch !
select spalte2mod 5 from tab !
select 26 modspalte2 from tab !
drop table tab !
commit work release !
