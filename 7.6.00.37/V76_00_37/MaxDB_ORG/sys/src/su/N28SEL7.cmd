*ID* N28SEL7  DBCMD    changed on 1992-09-28-13.25.56 by ADMIN     *
* Fehlerprotokolle 115, 151, 152
* konnte auch frueher nur zu Problemen fuehren, wenn
* vak24/37 mit -d updstat uebersetzt waren !
file connect  ( kern test !
create table staff (
empnum   char (3) not null,
empname  char (20),
grade    char  (4),
city     char (15),
manager  char (8))!
insert staff values ('E1', 'Alice',  '12', 'Deale', 'Miller')!
insert staff values ('E2', 'Betty',  '10', 'Vienna', 'Miller')!
insert staff values ('E3', 'Carmen', '13', 'Vienna', 'Miller')!
insert staff values ('E4', 'Don'  ,  '12', 'Deale', 'Miller')!
insert staff values ('E5', 'Ed'   ,  '13', 'Akron', 'Miller')!
create unique index "staff.empnum"
	on staff( empnum ) !
* 115 !
select empname, city, grade from staff
where empname>= 'Betty' and empnum < 'E35'
or city <= 'Deale     ' and grade > '12'
or grade = '13' and city <> 'Akron'!
fetch !
commit !
create index "staff.empname"
	on staff( empname ) !
create index "staff.grade"
	on staff( grade ) !
create index "staff.city"
	on staff( city ) !
* 151, 152 !
select * from staff
where
  ((
        empnum = 'E1'
    and empname like 'A%'
    and (grade like '1%' or city like 'A%')
   )
or
   (grade = '12' and city like 'D%' and manager like 'Mil%')
  )
and rowno <= 37 !
fetch!
* 152 !
select empname, city, grade from staff
where empname>= 'Betty' and empnum < 'E35'
or city <= 'Deale     ' and grade > '12'
or grade = '13' and city <> 'Akron'!
fetch !
rollback !
DROP INDEX "staff.empnum" !
CREATE INDEX "STAFF.EMPNAME"
	on STAFF( EMPNAME ) !
CREATE INDEX "STAFF.MANAGER"
	on STAFF( MANAGER ) !
PARS_THEN_EX !
DATA !
SELECT * FROM STAFF
WHERE
  ((
        EMPNUM = 'E1'
    AND (GRADE LIKE '1%' OR CITY LIKE :C)
    AND EMPNAME LIKE 'A%'
   )
OR
   (GRADE = '12' AND CITY LIKE 'D%' AND MANAGER LIKE 'Mil%')
  )
AND ROWNO <= 37 !
c 15 'A%'

SELECT * FROM STAFF
WHERE
  ((
        EMPNUM = 'E1'
    AND (GRADE LIKE '1%' OR CITY LIKE 'A%')
    AND EMPNAME LIKE :A
   )
OR
   (GRADE = '12' AND CITY LIKE 'D%' AND MANAGER LIKE :M)
  )
AND ROWNO <= 37 !
c 20 'A%' c 8 'Mil%'

nodata !
pars_execute !
commit release !
