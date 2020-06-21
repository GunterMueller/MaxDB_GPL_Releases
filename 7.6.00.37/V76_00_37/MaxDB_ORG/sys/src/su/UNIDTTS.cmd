*   *ID* UNIDTTS  DBCMD    changed on 1992-09-28-13.28.41 by ADMIN     *
* union mit date, time und timestamp in den unterschiedlichen Formaten
* Parameter :   &1 ::= Ausgabeformat (internal, europ, iso, usa, jis)
set format &1 !
select * from t2 where b1 = 'bbbbb' !
FETCH !
select * from t1 where a1 = 'aaaaa'
union
select * from t2 where b1 = 'bbbbb' !
FETCH !
create view v as
select * from t1 where a1 = 'aaaaa'
union
select * from t2 where b1 = 'bbbbb' !
select * from v !
FETCH !
commit work !
drop view v !
select * from t3 where a1 = 'aaaaa'
union
select * from t2 where b1 = 'bbbbb' !
FETCH !
create view v as
select * from t3 where a1 = 'aaaaa'
union
select * from t2 where b1 = 'bbbbb' !
select * from v !
FETCH !
commit work !
drop view v !
select * from t2 where b1 = 'bbbbb'
union
select * from t1 where a1 = 'aaaaa' !
FETCH !
create view v as
select * from t2 where b1 = 'bbbbb'
union
select * from t1 where a1 = 'aaaaa' !
select * from v !
FETCH !
commit work !
drop view v !
select * from t2 where b1 = 'bbbbb'
union
select * from t3 where a1 = 'aaaaa' !
FETCH !
create view v as
select * from t2 where b1 = 'bbbbb'
union
select * from t3 where a1 = 'aaaaa' !
select * from v !
FETCH !
commit work !
drop view v !
