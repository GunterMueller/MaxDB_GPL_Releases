FILE pnxdiff !

 * PAGE 170 !
 *++ Zum Auftakt eine einzeilige Differenz, in der das Tagesdatum erscheint!
select sysdate from dual!
fetch into :a!

 * PAGE 171 !
select holiday, act_date, celebrate from holiday !
fetch into :a, :b, :c !

 * PAGE 172 !
select power (4,3) from dual !
fetch into :a !

 * PAGE 173 !
select holiday, act_date, celebrate
from holiday
where celebrate - act_date != 0 !
fetch into :a, :b, :c !

 * PAGE 174 !
select add_months(celebrate, 6) "Feast Day"
from holiday
where holiday like 'FAST%' !
fetch into :a !

select add_months(celebrate, -6) "Last Day"
from holiday
where holiday = 'COLUMBUS DAY' !
fetch into :a !

select holiday, least(act_date, celebrate) "First",
       act_date, celebrate
from holiday
where act_date - celebrate != 0!
fetch into :a, :b, :c, :d !
 
 * PAGE 175 !
select least('20-JAN-92', '20-DEC-92') from dual!
fetch into :a!

select least(to_date('20-JAN-92'), to_date('20-DEC-92')) from dual!
fetch into :a!

 * PAGE 176 !
select holiday, celebrate
from holiday
where celebrate = least('20-JAN-92', '07-SEP-92') !
fetch into :a, :b!

select holiday, celebrate
from holiday
where celebrate = least(to_date('20-JAN-92'), 
                             to_date('07-SEP-92')) !
fetch into :a, :b!

select cycledate from payday !
fetch into :a !

 * PAGE 177 !
select next_day(cycledate, 'FRIDAY') "Payday!"
from payday !
fetch into :a !

select next_day(cycledate-1, 'FRIDAY') "Payday!"
from payday !
fetch into :a !

 * PAGE 178 !
select last_day(cycledate) "End Month"
from payday !
fetch into :a !

 * PAGE 179 !
select * from birthday !
fetch into :a !

 *++ Jetzt folgen fuenf einzeilige Differenzen, in der als "Actual Age" eine
 *++ taeglich wechselnde Float ausgegeben wird!
select firstname, lastname, birthdate, age, 
       months_between (sysdate, birthdate)/12 "ActualAge"
from birthday !
fetch into :a, :b, :c, :d, :e !

 *++ Jetzt kommt eine einzeilige Differenz, die das Tagesdatum und den 1. Tag
 *++ des siebtnaechsten Monats enthaelt!
select sysdate "Today", 
       last_day(add_months(sysdate,6)) + 1 "Review"
from dual !
fetch into :a, :b !

 *++ Zum Schluss noch drei einzeilige Differenzen mit taeglich
 *++ wechselnden Floats!
 * PAGE 180 !
select (last_day(add_months(sysdate,6)) + 1) - sysdate "Wait"
from dual !
fetch into :a !

select TO_DATE('11-FEB-92') - Sysdate from dual !
fetch into :a !

 * PAGE 181 !
select to_date('11-FEB-92') - round(sysdate) from dual !
fetch into :a !

 * PAGE 182 !
select birthdate, to_char(birthdate, 'MM/DD/YY') "Formatted"
from birthday
where firstname = 'VICTORIA' !
fetch into :a, :b !

 * PAGE 185 !
select birthdate, to_char(birthdate, 'YYMM>DD') "Formatted"
from birthday
where firstname = 'VICTORIA' !
fetch into :a, :b !

select birthdate, 
       to_char(birthdate, 'Month, DDth   "in, um," YyyY') "Formatted"
from birthday !
fetch into :a, :b !

 * PAGE 186 !
select birthdate, to_char(birthdate, 'Month, ddth, YyyY') "Formatted"
from birthday !
fetch into :a, :b !

 * PAGE 187 !
select birthdate, to_char(birthdate, 'fmMonth, ddth, YyyY') "Formatted"
from birthday !
fetch into :a, :b !

select firstname, birthdate, to_char(birthdate, 
       '"Baby Girl on" fmMonth ddth, YYYY, "at" HH:MI "in the Morning"')
       "Formatted"
from birthday
where firstname = 'VICTORIA' !
fetch into :a, :b, :c !

select firstname, birthdate, to_char(birthdate, 
       '"Baby Girl on the" Ddsp "of" fmMonth, YYYY, "at" HH:MI')
       "Formatted"
from birthday
where firstname = 'VICTORIA' !
fetch into :a, :b, :c !

 * PAGE 188 !
select firstname, birthdate, to_char(birthdate, 
       '"Baby Girl on the" Ddspth "of" fmMonth, YYYY, "at" HH:MI')
       "Formatted"
from birthday
where firstname = 'VICTORIA' !
fetch into :a, :b, :c !

select firstname, birthdate, to_char(birthdate, 
       '"Baby Girl on the" Ddspth "of" fmMonth, YYYY, "at" HH:MI P.M.')
       "Formatted"
from birthday
where firstname = 'VICTORIA' !
fetch into :a, :b, :c !

 * PAGE 189 !
select birthdate, new_time(birthdate, 'EST', 'HST')
from birthday
where firstname = 'VICTORIA' !
fetch into :a, :b !

select to_char(birthdate, 'fmMonth Ddth, YYYY, "at" HH:MI AM')
       "Birth", to_char (new_time(birthdate, 'EST', 'HST'),
       'fmMonth ddth, YYYY "at" HH:MI AM') "Birth"
from birthday
where firstname = 'VICTORIA' !
fetch into :a, :b !
 
 * PAGE 190 ! 
select to_date('22-FEB-92', 'DD-MON-YY') from dual !
fetch into :a !

select to_date('22-FEB-92') from dual !
fetch into :a !

 * PAGE 191 !
 * ORACLE error: ORA-1843 ! 
select to_date('02/22/92') from dual !
fetch into :a !

select to_date('02/22/92', 'MM/DD/YY') from dual !
fetch into :a !

 * ORACLE error: ORA-1481 ! 
select to_date('22-FEB-92', 'Day') from dual !
fetch into :a !

select to_char(to_date('22-FEB-92'), 'Day') from dual !
fetch into :a !

* CK: I have added here two zeros (behind the 11
*     and behind the 05) to be O7 compatible.
select to_date(1100501946, 'MM-DD-YYYY') from dual !
fetch into :a !

 * PAGE 192 !
 * ORACLE-error: ORA-1858 !
select to_date('Baby Girl on the Twentieth of May, 1949, at 3:27 A.M.',
       '"Baby Girl on the" Ddspth "of" fmMonth, YYYY, "at" HH:MI P.M.')
       "Formatted"
from birthday
where firstname = 'VICTORIA' !

select to_date('August, 20, 1949, 3:27 A.M.', 
               'fmMonth, Dd, YYYY, HH:MI P.M. ') "Formatted" 
from birthday
where firstname = 'VICTORIA' !
fetch into :a !

 * PAGE 193 !
select holiday, act_date, celebrate
from holiday
where celebrate - act_date != 0 !
fetch into :a, :b, :c !

select holiday, celebrate
from holiday
where celebrate between '01-JAN-92' and '22-FEB-92' !
fetch into :a, :b, :c !

select holiday, celebrate
from holiday
where celebrate in ('01-JAN-92', '22-FEB-92') !
fetch into :a, :b !

 * PAGE 194 !
select holiday, celebrate
from holiday
where celebrate = least('15-JAN-92', '07-SEP-92') !
fetch into :a, :b !

commit work !
