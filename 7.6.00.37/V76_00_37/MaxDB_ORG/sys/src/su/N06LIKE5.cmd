*   *ID* N06LIKE5 DBCMD    changed on 1992-09-28-13.25.10 by ADMIN     *
 *** 'LIKE' wird getestet.
 %%% Wahre 'NOT LIKE'. !
create table kunde  ( name char(10) key ) !
insert into kunde set name = 'GIAG' !
commit work !
select name from kunde where name  not like  ' '      !
fetch !
select name from kunde where not ( name like '_'    ) !
fetch !
select name from kunde where name  not like  '_'      !
fetch !
select name from kunde where not ( name like 'B'    ) !
fetch !
select name from kunde where name  not like  'a'      !
fetch !
select name from kunde where not ( name like 'IAG'  ) !
fetch !
select name from kunde where name  not like  '%GI'    !
fetch !
select name from kunde where not ( name like 'G%A'  ) !
fetch !
select name from kunde where name  not like  'AG%'    !
fetch !
select name from kunde where not ( name like '_____') !
fetch !
select name from kunde where name  not like  'G_IAG'  !
fetch !
select name from kunde where not ( name like 'G_G'  ) !
fetch !
select name from kunde where name  not like  '_GI%'   !
fetch !
select name from kunde where not ( name like 'G%G_' ) !
fetch !
