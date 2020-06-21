*   *ID* CLEARK   DBCMD    changed on 1992-09-28-13.22.56 by ADMIN     *
*
*  anstelle des haeufig benutzten 'file shutdwnk' / 'file xinitdbk'
*  wird jetzt kein 'shutdown' / 'restart' mehr gemacht, sondern nur noch
*  ein 'drop user kern' / 'create user kern'
*
connect sut identified by sut !
drop user kern !
*-------------------- fuer tests folgende zeilen freigeben
* commit work release !
* utility !
* connect sut identified by sut !
* file sel_user !
* file sel_table !
* exit !
* connect sut identified by sut!
*-----------------------------------
create user kern password test dba not exclusive !
commit work release !
