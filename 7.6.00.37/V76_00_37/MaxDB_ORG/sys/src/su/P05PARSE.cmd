*   *ID* P05PARSE DBCMD    changed on 1992-09-28-13.27.48 by ADMIN     *
 ***************************
 ***   P 0 5 P A R S E   ***
 ***************************!
 *** Pruefung der einfachsten Form der Privilegienvergabe !
grant !
grant all !
grant all all !
grant all on !
grant all on x !
grant all on to !
grant all on lnum to !
grant all on kern.lnum to x !
grant all priv on lnum to x !
grant all privileges on kern.lnum to x !
commit work !
 *** falsche Tablebezeichnung !
grant all on ?lnum to x !
grant all on lnum, lnum to x !
grant all on lnum lnum to x !
grant all on lnum , on lnum to x !
commit work !
 *** falsche Userclassbezeichnung !
grant all on lnum to ?eins !
grant all on lnum to ein.s !
grant all on lnum to *eins !
grant all on lnum to eins* !
commit work !
 *** falsche Privilegienbezeichnungen !
grant insert, del on lnum to x !
grant repl on lnum to x !
grant selectupd on lnum to x !
commit work !
 *** doppelte Privilegierung !
grant insert, insert, insert on lnum to x!
grant insert, select, insert on lnum to x !
grant all on kern.lnum, insert on kern.lnum,
   delete, update on kern.lnum to x !
grant update, update (l) on lnum to x !
grant update on kern.lnum, update (l) on kern.lnum to x !
grant update (l) on kern.lnum , update (l) on kern.lnum to x !
commit work !
 *** falsche Feldbezeichnungen !
grant update (, insert on lnum to x !
grant update () on lnum to x !
grant update (x) on kern.lnum to x !
grant update (l eins) on lnum to x !
grant update (l, l) on kern.lnum to x !
commit work !
 *** doppelte auth-id !
grant select on lnum to eins, eins !
commit work !
 *** fehlerhafte auth-id !
grant insert on kern.lnum to sut !
grant insert on kern.lnum to AUTHID !
grant insert on kern.lnum to dba !
commit work !
 *** nicht fuer den angegebenen User erlaubte Privilegien !
create user qqq password w !
grant owner on kern.lnum to qqq !
drop user qqq !
commit work !
