*   *ID* N06LIKA9 DBCMD    changed on 1992-09-28-13.25.06 by ADMIN     *
 *** 'LIKE' wird getestet.
 *** Grosse Spalte. !
create table firma  ( name char (250) ascii key ) !
insert into firma set name =
'abcdefghi00000000000000000000000000000000000000000'
'00000000000000000000000000000000000000000000000000'
'00000000000000000000000000000000000000000000000000'
'00000000000000000000000000000000000000000000000000'
'00000000000000000000000000000000000000001234567890' !
commit work !
select name from firma where name like '%90'      !
fetch !
select name from firma where name like '%8%90%'    !
fetch !
select name from firma where name like 'abc%'     !
fetch !
select name from firma where name like '%i_0%8__' !
fetch !
