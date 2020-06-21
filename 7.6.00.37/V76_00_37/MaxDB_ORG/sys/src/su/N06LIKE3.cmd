*   *ID* N06LIKE3 DBCMD    changed on 1992-09-28-13.25.09 by ADMIN     *
 *** 'LIKE' wird getestet.
 %%% Wahre 'LIKE' mit '?', '_', '*', '%'. !
create table kunde  ( name char(10) key ) !
insert into kunde set name = 'GIAG' !
commit work !
select name from kunde where name like '_%'      !
fetch !
select name from kunde where name like '%_'      !
fetch !
select name from kunde where name like '%_%'     !
fetch !
select name from kunde where name like '_%_'     !
fetch !
select name from kunde where name like '_%%'     !
fetch !
select name from kunde where name like '__%'     !
fetch !
select name from kunde where name like '%%_'     !
fetch !
select name from kunde where name like '%__'     !
fetch !
select name from kunde where name like 'G_%'     !
fetch !
select name from kunde where name like 'G%_'     !
fetch !
select name from kunde where name like '_%G'     !
fetch !
select name from kunde where name like '%_G'     !
fetch !
select name from kunde where name like '%I%_G'   !
fetch !
select name from kunde where name like '_I_%G'   !
fetch !
select name from kunde where name like 'GI%_%G'  !
fetch !
select name from kunde where name like 'G%__%G'  !
fetch !
select name from kunde where name like 'G_%_%G'  !
fetch !
