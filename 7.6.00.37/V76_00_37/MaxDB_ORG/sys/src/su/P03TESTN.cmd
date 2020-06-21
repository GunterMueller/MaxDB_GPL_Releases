*   *ID* P03TESTN DBCMD    changed on 1992-09-28-13.27.43 by ADMIN     *
 ***************************
 ***   P 0 3 T E S T N   ***
 ***************************!
 *** tests, die bis november 83 nicht gehen konnten, da
    die dazu benoetigten teile noch nicht implementiert waren !
alter table kern.abt add  neuer char (4)
      CHECK neuer between 'A' and 'F' OR neuer is null !
commit work !
insert into kern.abt set name = 'MAL', num2 = 1000, neuer= 'G' !
rollback work !
alter table kern.abt drop neuer RANGE !
commit work !
insert into kern.abt set name = 'MAL', num2 = 1000, neuer= 'G' !
rollback work !
alter table kern.abt drop neuer  !
commit work !
insert into kern.abt set name = 'MAL', num2 = 1000, neuer= 'G' !
rollback work !
