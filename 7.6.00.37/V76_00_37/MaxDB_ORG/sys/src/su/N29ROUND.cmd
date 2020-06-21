*   *ID* N29ROUND DBCMD    changed on 1992-09-28-13.25.59 by ADMIN     *
file connect ( kern test !
create table t ( a fixed (8,5),
                 b float (8))!
insert t values (1.00004, 123.00004)!
insert t values (1.00005, 123.00005)!
insert t values (1.5    , 123.5    )!
insert t values (1.0    , 123.0    )!
insert t values (1.00495, 123.00495)!
insert t values (1.00500, 123.00500)!
select round (a, 9) from t !
select round (a, 6), round (b, 6) from t !
fetch!
select round (a, 5), round (b, 5) from t !
fetch!
select round (a, 4), round (b, 4) from t !
fetch!
select round (a, 3), round (b, 3) from t !
fetch!
select round (a, 2), round (b, 2) from t !
fetch!
select round (a, 1), round (b, 1) from t !
fetch!
select round (a), round (b) from t !
fetch!
select round (0.00000678, -3) into :a from dual !
drop table t!
commit release !
