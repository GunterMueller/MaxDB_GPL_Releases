*   *ID* N71JOIN9 CMD
file connect ( kern test !
create table t1 ( a  char (15) key,
                  a1 fixed (10) key default 12345 )!
insert t1 set a = 'HG krankenhaus' !
create table t2 ( b char (17) key)!
insert t2 set b = 'HG krankenhaus' !
select * from <t1, t2> where a = b and a = 'HG krankenhaus' !
fetch !
select * from <t2, t1> where a = b and a = 'HG krankenhaus' !
fetch !
select * from <t1, t2> where a = b and b = 'HG krankenhaus' !
fetch !
select * from <t2, t1> where a = b and b = 'HG krankenhaus' !
fetch !
rollback work release !
