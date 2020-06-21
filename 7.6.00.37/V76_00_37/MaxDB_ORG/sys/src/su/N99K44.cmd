*   *ID* N99K44   DBCMD    changed on 1992-09-28-13.27.29 by ADMIN     *
file connect ( kern test !
CREATE TABLE KUNDE
   ( KNR      FIXED(4) KEY ,
     PLZ      FIXED(4)     ,
     ORT      CHAR(12)     ) !
INSERT KUNDE VALUES
   ( 3270 , 5000 , 'Koeln' ) !
CREATE TABLE TAB6 (SPALTE1 FIXED(5), SPALTE2 FIXED(5)) !
INSERT TAB6 VALUES ( 1, 2 ) !
INSERT TAB6 VALUES ( 2, 1 ) !
ROLLBACK WORK RELEASE !
