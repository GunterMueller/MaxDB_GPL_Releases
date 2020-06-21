*   *ID* N30SOUND DBCMD    changed on 1992-09-28-13.26.04 by ADMIN     *
file CONNECT ( KERN TEST !
create table t ( a char (6) ascii,
                 b char (25),
                 c fixed (8),
                 d char (8) byte,
                 s char (4))!
insert t values ('Gabe  ', 'Chef       ', 3, x'40c0c1c5d9',null)!
insert t values ('Gabe  ', 'Sack       ', 3, x'40c0c1c5d9',null)!
insert t values ('Ziege ', 'Sack       ', 3, x'40c0c1c5d9',null)!
insert t values (null    , null         , 3, x'40c0c1c5d9',null)!
insert t values ('Bart  ', 'Bartel     ', 3, x'40c0c1c5d9',null)!
insert t values ('Bartel', 'Bartels    ', 3, x'40c0c1c5d9',null)!
insert t values ('Bartel', 'Bertelsman ', 3, x'40c0c1c5d9',null)!
select * from t !
fetch !
select soundex (c) from t !
select soundex (d) from t !
select a, soundex(a), b, soundex(b) from t !
fetch !
select a, soundex (a), b from t where soundex(a) = soundex(b) !
fetch !
update t set s = soundex (b) where substr(soundex(a), 1, 1) = 'P' !
select b, soundex(b), s from t !
fetch !
drop table t !
commit release !
