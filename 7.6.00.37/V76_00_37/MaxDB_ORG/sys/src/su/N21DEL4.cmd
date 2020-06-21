*   *ID* N21DEL4  DBCMD    changed on 1992-09-28-13.21.45 by ADMIN     *
 *** 'DELETE' wird getestet .
 *** Vor diesem File ist 'N21DEL1 VDNMAC' aufzurufen. !
file connect ( kern test !
DELETE FROM tab WHERE    3    BETWEEN    2    AND 4   !
rollback work !
DELETE FROM tab WHERE spalte3 BETWEEN spalte2 AND 999 !
rollback work !
DELETE FROM tab WHERE   'KARIN'   = spalte1 !
rollback work !
DELETE FROM tab WHERE tab.spalte1 = 'KARIN' !
ROLLBACK WORK RELEASE !
