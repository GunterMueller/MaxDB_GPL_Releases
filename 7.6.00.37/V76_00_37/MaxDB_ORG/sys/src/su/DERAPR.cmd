*   *ID* DERAPR   CMD  changed on 1994-07-15-18.00.00 by heinz     *
 *** Bereichs-DELETE fuer Schluessel-Spalten wird auf KB-Ebene getestet.
 ***********************************************************!
connect kern test nolog!
insert RanDelBas values
('Zuse','Kaefer  ','reHinRad',x'00FFFFFF','Rad ','333','NK3','E')!
alter table RanDelBas add (ad1 char(250), ad2 char(250))!
update RanDelBas set ad1='1'           , ad2='2'!
update RanDelBas set ad1=lfill(ad1,'1'), ad2=lfill(ad2,'2')!
file DERA2!
create index namedind2 on RanDelTst (NK2,NK1)!
create index namedind1 on RanDelTst (NK1,NK2)!
delete RanDelTst where Key1of4 between 'K' and 'Zv'!
commit release!
dbmcli superdba,colduser !
db_connect kern,test !
db_execute diagnose index namedind1 on kern.RanDelTst!
db_execute diagnose index namedind2 on kern.RanDelTst!
db_release !
bye !
connect kern test nolog!
delete RanDelTst where Key1of4 =  'Volkswag' and Key2of4 = 'Polo'!
delete RanDelTst where Key1of4 <'Volkswag'!
delete RanDelTst where Key1of4 >'Volkswag'!
DECLARE SEL_MON CURSOR FOR
 SELECT DESCRIPTION,
        FIXED(VALUE,11,0) "VALUE"
   FROM SYSDBA.MONITOR_ROW 
  WHERE NOT (DESCRIPTION LIKE '%temp%') !
FETCH SEL_MON INTO :A, :B !
CLOSE SEL_MON !
commit release!
connect kern test nolog!
