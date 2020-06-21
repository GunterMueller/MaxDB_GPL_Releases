*   *ID* C00E111  DBCMD    changed on 1992-09-28-13.23.05 by ADMIN     *
*
*  movec crash bei langem select auf targon mit 2.4.04
*
file cleark !
file connect ( kern test !
create table pe (
   pn       char(16) ascii key,
   gebname  char(100) ascii,
   gebdat   char(10) ascii,
   geschl   char(1) ascii,
   gebort   char(100) ascii,
   famname  char(100) ascii,
   vorname  char(100) ascii,
   titel    char(100) ascii,
   wohn     char(100) ascii,
   stras    char(100) ascii,
   land     char(100) ascii,
   staat    char(9) ascii,
   mutname  char(100) ascii,
   bem      char(200) ascii,
   bzr      char(1) ascii,
   vorl     char(1) ascii) !
commit !
create table vf (
   az       char(17) ascii key,
   vfart1   char(1) ascii,
   vfart2   char(1) ascii,
   frdaz    char(100) ascii,
   einlbeh  char(1) ascii,
   einldat  char(8) ascii,
   eingdat  char(8) ascii,
   delgr1   char(1) ascii,
   delgr2   char(1) ascii,
   delgr3   char(3) ascii,
   dezanw   char(1) ascii,
   dezkz1   char(3) ascii,
   dezkz2   char(1) ascii,
   tatvorwf char(100) ascii,
   tatort   char(100) ascii,
   tatzeit  char(17) ascii,
   asserv   char(1) ascii,
   assnum   char(8) ascii,
   anzb     char(4) ascii,
   bez      char(100) ascii,
   zknum    char(5) ascii,
   eintdat  char(8) ascii,
   einlbeh2 char(1) ascii,
   einldat2 char(8) ascii,
   wglanf   char(5) ascii,
   wgldauer char(2) ascii,
   pomap    char(1) ascii,
   bem      char(140) ascii,
   alttermin char(8) ascii,
   schaden  char(10) ascii,
   anze     char(1) ascii,
   vstatus  char(1) ascii,
   rstatus  char(1) ascii
   ) !
commit !
create table tf (
   az         char(17) ascii key,
   pn         char(16) ascii key,
   lfdnr      char(  2) ascii,
   tatvorw    char(100) ascii,
   erlart     char(  2) ascii,
   freitxt    char( 70) ascii,
   spruchk    char(  1) ascii,
   empf       char(  3) ascii,
   betrag     char(  9) ascii,
   auflerf    char(  1) ascii,
   vfgdat     char(  8) ascii,
   bescheid   char(  1) ascii,
   uebern     char(  1) ascii,
   beschw     char(  1) ascii,
   bescherl   char(  1) ascii,
   bem        char(140) ascii,
   entschantr char(  1) ascii,
   stordat    char(  8) ascii,
   amger      char( 22) ascii,
   amaz       char( 17) ascii,
   ament1     char(  1) ascii,
   ament2     char( 17) ascii,
   ament3     char(  2) ascii,
   amdat      char(  8) ascii,
   amrm       char(  1) ascii,
   lnger      char( 22) ascii,
   lnaz       char( 17) ascii,
   lnent1     char(  1) ascii,
   lnent2     char( 17) ascii,
   lnent3     char(  2) ascii,
   lndat      char(  8) ascii,
   lnrm       char(  1) ascii,
   revaz      char( 17) ascii,
   revent1    char(  1) ascii,
   revent2    char( 23) ascii,
   rkdat      char(  8) ascii,
   vrsaz      char( 17) ascii,
   pstatus    char(  1) ascii
) !
commit !
*
insert pe set pn = '1', famname = 'Eins' !
insert pe set pn = '2', famname = 'Zwei' !
insert vf set az = '5 Js 7/90', tatvorwf = 'ladendiebstahl' !
insert tf set az = '5 Js 7/90', pn = '1', lfdnr = '1' !
insert tf set az = '5 Js 7/90', pn = '2', lfdnr = '2' !
*
parsing!
select * from tf, vf, pe
 where tf.az = vf.az
   and tf.pn = pe.pn
   and tf.lfdnr = :a
   and vf.az = :b
   for reuse !
IF &rc = 0 THEN BEGIN
* Fuer Unicode sind die Ausgabespalten des letzten und des naechsten
* Staements zu lang. Mit Pars/Execute-Trennung laesst sich der
* Kontrollfluss fuer die naechsten Statements deshalb nicht zur Deckung
* bringen ausser mit dieser harten Massnahme... (CK)
executing!
data!
select!
1
c 2  '2'
c 17 '5 Js 7/90'

nodata!
END
pars_execute!
drop parseid !
1
parsing!
declare e1 cursor for 
select 
tf.az, pe.pn, lfdnr, tatvorw, erlart,
freitxt, spruchk, empf, betrag, auflerf,
vfgdat, bescheid, uebern, beschw, bescherl,
pe.bem, entschantr, stordat, amger, amaz,
ament1, ament2, ament3, amdat, amrm,
lnger, lnaz, lnent1, lnent2, lnent3,
lndat, lnrm, revaz, revent1, revent2,
rkdat, vrsaz, pstatus, vf.az, vfart1,
vfart2, frdaz, einlbeh, einldat, eingdat,
delgr1, delgr2, delgr3, dezanw, dezkz1,
dezkz2, tatvorwf, tatort, tatzeit, asserv,
assnum, anzb, bez, zknum, eintdat,
einlbeh2, einldat2, wglanf, wgldauer, pomap,
tf.bem, alttermin, schaden, anze, vstatus,
rstatus, tf.pn, gebname, gebdat, geschl,
gebort, famname, vorname, titel, wohn,
stras, land, staat, mutname, vf.bem,
bzr, vorl
from tf, vf, pe
 where tf.az = vf.az
   and tf.pn = pe.pn
   and tf.lfdnr = :a
   and vf.az = :b
   for reuse !
IF &rc = 0 THEN BEGIN
executing!
data!
select!
1
c 2  '2'
c 17 '5 Js 7/90'

nodata!
END
pars_execute!
drop parseid !
1
* - - -
* parsing!
declare e1 cursor for 
select 
tf.az, pe.pn, lfdnr, tatvorw, erlart,
freitxt, spruchk, empf, betrag, auflerf,
vfgdat, bescheid, uebern, beschw, bescherl,
pe.bem, entschantr, stordat, amger, amaz,
ament1, ament2, ament3, amdat, amrm,
lnger, lnaz, lnent1, lnent2, lnent3,
lndat, lnrm, revaz, revent1, revent2,
rkdat, vrsaz, pstatus, vf.az, vfart1,
vfart2, frdaz, einlbeh, einldat, eingdat,
delgr1, delgr2, delgr3, dezanw, dezkz1,
dezkz2, tatvorwf, tatort, tatzeit, asserv,
assnum, anzb, bez, zknum, eintdat,
einlbeh2, einldat2, wglanf, wgldauer, pomap,
tf.bem, alttermin, schaden, anze, vstatus,
rstatus, tf.pn, gebname, gebdat, geschl,
gebort, famname, vorname, titel, wohn,
stras, land, staat, mutname, vf.bem,
bzr, vorl
from tf, vf, pe
 where tf.az = vf.az
   and tf.pn = pe.pn
   and tf.lfdnr = '2'
   and vf.az = '5 Js 7/90'
   for reuse !
* executing!
* data!
* select!
* c 2  '2'
* c 17 '5 Js 7/90'
*
* 1
* nodata!
* pars_execute!
commit   release!
