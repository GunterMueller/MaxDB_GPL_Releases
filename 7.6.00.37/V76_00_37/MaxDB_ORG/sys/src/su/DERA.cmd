*   *ID* DERA     DBCMD    changed on 1994-03-24-16.00.00 by heinz     *
 *** Bereichs-DELETE wird auf KB-Ebene getestet.
 *** Kreieren und Fuellen der Tabelle.
 ***********************************************************!
file connect ( kern test!
drop   table RanDelBas!
create table RanDelBas
(Key1of4    char(8) key,
 Key2of4    char(8) key,
 Key3of4    char(8) key,
 Key4of4 varchar(4) byte key default x'00FFFFFF',
 NK1        char(8) ,
 NK2     varchar(3) , 
 NK3        char(3) ,
 NK4        char(1) , constraint c1_RanDelBas unique(NK4) ) !
grant all on RanDelBas to public!
insert RanDelBas values
('Key1of4' ,'Key2of4' ,'Key3of4' ,x'4B657934','NK1' ,'NK2','NK3','1')!
insert RanDelBas values
('Volkswag','Polo    ','Polo    ',x'00FFFFFF','Plan','333','NK3','2')!
insert RanDelBas values
('Volkswag','Polo    ','liVorRad',x'00FFFFFF','Rad ','333','NK3','3')!
insert RanDelBas values
('Volkswag','Polo    ','BeifSitz',x'00FFFFFF','Sitz','444','NK3','4')!
insert RanDelBas values
('Volkswag','Polo    ','FahrSitz',x'00FFFFFF','Sitz','444','NK3','5')!
insert RanDelBas values
('Volkswag','Polo    ','HintBank',x'00FFFFFF','Sitz','333','NK3','6')!
insert RanDelBas values
('Volkswag','Porsche ','Porsche ',x'00FFFFFF','Plan','333','NK3','7')!
insert RanDelBas values
('Volkswag','Porsche ','liVorRad',x'00FFFFFF','Rad ','333','NK3','8')!
insert RanDelBas values
('Volkswag','Porsche ','reHinRad',x'00FFFFFF','Rad ','33' ,'NK3','9')!
insert RanDelBas values
('Volkswag','Porsche ','reVorRad',x'00FFFFFF','Rad ','333','NK3','A')!
insert RanDelBas values
('Volkswag','Porsche ','liHinRad',x'00FFFFFF','Rad ','333','NK3','B')!
insert RanDelBas values
('Volkswag','Porsche ','FahrSitz',x'00FFFFFF','Sitz','444','NK3','C')!
insert RanDelBas values
('Volkswag','Kaefer  ','reHinRad',x'00FFFFFF','Rad ','333','NK3','D')!
select count (*) from RanDelBas!
fetch!
select * from RanDelBas!
fetch!

