senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* SAG.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table statistik (kdnr   decimal not null,
                        monat  decimal not null,
                        umsatz decimal not null)!
insert statistik values (1, 1, 100)!
insert statistik values (3, 1, 200)!
insert statistik values (1, 3, 500)!
insert statistik values (2, 4, 100)!
insert statistik values (3, 4, 300)!
insert statistik values (1, 7, 800)!
insert statistik values (2, 7, 900)!
insert statistik values (3,10, 200)!
insert statistik values (2,11,1000)!
insert statistik values (1,12, 200)!
insert statistik values (3,12, 500)!
select a.kdnr, a.monat, a.umsatz, sum (b.umsatz) kumumsatz
from statistik a, statistik b
where a.kdnr = b.kdnr
  and a.monat >= b.monat
group by a.kdnr, a.monat, a.umsatz
order by a.kdnr, a.monat !
fetch!
rollback release !
file connect ( kern test !
create table personen (pers_nr decimal (6),
                       name    char   (30))!
insert into personen values (1, 'Schmitz,Peter')!
insert into personen values (2, 'Hofer,Anette')!
insert into personen values (3, 'Moericke,Michael')!
create table sprache (pers_nr decimal (6),
                      sprache char (12))!
insert into sprache values (1, 'Englisch')!
insert into sprache values (1, 'Tuerkisch')!
insert into sprache values (1, 'Arabisch')!
insert into sprache values (2, 'Englisch')!
insert into sprache values (2, 'Franzoesisch')!
insert into sprache values (3, 'Englisch')!
insert into sprache values (3, 'Spanisch')!
insert into sprache values (3, 'Russisch')!
create table anforderung (sprache char (12))!
insert into anforderung values ('Arabisch')!
insert into anforderung values ('Englisch')!
select pers_nr, name
  from personen p
 where not exists
       (select *
         from anforderung a
         where not exists
               (select * from sprache s
                 where s.pers_nr = p.pers_nr
                   and s.sprache = a.sprache))!
fetch !
select pers_nr, name
  from personen p, (select count(*) cnt from anforderung) acnt
 where acnt.cnt = (select count (*) 
                     from anforderung a, sprache s
                    where s.sprache = a.sprache
                      and s.pers_nr = p.pers_nr) ! 
fetch !
select p.name
  from personen p
 where p.pers_nr in
       (select s.pers_nr
          from sprache s, anforderung a
         where s.sprache = a.sprache
         group by s.pers_nr
         having count (*) =
                (select count (*) from anforderung)) !
fetch!
select pers_nr, name
  from personen p
 where not exists
       (select *
          from sprache s, anforderung a
         where s.sprache(+) = a.sprache
           and s.pers_nr(+) = p.pers_nr
           and s.sprache is null) !
fetch!
select pers_nr, name
  from personen p
 where not exists
       (select *
          from sprache s, anforderung a
         where s.sprache(+) = a.sprache
           and s.pers_nr(+) = p.pers_nr)!
fetch!
rollback release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* SAP.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t ( a char (8) ascii key, b char (5) ascii key, 
  c char (8) ascii)!
insert t values ('aaa', 'bb', 'ccc')!
insert t values ('neu', '22', 'c2c')!
insert t values ('neu', 'bb', '333')!
commit release !
file connect ( kern test !
create table mara (
MANDT              CHAR       ( 3 ),
MATNR              CHAR       ( 18 ),
ERSDA              CHAR       ( 8 ),
ERNAM              CHAR       ( 12 ),
LAEDA              CHAR       ( 8 ),
AENAM              CHAR       ( 12 ),
VPSTA              CHAR       ( 15 ),
PSTAT              CHAR       ( 15 ),
LVORM              CHAR       ( 1 ),
MTART              CHAR       ( 4 ),
MBRSH              CHAR       ( 1 ),
MATKL              CHAR       ( 9 ),
BISMT              CHAR (   18           ),
MEINS              CHAR (    3           ),
BSTME              CHAR (    3           ),
ZEINR              CHAR (   22           ),
ZEIAR              CHAR (    3           ),
ZEIVR              CHAR (    2           ),
ZEIFO              CHAR (    4           ),
AESZN              CHAR (    6           ),
BLATT              CHAR (    3           ),
BLANZ              CHAR (    3           ),
FERTH              CHAR (   18           ),
FORMT              CHAR (    4           ),
GROES              CHAR (   32           ),
WRKST              CHAR (   14           ),
NORMT              CHAR (   18           ),
LABOR              CHAR (    3           ),
EKWSL              CHAR (    4           ),
BRGEW              FIXED (          13 , 3 ),
NTGEW              FIXED (          13 , 3 ),
GEWEI              CHAR (    3           ),
VOLUM              FIXED (          13 , 3 ),
VOLEH              CHAR (    3           ),
BEHVO              CHAR (    2           ),
RAUBE              CHAR (    2           ),
TEMPB              CHAR (    2           ),
DISST              CHAR (    3           ),
TRAGR              CHAR (    4           ),
STOFF              CHAR (   18           ),
SPART              CHAR (    2           ),
KUNNR              CHAR (   10           ),
EANNR              CHAR (   14           ),
WESCH              FIXED (          13 , 3 ),
BWVOR              CHAR (    1           ),
BWSCL              CHAR (    1           ),
SAISO              CHAR (    4           ),
ETIAR              CHAR (    2           ),
ETIFO              CHAR (    2           ),
ENTAR              CHAR (    1           ),
EAN11              CHAR (   18           ),
NUMTP              CHAR (    2           ),
LAENG              FIXED (          13 , 3 ),
BREIT              FIXED (          13 , 3 ),
HOEHE              FIXED (          13 , 3 ),
MEABM              CHAR (    3           ),
PRDHA              CHAR (   18           ),
primary key  (mandt, matnr))!
insert into mara (mandt, matnr) values ('900', 'SD000001')!
 
create table makt (
MANDT                 CHAR (       3       ),
MATNR                 CHAR (      18       ),
SPRAS                 CHAR (       1       ),
MAKTX                 CHAR (      40       ),
primary key (mandt, matnr, spras))!
insert into makt (mandt, matnr, spras) values ('900', 'SD000001', 'a')!

create table mvke (
MANDT                 CHAR (       3       ),
MATNR                 CHAR (      18       ),
VKORG                 CHAR (       4       ),
VTWEG                 CHAR (       2       ),
LVORM                 CHAR (       1       ),
VERSG                 CHAR (       1       ),
BONUS                 CHAR (       2       ),
PROVG                 CHAR (       2       ),
SKTOF                 CHAR (       1       ),
VMSTA                 CHAR (       2       ),
VMSTD                 CHAR (       8       ),
AUMNG                 FIXED (        13 ,  3  ),
LFMNG                 FIXED (        13 ,  3  ),
EFMNG                 FIXED (        13 ,  3  ),
SCMNG                 FIXED (        13 ,  3  ),
SCHME                 CHAR (       3       ),
VRKME                 CHAR (       3       ),
MTPOS                 CHAR (       4       ),
DWERK                 CHAR (       4       ),
PRODH                 CHAR (      18       ),
PMATN                 CHAR (      18       ),
KONDM                 CHAR (       2       ),
KTGRM                 CHAR (       2       ),
MVGR1                 CHAR (       3       ),
MVGR2                 CHAR (       3       ),
MVGR3                 CHAR (       3       ),
MVGR4                 CHAR (       3       ),
MVGR5                 CHAR (       3       ),
primary key (mandt, matnr, vkorg, vtweg))!
insert into mvke (mandt, matnr, vkorg, vtweg) values (
'900', 'SD000001', 'vier', 'a')!
CREATE VIEW MAPOV ( MANDT , MATNR , SPRAS , VKORG , VTWEG , ZIEME , BRGEW ,   
       NTGEW , GEWEI , VOLUM , VOLEH , RAUBE , TEMPB , DISST , EAN11 , NUMTP ,    
       LAENG , BREIT , HOEHE , MEABM , PRDHA , KUNNR , VERSG , BONUS , PROVG ,    
       SKTOF , VMSTA , VMSTD , AUMNG , LFMNG , EFMNG , SCHME , SCMNG , VRKME ,    
       SPART , ARKTX , PRODH , PMATN , MTPOS , DWERK , MEINS , TRAGR , MATKL ,    
       MTART , KTGRM , KONDM , LVORM , MVGR1 , MVGR2 , MVGR3 , MVGR4 , MVGR5 )    
AS SELECT 
       T0001.MANDT , T0001.MATNR , T0002.SPRAS , T0003.VKORG , T0003.VTWEG ,   
       T0003.VRKME , T0001.BRGEW , T0001.NTGEW , T0001.GEWEI , T0001.VOLUM ,      
       T0001.VOLEH , T0001.RAUBE , T0001.TEMPB , T0001.DISST , T0001.EAN11 ,      
       T0001.NUMTP , T0001.LAENG , T0001.BREIT , T0001.HOEHE , T0001.MEABM ,      
       T0001.PRDHA , T0001.KUNNR , T0003.VERSG , T0003.BONUS , T0003.PROVG ,      
       T0003.SKTOF , T0003.VMSTA , T0003.VMSTD , T0003.AUMNG , T0003.LFMNG ,      
       T0003.EFMNG , T0003.SCHME , T0003.SCMNG , T0003.VRKME , T0001.SPART ,      
       T0002.MAKTX , T0003.PRODH , T0003.PMATN , T0003.MTPOS , T0003.DWERK ,      
       T0001.MEINS , T0001.TRAGR , T0001.MATKL , T0001.MTART , T0003.KTGRM ,      
       T0003.KONDM , T0003.LVORM , T0003.MVGR1 , T0003.MVGR2 , T0003.MVGR3 ,      
       T0003.MVGR4 , T0003.MVGR5                                                  
FROM MARA T0001, MAKT T0002, MVKE T0003                                           
WHERE T0001.MANDT = T0002.MANDT 
  AND T0001.MATNR = T0002.MATNR 
  AND T0001.MANDT = T0003.MANDT 
  AND T0001.MATNR = T0003.MATNR   !
select * from mapov where mandt = '900' and
matnr in ('SD000004', 'SD000003', 'SD000002', 'SD000001', 'SD000000')!
fetch!
rollback release !
file connect ( kern test !
CREATE TABLE "KERN              "."TVEU12            " (
"K1N5              " CHAR      (5) ASCII    ,
"F1I4              " FIXED     (10,0) NOT NULL,
"F2F8              " FLOAT     (18) NOT NULL,
"F3CH              " CHAR      (30) ASCII    NOT NULL,
"F4TI              " CHAR      (6) ASCII    NOT NULL
, PRIMARY KEY ("K1N5              "
)) !
insert kern.tveu12 values ('00001',     1, 1.00000000000000010E-01, 
    'SATZNUMMER : 00001............', '093839') !
insert kern.tveu12 values ('00002',     9, 9.00000000000000020E-01, 
    'SATZNUMMER : 00002............', '093838') !
insert kern.tveu12 values ('00011',    40, 4.00000000000000000E+00, 
    'SATZNUMMER : 00011............', '093829') !
insert kern.tveu12 values ('00051',   100, 1.00000000000000000E+01, 
    'SATZNUMMER : 00051............', '093749') !
insert kern.tveu12 values ('00151',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00151............', '093609') !
insert kern.tveu12 values ('00153',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00153............', '093607') !
insert kern.tveu12 values ('00155',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00155............', '093605') !
insert kern.tveu12 values ('00157',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00157............', '093603') !
insert kern.tveu12 values ('00159',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00159............', '093601') !
insert kern.tveu12 values ('00161',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00161............', '093559') !
insert kern.tveu12 values ('00163',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00163............', '093557') !
insert kern.tveu12 values ('00165',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00165............', '093555') !
insert kern.tveu12 values ('00167',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00167............', '093553') !
insert kern.tveu12 values ('00169',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00169............', '093551') !
insert kern.tveu12 values ('00171',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00171............', '093549') !
insert kern.tveu12 values ('00173',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00173............', '093547') !
insert kern.tveu12 values ('00175',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00175............', '093545') !
insert kern.tveu12 values ('00177',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00177............', '093543') !
insert kern.tveu12 values ('00179',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00179............', '093541') !
insert kern.tveu12 values ('00181',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00181............', '093539') !
insert kern.tveu12 values ('00183',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00183............', '093537') !
insert kern.tveu12 values ('00185',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00185............', '093535') !
insert kern.tveu12 values ('00187',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00187............', '093533') !
insert kern.tveu12 values ('00189',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00189............', '093531') !
insert kern.tveu12 values ('00191',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00191............', '093529') !
insert kern.tveu12 values ('00193',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00193............', '093527') !
insert kern.tveu12 values ('00195',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00195............', '093525') !
insert kern.tveu12 values ('00197',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00197............', '093523') !
insert kern.tveu12 values ('00199',     2, 2.00000000000000010E-01, 
    'SATZNUMMER : 00199............', '093521') !
CREATE TABLE "KERN              "."TVEU13            " (
"K1N5              " CHAR      (5) ASCII    ,
"K2N5              " CHAR      (5) ASCII    ,
"F1I4              " FIXED     (10,0) NOT NULL,
"F2TX              " CHAR      (30) ASCII    NOT NULL,
"F3TI              " CHAR      (6) ASCII    NOT NULL
, PRIMARY KEY ("K1N5              "
,"K2N5              "
)) !
insert KERN.TVEU13 values ('00001', '00001',   1, 
     '%Die Mitternacht  ............', '093839') !
insert KERN.TVEU13 values ('00002', '00002',   1, 
     '_zog naeher schon,............', '093838') !
insert KERN.TVEU13 values ('00002', '00003',   2, 
     '%in stiller Ruh   ............', '093837') !
insert KERN.TVEU13 values ('00002', '00004',   3, 
     '_lag Babylon.     ............', '093836') !
insert KERN.TVEU13 values ('00002', '00005',   4, 
     '%Die Mitternacht  ............', '093835') !
insert KERN.TVEU13 values ('00002', '00006',   5, 
     '_zog naeher schon,............', '093834') !
insert KERN.TVEU13 values ('00002', '00007',   6, 
     '%in stiller Ruh   ............', '093833') !
insert KERN.TVEU13 values ('00002', '00008',   7, 
     '_lag Babylon.     ............', '093832') !
insert KERN.TVEU13 values ('00002', '00009',   8, 
     '%Die Mitternacht  ............', '093831') !
insert KERN.TVEU13 values ('00002', '00010',   9, 
     '_zog naeher schon,............', '093830') !
insert KERN.TVEU13 values ('00011', '00011',   1, 
     '%in stiller Ruh   ............', '093829') !
insert KERN.TVEU13 values ('00011', '00012',   2, 
     '_lag Babylon.     ............', '093828') !
insert KERN.TVEU13 values ('00011', '00013',   3, 
     '%Die Mitternacht  ............', '093827') !
insert KERN.TVEU13 values ('00011', '00014',   4, 
     '_zog naeher schon,............', '093826') !
insert KERN.TVEU13 values ('00011', '00015',   5, 
     '%in stiller Ruh   ............', '093825') !
insert KERN.TVEU13 values ('00011', '00016',   6, 
     '_lag Babylon.     ............', '093824') !
insert KERN.TVEU13 values ('00011', '00017',   7, 
     '%Die Mitternacht  ............', '093823') !
insert KERN.TVEU13 values ('00011', '00018',   8, 
     '_zog naeher schon,............', '093822') !
insert KERN.TVEU13 values ('00011', '00019',   9, 
     '%in stiller Ruh   ............', '093821') !
insert KERN.TVEU13 values ('00011', '00020',  10, 
     '_lag Babylon.     ............', '093820') !
insert KERN.TVEU13 values ('00011', '00021',  11, 
     '%Die Mitternacht  ............', '093819') !
insert KERN.TVEU13 values ('00011', '00022',  12, 
     '_zog naeher schon,............', '093818') !
insert KERN.TVEU13 values ('00011', '00023',  13, 
     '%in stiller Ruh   ............', '093817') !
insert KERN.TVEU13 values ('00011', '00024',  14, 
     '_lag Babylon.     ............', '093816') !
insert KERN.TVEU13 values ('00011', '00025',  15, 
     '%Die Mitternacht  ............', '093815') !
insert KERN.TVEU13 values ('00011', '00026',  16, 
     '_zog naeher schon,............', '093814') !
insert KERN.TVEU13 values ('00011', '00027',  17, 
     '%in stiller Ruh   ............', '093813') !
insert KERN.TVEU13 values ('00011', '00028',  18, 
     '_lag Babylon.     ............', '093812') !
insert KERN.TVEU13 values ('00011', '00029',  19, 
     '%Die Mitternacht  ............', '093811') !
insert KERN.TVEU13 values ('00011', '00030',  20, 
     '_zog naeher schon,............', '093810') !
insert KERN.TVEU13 values ('00011', '00031',  21, 
     '%in stiller Ruh   ............', '093809') !
insert KERN.TVEU13 values ('00011', '00032',  22, 
     '_lag Babylon.     ............', '093808') !
insert KERN.TVEU13 values ('00011', '00033',  23, 
     '%Die Mitternacht  ............', '093807') !
insert KERN.TVEU13 values ('00011', '00034',  24, 
     '_zog naeher schon,............', '093806') !
insert KERN.TVEU13 values ('00011', '00035',  25, 
     '%in stiller Ruh   ............', '093805') !
insert KERN.TVEU13 values ('00011', '00036',  26, 
     '_lag Babylon.     ............', '093804') !
insert KERN.TVEU13 values ('00011', '00037',  27, 
     '%Die Mitternacht  ............', '093803') !
insert KERN.TVEU13 values ('00011', '00038',  28, 
     '_zog naeher schon,............', '093802') !
insert KERN.TVEU13 values ('00011', '00039',  29, 
     '%in stiller Ruh   ............', '093801') !
insert KERN.TVEU13 values ('00011', '00040',  30, 
     '_lag Babylon.     ............', '093800') !
insert KERN.TVEU13 values ('00011', '00041',  31, 
     '%Die Mitternacht  ............', '093759') !
insert KERN.TVEU13 values ('00011', '00042',  32, 
     '_zog naeher schon,............', '093758') !
insert KERN.TVEU13 values ('00011', '00043',  33, 
     '%in stiller Ruh   ............', '093757') !
insert KERN.TVEU13 values ('00011', '00044',  34, 
     '_lag Babylon.     ............', '093756') !
insert KERN.TVEU13 values ('00011', '00045',  35, 
     '%Die Mitternacht  ............', '093755') !
insert KERN.TVEU13 values ('00011', '00046',  36, 
     '_zog naeher schon,............', '093754') !
insert KERN.TVEU13 values ('00011', '00047',  37, 
     '%in stiller Ruh   ............', '093753') !
insert KERN.TVEU13 values ('00011', '00048',  38, 
     '_lag Babylon.     ............', '093752') !
insert KERN.TVEU13 values ('00011', '00049',  39, 
     '%Die Mitternacht  ............', '093751') !
insert KERN.TVEU13 values ('00011', '00050',  40, 
     '_zog naeher schon,............', '093750') !
insert KERN.TVEU13 values ('00051', '00051',   1, 
     '%in stiller Ruh   ............', '093749') !
insert KERN.TVEU13 values ('00051', '00052',   2, 
     '_lag Babylon.     ............', '093748') !
insert KERN.TVEU13 values ('00051', '00053',   3, 
     '%Die Mitternacht  ............', '093747') !
insert KERN.TVEU13 values ('00051', '00054',   4, 
     '_zog naeher schon,............', '093746') !
insert KERN.TVEU13 values ('00051', '00055',   5, 
     '%in stiller Ruh   ............', '093745') !
insert KERN.TVEU13 values ('00051', '00056',   6, 
     '_lag Babylon.     ............', '093744') !
insert KERN.TVEU13 values ('00051', '00057',   7, 
     '%Die Mitternacht  ............', '093743') !
insert KERN.TVEU13 values ('00051', '00058',   8, 
     '_zog naeher schon,............', '093742') !
insert KERN.TVEU13 values ('00051', '00059',   9, 
     '%in stiller Ruh   ............', '093741') !
insert KERN.TVEU13 values ('00051', '00060',  10, 
     '_lag Babylon.     ............', '093740') !
insert KERN.TVEU13 values ('00051', '00061',  11, 
     '%Die Mitternacht  ............', '093739') !
insert KERN.TVEU13 values ('00051', '00062',  12, 
     '_zog naeher schon,............', '093738') !
insert KERN.TVEU13 values ('00051', '00063',  13, 
     '%in stiller Ruh   ............', '093737') !
insert KERN.TVEU13 values ('00051', '00064',  14, 
     '_lag Babylon.     ............', '093736') !
insert KERN.TVEU13 values ('00051', '00065',  15, 
     '%Die Mitternacht  ............', '093735') !
insert KERN.TVEU13 values ('00051', '00066',  16, 
     '_zog naeher schon,............', '093734') !
insert KERN.TVEU13 values ('00051', '00067',  17, 
     '%in stiller Ruh   ............', '093733') !
insert KERN.TVEU13 values ('00051', '00068',  18, 
     '_lag Babylon.     ............', '093732') !
insert KERN.TVEU13 values ('00051', '00069',  19, 
     '%Die Mitternacht  ............', '093731') !
insert KERN.TVEU13 values ('00051', '00070',  20, 
     '_zog naeher schon,............', '093730') !
insert KERN.TVEU13 values ('00051', '00071',  21, 
     '%in stiller Ruh   ............', '093729') !
insert KERN.TVEU13 values ('00051', '00072',  22, 
     '_lag Babylon.     ............', '093728') !
insert KERN.TVEU13 values ('00051', '00073',  23, 
     '%Die Mitternacht  ............', '093727') !
insert KERN.TVEU13 values ('00051', '00074',  24, 
     '_zog naeher schon,............', '093726') !
insert KERN.TVEU13 values ('00051', '00075',  25, 
     '%in stiller Ruh   ............', '093725') !
insert KERN.TVEU13 values ('00051', '00076',  26, 
     '_lag Babylon.     ............', '093724') !
insert KERN.TVEU13 values ('00051', '00077',  27, 
     '%Die Mitternacht  ............', '093723') !
insert KERN.TVEU13 values ('00051', '00078',  28, 
     '_zog naeher schon,............', '093722') !
insert KERN.TVEU13 values ('00051', '00079',  29, 
     '%in stiller Ruh   ............', '093721') !
insert KERN.TVEU13 values ('00051', '00080',  30, 
     '_lag Babylon.     ............', '093720') !
insert KERN.TVEU13 values ('00051', '00081',  31, 
     '%Die Mitternacht  ............', '093719') !
insert KERN.TVEU13 values ('00051', '00082',  32, 
     '_zog naeher schon,............', '093718') !
insert KERN.TVEU13 values ('00051', '00083',  33, 
     '%in stiller Ruh   ............', '093717') !
insert KERN.TVEU13 values ('00051', '00084',  34, 
     '_lag Babylon.     ............', '093716') !
insert KERN.TVEU13 values ('00051', '00085',  35, 
     '%Die Mitternacht  ............', '093715') !
insert KERN.TVEU13 values ('00051', '00086',  36, 
     '_zog naeher schon,............', '093714') !
insert KERN.TVEU13 values ('00051', '00087',  37, 
     '%in stiller Ruh   ............', '093713') !
insert KERN.TVEU13 values ('00051', '00088',  38, 
     '_lag Babylon.     ............', '093712') !
insert KERN.TVEU13 values ('00051', '00089',  39, 
     '%Die Mitternacht  ............', '093711') !
insert KERN.TVEU13 values ('00051', '00090',  40, 
     '_zog naeher schon,............', '093710') !
insert KERN.TVEU13 values ('00051', '00091',  41, 
     '%in stiller Ruh   ............', '093709') !
insert KERN.TVEU13 values ('00051', '00092',  42, 
     '_lag Babylon.     ............', '093708') !
insert KERN.TVEU13 values ('00051', '00093',  43, 
     '%Die Mitternacht  ............', '093707') !
insert KERN.TVEU13 values ('00051', '00094',  44, 
     '_zog naeher schon,............', '093706') !
insert KERN.TVEU13 values ('00051', '00095',  45, 
     '%in stiller Ruh   ............', '093705') !
insert KERN.TVEU13 values ('00051', '00096',  46, 
     '_lag Babylon.     ............', '093704') !
insert KERN.TVEU13 values ('00051', '00097',  47, 
     '%Die Mitternacht  ............', '093703') !
insert KERN.TVEU13 values ('00051', '00098',  48, 
     '_zog naeher schon,............', '093702') !
insert KERN.TVEU13 values ('00051', '00099',  49, 
     '%in stiller Ruh   ............', '093701') !
insert KERN.TVEU13 values ('00051', '00100',  50, 
     '_lag Babylon.     ............', '093700') !
insert KERN.TVEU13 values ('00051', '00101',  51, 
     '%Die Mitternacht  ............', '093659') !
insert KERN.TVEU13 values ('00051', '00102',  52, 
     '_zog naeher schon,............', '093658') !
insert KERN.TVEU13 values ('00051', '00103',  53, 
     '%in stiller Ruh   ............', '093657') !
insert KERN.TVEU13 values ('00051', '00104',  54, 
     '_lag Babylon.     ............', '093656') !
insert KERN.TVEU13 values ('00051', '00105',  55, 
     '%Die Mitternacht  ............', '093655') !
insert KERN.TVEU13 values ('00051', '00106',  56, 
     '_zog naeher schon,............', '093654') !
insert KERN.TVEU13 values ('00051', '00107',  57, 
     '%in stiller Ruh   ............', '093653') !
insert KERN.TVEU13 values ('00051', '00108',  58, 
     '_lag Babylon.     ............', '093652') !
insert KERN.TVEU13 values ('00051', '00109',  59, 
     '%Die Mitternacht  ............', '093651') !
insert KERN.TVEU13 values ('00051', '00110',  60, 
     '_zog naeher schon,............', '093650') !
insert KERN.TVEU13 values ('00051', '00111',  61, 
     '%in stiller Ruh   ............', '093649') !
insert KERN.TVEU13 values ('00051', '00112',  62, 
     '_lag Babylon.     ............', '093648') !
insert KERN.TVEU13 values ('00051', '00113',  63, 
     '%Die Mitternacht  ............', '093647') !
insert KERN.TVEU13 values ('00051', '00114',  64, 
     '_zog naeher schon,............', '093646') !
insert KERN.TVEU13 values ('00051', '00115',  65, 
     '%in stiller Ruh   ............', '093645') !
insert KERN.TVEU13 values ('00051', '00116',  66, 
     '_lag Babylon.     ............', '093644') !
insert KERN.TVEU13 values ('00051', '00117',  67, 
     '%Die Mitternacht  ............', '093643') !
insert KERN.TVEU13 values ('00051', '00118',  68, 
     '_zog naeher schon,............', '093642') !
insert KERN.TVEU13 values ('00051', '00119',  69, 
     '%in stiller Ruh   ............', '093641') !
insert KERN.TVEU13 values ('00051', '00120',  70, 
     '_lag Babylon.     ............', '093640') !
insert KERN.TVEU13 values ('00051', '00121',  71, 
     '%Die Mitternacht  ............', '093639') !
insert KERN.TVEU13 values ('00051', '00122',  72, 
     '_zog naeher schon,............', '093638') !
insert KERN.TVEU13 values ('00051', '00123',  73, 
     '%in stiller Ruh   ............', '093637') !
insert KERN.TVEU13 values ('00051', '00124',  74, 
     '_lag Babylon.     ............', '093636') !
insert KERN.TVEU13 values ('00051', '00125',  75, 
     '%Die Mitternacht  ............', '093635') !
insert KERN.TVEU13 values ('00051', '00126',  76, 
     '_zog naeher schon,............', '093634') !
insert KERN.TVEU13 values ('00051', '00127',  77, 
     '%in stiller Ruh   ............', '093633') !
insert KERN.TVEU13 values ('00051', '00128',  78, 
     '_lag Babylon.     ............', '093632') !
insert KERN.TVEU13 values ('00051', '00129',  79, 
     '%Die Mitternacht  ............', '093631') !
insert KERN.TVEU13 values ('00051', '00130',  80, 
     '_zog naeher schon,............', '093630') !
insert KERN.TVEU13 values ('00051', '00131',  81, 
     '%in stiller Ruh   ............', '093629') !
insert KERN.TVEU13 values ('00051', '00132',  82, 
     '_lag Babylon.     ............', '093628') !
insert KERN.TVEU13 values ('00051', '00133',  83, 
     '%Die Mitternacht  ............', '093627') !
insert KERN.TVEU13 values ('00051', '00134',  84, 
     '_zog naeher schon,............', '093626') !
insert KERN.TVEU13 values ('00051', '00135',  85, 
     '%in stiller Ruh   ............', '093625') !
insert KERN.TVEU13 values ('00051', '00136',  86, 
     '_lag Babylon.     ............', '093624') !
insert KERN.TVEU13 values ('00051', '00137',  87, 
     '%Die Mitternacht  ............', '093623') !
insert KERN.TVEU13 values ('00051', '00138',  88, 
     '_zog naeher schon,............', '093622') !
insert KERN.TVEU13 values ('00051', '00139',  89, 
     '%in stiller Ruh   ............', '093621') !
insert KERN.TVEU13 values ('00051', '00140',  90, 
     '_lag Babylon.     ............', '093620') !
insert KERN.TVEU13 values ('00051', '00141',  91, 
     '%Die Mitternacht  ............', '093619') !
insert KERN.TVEU13 values ('00051', '00142',  92, 
     '_zog naeher schon,............', '093618') !
insert KERN.TVEU13 values ('00051', '00143',  93, 
     '%in stiller Ruh   ............', '093617') !
insert KERN.TVEU13 values ('00051', '00144',  94, 
     '_lag Babylon.     ............', '093616') !
insert KERN.TVEU13 values ('00051', '00145',  95, 
     '%Die Mitternacht  ............', '093615') !
insert KERN.TVEU13 values ('00051', '00146',  96, 
     '_zog naeher schon,............', '093614') !
insert KERN.TVEU13 values ('00051', '00147',  97, 
     '%in stiller Ruh   ............', '093613') !
insert KERN.TVEU13 values ('00051', '00148',  98, 
     '_lag Babylon.     ............', '093612') !
insert KERN.TVEU13 values ('00051', '00149',  99, 
     '%Die Mitternacht  ............', '093611') !
insert KERN.TVEU13 values ('00051', '00150', 100, 
     '_zog naeher schon,............', '093610') !
insert KERN.TVEU13 values ('00151', '00151',   1, 
     '%in stiller Ruh   ............', '093609') !
insert KERN.TVEU13 values ('00151', '00152',   2, 
     '_lag Babylon.     ............', '093608') !
insert KERN.TVEU13 values ('00153', '00153',   1, 
     '%Die Mitternacht  ............', '093607') !
insert KERN.TVEU13 values ('00153', '00154',   2, 
     '_zog naeher schon,............', '093606') !
insert KERN.TVEU13 values ('00155', '00155',   1, 
     '%in stiller Ruh   ............', '093605') !
insert KERN.TVEU13 values ('00155', '00156',   2, 
     '_lag Babylon.     ............', '093604') !
insert KERN.TVEU13 values ('00157', '00157',   1, 
     '%Die Mitternacht  ............', '093603') !
insert KERN.TVEU13 values ('00157', '00158',   2, 
     '_zog naeher schon,............', '093602') !
insert KERN.TVEU13 values ('00159', '00159',   1, 
     '%in stiller Ruh   ............', '093601') !
insert KERN.TVEU13 values ('00159', '00160',   2, 
     '_lag Babylon.     ............', '093600') !
insert KERN.TVEU13 values ('00161', '00161',   1, 
     '%Die Mitternacht  ............', '093559') !
insert KERN.TVEU13 values ('00161', '00162',   2, 
     '_zog naeher schon,............', '093558') !
insert KERN.TVEU13 values ('00163', '00163',   1, 
     '%in stiller Ruh   ............', '093557') !
insert KERN.TVEU13 values ('00163', '00164',   2, 
     '_lag Babylon.     ............', '093556') !
insert KERN.TVEU13 values ('00165', '00165',   1, 
     '%Die Mitternacht  ............', '093555') !
insert KERN.TVEU13 values ('00165', '00166',   2, 
     '_zog naeher schon,............', '093554') !
insert KERN.TVEU13 values ('00167', '00167',   1, 
     '%in stiller Ruh   ............', '093553') !
insert KERN.TVEU13 values ('00167', '00168',   2, 
     '_lag Babylon.     ............', '093552') !
insert KERN.TVEU13 values ('00169', '00169',   1, 
     '%Die Mitternacht  ............', '093551') !
insert KERN.TVEU13 values ('00169', '00170',   2, 
     '_zog naeher schon,............', '093550') !
insert KERN.TVEU13 values ('00171', '00171',   1, 
     '%in stiller Ruh   ............', '093549') !
insert KERN.TVEU13 values ('00171', '00172',   2, 
     '_lag Babylon.     ............', '093548') !
insert KERN.TVEU13 values ('00173', '00173',   1, 
     '%Die Mitternacht  ............', '093547') !
insert KERN.TVEU13 values ('00173', '00174',   2, 
     '_zog naeher schon,............', '093546') !
insert KERN.TVEU13 values ('00175', '00175',   1, 
     '%in stiller Ruh   ............', '093545') !
insert KERN.TVEU13 values ('00175', '00176',   2, 
     '_lag Babylon.     ............', '093544') !
insert KERN.TVEU13 values ('00177', '00177',   1, 
     '%Die Mitternacht  ............', '093543') !
insert KERN.TVEU13 values ('00177', '00178',   2, 
     '_zog naeher schon,............', '093542') !
insert KERN.TVEU13 values ('00179', '00179',   1, 
     '%in stiller Ruh   ............', '093541') !
insert KERN.TVEU13 values ('00179', '00180',   2, 
     '_lag Babylon.     ............', '093540') !
insert KERN.TVEU13 values ('00181', '00181',   1, 
     '%Die Mitternacht  ............', '093539') !
insert KERN.TVEU13 values ('00181', '00182',   2, 
     '_zog naeher schon,............', '093538') !
insert KERN.TVEU13 values ('00183', '00183',   1, 
     '%in stiller Ruh   ............', '093537') !
insert KERN.TVEU13 values ('00183', '00184',   2, 
     '_lag Babylon.     ............', '093536') !
insert KERN.TVEU13 values ('00185', '00185',   1, 
     '%Die Mitternacht  ............', '093535') !
insert KERN.TVEU13 values ('00185', '00186',   2, 
     '_zog naeher schon,............', '093534') !
insert KERN.TVEU13 values ('00187', '00187',   1, 
     '%in stiller Ruh   ............', '093533') !
insert KERN.TVEU13 values ('00187', '00188',   2, 
     '_lag Babylon.     ............', '093532') !
insert KERN.TVEU13 values ('00189', '00189',   1, 
     '%Die Mitternacht  ............', '093531') !
insert KERN.TVEU13 values ('00189', '00190',   2, 
     '_zog naeher schon,............', '093530') !
insert KERN.TVEU13 values ('00191', '00191',   1, 
     '%in stiller Ruh   ............', '093529') !
insert KERN.TVEU13 values ('00191', '00192',   2, 
     '_lag Babylon.     ............', '093528') !
insert KERN.TVEU13 values ('00193', '00193',   1, 
     '%Die Mitternacht  ............', '093527') !
insert KERN.TVEU13 values ('00193', '00194',   2, 
     '_zog naeher schon,............', '093526') !
insert KERN.TVEU13 values ('00195', '00195',   1, 
     '%in stiller Ruh   ............', '093525') !
insert KERN.TVEU13 values ('00195', '00196',   2, 
     '_lag Babylon.     ............', '093524') !
insert KERN.TVEU13 values ('00197', '00197',   1, 
     '%Die Mitternacht  ............', '093523') !
insert KERN.TVEU13 values ('00197', '00198',   2, 
     '_zog naeher schon,............', '093522') !
insert KERN.TVEU13 values ('00199', '00199',   1, 
     '%in stiller Ruh   ............', '093521') !
insert KERN.TVEU13 values ('00199', '00200',   2, 
     '_lag Babylon.     ............', '093520') !
CREATE VIEW VVER2 (N5_12, N5_13, I4_12, CH_12, I4_13, TX_13)                
AS SELECT T0001.K1N5, T0002.K2N5, T0001.F1I4, T0001.F3CH, 
          T0002.F1I4, T0002.F2TX
FROM TVEU12 T0001, TVEU13 T0002 
WHERE T0001.K1N5 = T0002.K1N5 AND 
     ( T0002.K1N5 <= '00050' OR T0002.K1N5 > '00150' ) !
explain select * from vver2 !
explain SELECT T0001.K1N5, T0002.K2N5, T0001.F1I4, T0001.F3CH, 
          T0002.F1I4, T0002.F2TX
FROM <TVEU12 T0001, TVEU13 T0002> 
WHERE T0001.K1N5 = T0002.K1N5 AND 
     ( T0002.K1N5 <= '00050' OR T0002.K1N5 > '00150' ) !
explain SELECT T0001.K1N5, T0002.K2N5, T0001.F1I4, T0001.F3CH, 
          T0002.F1I4, T0002.F2TX
FROM <TVEU13 T0002, TVEU12 T0001> 
WHERE T0001.K1N5 = T0002.K1N5 AND 
     ( T0002.K1N5 <= '00050' OR T0002.K1N5 > '00150' ) !
select * from vver2 !
fetch !
rollback work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* SAPR3.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect (kern test !
CREATE TABLE ANLA (
"MANDT" VARCHAR   (3)  ,"BUKRS" VARCHAR   (4)  ,"ANLN1" VARCHAR   (12)  ,
"ANLN2" VARCHAR   (4)  ,
"ANLKL" VARCHAR   (8)  ,"GEGST" VARCHAR   (8)  ,
"ANLAR" VARCHAR   (5)  ,"ERNAM" VARCHAR   (12)  ,"ERDAT" VARCHAR   (8)  ,
"AENAM" VARCHAR   (12)  ,"AEDAT" VARCHAR   (8)  ,"XLOEV" VARCHAR   (1)  ,
"XSPEB" VARCHAR   (1)  ,"FELEI" VARCHAR   (4)  ,"KTOGR" VARCHAR   (8)  ,
"XOPVW" VARCHAR   (1)  ,"ANLTP" VARCHAR   (1)  ,"ZUJHR" VARCHAR   (4)  ,
"ZUPER" VARCHAR   (3)  ,"ZUGDT" VARCHAR   (8)  ,"AKTIV" VARCHAR   (8)  ,
"ABGDT" VARCHAR   (8)  ,"DEAKT" VARCHAR   (8)  ,"GPLAB" VARCHAR   (8)  ,
"BSTDT" VARCHAR   (8)  ,"ORD41" VARCHAR   (4)  ,"ORD42" VARCHAR   (4)  ,
"ORD43" VARCHAR   (4)  ,"ORD44" VARCHAR   (4)  ,"ANLUE" VARCHAR   (12)  ,
"ZUAWA" VARCHAR   (3)  ,"ANEQK" VARCHAR   (1)  ,"ANEQS" VARCHAR   (1)  ,
"LIFNR" VARCHAR   (10)  ,"LAND1" VARCHAR   (3)  ,"LIEFE" VARCHAR   (30)  ,
"HERST" VARCHAR   (30)  ,"EIGKZ" VARCHAR   (1)  ,"AIBN1" VARCHAR   (12)  ,
"AIBN2" VARCHAR   (4)  ,"AIBDT" VARCHAR   (8)  ,"URJHR" VARCHAR   (4)  ,
"URWRT" FIXED     (13,2) ,"ANTEI" FIXED     (5,2) ,"PROJN" VARCHAR   (16)  ,
"EAUFN" VARCHAR   (12)  ,"MEINS" VARCHAR   (3)  ,"MENGE" FIXED     (13,3) ,
"TYPBZ" VARCHAR   (15)  ,"IZWEK" VARCHAR   (2)  ,"INKEN" VARCHAR   (1)  ,
"IVDAT" VARCHAR   (8)  ,"INVZU" VARCHAR   (15)  ,"VMGLI" VARCHAR   (4)  ,
"XVRMW" VARCHAR   (1)  ,"WRTMA" FIXED     (13,2) ,"EHWRT" FIXED     (13,
2) ,"AUFLA" VARCHAR   (8)  ,"EHWZU" VARCHAR   (8)  ,"EHWNR" VARCHAR   (16)  ,
"GRUVO" VARCHAR   (8)  ,"GREIN" VARCHAR   (8)  ,"GRBND" VARCHAR   (5)  ,
"GRBLT" VARCHAR   (5)  ,"GRLFD" VARCHAR   (4)  ,"FLURK" VARCHAR   (4)  ,
"FLURN" VARCHAR   (5)  ,"FIAMT" VARCHAR   (25)  ,"STADT" VARCHAR   (25)  ,
"GRUND" VARCHAR   (3)  ,"FEINS" VARCHAR   (3)  ,"GRUFL" FIXED     (13,3) ,
"INVNR" VARCHAR   (25)  ,"VBUND" VARCHAR   (6)  ,"SPRAS" VARCHAR   (1)  ,
"TXT50" VARCHAR   (50)  ,"TXA50" VARCHAR   (50)  ,"XLTXID" VARCHAR   (1)  ,
"XVERID" VARCHAR   (1)  ,"XTCHID" VARCHAR   (1)  ,"XKALID" VARCHAR   (1)  ,
"XHERID" VARCHAR   (1)  ,"XLEAID" VARCHAR   (1)  ,"LEAFI" VARCHAR   (10)  ,
"LVDAT" VARCHAR   (8)  ,"LKDAT" VARCHAR   (8)  ,"LEABG" VARCHAR   (8)  ,
"LEJAR" VARCHAR   (3)  ,"LEPER" VARCHAR   (3)  ,"LRYTH" VARCHAR   (3)  ,
"LEGEB" FIXED     (13,2) ,"LBASW" FIXED     (13,2) ,"LKAUF" FIXED     (13,
2) ,"LMZIN" FIXED     (7,4) ,"LZINS" FIXED     (7,4) ,"LTZBW" VARCHAR   (8)  ,
"LKUZA" FIXED     (13,2) ,"LKUZI" FIXED     (13,2) ,"LLAVB" FIXED     (13,
2) ,"LEANZ" FIXED   (5,0) ,"LVTNR" VARCHAR   (15)  ,"LETXT" VARCHAR   (50)  ,
"XAKTIV" VARCHAR   (1)  ,"ANUPD" VARCHAR   (1)  ,"LBLNR" VARCHAR   (10)  ,
"XV0DT" VARCHAR   (8)  ,"XV0NM" VARCHAR   (12)  ,"XV1DT" VARCHAR   (8)  ,
"XV1NM" VARCHAR   (12)  ,"XV2DT" VARCHAR   (8)  ,"XV2NM" VARCHAR   (12)  ,
"XV3DT" VARCHAR   (8)  ,"XV3NM" VARCHAR   (12)  ,"XV4DT" VARCHAR   (8)  ,
"XV4NM" VARCHAR   (12)  ,"XV5DT" VARCHAR   (8)  ,"XV5NM" VARCHAR   (12)  ,
"XV6DT" VARCHAR   (8)  ,"XV6NM" VARCHAR   (12)  ,"AIMMO" VARCHAR   (1)  ,
"OBJNR" VARCHAR   (22)  ,"LEART" VARCHAR   (2)  ,"LVORS" VARCHAR   (1)  ,
"GDLGRP" VARCHAR   (8)  ,"POSNR" VARCHAR   (8)  ,"XERWRT" VARCHAR   (1)  ,
"XAFABCH" VARCHAR   (1)  
, PRIMARY KEY ("MANDT","BUKRS","ANLN1","ANLN2")) !
CREATE TABLE ANEP (
"MANDT" VARCHAR   (3)  ,"BUKRS" VARCHAR   (4)  ,"ANLN1" VARCHAR   (12)  ,
"ANLN2" VARCHAR   (4)  ,"GJAHR" VARCHAR   (4)  ,"LNRAN" VARCHAR   (5)  ,
"AFABE" VARCHAR   (2)  ,"ZUJHR" VARCHAR   (4)  ,"ZUCOD" VARCHAR   (4)  ,
"PERAF" VARCHAR   (3)  ,"BELNR" VARCHAR   (10)  ,"BUZEI" VARCHAR   (3)  ,
"BZDAT" VARCHAR   (8)  ,"BWASL" VARCHAR   (3)  ,"XAFAR" VARCHAR   (1)  ,
"ANBTR" FIXED     (13,2) ,"NAFAB" FIXED     (13,2) ,"SAFAB" FIXED     (13,2) ,
"ZINSB" FIXED     (13,2) ,"XANTW" VARCHAR   (1)  ,"XAWBT" VARCHAR   (1)  ,
"LNSAN" VARCHAR   (5)  ,"ANUPD" VARCHAR   (1)  ,"AUGLN" VARCHAR   (5)  
, PRIMARY KEY ("MANDT","BUKRS","ANLN1","ANLN2","GJAHR",
"LNRAN","AFABE","ZUJHR","ZUCOD"
)) !
CREATE TABLE ANLZ (
"MANDT" VARCHAR   (3)  ,"BUKRS" VARCHAR   (4)  ,"ANLN1" VARCHAR   (12)  ,
"ANLN2" VARCHAR   (4)  ,"BDATU" VARCHAR   (8)  ,"ADATU" VARCHAR   (8)  ,
"KOSTL" VARCHAR   (10)  ,"WERKS" VARCHAR   (4)  ,"GSBER" VARCHAR   (4)  ,
"LSTAR" VARCHAR   (6)  ,"MSFAK" FIXED     (3,2) ,"XSTIL" VARCHAR   (1)  ,
"STORT" VARCHAR   (10)  ,"CAUFN" VARCHAR   (12)  ,"PLAN1" VARCHAR   (12)  ,
"PLAN2" VARCHAR   (4)  ,"RAUMN" VARCHAR   (8)  ,"IAUFN" VARCHAR   (12)  ,
"IPROJ" VARCHAR   (16)  ,"TPLKZ" VARCHAR   (3)  ,"TPLNR" VARCHAR   (30)  ,
"ANUPD" VARCHAR   (1)  ,"TXJCD" VARCHAR   (15)  ,"IPSNR" VARCHAR   (8)  ,
"KFZKZ" VARCHAR   (15)  
, PRIMARY KEY ("MANDT","BUKRS","ANLN1","ANLN2","BDATU")) !

SET FORMAT INTERNAL !
CREATE VIEW "V_ANLAB" ("MANDT","BUKRS","ANLN1","ANLN2","GJAHR","LNRAN",
"AFABE","ANEPZUJHR","ZUCOD","BDATU","ANLKL","GEGST","ANLAR","ERNAM", 
"ERDAT","AENAM","AEDAT","XLOEV","XSPEB","FELEI","KTOGR","XOPVW","ANLTP",
"ZUJHR","ZUPER","ZUGDT","AKTIV","ABGDT",
"DEAKT","GPLAB","BSTDT", "ORD41","ORD42","ORD43","ORD44","ANLUE","ZUAWA",
"ANEQK","ANEQS","LIFNR","LAND1","LIEFE","HERST","EIGKZ","AIBN1",
"AIBN2","AIBDT","URJHR", "URWRT","ANTEI","PROJN","EAUFN","MEINS","MENGE",
"TYPBZ","IZWEK","INKEN","IVDAT","INVZU",
"VMGLI","XVRMW","WRTMA","EHWRT","AUFLA","EHWZU", "EHWNR","GRUVO","GREIN",
"GRBND","GRBLT","GRLFD","FLURK","FLURN","FIAMT","STADT","GRUND",
"FEINS","GRUFL","INVNR","VBUND","SPRAS","TXT50", "TXA50","XLTXID","XVERID",
"XTCHID","XKALID","XHERID","XLEAID",
"LEAFI","LVDAT","LKDAT","LEABG","LEJAR","LEPER","LRYTH","LEGEB","LBASW", 
"LKAUF","LMZIN","LZINS","LTZBW","LKUZA","LKUZI","LLAVB","LEANZ",
"LVTNR","LETXT","XAKTIV","ANUPD","LBLNR","XV0DT","XV0NM","XV1DT","XV1NM", 
"XV2DT","XV2NM","XV3DT","XV3NM","XV4DT",
"XV4NM","XV5DT","XV5NM","XV6DT","XV6NM","AIMMO","OBJNR","LEART","LVORS",
"GDLGRP","POSNR","XERWRT", "XAFABCH","ADATU","KOSTL","WERKS","GSBER",
"LSTAR","MSFAK","XSTIL","STORT","CAUFN","PLAN1","PLAN2","RAUMN","IAUFN",
"IPROJ","TPLKZ","TPLNR", "TXJCD",
"IPSNR") AS SELECT T0001.MANDT, T0001.BUKRS, T0001.ANLN1, T0001.ANLN2, 
T0001.GJAHR, T0001.LNRAN, T0001.AFABE, T0001.ZUJHR, T0001.ZUCOD, 
T0003.BDATU, T0002.ANLKL, T0002.GEGST, T0002.ANLAR, T0002.ERNAM, 
T0002.ERDAT, T0002.AENAM, T0002.AEDAT, T0002.XLOEV,
T0002.XSPEB, T0002.FELEI, T0002.KTOGR, T0002.XOPVW, T0002.ANLTP, 
T0002.ZUJHR, T0002.ZUPER, T0002.ZUGDT, T0002.AKTIV, T0002.ABGDT, 
T0002.DEAKT, T0002.GPLAB, T0002.BSTDT, T0002.ORD41, T0002.ORD42, 
T0002.ORD43, T0002.ORD44, T0002.ANLUE, T0002.ZUAWA, T0002.ANEQK, 
T0002.ANEQS, T0002.LIFNR, T0002.LAND1, T0002.LIEFE, T0002.HERST, 
T0002.EIGKZ, T0002.AIBN1, T0002.AIBN2, T0002.AIBDT, T0002.URJHR, 
T0002.URWRT, T0002.ANTEI, T0002.PROJN, T0002.EAUFN, T0002.MEINS, 
T0002.MENGE, T0002.TYPBZ, T0002.IZWEK, T0002.INKEN,
T0002.IVDAT, T0002.INVZU, T0002.VMGLI, T0002.XVRMW, T0002.WRTMA, 
T0002.EHWRT, T0002.AUFLA, T0002.EHWZU, T0002.EHWNR, T0002.GRUVO, 
T0002.GREIN, T0002.GRBND, T0002.GRBLT, T0002.GRLFD, T0002.FLURK, 
T0002.FLURN, T0002.FIAMT, T0002.STADT, T0002.GRUND, T0002.FEINS, 
T0002.GRUFL, T0002.INVNR, T0002.VBUND, T0002.SPRAS, T0002.TXT50, 
T0002.TXA50, T0002.XLTXID, T0002.XVERID, T0002.XTCHID, T0002.XKALID, 
T0002.XHERID, T0002.XLEAID, T0002.LEAFI, T0002.LVDAT, T0002.LKDAT, 
T0002.LEABG, T0002.LEJAR, T0002.LEPER, T0002.
LRYTH, T0002.LEGEB, T0002.LBASW, T0002.LKAUF, T0002.LMZIN, T0002.LZINS, 
T0002.LTZBW, T0002.LKUZA, T0002.LKUZI, T0002.LLAVB, T0002.LEANZ, 
T0002.LVTNR, T0002.LETXT, T0002.XAKTIV, T0002.ANUPD, T0002.LBLNR, 
T0002.XV0DT, T0002.XV0NM, T0002.XV1DT, T0002.XV1NM,
 T0002.XV2DT, T0002.XV2NM, T0002.XV3DT, T0002.XV3NM, T0002.XV4DT, 
T0002.XV4NM, T0002.XV5DT, T0002.XV5NM, T0002.XV6DT, T0002.XV6NM, 
T0002.AIMMO, T0002.OBJNR, T0002.LEART, T0002.LVORS, T0002.GDLGRP, 
T0002.POSNR, T0002.XERWRT, T0002.XAFABCH, T0003.ADATU,
T0003.KOSTL, T0003.WERKS, T0003.GSBER, T0003.LSTAR, T0003.MSFAK, 
T0003.XSTIL, T0003.STORT, T0003.CAUFN, T0003.PLAN1, T0003.PLAN2, 
T0003.RAUMN, T0003.IAUFN, T0003.IPROJ, T0003.TPLKZ, T0003.TPLNR, 
T0003.TXJCD, T0003.IPSNR FROM ANEP T0001, ANLA T0002, ANLZ T0003 
WHERE T0002.MANDT = T0001.MANDT AND T0002.BUKRS = T0001.BUKRS 
AND T0002.ANLN1 = T0001.ANLN1 AND T0002.ANLN2 = T0001.ANLN2 AND 
T0002.MANDT = T0003.MANDT AND T0002.BUKRS = T0003.BUKRS AND 
T0002.ANLN1 = T0003.ANLN1 AND T0002.ANLN2 = T0003.ANLN2 !

insert ANLA set
     MANDT  = '001'          , 
     BUKRS  = 'D001'         ,
     ANLN1  = '000000940268' , 
     ANLN2  = 'xxxx' , 
     zujhr = '0000'          ,
     "XLOEV" = ' '  !

insert ANEP set
     MANDT  = '001'          , 
     BUKRS  = 'D001'         ,
     ANLN1  = '000000940268' , 
     ANLN2  = 'xxxx' , 
     gjahr = '1995' , 
     lnran = '54321' ,
     AFABE = '01'   ,
     ZUJHR = '0000' , 
     "ZUCOD"     = '0000' !


insert ANLZ set
     MANDT  = '001'          , 
     BUKRS  = 'D001'         ,
     ANLN1  = '000000940268' , 
     ANLN2  = 'xxxx' , 
     BDATU = '19951231' , 
     ADATU = '19951231' !
commit release!
connect kern identified by test sqlmode sapr3 isolation level  0 !
parsing!
SELECT  "MANDT" ,  "BUKRS" ,  "ANLN1" ,  "ANLN2" ,  "GJAHR" ,  "LNRAN" , 
 "AFABE" ,  "ANEPZUJHR" ,  "ZUCOD" ,  "BDATU" ,  "ANLKL" ,  "GEGST" ,
 "ANLAR" , "ERNAM" ,  "ERDAT" ,  "AENAM" ,  "AEDAT" ,  "XLOEV" ,  "XSPEB" ,
 "FELEI" ,  "KTOGR" ,  "XOPVW" ,  "ANLTP" ,  "ZUJHR" ,  "ZUPER" ,  "ZUGDT" ,
 "AKTIV" ,  "ABGDT" ,  "DEAKT" ,  "GPLAB" ,  "BSTDT" ,  "ORD41" ,  "ORD42" ,
 "ORD43" ,  "ORD44" ,  "ANLUE" ,  "ZUAWA" ,  "ANEQK" ,  "ANEQS" ,  "LIFNR" ,
 "LAND1" ,  "LIEFE" ,  "HERST" ,  "EIGKZ" ,  "AIBN1" ,  "AIBN2" ,  "AIBDT" ,
 "URJHR" ,  "URWRT" ,  "ANTEI" ,  "PROJN" ,  "EAUFN" ,  "MEINS" ,  "MENGE" ,
 "TYPBZ" ,  "IZWEK" ,  "INKEN" ,  "IVDAT" ,  "INVZU" ,  "VMGLI" ,  "XVRMW" ,
 "WRTMA" ,  "EHWRT" ,  "AUFLA" ,  "EHWZU" ,  "EHWNR" ,  "GRUVO" ,  "GREIN" ,
 "GRBND" ,  "GRBLT" ,  "GRLFD" ,  "FLURK" ,  "FLURN" ,  "FIAMT" ,  "STADT" ,
 "GRUND" ,  "FEINS" ,  "GRUFL" ,  "INVNR" ,  "VBUND" ,  "SPRAS" ,  "TXT50" ,
 "TXA50" ,  "XLTXID" ,  "XVERID" ,  "XTCHID" ,  "XKALID" ,  "XHERID" ,
 "XLEAID" ,  "LEAFI" ,  "LVDAT" ,  "LKDAT" ,  "LEABG" ,  "LEJAR" ,  "LEPER" ,
 "LRYTH" ,  "LEGEB" ,  "LBASW" ,  "LKAUF" ,  "LMZIN" ,  "LZINS" ,  "LTZBW" ,
 "LKUZA" ,  "LKUZI" ,  "LLAVB" ,  "LEANZ" ,  "LVTNR" ,  "LETXT" ,  "XAKTIV" ,
 "ANUPD" ,  "LBLNR" ,  "XV0DT" ,  "XV0NM" ,  "XV1DT" ,  "XV1NM" ,  "XV2DT" ,
 "XV2NM" ,  "XV3DT" ,  "XV3NM" ,  "XV4DT" ,  "XV4NM" ,  "XV5DT" ,  "XV5NM" ,
 "XV6DT" ,  "XV6NM" ,  "AIMMO" ,  "OBJNR" ,  "LEART" ,  "LVORS" ,  "GDLGRP" ,
 "POSNR" ,  "XERWRT" ,  "XAFABCH" ,  "ADATU" ,  "KOSTL" ,  "WERKS" ,
 "GSBER" ,  "LSTAR" ,  "MSFAK" ,  "XSTIL" ,  "STORT" ,  "CAUFN" ,  "PLAN1" ,
 "PLAN2" ,  "RAUMN" ,  "IAUFN" ,  "IPROJ" ,  "TPLKZ" ,  "TPLNR" ,  "TXJCD" ,
 "IPSNR"  FROM  "V_ANLAB"
 WHERE  
MANDT  = :a001 and BUKRS = :bD001 and
ANLN1 = :c000000940268 and gjahr = :d1995 and AFABE = :e01 and
"ANEPZUJHR"  = :f0000 and "ZUCOD"  = :g0000 and ( "XLOEV"  between
:h and :i) and BDATU >= :j19951231 and ADATU <= :k19951231 and
zujhr >= :l0000
ORDER BY  "MANDT" ,  "BUKRS" ,  "ANLN1" ,  "ANLN2" !

data!
executing!
SELECT !
1
c 3 '001'
c 4 'D001'
c 12 '000000940268'
c 4 '1995'
c 2 '01'
c 4 '0000'
c 4 '0000'
c 1 ' '
c 1 ' '
c 8 '19951231'
c 8 '19951231'
c 4 '0000'

nodata!
pars_then_ex!
fetch into :i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,
:i,:i,:i,:i,:i,:i,:i,:i,:i,:i,:i!

pars_execute!

drop TABLE ANLA !
drop TABLE ANEP !
drop TABLE ANLZ !
rollback work release!
file connect ( kern test sqlmode sapr3 !
create table t ( a char (4), b char (4))!
insert into t values ('aa', 'bb')!
create table t1 ( a1 char (3))!
insert into t1 values ('ccc')!
pars_then_ex !
data !
select
b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,
b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,
b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,
b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,
b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,
b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,
b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,
b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,
b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,
b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,
b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,
b,b,b,b,b,b,b,b,b,b
from t, t1
where a in (
:a,:a,:a,:a,:a,:a,:a,:a,:a,:a,
:a,:a,:a,:a,:a,:a,:a,:a,:a,:a
) !
c 4 'aa' c 4 'aa' c 4 'aa' c 4 'aa' c 4 'aa'
c 4 'aa' c 4 'aa' c 4 'aa' c 4 'aa' c 4 'aa'
c 4 'aa' c 4 'aa' c 4 'aa' c 4 'aa' c 4 'aa'
c 4 'aa' c 4 'aa' c 4 'aa' c 4 'aa' c 4 'aa'

nodata!
fetch into 
:a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:l,:m,:n,:o,:p,:q,:r,:s,:t,
:a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:l,:m,:n,:o,:p,:q,:r,:s,:t,
:a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:l,:m,:n,:o,:p,:q,:r,:s,:t,
:a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:l,:m,:n,:o,:p,:q,:r,:s,:t,
:a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:l,:m,:n,:o,:p,:q,:r,:s,:t,
:a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:l,:m,:n,:o,:p,:q,:r,:s,:t,
:a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:l,:m,:n,:o,:p,:q,:r,:s,:t,
:a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:l,:m,:n,:o,:p,:q,:r,:s,:t,
:a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:l,:m,:n,:o,:p,:q,:r,:s,:t,
:a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:l,:m,:n,:o,:p,:q,:r,:s,:t,
:a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:l,:m,:n,:o,:p,:q,:r,:s,:t,
:a,:b,:c,:d,:e,:f,:g,:h,:i,:j !
pars_execute!
drop table t !
drop table t1 !
commit release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* SELDEFLT.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 (s1 char(20) default 'Default s1', 
                 s2 char(20) default 'Default s2',
                 s3 char(20) default 'Default s3',
                 s4 char(20)) !
insert t1 set s1 = 'aaaaa' !
insert t1 set s2 = 'bbbbb' !
insert t1 set s3 = 'ccccc' !
insert t1 set s1 = 'aaaaa',
              s2 = 'bbbbb',
              s3 = 'ccccc' !
create table t2 (s21 char(20) default 'Default s21', 
                 s22 char(20) default 'Default s22',
                 s23 char(20) default 'Default s23',
                 s24 char(20)) !
insert t2 set s21 = 'aaaaa' !
insert t2 set s22 = 'bbbbb' !
insert t2 set s23 = 'ccccc' !
insert t2 set s21 = 'aaaaa',
              s22 = 'bbbbb',
              s23 = 'ccccc' !
commit work !
select * from t1,t2 where s1 = 'aaaaa' AND DEFAULT = s2 AND s3 = DEFAULT !
fetch !
select * from t2,t1 where s1 = 'aaaaa' AND DEFAULT = s2 AND s3 = DEFAULT !
fetch !
select * from t1,t2 where s1 = 'aaaaa' AND s2 = DEFAULT AND s23 = DEFAULT !
fetch !
select * from t1,t2 where s1 = 'aaaaa' AND s2 = DEFAULT AND s3 = DEFAULT !
fetch !
select * from t2,t1 where s1 = 'aaaaa' AND s2 = DEFAULT AND s3 = DEFAULT !
fetch !
select * from t1,t2 where s1 = 'aaaaa' AND s2 = DEFAULT AND s23 = DEFAULT !
fetch !
rollback work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* SELFET1.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
FILE CONNECT ( kern test !
CREATE TABLE T1 ( A CHAR (4) KEY, B CHAR (11) KEY,
   C CHAR (1978), D CHAR (5) NOT NULL, E CHAR (7),
                  F FIXED (6))!
create table t_ora (a char (80) primary key, d timestamp, b char (40))!
insert t_ora values ('char_ora  1', '19941218101345000000', 'bbb')!
insert t_ora values ('char_ora  2', '19980219231134123456', 'bbb')!
insert t_ora values ('char_ora  3', '19651023090807654321', 'bbb')!
commit release !
file connect ( kern test sqlmode oracle !
pars_execute !
parsing !
parse 
    select c,a,b from t1, dual where e = :e and  b = :b !
executing !
data !
parse declare erg cursor for
    select c,a,b from t1, dual where e = :e and  b = :b !
1
c 7 'MANNN' c 11 'ABCDEFG'

pars_then_ex !
nodata !
fetch erg into :a, :b, :c !
nodata !
pars_execute !
insert into t1 values ('eins', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('drei', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('zwei', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('fil1', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('fil2', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('fil3', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('fil4', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('fil5', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('fil6', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('fil7', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('fil8', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('fil9', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('filA', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('filB', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('filC', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('filD', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('filE', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
insert into t1 values ('filF', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4)!
pars_then_ex !
data !
parse declare erg cursor for
      select c,a,b from t1, dual where e = :e and  b = :b !
c 7 'e char7' c 11 'das ist b'

nodata !
fetch erg into :c, :a, :b !
close erg !
nodata !
pars_execute!
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* SELFET2.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
FILE CONNECT ( kern test !
CREATE TABLE T1 ( A CHAR (4) KEY, B CHAR (11) KEY,
   C CHAR (3850), D CHAR (5) NOT NULL, E CHAR (7),
                  F FIXED (6), G LONG ASCII)!
commit release !
file connect ( kern test sqlmode oracle !
parsing !
parse 
    select c,a,b,g from t1, dual where e = :e and  b = :b !
executing !
data !
parse declare erg cursor for
    select c,a,b,g from t1, dual where e = :e and  b = :b !
1
c 7 'MANNN' c 11 'ABCDEFG'

pars_then_ex !
nodata !
fetch erg into :a, :b, :g, :c !
nodata !
pars_execute !
insert into t1 values ('eins', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('drei', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('zwei', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
pars_then_ex !
data !
update t1 set g = :g where a = :a and b = :b !
l 500 'Das ist der Inhalt der LONG-Spalte. eins !'
c 4 'eins' c 11 'das ist b'

update t1 set g = :g where a = :a and b = :b !
l 500 'Das ist der Inhalt der LONG-Spalte. drei'
c 4 'drei' c 11 'das ist b'

update t1 set g = :g where a = :a and b = :b !
l 500 'Das ist der Inhalt der LONG-Spalte. zwei'
c 4 'zwei' c 11 'das ist b'

parse declare erg cursor for
      select c,a,b,g from t1, dual where e = :e and  b = :b !
c 7 'e char7' c 11 'das ist b'

nodata !
fetch erg into :c, :a, :b, :g !
close erg !
pars_execute !
insert into t1 values ('fil1', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('fil2', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('fil3', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('fil4', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('fil5', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('fil6', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('fil7', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('fil8', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('fil9', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('filA', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('filB', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('filC', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('filD', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('filE', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
insert into t1 values ('filF', 'das ist b', 'c ist in ascii ',
'd c_5', 'e char7', 4, null)!
pars_then_ex !
data !
update t1 set g = :g where a = :a and b = :b !
l 500 'Das ist der Inhalt der LONG-Spalte. fil1'
c 4 'fil7' c 11 'das ist b'

update t1 set g = :g where a = :a and b = :b !
l 500 'Das ist der Inhalt der LONG-Spalte. fil4'
c 4 'filA' c 11 'das ist b'

update t1 set g = :g where a = :a and b = :b !
l 500 'Das ist der Inhalt der LONG-Spalte. fil7'
c 4 'filD' c 11 'das ist b'

update t1 set g = :g where a = :a and b = :b !
l 500 'Das ist der Inhalt der LONG-Spalte. filB'
c 4 'fil2' c 11 'das ist b'

pars_then_ex !
data !
parse declare erg cursor for
      select c,a,b,g from t1, dual where e = :e and  b = :b !
c 7 'e char7' c 11 'das ist b'

nodata !
fetch erg into :c, :a, :b, :g !
close erg !
pars_execute!
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* SETFUNC.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
CREATE TABLE T1 ( A CHAR (4),  B FIXED (3))!
INSERT T1 VALUES ('aaa', 1)!
INSERT T1 VALUES ('bbb', 1)!
SELECT A, AVG(B) FROM T1 GROUP BY A !
MFETCH !
3
CREATE TABLE T2 (A2 FIXED (5))!
INSERT T2 SET A2 = 45!
SELECT AVG(B) FROM T1, T2 !
MFETCH !
3
SELECT A, A2, AVG(B) FROM T1, T2 GROUP BY A, A2 ORDER BY A2 !
MFETCH !
3
SELECT A, A2, AVG(B) FROM T1, T2 GROUP BY A, A2 HAVING A < 'z'!
MFETCH !
3
SELECT A, A2, AVG(B) FROM T1, T2 GROUP BY A, A2 HAVING AVG(B) < 1000!
MFETCH !
3
ROLLBACK !
create table t ( a char (4), b fixed (3) )!
create table t1 (a1 char (4) )!
insert t set a = 'aaa', b = 4 !
insert t set a = 'ccc', b = 4 !
insert t set a = 'a11', b = 4 !
insert t1 set a1 = 'a11'!
insert t1 set a1 = 'yy'!
pars_then_ex !
data !
select a, a1, b*:a, count (a) from t, t1
group by a, b, a1!
n 38 3

nodata !
fetch into :a, :b, :c, :d !
data !
select b*:a, count (a) from t, t1
group by b!
n 38 3

nodata !
fetch into :a, :b !
data !
select a, a1, b*:a, count (a) from t, t1
where a <> a1
group by a, b, a1 !
n 38 3

nodata !
fetch into :a, :b, :c, :d !
data !
select a, a1, b*:a, count (a) from t, t1
                  where a <> a1 and a < 'fff' and b div 5 < 10
group by a !
nodata !
fetch into :a, :b, :c, :d !
data !
select a, b*:a, count (a) from t, t1
                  where a <> a1 and a < 'fff' and b div 5 < 10
group by a , b!
n 38 3

nodata !
mfetch into :a, :b, :c !
5
data !
select a, b*:a, count (a) from t, t1
                  where a <> a1 and a < 'fff' and b div 5 < 10
group by a, b
having count (*) > 1!
n 38 3

nodata !
fetch into :a, :b, :c!
data !
select a, b*:a, count (a) from t, t1
                  where a <> a1 and a < 'fff' and b div 5 < 10
group by a, b
having count (*) > 1 order by 3!
n 38 3

nodata !
fetch into :a, :b, :c !
data !
select a, a1, b*:a, count (a) from t, t1
                  where a <> a1 and a < 'fff' and b div 5 < 10
group by a, b, a1
having count (*) > 0 order by a1!
n 38 3

nodata !
fetch into :a, :b, :c, :d !
pars_execute!
rollback !
create table t ( a char (4), b fixed (3) )!
create table t1 (a1 char (4), b1 float (4))!
insert t set a = 'aaa', b = 4 !
insert t set a = 'ccc', b = 4 !
insert t set a = 'a11', b = 4 !
insert t1 set a1 = 'a11', b1 = 7!
insert t1 set a1 = 'yy', b1 = 12!
pars_then_ex !
data !
select a & a1, b*b1+:a from t, t1 !
n 38 3

nodata !
mfetch into :a,:b !
8
select length(a & a1), index (a & a1, 'cy'), count(*) from t, t1!
fetch into :a, :b, :c !
select length(a & a1), index (a & a1, 'cy'), count(*) from t, t1
group by a, a1!
fetch into :a, :b , :c !
select index ('aaa' & a,a1), 'aaa' & a, a1,
       index (a & a1, 'cy'), count(*) from t, t1
group by a, a1!
fetch into :a,:b,:c,:d,:e,:f !
pars_execute !
select b*b1+b1, count (a & a1) from t, t1 group by b,a,a1,b1
having b < 10 !
fetch !
select a, a1, min(b1) from t, t1 where a & a1 > 'ggg' group by a, a1!
fetch!
select a, a1, min(b1) from t, t1 group by a, a1 having a & a1 > 'ggg'!
fetch!
rollback !
commit release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* SPACE.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table cr_t ( cr char (1) ascii)!
pars_then_ex !
data !
insert cr_t values (:a)!
b 1 0a

pars_execute !
nodata !
create table t ( cfk char (8), cvk char (8), 
                 cfc char (8), cvc char (40), 
                 id fixed(3), primary key (cfk, cvk, id))!
insert t values ('', '', '', '', 0)!
insert t values (' ', ' ', ' ', ' ', 1)!
insert t values ('  ', '  ', '  ', '  ', 2)!
insert t values ('a', 'a', 'a', 'a', 10)!
insert t values ('b ', 'b ', 'b ', 'b ', 21)!
commit release !
file connect ( kern test !
SELECT id, '''' || GREATEST  ( cfk, cr ) || '''' e from t, cr_t !
  fetch into :id, :a !
SELECT id, '''' || LEAST     ( cfk, cr ) || '''' e from t, cr_t !
  fetch into :id, :a !
commit release !

file connect ( kern test space option !
SELECT id, '''' || GREATEST  ( cfk, cr ) || '''' e from t, cr_t !
  fetch into :id, :a !
SELECT id, '''' || LEAST     ( cfk, cr ) || '''' e from t, cr_t !
  fetch into :id, :a !
commit release !

file connect ( kern test !
insert t values ('null','null', null, null, 100)!
commit release ! 
file connect ( kern test !
SELECT id, '''' || GREATEST  ( cvc, cr ) || '''' e from t, cr_t !
  fetch into :id, :a !
SELECT id, '''' || LEAST     ( cvc, cr ) || '''' e from t, cr_t !
  fetch into :id, :a !
commit release !
file connect ( kern test space option !
SELECT id, '''' || GREATEST  ( cvc, cr ) || '''' e from t, cr_t !
  fetch into :id, :a !
SELECT id, '''' || LEAST     ( cvc, cr ) || '''' e from t, cr_t !
  fetch into :id, :a !
commit release !
file connect ( kern test !
drop table cr_t !
drop table t !
commit release !

file connect ( kern test !
create table j ( j1 char (1))!
insert j values (' ')!
CREATE TABLE T ( K1 INT,
                 I1 CHAR (5) UNICODE)!
   IF &rc = -3002 THEN 
   BEGIN
*
   CREATE TABLE T ( K1 INT,
                 I1 CHAR (5) ASCII)!
*
   END

CREATE INDEX "t.i1" ON T( I1 ) !
COMMIT! 

explain 
  select /*+ORDERED,JOIN(;INDEXACCESS)*/ hex(i1) from j, t where j1=i1 !
select /*+ORDERED,JOIN(;INDEXACCESS)*/ hex(i1) from j, t where j1=i1 !
fetch!
explain 
  select /*+ORDERED,JOIN(;NOACCESSPATH)*/ hex(i1) from j, t where j1=i1 !
select /*+ORDERED,JOIN(;NOACCESSPATH)*/ hex(i1) from j, t where j1=i1 !
fetch!
explain 
  select /*+ORDERED*/ hex(i1) from t, j where j1=i1 !
select /*+ORDERED*/ hex(i1) from t, j where j1=i1 !
fetch!

delete t where k1 in (0,3)!
drop index "t.i1" ! 
diagnose share parse drop !
alter table t add primary key (i1)!

explain 
  select /*+ORDERED,JOIN(;KEYACCESS)*/ hex(i1) from j, t where j1=i1 !
select /*+ORDERED,JOIN(;KEYACCESS)*/ hex(i1) from j, t where j1=i1 !
fetch!

drop table t !
drop table j !
diagnose share parse drop !
commit work release !

file connect ( kern test space option !
create table j ( j1 char (1))!
insert j values (' ')!
CREATE TABLE T ( K1 INT,
                 I1 CHAR (5) UNICODE)!
   IF &rc = -3002 THEN 
   BEGIN
*
   CREATE TABLE T ( K1 INT,
                 I1 CHAR (5) ASCII)!
*
   END

CREATE INDEX "t.i1" ON T( I1 ) !
COMMIT! 

explain 
  select /*+ORDERED,JOIN(;INDEXACCESS)*/ hex(i1) from j, t where j1=i1 !
select /*+ORDERED,JOIN(;INDEXACCESS)*/ hex(i1) from j, t where j1=i1 !
fetch!
explain 
  select /*+ORDERED,JOIN(;NOACCESSPATH)*/ hex(i1) from j, t where j1=i1 !
select /*+ORDERED,JOIN(;NOACCESSPATH)*/ hex(i1) from j, t where j1=i1 !
fetch!
explain 
  select /*+ORDERED*/ hex(i1) from t, j where j1=i1 !
select /*+ORDERED*/ hex(i1) from t, j where j1=i1 !
fetch!

delete t where k1 in (0,3)!
drop index "t.i1" ! 
diagnose share parse drop !
alter table t add primary key (i1)!

explain 
  select /*+ORDERED,JOIN(;KEYACCESS)*/ hex(i1) from j, t where j1=i1 !
select /*+ORDERED,JOIN(;KEYACCESS)*/ hex(i1) from j, t where j1=i1 !
fetch!

drop table t !
drop table j !
diagnose share parse drop !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* SQL.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table s ( sno char(5) key,
                 sname char(20),
                 status decimal(3),
                 city char(15) ) !
create table p ( pno char(6) key,
                 pname char(20),
                 color char(6),
                 weight decimal(3),
                 city char(15) ) !
create table j ( jno char(4) key,
                 jname char(10),
                 city char(15) ) !
create table spj ( sno char(5) key,
                   pno char(6) key,
                   jno char(4) key,
                   qty decimal(5)) !
commit work !
insert s values ( 'S1' , 'Smith' , 20 , 'London' ) !
insert s values ( 'S2' , 'Jones' , 10 , 'Paris'  ) !
insert s values ( 'S3' , 'Blake' , 30 , 'Paris'  ) !
insert s values ( 'S4' , 'Clark' , 20 , 'London' ) !
insert s values ( 'S5' , 'Adams' , 30 , 'Athens' ) !
commit work !
insert p values ( 'P1' , 'Nut'   , 'Red'   , 12 , 'London' ) !
insert p values ( 'P2' , 'Bolt'  , 'Green' , 17 , 'Paris'  ) !
insert p values ( 'P3' , 'Screw' , 'Blue'  , 17 , 'Rome'   ) !
insert p values ( 'P4' , 'Screw' , 'Red'   , 14 , 'London' ) !
insert p values ( 'P5' , 'Cam'   , 'Blue'  , 12 , 'Paris'  ) !
insert p values ( 'P6' , 'Cog'   , 'Red'   , 19 , 'London' ) !
commit work !
insert j values ( 'J1' , 'Sorter'   , 'Paris'  ) !
insert j values ( 'J2' , 'Punch'    , 'Rome'   ) !
insert j values ( 'J3' , 'Reader'   , 'Athens' ) !
insert j values ( 'J4' , 'Console'  , 'Athens' ) !
insert j values ( 'J5' , 'Collator' , 'London' ) !
insert j values ( 'J6' , 'Terminal' , 'Oslo'   ) !
insert j values ( 'J7' , 'Tape'     , 'London' ) !
commit work !
insert spj values ( 'S1' , 'P1' , 'J1' , 200 ) !
insert spj values ( 'S1' , 'P1' , 'J4' , 700 ) !
insert spj values ( 'S2' , 'P3' , 'J1' , 400 ) !
insert spj values ( 'S2' , 'P3' , 'J2' , 200 ) !
insert spj values ( 'S2' , 'P3' , 'J3' , 200 ) !
insert spj values ( 'S2' , 'P3' , 'J4' , 500 ) !
insert spj values ( 'S2' , 'P3' , 'J5' , 600 ) !
insert spj values ( 'S2' , 'P3' , 'J6' , 400 ) !
insert spj values ( 'S2' , 'P3' , 'J7' , 800 ) !
insert spj values ( 'S2' , 'P5' , 'J2' , 100 ) !
insert spj values ( 'S3' , 'P3' , 'J1' , 200 ) !
insert spj values ( 'S3' , 'P4' , 'J2' , 500 ) !
insert spj values ( 'S4' , 'P6' , 'J3' , 300 ) !
insert spj values ( 'S4' , 'P6' , 'J7' , 300 ) !
insert spj values ( 'S5' , 'P2' , 'J2' , 200 ) !
insert spj values ( 'S5' , 'P2' , 'J4' , 100 ) !
insert spj values ( 'S5' , 'P5' , 'J5' , 500 ) !
insert spj values ( 'S5' , 'P5' , 'J7' , 100 ) !
insert spj values ( 'S5' , 'P6' , 'J2' , 200 ) !
insert spj values ( 'S5' , 'P1' , 'J4' , 100 ) !
insert spj values ( 'S5' , 'P3' , 'J4' , 200 ) !
insert spj values ( 'S5' , 'P4' , 'J4' , 800 ) !
insert spj values ( 'S5' , 'P5' , 'J4' , 400 ) !
insert spj values ( 'S5' , 'P6' , 'J4' , 500 ) !
commit work !
create view london_projects ( jno, jname, city )
   as select j.jno, j.jname, j.city
      from j
      where j.city = 'London' !
create view sp ( sno, pno, qty )
   as select spj.sno, spj.pno, sum ( spj.qty )
      from spj
      group by spj.sno, spj.pno !
create view jc ( jno, city )
   as select distinct j.jno, j.city
      from j, spj
      where j.jno = spj.jno
      and ( spj.sno = 'S1' or
            spj.pno = 'P1' ) !
select * from jc !
fetch !
create view non_colocated ( sno, pno )
   as select s.sno, p.pno
      from s, p
      where s.city <> p.city !
select * from non_colocated !
fetch !
commit work !
select s.sno, p.pno, j.jno
   from s, p, j
   where s.city = p.city
   and   p.city = j.city
   order by s.sno, j.jno, p.pno !
fetch !
select s.sno, p.pno, j.jno
   from s, p, j
   where not ( s.city = p.city and p.city = j.city ) !
fetch !
select s.sno, p.pno, j.jno
   from s, p, j
   where s.city <> p.city
    and  p.city <> j.city
    and  j.city <> s.city !
fetch !
select distinct spj.pno
   from spj, s
   where spj.sno = s.sno
   and  s.city = 'London' !
fetch !
select distinct spj.pno
   from spj, s, j
   where spj.sno = s.sno
   and   spj.jno = j.jno
   and   s.city = 'London'
   and   j.city = 'London' !
fetch !
select distinct s.city, j.city
   from s, spj, j
   where s.sno = spj.sno
   and   spj.jno = j.jno
   order by s.city, j.city !
fetch !
select distinct spj.pno
   from spj, s, j
   where spj.sno = s.sno
   and   spj.jno = j.jno
   and   s.city = j.city !
fetch !
select distinct j.jno
   from spj, s, j
   where spj.sno = s.sno
   and   spj.jno = j.jno
   and   s.city <> j.city
   order by j.jno !
fetch !
select spjx.pno, spjy.pno
   from spj spjx, spj spjy
   where spjx.sno = spjy.sno
   and   spjx.pno > spjy.pno !
fetch !
rollback work !
create table reds ( sno char(5) ) !
commit work !
insert into reds ( sno )
       select distinct spj.sno
       from spj, p
       where spj.pno = p.pno
       and   p.color = 'Red' !
select * from reds !
fetch !
rollback work !
commit work release !

file connect ( kern test !
select max(qty) - min(qty)
   from p px, spj spjx
   where px.pno = spjx.pno
     and spjx.sno =
   any ( select s.sno
         from p, spj, s, j
         where p.pno = spjx.pno
           and s.city = j.city
           and px.city = s.city
         group by s.sno )
   group by qty !
fetch !
select spjx.sno, spjx.pno, spjx.jno, s.sname
  from spj spjx, s, j jx
  where s.status <
    all ( select max(qty) - min(qty)
          from spj spjy, j
          where jx.jno = spjy.jno
            and spjy.qty <
                  all ( select max ( spj.qty)
                        from spj
                        where spj.jno < spjx.jno
                        group by sno )
          group by qty ) !
select spjx.sno, spjx.pno, spjx.jno, s.sname
  from spj spjx, s, j jx
  where s.status <>
    all ( select max(qty) - min(qty)
          from spj spjy, j
          where jx.jno = spjy.jno
            and spjy.qty <
                  all ( select max ( spj.qty)
                        from spj
                        where spj.jno < spjx.jno
                        group by sno )
          group by qty ) !
select distinct max(qty) - min(qty)
   from p px, spj spjx
   where px.pno = spjx.pno
     and spjx.sno =
   any ( select distinct s.sno
         from p, spj, s, j
         where p.pno = spjx.pno
           and s.city = j.city
           and px.city = s.city
         group by s.sno )
   group by qty !
fetch !
select distinct spjx.sno, spjx.pno, spjx.jno, s.sname
  from spj spjx, s, j jx
  where s.status <
    all ( select distinct max(qty) - min(qty)
          from spj spjy, j
          where jx.jno = spjy.jno
            and spjy.qty <
                  all ( select distinct max ( spj.qty)
                        from spj
                        where spj.jno < spjx.jno
                        group by sno )
          group by qty ) !
select spjx.sno, px.city
  from spj spjx, p px
  where spjx.sno in
        ( select s.sno from s
          where px.pno = spjx.pno )
          and spjx.sno = 'S3'
           or spjx.sno = 'S4' !
fetch !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* SQLMTST.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 ( a fixed(5)) !
insert t1 values ( 1) !
insert t1 values ( 2) !
insert t1 values ( 3) !
insert t1 values ( 4) !
insert t1 values ( 4) !
insert t1 values ( 5) !
insert t1 values ( 5) !
create table t2 ( b fixed(7)) !
insert t2 values ( 3) !
insert t2 values ( 3) !
insert t2 values ( 4) !
insert t2 values ( 4) !
insert t2 values ( 5) !
insert t2 values ( 6) !
insert t2 values ( 7) !
commit work release !
file connect (kern test sqlmode internal !
*
declare c cursor for select * from t1, t2 where a (+) = b     !
fetch c into :a, :b !
close c !
*
declare c cursor for select * from t1, t2 where a     = b (+) !
fetch c into :a, :b !
close c !
*
declare c cursor for select * from t1, t2 where a (+) = b (+) !
fetch c into :a, :b !
close c !
*
declare c cursor for select * from (select * from t1), t2 where a = b !
fetch c into :a, :b !
close c !
*
rollback work release !
file connect (kern test sqlmode oracle !
*
declare c cursor for select * from t1, t2 where a (+) = b     !
fetch c into :a, :b !
close c !
*
declare c cursor for select * from t1, t2 where a     = b (+) !
fetch c into :a, :b !
close c !
*
declare c cursor for select * from t1, t2 where a (+) = b (+) !
fetch c into :a, :b !
close c !
*
declare c cursor for select * from (select * from t1), t2 where a = b !
fetch c into :a, :b !
close c !
*
rollback work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* STAU.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file CONNECT ( SUT SUT !
*
CREATE USER USER0 PASSWORD USER0 DBA NOT EXCLUSIVE!
*
COMMIT WORK RELEASE!
*
file CONNECT ( USER0 USER0!
*
* Anlegen der Arbeitstabellen
*
CREATE TABLE TABX
(K FIXED (18),
 I FIXED (18),
 N FIXED (18),
 F4 FIXED (5),
 F5 FIXED (5),
 FILL1 CHAR (250),
 FILL2 CHAR (250),
 FILL3 CHAR (250),
 FILL4 CHAR (250),
 PRIMARY KEY (K))!
*
INSERT INTO TABX VALUES(1 ,1 ,1 ,5001,1 ,'','','','')!
INSERT INTO TABX VALUES(2 ,4 ,5 ,5002,1 ,'','','','')!
INSERT INTO TABX VALUES(3 ,2 ,4 ,5003,2 ,'','','','')!
INSERT INTO TABX VALUES(4 ,5 ,2 ,5004,2 ,'','','','')!
INSERT INTO TABX VALUES(5 ,3 ,3 ,5004,3 ,'','','','')!
UPDATE TABX SET FILL1 = RFILL('a','#',250)!
UPDATE TABX SET FILL2 = RFILL('a','#',250)!
UPDATE TABX SET FILL3 = RFILL('a','#',250)!
UPDATE TABX SET FILL4 = RFILL('a','#',250)!
*
CREATE UNIQUE INDEX "TABX.I"
	on TABX( I ) !
*
COMMIT!
CREATE VIEW VE1 AS SELECT K,I,N,F4,FILL1,FILL2,FILL3,FILL4 FROM TABX
            WHERE (K = 1) OR (N = 1) OR (I = 1) 
               OR (N + K = 2) OR (N + I = 2) OR (I + K = 2)
               OR (N * K = 1) OR (N * I = 1) OR (I * K = 1)
               OR (SQRT(N) = POWER(K,2)) OR (SQRT(N) = POWER(I,2)) 
               OR (SQRT(I) = POWER(K,2))!
CREATE VIEW VW1 AS SELECT K,I,N,F4,FILL1,FILL2,FILL3,FILL4 FROM TABX
            WHERE (K < 6) AND (I < 6) AND (N < 6)
              AND (N + K < 10) AND (N + I < 10) AND (K + I < 10)
              AND (N * K < 10) AND (N * I < 10) AND (K * I < 10)
              AND (SQRT(N) = POWER(K,2)) AND (SQRT(N) = POWER(I,2)) 
              AND (SQRT(I) = POWER(K,2))!
CREATE VIEW VS1 AS SELECT K,I,N,F4,FILL1,FILL2,FILL3,FILL4 FROM TABX
            WHERE I IN ( -1.25, 13E+14, K * 120, 1, N * N, I / (I + 1)) 
              AND K IN ( -2.25, 12E-14, N * 20, 1, I * I, N / (N + 1)) 
              AND N IN ( -3.25, 11E+55, I * 21, 1, K * K, K / (K + 1))! 
CREATE VIEW VX1 AS SELECT T1.K,T1.N FROM VE1 T1,VW1 T2,VS1 T3 
WHERE T1.K = T2.K AND T1.N = T2.N 
  AND T3.N = T2.N AND T3.K = T2.K!
*
CREATE VIEW VY1 AS SELECT N,K FROM VE1
                    UNION ALL
                    SELECT N,K FROM VW1
                    UNION ALL
                    SELECT N,K FROM VS1!
*
CREATE VIEW VZ1 AS SELECT T1.K,T1.N FROM VE1 T1,VW1 T2,VS1 T3 
                    WHERE T1.K = T2.K OR T1.N = T2.N 
                    OR T3.N = T2.N OR T3.K = T2.K
                    OR T2.K = T3.N OR T1.N = T2.K!
*
CREATE VIEW VA1 AS SELECT K,I,N,F4,FILL1,FILL2,FILL3,FILL4 
                    FROM VE1 
                    WHERE K IN 
                          (SELECT N FROM VW1
                           WHERE K IN 
                                (SELECT N FROM VS1
                                 WHERE (K = 2 * N) OR (K = 1)))!
*
CREATE VIEW VB1 AS SELECT K,I,N,F4,FILL1,FILL2,FILL3,FILL4 
                    FROM VE1 
                    WHERE (K IN 
                             (SELECT N FROM VW1
                              WHERE K = 1))
                      AND (N IN    
                             (SELECT K FROM VS1
                              WHERE K = 1))!
*
CREATE VIEW VC1 AS SELECT N FROM VE1 T0
WHERE T0.K IN
              (SELECT N FROM VW1 T1
               WHERE K = T0.N)
  AND T0.N IN
              (SELECT K FROM VS1 T2
               WHERE N = T0.K)!
*
CREATE VIEW VD1 AS SELECT N FROM VE1 T0
WHERE T0.K IN
              (SELECT N FROM VW1 T1
               WHERE K = T0.N
                         AND T0.N IN
                                    (SELECT N FROM VS1 T2
                                     WHERE K = T0.K))!
*
CREATE VIEW VF1 AS SELECT N FROM VE1 T0
WHERE T0.K IN
              (SELECT N FROM VW1 T1
               WHERE K = T0.N
                         AND T0.K IN
                                    (SELECT N FROM VS1 T2
                                     WHERE N = T1.K))!
*
CREATE VIEW VG1 AS SELECT N FROM VE1 T0
WHERE T0.K IN
              (SELECT N FROM VW1 T1
               WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS1 T2
                                     WHERE N = T0.K))!
*
CREATE VIEW VH1 AS SELECT N FROM VE1 T0
WHERE T0.K IN
              (SELECT N FROM VW1 T1
               WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS1 T2
                                     WHERE N = T1.K))!
*
CREATE VIEW VI1 AS SELECT N FROM VE1 T0
WHERE K = N AND NOT EXISTS
               (SELECT N FROM VW1 T1
                WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS1 T2
                                     WHERE N = T1.K AND K IS NULL))!
*
CREATE VIEW VJ1 AS SELECT N FROM VE1 T0
WHERE K = N AND NOT EXISTS
               (SELECT N FROM VW1 T1
                WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS1 T2
                                     WHERE N = T1.K AND T0.K IS NULL))!
*
CREATE VIEW VK1 AS SELECT T0.N FROM VE1 T0
WHERE T0.N IN
              (SELECT T1.N FROM VW1 T1, VS1 T2
               WHERE T1.N = T0.N
                 AND T2.N = T0.K)!
*
FILE STAUAS( 1!
* 
COMMIT!
*
* Anlegen der Stapel-Views + SELECT
FILE STAUAB( 2 1!
FILE STAUAB( 3 2!
FILE STAUAB( 4 3!
FILE STAUAB( 5 4!
FILE STAUAB( 6 5!
FILE STAUAB( 7 6!
FILE STAUAB( 8 7!
FILE STAUAB( 9 8!
FILE STAUAB( 10 9!
FILE STAUAB( 11 10!
FILE STAUAB( 12 11!

FILE STAUAB( 13 12!
FILE STAUAB( 14 13!
FILE STAUAB( 15 14!
*
* Loeschen der untersten VIEWs
DROP VIEW VE1!
commit !
DROP VIEW VW1!
commit !
DROP VIEW VS1!
* 
COMMIT!
*
DROP TABLE TABX!
*
COMMIT WORK RELEASE!
*
file CONNECT ( SUT SUT !
*
DROP USER USER0 !
*
COMMIT WORK RELEASE!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* STAV.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
FILE CROBJ!
*
file CONNECT ( USERX USERX!
FILE STAVA( TAB0!
*
COMMIT WORK RELEASE!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* SUBQUERY.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
CREATE TABLE emp  ( name CHAR (15) KEY,
                       deptno FIXED (2) NOT NULL RANGE BETWEEN 10 AND 90,
                       currsal FIXED (5) NOT NULL,
                       manager CHAR (15) NOT NULL,
                       startsal FIXED (5) NOT NULL,
                       sex CHAR (6) NOT NULL ) !
if &rc <> 0 then return
CREATE TABLE fam   ( name CHAR (15) KEY,
                       deptno FIXED (2) NOT NULL RANGE BETWEEN 10 AND 90,
                       status CHAR (8) NOT NULL,
                       depend FIXED (2) NOT NULL RANGE BETWEEN 0 AND 15,
                       address CHAR (30) NOT NULL ) !
CREATE TABLE dept  ( deptno FIXED (2) KEY RANGE BETWEEN 10 AND 90 ,
                        deptname CHAR (10) NOT NULL,
                        locname  CHAR (15) NOT NULL )  !
commit work !
LOCK TABLE emp, fam, dept IN EXCLUSIVE MODE !
INSERT INTO emp SET name = 'Fred Gerard',
             deptno = 50,
             currsal = 15000,
             manager = 'George Taylor',
             startsal = 8000,
             sex = 'male'!
INSERT INTO emp SET name = 'Norma Wilson',
             deptno = 40,
             currsal = 15500,
             manager = 'Jill Crowley',
             startsal = 7500,
             sex = 'female'!
INSERT INTO emp SET name = 'Paul Brown',
             deptno = 70,
             currsal = 15000,
             manager = 'George Taylor',
             startsal = 7000,
             sex = 'male'!
INSERT INTO emp SET name = 'Paul Rogers',
             deptno = 40,
             currsal = 15000,
             manager = 'Jill Crowley',
             startsal = 8000,
             sex = 'male'!
INSERT INTO emp SET name = 'John Johnes',
             deptno = 60,
             currsal = 16500,
             manager = 'Dana Smith',
             startsal = 8500,
             sex = 'male'!
INSERT INTO emp SET name = 'Pat Mingo',
             deptno = 50,
             currsal = 13000,
             manager = 'Fred Gerard',
             startsal = 6500,
             sex = 'female'!
INSERT INTO emp SET name = 'Dana Smith',
             deptno = 60,
             currsal = 16000,
             manager = 'George Taylor',
             startsal = 7500,
             sex = 'female'!
INSERT INTO emp SET name = 'Mary Rowen',
             deptno = 60,
             currsal = 15500,
             manager = 'Dana Smith',
             startsal = 9000,
             sex = 'female'!
INSERT INTO emp SET name = 'Jill Crowley',
             deptno = 40,
             currsal = 17000,
             manager = 'George Taylor',
             startsal = 8250,
             sex = 'female'!
INSERT INTO emp SET name = 'Jerry Denny',
             deptno = 70,
             currsal = 14000,
             manager = 'Paul Brown',
             startsal = 6000,
             sex = 'male'!
INSERT INTO emp SET name = 'Alice Teir',
             deptno = 50,
             currsal = 13500,
             manager = 'Fred Gerard',
             startsal = 8500,
             sex = 'female'!
INSERT INTO emp SET name = 'Dave Cowel',
             deptno = 60,
             currsal = 14000,
             manager = 'Dana Smith',
             startsal = 7500,
             sex = 'male'!
INSERT INTO emp SET name = 'George Taylor',
             deptno = 10,
             currsal = 20000,
             manager = ' ',
             startsal = 10000,
             sex = 'male'!
INSERT INTO emp SET name = 'Joan Bird',
             deptno = 50,
             currsal = 14500,
             manager = 'Fred Gerard',
             startsal = 8000,
             sex = 'female'!
INSERT INTO emp SET name = 'Ed Nader',
             deptno = 70,
             currsal = 8000,
             manager = 'Paul Brown',
             startsal = 8000,
             sex = 'male'!
INSERT INTO emp SET name = 'Joyce Adams',
             deptno = 70,
             currsal = 16000,
             manager = 'Paul Brown',
             startsal = 6500,
             sex = 'female'!
INSERT INTO fam SET name = 'Fred Gerard',
                    deptno = 50,
               status = 'married',
               depend = 3,
               address = '37 Drake St. #2, Carmichael'!
INSERT INTO fam SET name = 'Norma Wilson',
                    deptno = 40,
               status = 'single',
               depend = 1,
               address = '4175 Oak Ct., Daly City'!
INSERT INTO fam SET name = 'Paul Brown',
                    deptno = 70,
               status = 'married',
               depend = 4,
               address = '271 Oxnard St., Lodi'!
INSERT INTO fam SET name = 'Paul Rogers',
                    deptno = 40,
               status = 'divorced',
               depend = 5,
               address = '27811 Embarcodero, S.F.'!
INSERT INTO fam SET name = 'John Johnes',
                    deptno = 60,
               status = 'single',
               depend = 1,
               address = '2785 Drake St., Stockton'!
INSERT INTO  fam SET name = 'Pat Mingo',
                    deptno = 50,
               status = 'married',
               depend = 2,
               address = '2751 Peer St., #1, Reno'!
INSERT INTO fam SET name = 'Dana Smith',
                    deptno = 60,
               status = 'divorced',
               depend = 1,
               address = 'PO Box 571, Rt 51, Waterloo'!
INSERT INTO fam SET name = 'Mary Rowen',
                    deptno = 60,
               status = 'married',
               depend = 3,
               address = '27451 Mills St., San Rafael'!
INSERT INTO fam SET name = 'Jill Crowley',
                    deptno = 40,
               status = 'divorced',
               depend = 1,
               address = '5671 Powell St., S.F.'!
INSERT INTO fam SET name = 'Jerry Denny',
                    deptno = 70,
               status = 'married',
               depend = 4,
               address = '67511 Market St., Stockton'!
INSERT INTO fam SET name = 'Alice Teir',
                    deptno = 50,
               status = 'single',
               depend = 1,
               address = '2751 Maple Ct., Davis'!
INSERT INTO fam SET name = 'Dave Cowel',
                    deptno = 60,
               status = 'divorced',
               depend = 1,
               address = '1561 Main St., French Camp'!
INSERT INTO fam SET name = 'George Taylor',
                    deptno = 10,
               status = 'married',
               depend = 5,
               address = '56715 - 31st St., Sacramento'!
INSERT INTO fam SET name = 'Joan Bird',
                    deptno = 50,
               status = 'divorced',
               depend = 1,
               address = '2731 Coronado Wy, Pacifica'!
INSERT INTO fam SET name = 'Ed Nader',
                    deptno = 70,
               status = 'divorced',
               depend = 1,
               address = '774 S. St., Victor'!
INSERT INTO fam SET name = 'Joyce Adams',
                    deptno = 70,
               status = 'married',
               depend = 2,
               address = '8741 Takay Bowl Blvd, Lodi'!
INSERT INTO dept SET deptno = 10,
              deptname = 'Management',
              locname  = 'San Francisco'!
INSERT INTO dept SET deptno = 40,
              deptname = 'Sales',
              locname  = 'San Francisco'!
INSERT INTO dept SET deptno = 50,
              deptname = 'Materials',
              locname   = 'Sacramento'!
INSERT INTO dept SET deptno = 60,
              deptname = 'Production',
              locname  = 'Stockton'!
INSERT INTO dept SET deptno = 70,
              deptname = 'Supply',
              locname  = 'Lodi'!
commit release !
file connect ( kern test !
* correlated subqueries mit sum/avg, joins, baumartigem aufbau,
*         mehreren mehrstufigen selects !
select sum(depend) from fam f, emp
  where f.name > 'B' and
        f.name = emp.name and
        f.name = any (select name from emp
                 where emp.deptno > 0 and
                       emp.name = f.name and
                       deptno = any (select deptno from dept
                                       where dept.deptno = f.deptno))
   and
        f.deptno = all ( select distinct deptno from emp
                 where emp.deptno = f.deptno) !
fetch!
select f.name, address, depend from fam f, emp
  where f.name > 'B' and
        f.name = emp.name and
        f.name = any (select name from emp
                 where emp.deptno > 0 and
                       emp.name = f.name and
                       deptno = any (select deptno from dept
                                       where dept.deptno = f.deptno))
   and
        f.deptno = all ( select distinct deptno from emp
                 where emp.deptno = f.deptno) !
fetch !
select sum(depend) from fam f, emp
  where f.name > 'B' and
        f.name = emp.name and
        f.name = any (select min(name) & ' ' from emp
                 where deptno > 0 and
                       emp.name >= f.name)!
fetch!
select f.name, address, depend from fam f, emp
  where f.name > 'B' and
        f.name = emp.name and
        f.name = any (select name from emp
                 where emp.deptno > 0 and
                       emp.name = f.name and
                       deptno = any (select deptno from dept
                                       where dept.deptno = f.deptno))
   and
        f.deptno = all ( select distinct deptno from emp
                 where emp.deptno = f.deptno) order by depend !
fetch !
select sum(depend) from fam f, emp
  where f.name > 'B' and
        f.name = emp.name and
        f.name = any (select fam.name from emp, fam
                 where emp.deptno > 0 and
                       emp.name >= f.name)!
fetch!
rollback release!

file connect ( kern test !
create table t (a char (8) ascii, tdummy char (2))!
insert t set a = 'aaa'!
create table u (a char (8), udummy char (2)) !
insert u set a = 'aaa'!
select t.a,u.a into :a, :b from t,u where t.a = u.a !
select t.a,u.a into :a, :b from t,u where t.a between u.a and 'bbb'!
select t.a,u.a into :a, :b from t,u where t.a in (u.a, 'bbb')!
create index ti on t (a,tdummy)!
create index ui on u (a,udummy)!
select t.a,u.a into :a, :b from t,u where t.a = u.a !
select t.a,u.a into :a, :b from t,u where t.a between u.a and 'bbb'!
select t.a,u.a into :a, :b from t,u where t.a in (u.a, 'bbb')!
rollback release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* SUBVIEW.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
CREATE TABLE emp  ( name CHAR (15) KEY,
                       deptno FIXED (2) NOT NULL RANGE BETWEEN 10 AND 90,
                       curr_sal FIXED (5) NOT NULL,
                       manager CHAR (15) NOT NULL,
                       startsal FIXED (5) NOT NULL,
                       sex CHAR (6) NOT NULL ) !
if &rc <> 0 then return
CREATE TABLE fam   ( name CHAR (15) KEY,
                       deptno FIXED (2) NOT NULL RANGE BETWEEN 10 AND 90,
                       status CHAR (8) NOT NULL,
                       depend FIXED (2) NOT NULL RANGE BETWEEN 0 AND 15,
                       address CHAR (30) NOT NULL ) !
CREATE TABLE dept  ( deptno FIXED (2) KEY RANGE BETWEEN 10 AND 90 ,
                        deptname CHAR (10) NOT NULL,
                        locname  CHAR (15) NOT NULL )  !
commit work !
LOCK TABLE emp, fam, dept IN EXCLUSIVE MODE !
INSERT INTO emp SET name = 'Fred Gerard',
             deptno = 50,
             curr_sal = 15000,
             manager = 'George Taylor',
             startsal = 8000,
             sex = 'male'!
INSERT INTO emp SET name = 'Norma Wilson',
             deptno = 40,
             curr_sal = 15500,
             manager = 'Jill Crowley',
             startsal = 7500,
             sex = 'female'!
INSERT INTO emp SET name = 'Paul Brown',
             deptno = 70,
             curr_sal = 15000,
             manager = 'George Taylor',
             startsal = 7000,
             sex = 'male'!
INSERT INTO emp SET name = 'Paul Rogers',
             deptno = 40,
             curr_sal = 15000,
             manager = 'Jill Crowley',
             startsal = 8000,
             sex = 'male'!
INSERT INTO emp SET name = 'John Johnes',
             deptno = 60,
             curr_sal = 16500,
             manager = 'Dana Smith',
             startsal = 8500,
             sex = 'male'!
INSERT INTO emp SET name = 'Pat Mingo',
             deptno = 50,
             curr_sal = 13000,
             manager = 'Fred Gerard',
             startsal = 6500,
             sex = 'female'!
INSERT INTO emp SET name = 'Dana Smith',
             deptno = 60,
             curr_sal = 16000,
             manager = 'George Taylor',
             startsal = 7500,
             sex = 'female'!
INSERT INTO emp SET name = 'Mary Rowen',
             deptno = 60,
             curr_sal = 15500,
             manager = 'Dana Smith',
             startsal = 9000,
             sex = 'female'!
INSERT INTO emp SET name = 'Jill Crowley',
             deptno = 40,
             curr_sal = 17000,
             manager = 'George Taylor',
             startsal = 8250,
             sex = 'female'!
INSERT INTO emp SET name = 'Jerry Denny',
             deptno = 70,
             curr_sal = 14000,
             manager = 'Paul Brown',
             startsal = 6000,
             sex = 'male'!
INSERT INTO emp SET name = 'Alice Teir',
             deptno = 50,
             curr_sal = 13500,
             manager = 'Fred Gerard',
             startsal = 8500,
             sex = 'female'!
INSERT INTO emp SET name = 'Dave Cowel',
             deptno = 60,
             curr_sal = 14000,
             manager = 'Dana Smith',
             startsal = 7500,
             sex = 'male'!
INSERT INTO emp SET name = 'George Taylor',
             deptno = 10,
             curr_sal = 20000,
             manager = ' ',
             startsal = 10000,
             sex = 'male'!
INSERT INTO emp SET name = 'Joan Bird',
             deptno = 50,
             curr_sal = 14500,
             manager = 'Fred Gerard',
             startsal = 8000,
             sex = 'female'!
INSERT INTO emp SET name = 'Ed Nader',
             deptno = 70,
             curr_sal = 8000,
             manager = 'Paul Brown',
             startsal = 8000,
             sex = 'male'!
INSERT INTO emp SET name = 'Joyce Adams',
             deptno = 70,
             curr_sal = 16000,
             manager = 'Paul Brown',
             startsal = 6500,
             sex = 'female'!
INSERT INTO fam SET name = 'Fred Gerard',
                    deptno = 50,
               status = 'married',
               depend = 3,
               address = '37 Drake St. #2, Carmichael'!
INSERT INTO fam SET name = 'Norma Wilson',
                    deptno = 40,
               status = 'single',
               depend = 1,
               address = '4175 Oak Ct., Daly City'!
INSERT INTO fam SET name = 'Paul Brown',
                    deptno = 70,
               status = 'married',
               depend = 4,
               address = '271 Oxnard St., Lodi'!
INSERT INTO fam SET name = 'Paul Rogers',
                    deptno = 40,
               status = 'divorced',
               depend = 5,
               address = '27811 Embarcodero, S.F.'!
INSERT INTO fam SET name = 'John Johnes',
                    deptno = 60,
               status = 'single',
               depend = 1,
               address = '2785 Drake St., Stockton'!
INSERT INTO  fam SET name = 'Pat Mingo',
                    deptno = 50,
               status = 'married',
               depend = 2,
               address = '2751 Peer St., #1, Reno'!
INSERT INTO fam SET name = 'Dana Smith',
                    deptno = 60,
               status = 'divorced',
               depend = 1,
               address = 'PO Box 571, Rt 51, Waterloo'!
INSERT INTO fam SET name = 'Mary Rowen',
                    deptno = 60,
               status = 'married',
               depend = 3,
               address = '27451 Mills St., San Rafael'!
INSERT INTO fam SET name = 'Jill Crowley',
                    deptno = 40,
               status = 'divorced',
               depend = 1,
               address = '5671 Powell St., S.F.'!
INSERT INTO fam SET name = 'Jerry Denny',
                    deptno = 70,
               status = 'married',
               depend = 4,
               address = '67511 Market St., Stockton'!
INSERT INTO fam SET name = 'Alice Teir',
                    deptno = 50,
               status = 'single',
               depend = 1,
               address = '2751 Maple Ct., Davis'!
INSERT INTO fam SET name = 'Dave Cowel',
                    deptno = 60,
               status = 'divorced',
               depend = 1,
               address = '1561 Main St., French Camp'!
INSERT INTO fam SET name = 'George Taylor',
                    deptno = 10,
               status = 'married',
               depend = 5,
               address = '56715 - 31st St., Sacramento'!
INSERT INTO fam SET name = 'Joan Bird',
                    deptno = 50,
               status = 'divorced',
               depend = 1,
               address = '2731 Coronado Wy, Pacifica'!
INSERT INTO fam SET name = 'Ed Nader',
                    deptno = 70,
               status = 'divorced',
               depend = 1,
               address = '774 S. St., Victor'!
INSERT INTO fam SET name = 'Joyce Adams',
                    deptno = 70,
               status = 'married',
               depend = 2,
               address = '8741 Takay Bowl Blvd, Lodi'!
INSERT INTO dept SET deptno = 10,
              deptname = 'Management',
              locname  = 'San Francisco'!
INSERT INTO dept SET deptno = 40,
              deptname = 'Sales',
              locname  = 'San Francisco'!
INSERT INTO dept SET deptno = 50,
              deptname = 'Materials',
              locname   = 'Sacramento'!
INSERT INTO dept SET deptno = 60,
              deptname = 'Production',
              locname  = 'Stockton'!
INSERT INTO dept SET deptno = 70,
              deptname = 'Supply',
              locname  = 'Lodi'!
select * from emp !
fetch !
select * from fam !
fetch !
select * from dept !
fetch !
commit release !
file connect ( kern test !
create view v1 (a) as select sum(depend) from fam f, emp
  where emp.name > 'B' and
        f.name = emp.name and
        emp.name = any (select name from emp
                 where deptno > 0 and
                       emp.name = f.name and
                       deptno = any (select deptno from dept
                                       where deptno = f.deptno))
   and
        emp.deptno = all ( select distinct deptno from emp
                 where deptno = f.deptno) !
select * from v1 !
fetch!
create view v2 (b) as select sum(depend) from fam f
  where name > 'B' and
        name = any (select name from emp
                 where deptno > 0 and
                       name = f.name and
                       deptno = any (select deptno from dept
                                       where deptno = f.deptno))
   and
        deptno = all ( select distinct deptno from emp
                 where deptno = f.deptno) !
select * from v2 !
fetch !
select * from v1 where a in (select b from v2 where b > 2) !
fetch !
select * from v1 v where a in (select b from v2 where v2.b = v.a) !
fetch !
select * from v1 v where a in
         (select b from v2 where v2.b in
               (select b from v2 where v2.b = v.a)) !
fetch !
create view v3 as select emp.name, address, depend from fam f, emp
  where emp.name > 'B' and
        f.name = emp.name and
        emp.name = any (select name from emp
                 where emp.deptno > 0 and
                       name = f.name and
                       deptno = any (select deptno from dept
                                       where deptno = f.deptno))
   and
        emp.deptno = all ( select distinct deptno from emp
                 where deptno = f.deptno) !
select * from v3 !
fetch !
create view v4 as select name, address, depend from fam f
  where name > 'B' and
        name = any (select name from emp
                 where deptno > 0 and
                       name = f.name)!
select * from v4 !
fetch !
select name, address from v3 v where
       (name = 'Ed Nader' and name in
             (select name from v4 where name = v.name))
       or
       (name = 'Norma Wilson' and name in
             (select name from v4 where name = v.name)) !
fetch !
rollback !
create view v1 as select sum(depend) from fam f, emp
  where emp.name > 'B' and
        f.name = emp.name and
        emp.name = any (select min(name) & ' ' from emp
                 where deptno > 0 and
                       emp.name >= f.name)!
select * from v1 !
fetch!
create view v2 as select emp.name, address, depend from fam f, emp
  where emp.name > 'B' and
        f.name = emp.name and
        f.name = any (select name from emp
                 where deptno > 0 and
                       emp.name = f.name and
                       deptno = any (select deptno from dept
                                       where deptno = f.deptno))
   and
        f.deptno = all ( select distinct deptno from emp
                 where deptno = f.deptno) !
select * from v2 order by depend !
fetch !
create view v3 as select sum(depend) from fam f, emp
  where f.name > 'B' and
        f.name = emp.name and
        f.name = any (select fam.name from emp, fam
                 where emp.deptno > 0 and
                       emp.name >= f.name)!
select * from v3 !
fetch!
* distinct/into auf einfache und correlated subqueries !
create view v4 as select name from emp
   where name > 'Paul' and
         exists (select deptno from dept) !
select * from v4 !
fetch !
rollback release!

file connect ( kern test !
create view v1 as select emp.name, emp.deptno, 
                         fam.address from emp, fam !
* join view in correlation column
* two buffers for the results of first select !
create view v2 as select * from v1
    where deptno > 65 and
    exists ( select deptno from dept where v1.deptno = deptno) !
select * from v2 !
fetch !
* join view using more than one table in one expression !
create view v3 (mul) as select emp.deptno * fam.deptno
      from emp, fam !
create view v4 as select * from v3 where exists
      (select locname from dept where v3.mul > deptno)!
select * from v4 !
fetch!
* complicated select, (bool1 has to be false, no new_stpos ) !
create view v5 (a,b,c,d,e,f,g,h,i,j,k,l,m,n)
               as select * from emp, fam, dept
   where emp.deptno * fam.deptno > dept.deptno and
         emp.name = fam.name and
         emp.deptno = any ( select deptno from dept
                               where emp.name > dept.locname )!
select * from v5 !
fetch !
commit release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* SUM.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t ( a fixed (7), b fixed (5))!
insert t set a = 2, b = 3!
insert t set a = 6, b = 3!
insert t set a = 4, b = 3!
insert t set a =12, b = 2!
commit !
create table t1 ( c fixed (10))!
insert t1 set c = 12 !
insert t1 set c = 24 !
select a                  from t, t1!
fetch !
select a + c              from t, t1!
fetch !
select sum(a)             from t, t1!
fetch !
select sum(a)+c           from t, t1 group by c!
fetch !
select sum(a+c)           from t, t1!
fetch !
select distinct a + c     from t, t1!
fetch !
select distinct sum(a)    from t, t1!
fetch !
select distinct sum(a)+c  from t, t1 group by c!
fetch !
select distinct sum(a+c)  from t, t1!
fetch !
select distinct a + c     from t, t1 order by 1 !
fetch !
select distinct sum(a)    from t, t1 order by 1 !
fetch !
select distinct sum(a)+c  from t, t1 group by c order by 1 !
fetch !
select distinct sum(a+c)  from t, t1 order by 1 !
fetch !
select a                  from t, t1 where a > 0 !
fetch !
select a + c              from t, t1 where a > 0 !
fetch !
select sum(a)             from t, t1 where a > 0 !
fetch !
select sum(a)+c           from t, t1 where a > 0  group by c!
fetch !
select sum(a+c)           from t, t1 where a > 0 !
fetch !
select distinct a + c     from t, t1 where a > 0 !
fetch !
select distinct sum(a)    from t, t1 where a > 0 !
fetch !
select distinct sum(a)+c  from t, t1 where a > 0  group by c!
fetch !
select distinct sum(a+c)  from t, t1 where a > 0 !
fetch !
select distinct a + c     from t, t1 where a > 0  order by 1 !
fetch !
select distinct sum(a)    from t, t1 where a > 0  order by 1 !
fetch !
select distinct sum(a)+c  from t, t1 where a > 0  group by c order by 1 !
fetch !
select distinct sum(a+c)  from t, t1 where a > 0  order by 1 !
fetch !
select rowno, a           from t, t1!
fetch !
select rowno, a + c       from t, t1!
fetch !
select rowno, sum(a)      from t, t1!
fetch !
select rowno, sum(a)+c    from t, t1 group by c!
fetch !
select rowno, sum(a+c)    from t, t1!
fetch !
select a                  from t, t1 where rowno < 3!
fetch !
select a + c              from t, t1 where rowno < 3!
fetch !
select sum(a)             from t, t1 where rowno < 3!
fetch !
* Added < > around from list, because in a distributed database the join
* optimizer chooses a different sequence. This leads to totally different
* results, since the rowno predicate is evaluated first... (CK, 18.2.97)
select sum(a)+c           from<t1, t>where rowno < 3 group by c!
fetch !
select sum(a+c)           from t, t1 where rowno < 3!
fetch !
select sum(a)             from t, t1 group by a having a > 2!
fetch !
select sum(a)+c           from t, t1 group by a,c having a > 2!
fetch !
select sum(a+c)           from t, t1 group by a having a > 2!
fetch !
select a + c              from t, t1 group by a,c having a+c > 18!
fetch !
select sum(a)             from t, t1 group by a,c having a+c > 18!
fetch !
select sum(a)+c           from t, t1 group by a,c having a+c > 18!
fetch !
select sum(a+c)           from t, t1 group by a,c having a+c > 18!
fetch !
select a                  from t, t1 group by a,c having sum(a) > 2!
fetch !
select sum(a)             from t, t1 group by a having sum(a) > 8!
fetch !
select sum(a)+c           from t, t1 group by a,c having sum(a) > 2!
fetch !
select sum(a+c)           from t, t1 group by a having sum(a) > 8!
fetch !
select a                  from t, t1 group by a,c having sum(a)+c > 16!
fetch !
select a + c              from t, t1 group by a,c having sum(a)+c > 16!
fetch !
select sum(a)             from t, t1 group by a,c having sum(a)+c > 16!
fetch !
select sum(a)+c           from t, t1 group by a,c having sum(a)+c > 16!
fetch !
select sum(a+c)           from t, t1 group by a,c having sum(a)+c > 16!
fetch !
select a                  from t, t1 group by a having sum(a+c) > 40!
fetch !
select a + c              from t, t1 group by a,c having sum(a+c) > 18!
fetch !
select sum(a)             from t, t1 group by a having sum(a+c) > 40!
fetch !
select sum(a)+c           from t, t1 group by c having sum(a+c) > 40!
fetch !
select sum(a+c)           from t, t1 group by c having sum(a+c) > 40!
fetch !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
