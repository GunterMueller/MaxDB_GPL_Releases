*   *ID* OUTERKEY DBCMD    changed on 1992-09-28-13.27.35 by ADMIN     *
file connect ( kern test !
create table ta (a1 fixed (4) key, a2 int) !
insert ta values (1, 1) !
insert ta values (2, 10) !
insert ta values (3, 10) !
insert ta values (4, 1) !
insert ta values (6, 1) !
insert ta values (8, 1) !
create table tb (b1 fixed (6) key, b2 int) !
commit work !
!********** Table TB is empty   !
file outrslct !
file outrview !
!********** Table TB is not empty !
insert tb values (2, 20) !
insert tb values (3, 20) !
insert tb values (5, 2) !
insert tb values (7, 2) !
commit work !
file outrslct !
file outrview !
commit work release !
