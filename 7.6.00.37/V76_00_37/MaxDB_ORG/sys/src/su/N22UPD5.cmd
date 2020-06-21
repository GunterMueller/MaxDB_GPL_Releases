*   *ID* N22UPD5  DBCMD    changed on 1992-09-28-13.25.33 by ADMIN     *
file connect ( kern test !
create view v as select spalte3, spalte5 from tab !
update v set spalte5 = 'FREIBURG' !
select * from v !
fetch !
rollback work !
update tab set spalte1 = 'HANS' !
update tab set spalte1 = 'HANS' where spalte1 = 'HANS' !
rollback work release !
