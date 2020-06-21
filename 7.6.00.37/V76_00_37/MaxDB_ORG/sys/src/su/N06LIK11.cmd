*   *ID* N06LIK11 DBCMD    changed on 1992-09-28-13.25.12 by ADMIN     *
file connect ( kern test !
create table t ( nachname char (20),
                 vorname char (20))!
create index "t.nachname"
	on t( nachname ) !
create index "t.vorname"
	on t( vorname ) !
insert t values ('Mueller', 'Egon')!
select * from t where nachname like 'Muell%' or
                      vorname  like 'E%' !
fetch !
pars_then_ex !
select * from t where nachname like 'Muell%' or
                      vorname  like 'E%' !
fetch into :a, :b !
data !
select * from t where nachname like :a or
                      vorname  like :b !
c 20 'Muell%' c 20 'E%'

nodata !
fetch into :a, :b !
pars_execute !
drop table t !
commit release !

file CONNECT ( kern test!

CREATE TABLE tt (val CHAR(10))!
INSERT INTO tt VALUES ('*')!
INSERT INTO tt VALUES ('%')!
SELECT * from tt WHERE val LIKE val!
FETCH INTO :A!
ROLLBACK WORK RELEASE!
