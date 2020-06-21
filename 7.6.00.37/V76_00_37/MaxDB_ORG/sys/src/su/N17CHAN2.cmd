*   *ID* N17CHAN2 DBCMD    changed on 1992-09-28-13.25.25 by ADMIN     *
 *** 'CHANGE USER' wird getestet.
 *** Initialisierung von Privilegien auf Basistabellen.
 *** Vor diesem File muss 'N17CHAN1 DBCMD' aufgerufen werden.
 ***********************************************************!
file connect (p3 pw !
grant all on t1, all on t2 to d1 !
grant all on t1, insert, delete on t2 to d2, d2_r1, d2_p1 !
commit work release !
file connect ( sut sut !
grant select on d2_pt1 to d1 !
grant all on d1_pt1 to d2 !
grant selupd ( d1_pt1_c2 ) on d1_pt1 to r1,p1 !
commit work release !
 ***********************************************************!
file connect (d1 d1_w1 !
grant select (d1_pt2_c1), update on d1_pt2
      to d2,d1_r1,d1_p1,d2_r1,d2_p1,r1,p1 !
grant all on d1_t1 to d2,d1_r1,d1_p1,d2_r1,d2_p1,r1,p1 !
commit work release !
 ***********************************************************!
file connect (d2 d2_w1 !
grant  insert  , select on  d2_t1 to d2_p1  ,  r2 !
commit work release !
 ***********************************************************!
file connect (r2 r2_w1 !
grant insert,delete  on r2_t1  to d2, d1, r1,p1,d2_p1 ,d1_r2 !
commit work release !
 ***********************************************************!
file connect (d1_r1 d1_r1_w1 !
grant all privileges on d1r1t1 to  d1_r2 !
commit work release !
 ***********************************************************!
file connect (d1_r2 d1_r2_w1 !
grant insert on d1r2t1,
    selupd (d1r2t2_c2, d1r2t2_c1) on d1r2t2
    to d1,d1_r1 ,d1_p1, r2,p2,p1,d2_r2,d2_p2 !
commit work release !
 ***********************************************************!
