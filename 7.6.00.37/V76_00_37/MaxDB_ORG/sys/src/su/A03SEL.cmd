*   *ID* A03SEL   DBCMD    changed on 1992-09-28-13.22.21 by ADMIN     *
 ***************************
 ***   A 0 3 S E L       ***
 ***************************!
connect kern IDENTIFIED BY :A !
TEST
declare erg1 cursor for 
select k1, f1, f2, n1, n2 from kern.p1 !
fetch erg1!
declare erg2 cursor for 
select f1, f2, n1, n2 from kern.t1 !
fetch erg2!
commit work release !
