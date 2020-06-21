*   *ID* N99A21   DBCMD    changed on 1992-09-28-13.27.16 by ADMIN     *
file connect ( kern test !
* in se_or versauter Pars-Baum (Rattay o.?a. Nov. 89 ) !
create table person (
gkz   fixed (4) key,
persnr fixed(2) key,
anrede fixed (1),
titel fixed (1),
name char (25),
vname char (25),
gebdat char(6),
telnr char (15),
telefax char(15),
partei fixed (1),
amtsantritt char(6),
wzende char(6),
vorzeit char(1))!
select * from person
where gkz = 1111 or anrede = 1 or titel between 1 and 2
or name like '1111111111111111111111111'
or vname like '1111111111111111111111111'
or ((gebdat >= '601111' and gebdat >= '200101')
or (gebdat < '191111' and gebdat < '200101'))
or telnr like '111111111111111' or telefax
like '111111111111111' or partei between 1 and 2
or ((amtsantritt >= '601111' and amtsantritt >= '200201')
or (amtsantritt <'190101' and amtsantritt <'200101'))
or ((wzende >= '601111' and wzende >= '200101')
or (wzende < '190101' and wzende < '200101'))
or vorzeit = 'J' order by person.partei, ascii(person.telefax),
ascii(person.telnr), ascii(person.gebdat), ascii(person.vname), 
ascii(person.name), person.titel, person.anrede, 
person.gkz for reuse !
rollback release !
