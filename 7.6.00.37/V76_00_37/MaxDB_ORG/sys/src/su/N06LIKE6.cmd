*   *ID* N06LIKE6 DBCMD    changed on 1992-09-28-13.25.10 by ADMIN     *
 *** 'LIKE' wird getestet.
 %%% Falsche 'NOT LIKE'. !
create table kunde  ( name char(10) key ) !
insert into kunde set name = 'GIAG' !
commit work !
select name from kunde where name not like  '%'      !
fetch !
select name from kunde where not (name like '%GI%' ) !
fetch !
select name from kunde where name not like  'GI%'    !
fetch !
select name from kunde where not (name like '%AG%')  !
fetch !
select name from kunde where name not like  '%AG'    !
fetch !
select name from kunde where not (name like '____' ) !
fetch !
select name from kunde where name not like  'G%%AG'  !
fetch !
select name from kunde where not (name like 'GI%G%') !
fetch !
