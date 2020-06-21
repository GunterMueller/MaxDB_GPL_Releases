*   *ID* N16CHAN3 DBCMD    changed on 1992-09-28-13.25.23 by ADMIN     *
 *** Getestet wird 'ALTER USER'.
 *** Fehlerhafte 'ALTER USER'. Ein User versucht 'ALTER USER'
 *** abzusetzen fuer einen Benutzer, fuer den er nicht
 *** verantwortlich ist.
 *** Initialisierung durch 'N16CHAN1 VDNMAC'. !
file connect ( d1 d1_w1 !
alter user d1    dba       !
alter user d1    resource  !
alter user d1    standard  !
alter user d2    dba       !
alter user d2    resource  !
alter user d2    standard  !
alter user r1    dba       !
alter user r1    resource  !
alter user r1    standard  !
alter user p1    dba       !
alter user p1    resource  !
alter user p1    standard  !
alter user d2_r1 dba       !
alter user d2_r1 resource  !
alter user d2_r1 standard  !
alter user d2_p1 dba       !
alter user d2_p1 resource  !
alter user d2_p1 standard  !
rollback work release !
file connect ( r1 r1_w1 !
alter user d1    dba       !
alter user d1    resource  !
alter user d1    standard  !
alter user r1    dba       !
alter user r1    resource  !
alter user r1    standard  !
alter user r2    dba       !
alter user r2    resource  !
alter user r2    standard  !
alter user p1    dba       !
alter user p1    resource  !
alter user p1    standard  !
alter user d1_r1 dba       !
alter user d1_r1 resource  !
alter user d1_r1 standard  !
alter user d1_p1 dba       !
alter user d1_p1 resource  !
alter user d1_p1 standard  !
rollback work release !
file connect ( p1 p1_w1 !
alter user d1    dba       !
alter user d1    resource  !
alter user d1    standard  !
alter user r1    dba       !
alter user r1    resource  !
alter user r1    standard  !
alter user p1    dba       !
alter user p1    resource  !
alter user p1    standard  !
alter user p2    dba       !
alter user p2    resource  !
alter user p2    standard  !
alter user d1_r1 dba       !
alter user d1_r1 resource  !
alter user d1_r1 standard  !
alter user d1_p1 dba       !
alter user d1_p1 resource  !
alter user d1_p1 standard  !
rollback work release !
file connect ( d1_r1 d1_r1_w1 !
alter user d1    dba       !
alter user d1    resource  !
alter user d1    standard  !
alter user d2    dba       !
alter user d2    resource  !
alter user d2    standard  !
alter user r1    dba       !
alter user r1    resource  !
alter user r1    standard  !
alter user p1    dba       !
alter user p1    resource  !
alter user p1    standard  !
alter user d1_r1 dba       !
alter user d1_r1 resource  !
alter user d1_r1 standard  !
alter user d1_r2 dba       !
alter user d1_r2 resource  !
alter user d1_r2 standard  !
alter user d1_p1 dba       !
alter user d1_p1 resource  !
alter user d1_p1 standard  !
alter user d2_r1 dba       !
alter user d2_r1 resource  !
alter user d2_r1 standard  !
alter user d2_p1 dba       !
alter user d2_p1 resource  !
alter user d2_p1 standard  !
rollback work release !
file connect ( d1_p1 d1_p1_w1 !
alter user d1    dba       !
alter user d1    resource  !
alter user d1    standard  !
alter user d2    dba       !
alter user d2    resource  !
alter user d2    standard  !
alter user r1    dba       !
alter user r1    resource  !
alter user r1    standard  !
alter user p1    dba       !
alter user p1    resource  !
alter user p1    standard  !
alter user d1_r1 dba       !
alter user d1_r1 resource  !
alter user d1_r1 standard  !
alter user d1_p1 dba       !
alter user d1_p1 resource  !
alter user d1_p1 standard  !
alter user d1_p2 dba       !
alter user d1_p2 resource  !
alter user d1_p2 standard  !
alter user d2_r1 dba       !
alter user d2_r1 resource  !
alter user d2_r1 standard  !
alter user d2_p1 dba       !
alter user d2_p1 resource  !
alter user d2_p1 standard  !
rollback work release !
