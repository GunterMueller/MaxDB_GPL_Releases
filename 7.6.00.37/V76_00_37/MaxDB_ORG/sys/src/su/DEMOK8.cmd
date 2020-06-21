*   *ID* DEMOK8   DBCMD    changed on 1992-09-28-13.21.37 by ADMIN     *
file connect ( travel00 travel00 sqlmode db2 !
* April 1992, Folie 8.030 !
select '19910314' + 1 year into :a from dual !
select '19920518' + 3 month - 14 days into :a from dual !
select '19921011' + 5 days - '19800601' into :a from dual !
select '00090000' + 45 minutes into :a from dual !
commit release !
file connect ( travel00 travel00 sqlmode oracle !
select sysdate + 12 into :a from dual !
select sysdate -  3 into :a from dual !
commit release !
file connect ( travel00 travel00 !
* April 1992, Folie 8.050 !
select firstname, name
  from customer
 where name like 'A%' OR
       name like 'B%' OR
       name like 'C%' OR
       name like 'D%' !
fetch !
select firstname, name
  from customer
 where name like 'Pa%' OR
       name like 'Pe%' OR
       name like 'Pi%' !
fetch !
commit release !
file connect ( travel00 travel00 !
* April 1992, Folie 8.060 !
create table testtable ( text char (30))!
insert testtable set text = 'This is a sentence' !
insert testtable set text = 'The VAT is 15%.'!
select text
  from testtable
 where text like '%@%%' escape '@' !
fetch !
rollback release !
file connect ( travel00 travel00 !
* April 1992, Folie 8.070 !
select *
  from booking
 where year(departure) in (2000, year(arrival)+1) !
fetch !
rollback release !
