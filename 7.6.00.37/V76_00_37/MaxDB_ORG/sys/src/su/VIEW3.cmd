*   *ID* VIEW3    DBCMD    changed on 1992-09-28-13.28.57 by ADMIN     *
file cleark !
file connect ( kern test !
create table probew (oid char(36) key,
i_rel fixed(10) key, i_concat_id char (36), land_nr fixed (10),
lfd_nr fixed (10))!
insert probew values ('aaa', 1, 'a', 116, 3)!
create table probewbewill
(oid char(36) key, i_rel fixed (10) key,
i_tab_key char(8) byte, i_concat_id char(9))!
insert probewbewill values ('aaa', 1, x'99', 'a')!
create table project(oid char(27) key,
i_rel fixed(10) key, i_concat_id char (27), land_nr fixed (10),
lfd_nr fixed (10))!
insert project values ('aaa', 1, 'a', 116, 3)!
create view grundbew as select
 probew.oid, project.i_rel, project.land_nr, project.lfd_nr
 from probew, probewbewill, project
where probew.oid = probewbewill.oid
  and probew.land_nr = project.land_nr
  and probew.lfd_nr = project.lfd_nr !
create index "project.land_nr"
	on project( land_nr ) !
drop   index "probew.land_nr" !
explain select * from grundbew where land_nr = 116 !
select * from grundbew where land_nr = 116 !
fetch !
rollback work release !
