*   *ID* N06LIKE1 DBCMD    changed on 1992-09-28-13.25.08 by ADMIN     *
 *** 'LIKE' wird getestet.
 %%% Wahre 'LIKE' mit '*' und '%' , ohne '_' und '?'. !
create table kunde  ( name char(10) key ) !
insert into kunde set name = 'GIAG' !
commit work !
select name from kunde where name like '%'        !
fetch !
select name from kunde where name like 'G%'       !
fetch !
select name from kunde where name like '%G%'      !
fetch !
select name from kunde where name like 'GI%'      !
fetch !
select name from kunde where name like '%GI%'     !
fetch !
select name from kunde where name like '%I%'      !
fetch !
select name from kunde where name like '%IA%'     !
fetch !
select name from kunde where name like '%AG%'     !
fetch !
select name from kunde where name like '%AG'      !
fetch !
select name from kunde where name like '%G'       !
fetch !
select name from kunde where name like 'GIAG'     !
fetch !
select name from kunde where name like 'GIAG '    !
fetch !
select name from kunde where name like 'GIAG%'    !
fetch !
select name from kunde where name like '%GIAG'    !
fetch !
select name from kunde where name like '%GIAG%'   !
fetch !
select name from kunde where name like 'G%AG'     !
fetch !
select name from kunde where name like 'G%G'      !
fetch !
select name from kunde where name like 'GI%AG'    !
fetch !
select name from kunde where name like 'GI%G'     !
fetch !
select name from kunde where name like 'G%I%'     !
fetch !
select name from kunde where name like 'G%A%'     !
fetch !
select name from kunde where name like 'G%AG%'    !
fetch !
select name from kunde where name like '%G%AG'    !
fetch !
select name from kunde where name like '%G%AG'    !
fetch !
select name from kunde where name like 'GI%AG%'   !
fetch !
select name from kunde where name like 'GI%G%'    !
fetch !
select name from kunde where name like '%GI%AG'   !
fetch !
select name from kunde where name like '%GI%G'    !
fetch !
select name from kunde where name like '%G%I%'    !
fetch !
select name from kunde where name like '%G%IAG%'  !
fetch !
select name from kunde where name like '%G%G%'    !
fetch !
select name from kunde where name like '%GI%AG%'  !
fetch !
select name from kunde where name like '%GI%G%'   !
fetch !
select name from kunde where name like '%GIA%G%'  !
fetch !
select name from kunde where name like '%I%G'     !
fetch !
select name from kunde where name like '%IA%G'    !
fetch !
select name from kunde where name like 'G%I%AG'   !
fetch !
select name from kunde where name like 'G%I%G'    !
fetch !
select name from kunde where name like 'G%A%G'    !
fetch !
select name from kunde where name like 'GI%A%G'   !
fetch !
select name from kunde where name like '%I%A%G'   !
fetch !
select name from kunde where name like '%%'       !
fetch !
select name from kunde where name like 'G%%'      !
fetch !
select name from kunde where name like '%%G%'     !
fetch !
select name from kunde where name like '%G%%'     !
fetch !
select name from kunde where name like '%%G%%'    !
fetch !
select name from kunde where name like '%G%%IA%%' !
fetch !
select name from kunde where name like '%GI%G'    !
fetch !
select name from kunde where name like 'G%A%'     !
fetch !
select name from kunde where name like 'G%%'      !
fetch !
select name from kunde where substr(name,5) like '%' !
fetch !
select name from kunde where substr(name,5) like '%%' !
fetch !
