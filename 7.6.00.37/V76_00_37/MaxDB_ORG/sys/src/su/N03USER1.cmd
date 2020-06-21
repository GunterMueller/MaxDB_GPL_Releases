*   *ID* N03USER1 DBCMD    changed on 1992-09-28-13.25.03 by ADMIN     *
 *** CREATE USER und DROP USER wird getestet.
 *** Semantiktest, kein Syntaxtest. !
CREATE USER d1 password w   DBA    !
CREATE USER r1 password w RESOURCE !
CREATE USER p1 password w         !
CREATE USER d1 password w DBA !
CREATE USER r1 password w DBA !
CREATE USER p1 password w DBA !
CREATE USER d1 password w RESOURCE !
CREATE USER r1 password w RESOURCE !
CREATE USER p1 password w RESOURCE !
CREATE USER d1 password d1_w1 !
CREATE USER r1 password r1_w1 !
CREATE USER p1 password p1_w1 !
COMMIT WORK !

file sel_user !
ROLLBACK WORK RELEASE !
file connect ( d1 w !
CREATE USER d1_d1 password d1_d1xw DBA      !
CREATE USER r1    password r1xw RESOURCE !
CREATE USER d1_r1 PASSWORD d1_r1_w1 RESOURCE !
CREATE USER d1_r2 PASSWORD d1_r2_w1 RESOURCE !
CREATE USER d1_p1 PASSWORD d1_p1_w1          !
CREATE USER d1_p2 PASSWORD d1_p2_w1 !
COMMIT WORK !

file sel_user !
ROLLBACK WORK RELEASE !
file connect ( r1 w !
CREATE USER r1_d1 password w DBA      !
CREATE USER r1_r1 password w RESOURCE !
CREATE USER r1_p1 password w !
ROLLBACK WORK RELEASE !
file connect ( p1 w !
CREATE USER p1_d1 password w DBA      !
CREATE USER p1_r2 password w RESOURCE !
CREATE USER p1_p1 password w         !
ROLLBACK WORK RELEASE !
file connect ( d1_r1 d1_r1_w1 !
CREATE USER d1_r1_p1 password w !
CREATE TABLE tabelle ( spalte CHAR(3) ) !
GRANT INSERT, SELECT ON tabelle TO d1_p1 !
COMMIT WORK RELEASE !
file connect ( d1_p1 d1_p1_w1 !
CREATE USER d1_p1_p1 password w !
INSERT       tabelle SET spalte = 'aaa' !
INSERT d1_r1.tabelle SET spalte = 'aaa' !
COMMIT WORK !
SELECT spalte FROM d1_r1.tabelle !
FETCH !
ROLLBACK WORK RELEASE !
file connect ( d1_p1 d1_p1_w1 !
DROP USER d1    !
DROP USER r1    !
DROP USER p1    !
DROP USER d1_r1 !
DROP USER d1_p1 !
COMMIT WORK RELEASE !
file connect ( d1_r1 d1_r1_w1 !
DROP USER d1    !
DROP USER r1    !
DROP USER p1    !
DROP USER d1_r1 !
DROP USER d1_p1 !
COMMIT WORK RELEASE !
file connect ( r1 w !
DROP USER d1    !
DROP USER r1    !
DROP USER p1    !
DROP USER d1_r1 !
DROP USER d1_p1 !
COMMIT WORK RELEASE !
file connect ( d1 w !
DROP USER d1    !
DROP USER r1    !
DROP USER p1    !
DROP USER d1_r1 !
DROP USER d1_p1 !
COMMIT WORK !

file sel_user !
ROLLBACK WORK RELEASE !
file connect ( sut sut !
DROP USER d1_r1 !
DROP USER d1_p1 !
DROP USER r1    !
DROP USER p1    !
DROP USER d1_r2 !
DROP USER d1    !

file sel_user !
DROP USER d1_p2 !
COMMIT WORK !

file sel_user !
ROLLBACK WORK RELEASE !
* PTS 1132130 !
file connect ( sut sut!
create user "GANZ_LANGER_USER.WDF.SAP.CORP" password lang !
file sel_user !
commit release !
connect "GANZ_LANGER_USER.WDF.SAP.CORP" lang !
select * from dual 
where 'GANZ_LANGER_USER.WDF.SAP.CORP' = user !
commit release !
file connect ( sut sut!
drop user "GANZ_LANGER_USER.WDF.SAP.CORP" !
commit release !
