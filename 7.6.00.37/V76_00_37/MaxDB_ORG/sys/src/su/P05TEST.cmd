*   *ID* P05TEST  DBCMD    changed on 1992-09-28-13.27.49 by ADMIN     *
 ***************************
 ***   P 0 5 T E S T     ***
 ***************************!
 *** richtige grants !
grant all on kern.lnum to eins !
commit work !
grant insert on kern.lnum to eins !
grant update ( l, eins) on kern.lnum to eins !
revoke all on kern.lnum from eins !
grant delete on kern.lnum to eins !
grant all privileges on kern.lnum to eins !
commit work !
 *** grants auf subsets !
create view sh as select * from kern.abt
    where ort = 'BONN' !
create view si as select * from kern.abt
    where ort = 'BONN' and
          arbeiter > 45 !
grant insert on sh to eins !
grant insert on si to eins !
grant select (name, num2) on si to eins !
commit work !
file sel_priv_granted ( EINS !
commit work release !
 *** Ueberpruefung, ob der betreffende alles sieht !
file connect ( eins eins !
file sel_priv_on !
commit work release !
 * aufraeumen !
file connect ( sut sut !
drop user eins !
commit work release !
file connect ( kern test !
