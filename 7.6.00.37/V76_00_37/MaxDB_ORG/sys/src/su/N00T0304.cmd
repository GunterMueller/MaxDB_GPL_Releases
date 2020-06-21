*   *ID* N00T0304 DBCMD    changed on 1992-09-28-13.24.45 by ADMIN     *
 ***  3.10 <expression> !
file connect ( kern test !
create table tab ( spalte1 fixed(8,2),
                   spalte2 fixed(18,3),
                   spalte3 float(10) ) !
insert tab values ( 6335.72, 12345678901234.203, 88E12 ) !
insert tab values ( - 76, null, - 7.7e-42 ) !
insert tab values ( null, 6000000000.06, .7e36 ) !
commit work !
declare erg1 cursor for
select spalte1 + 7.00-8. * (6/ 3) *((+spalte2)- ((-05))),
            ( 0.6 +.5*(2 /1) / 3.01 * ( 3  +  (  -  7. )) ) 
   from tab !
fetch erg1 !
select   - spalte1*  spalte2 * (3) ,
         0.6E+1 * tab.spalte3/(-spalte1),
         spalte3/0.5 +2
   from tab !
fetch !
select    -  0.32 * ( - spalte2 ) -(.3* .30 ) neu1,
          0e32 / .4  * spalte3,
          spalte2 -spalte1 *(3.43- (- 004.300 ) ) neu3
   from tab !
fetch !
select ( spalte2* (- kern.tab.spalte1 )),
         - 2.e-12 -82 * 13.e3 - 34*spalte3
   from tab !
fetch !
select * from tab where 3* spalte2/( - 5)< ( spalte3 +spalte2 * 5.7) !
fetch !
select spalte1 from tab
   where (spalte3* .1E-12 + ((042e20) )) /spalte3 <> 3/(-spalte2-(+1))
     and spalte2*spalte2 between  -(10e30+10E+30)
     and tab.spalte2* (kern.tab.spalte2 + 1000000 ) !
fetch !
update  tab set spalte1 = 5.0/2.00/spalte3* spalte3 -  1.78e5,
                spalte3 = ( -1E-32 - 6.66E06 ) * spalte3 !
select * from tab !
fetch !
rollback work !
update  tab set spalte2 = 4.08880*spalte1-(2/5)+tab.spalte2,
                spalte3 = 0046.738*(+(-kern.tab.spalte1))+ .1e13 !
select * from tab !
fetch !
drop table tab !
commit work release !
