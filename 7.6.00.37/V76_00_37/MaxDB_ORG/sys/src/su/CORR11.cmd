file connect ( kern test !
* Corr mit SELECT INTO lief manchmal in -9042 !
CREATE TABLE BTXTAX (
BSIAUTH CHAR    (8) ASCII,
TAXTYPE CHAR    (2) ASCII,
EFFECTIVE CHAR  (8) ASCII,
RESCIND CHAR    (8) ASCII,
PRIMARY KEY (BSIAUTH ,TAXTYPE ,EFFECTIVE)) !
insert btxtax values ('00000000','01','19930101','00000000')!
insert btxtax values ('00000000','01','19940101','00000000')!
insert btxtax values ('00000000','01','19950101','00000000')!
insert btxtax values ('00000000','02','19930101','00000000')!
insert btxtax values ('00000000','02','19940101','00000000')!
insert btxtax values ('00000000','02','19950101','00000000')!
insert btxtax values ('00000000','03','19930101','00000000')!
insert btxtax values ('00000000','03','19940101','00000000')!
insert btxtax values ('00000000','03','19950101','00000000')!
insert btxtax values ('00000000','04','19930101','00000000')!
insert btxtax values ('00000000','04','19940101','00000000')!
insert btxtax values ('00000000','04','19950101','00000000')!
insert btxtax values ('00000000','05','19930101','00000000')!
insert btxtax values ('00000000','05','19940101','00000000')!
insert btxtax values ('00000000','06','19930101','00000000')!
insert btxtax values ('00000000','06','19940101','00000000')!
insert btxtax values ('00000000','10','19830101','00000000')!
insert btxtax values ('00000000','55','19930101','00000000')!
insert btxtax values ('00000000','55','19940101','00000000')!
insert btxtax values ('00000000','55','19950101','00000000')!
insert btxtax values ('00000000','56','19930101','00000000')!
insert btxtax values ('00000000','56','19940101','00000000')!
insert btxtax values ('00000000','56','19950101','00000000')!
insert btxtax values ('00000000','57','19930101','00000000')!
insert btxtax values ('00000000','57','19940101','00000000')!
insert btxtax values ('00000000','57','19950101','00000000')!
insert btxtax values ('00000000','58','19930101','00000000')!
insert btxtax values ('00000000','58','19940101','00000000')!
insert btxtax values ('00000000','58','19950101','00000000')!
insert btxtax values ('00000000','59','19930101','00000000')!
insert btxtax values ('00000000','59','19940101','00000000')!
insert btxtax values ('00000000','59','19950101','00000000')!
insert btxtax values ('00000000','60','19930101','00000000')!
insert btxtax values ('00000000','60','19940101','00000000')!
insert btxtax values ('00000000','60','19950101','00000000')!
insert btxtax values ('00010000','01','19850101','00000000')!
insert btxtax values ('00010000','10','19830101','00000000')!
insert btxtax values ('00010001','51','19810101','00000000')!
insert btxtax values ('00010002','51','19910201','00000000')!
insert btxtax values ('00010003','51','19870623','00000000')!
insert btxtax values ('00010004','51','19901207','00000000')!
insert btxtax values ('00010005','51','19860401','00000000')!
insert btxtax values ('00010006','51','19780307','00000000')!
insert btxtax values ('00010007','51','19830620','00000000')!
insert btxtax values ('00010008','51','19861001','00000000')!
insert btxtax values ('00010009','51','19781001','00000000')!
insert btxtax values ('00010010','51','19910901','00000000')!
insert btxtax values ('00010011','51','19690901','00000000')!
insert btxtax values ('00010012','51','19781001','00000000')!
insert btxtax values ('00010013','51','19930801','00000000')!
insert btxtax values ('00010014','51','19890101','00000000')!
insert btxtax values ('00010015','51','19930101','00000000')!
insert btxtax values ('00010073','01','19830101','00000000')!
insert btxtax values ('00010073','51','19830101','00000000')!
insert btxtax values ('00010501','01','19701201','00000000')!
insert btxtax values ('00010501','51','19701201','00000000')!
insert btxtax values ('00010521','01','19880101','00000000')!
insert btxtax values ('00010521','51','19880101','00000000')!
insert btxtax values ('00040000','01','19930101','00000000')!
insert btxtax values ('00040000','01','19940101','00000000')!
insert btxtax values ('00040000','10','19830101','00000000')!
insert btxtax values ('00100000','01','19880101','00000000')!
insert btxtax values ('00100000','10','19870101','00000000')!
insert btxtax values ('00100502','01','19830701','00000000')!
insert btxtax values ('00120000','10','19830101','00000000')!
insert btxtax values ('00130000','01','19870101','00000000')!
insert btxtax values ('00130000','01','19940701','00000000')!
insert btxtax values ('00130000','10','19900101','00000000')!
insert btxtax values ('00240000','01','19920701','00000000')!
insert btxtax values ('00240000','01','19950101','00000000')!
insert btxtax values ('00240000','10','19930101','00000000')!
insert btxtax values ('00240001','01','19920701','00000000')!
insert btxtax values ('00240002','01','19930101','00000000')!
insert btxtax values ('00240003','01','19920701','00000000')!
insert btxtax values ('00240004','01','19920701','00000000')!
insert btxtax values ('00240004','01','19940101','00000000')!
insert btxtax values ('00240005','01','19930101','00000000')!
insert btxtax values ('00240006','01','19920701','00000000')!
insert btxtax values ('00240007','01','19920701','00000000')!
insert btxtax values ('00240008','01','19920701','00000000')!
insert btxtax values ('00240008','01','19960101','00000000')!
insert btxtax values ('00240009','01','19920701','00000000')!
insert btxtax values ('00240009','01','19960101','00000000')!
insert btxtax values ('00240010','01','19920701','00000000')!
insert btxtax values ('00240011','01','19920701','00000000')!
insert btxtax values ('00240012','01','19920701','00000000')!
insert btxtax values ('00240013','01','19920701','00000000')!
insert btxtax values ('00240014','01','19920701','00000000')!
insert btxtax values ('00240015','01','19920701','00000000')!
insert btxtax values ('00240016','01','19920701','00000000')!
insert btxtax values ('00240017','01','19920701','00000000')!
insert btxtax values ('00240018','01','19930101','00000000')!
insert btxtax values ('00240019','01','19920701','00000000')!
insert btxtax values ('00240019','01','19950101','00000000')!
insert btxtax values ('00240020','01','19920701','00000000')!
insert btxtax values ('00240020','01','19940101','00000000')!
insert btxtax values ('00240021','01','19920701','00000000')!
insert btxtax values ('00240021','01','19940101','00000000')!
insert btxtax values ('00240022','01','19920701','00000000')!
insert btxtax values ('00240023','01','19920701','00000000')!
insert btxtax values ('00240023','01','19940101','00000000')!
insert btxtax values ('00240101','01','19920701','00000000')!
commit !
SELECT BSIAUTH, TAXTYPE, EFFECTIVE, RESCIND
INTO :P_1 , :P_2 , :P_3 , :P_4
FROM BTXTAX A 
WHERE 
   A.BSIAUTH = '00000000' AND 
   A.TAXTYPE = '01' AND 
   A.EFFECTIVE = (SELECT MAX(EFFECTIVE) 
                  FROM BTXTAX B 
                  WHERE 
                    A.BSIAUTH = B.BSIAUTH AND A.TAXTYPE = B.TAXTYPE AND 
                    B.EFFECTIVE <= '20250101') 
                 AND 
(A.RESCIND > '20250101' OR A.RESCIND = '00000000')!
drop table btxtax !
commit release !
* PTS4253 !
file connect ( sut sut !
create user "ACCOUNT" password test dba not exclusive !
commit release !
file connect ( "ACCOUNT" test !
CREATE TABLE "ACCOUNT"."REVENUE" (
"CUSTOMER" CHAR(20) ASCII ,
"COUNTRY" CHAR(3) ASCII ,
"YEARQU" CHAR(7) ASCII ,
"INVLICENCE" FIXED(9,0) ,
"INVMAINTENANCE" FIXED(9,0) ,
"INVSERVICE" FIXED(9,0) ,
"LASTUPDATEUSER" CHAR(8) ASCII NOT NULL
, PRIMARY KEY ("CUSTOMER" ,"COUNTRY" ,"YEARQU"))  !
insert revenue values ('ZZEGAL','FRA','1997/Q4',1,2,3,'ELKE')!
CREATE TABLE "ACCOUNT"."PIPELINE" (
"CUSTOMER" CHAR(20) ASCII ,
"COUNTRY" CHAR(3) ASCII ,
"PYEARQU" CHAR(7) ASCII ,
"IYEARQU" CHAR(7) ASCII ,   
"LICENCE" FIXED(9,0) ,
"LICENCEPB" FIXED(3,0) DEFAULT 100,
"MAINTENANCE" FIXED(9,0) ,
"MAINTENANCEPB" FIXED(3,0) DEFAULT 100,
"SERVICES" FIXED(9,0) ,
"SERVICESPB" FIXED(3,0) DEFAULT 100,
"LASTUPDATEUSER" CHAR(8) ASCII NOT NULL
, CONSTRAINT "CONSTRAINT1" CHECK MAINTENANCEPB IN (25,50,75,100)
, CONSTRAINT "CONSTRAINT2" CHECK SERVICESPB IN (25,50,75,100)
, CONSTRAINT "PERCENT" CHECK LICENCEPB IN (25,50,75,100)
, PRIMARY KEY ("CUSTOMER" ,"COUNTRY" ,"PYEARQU" ,"IYEARQU"))!
insert pipeline values ('ZZEGAL1 ','POL','1999/Q1','1998/Q2',1,25,3,
50,5,75,'ELKE')!
SELECT  REV.CUSTOMER FROM REVENUE REV
 UNION
 SELECT PIP.CUSTOMER  FROM PIPELINE PIP
    WHERE PIP.CUSTOMER = 'ZZEGAL1'
      AND PIP.COUNTRY  BETWEEN ' ' AND 'ZZZ'
      AND PIP.PYEARQU   BETWEEN '1997/Q1' AND '2000/Q4'
      AND PIP.IYEARQU IN
        (SELECT MAX(PIP2.IYEARQU)
           FROM PIPELINE PIP2
           WHERE PIP2.CUSTOMER = PIP.CUSTOMER
             AND PIP2.COUNTRY  = PIP.COUNTRY
             AND PIP2.PYEARQU  = PIP.PYEARQU
             AND PIP2.PYEARQU NOT IN
             (SELECT REV1.YEARQU FROM REVENUE REV1
                 WHERE REV1.CUSTOMER = 'ZZEGAL')   )!
fetch into :a !
commit release !
file connect ( sut sut !
drop user "ACCOUNT" !
commit release !
* PTS4734 !
file connect ( kern test !
CREATE TABLE "S_TG_ME_PN"         (
"TAG_ID            " DATE      ,
"MEDIUM_ID         " CHAR      (2) ASCII    ,
"PN1_KEY           " CHAR      (10) ASCII    ,
"UMSATZ_DM_S       " FIXED     (15,2) ,
"UMSATZ_DM_H       " FIXED     (15,2) ,
"UMSATZ_EURO_S     " FIXED     (15,2) ,
"UMSATZ_EURO_H     " FIXED     (15,2) ,
"GEBUEHR_DM        " FIXED     (8,2) ,
"GEBUEHR_EURO      " FIXED     (8,2) ,
"ANZAHL_S          " FIXED     (9,0) ,
"ANZAHL_H          " FIXED     (9,0) 
, PRIMARY KEY ("TAG_ID"            
,"MEDIUM_ID"         
,"PN1_KEY"           
))
!
CREATE INDEX  "S_TG_ME_PN.MEDIUM_ID" 
    ON  S_TG_ME_PN( MEDIUM_ID ASC ) !
CREATE INDEX  "S_TG_ME_PN.PN1_KEY"
    ON S_TG_ME_PN( PN1_KEY ASC ) !
CREATE INDEX  "S_TG_ME_PN.TAG_ID"
    ON S_TG_ME_PN( TAG_ID ASC ) !
insert into "S_TG_ME_PN"        values (
'19970609','AE','AA10CI9831', 434.06,434.06,0,0,0,0,1,1)!
insert into "S_TG_ME_PN"        values (
'19970609','AE','AA10QR9831',1002.00,0,0,0,0,0,1,0)!
insert into "S_TG_ME_PN"        values (
'19970609','AE','AA11GI9831',1000.00,1000.00,0,0,0,0,1,1)!
insert into "S_TG_ME_PN"        values (
'19970609','AE','AA15AE9831',677.95,0,0,0,0,0,1,0)!
insert into "S_TG_ME_PN"        values (
'19970609','AE','AA15AT9831',746.94,0,0,0,0,0,2,0)!
insert into "S_TG_ME_PN"        values (
'19970609','AE','AA69AQ9831',2091.00,0,0,0,0,0,2,0)!
commit !
CREATE TABLE "UMSATZ"             (
"TAG_ID            " DATE      ,
"LFD_NR            " FIXED     (9,0) ,
"KUNDE_ID          " CHAR      (10) ASCII    ,
"ORGA1_KEY         " CHAR      (12) ASCII    ,
"PN1_KEY           " CHAR      (10) ASCII    ,
"PRD1_KEY          " CHAR      (14) ASCII    ,
"BANK1_KEY         " CHAR      (10) ASCII    ,
"MEDIUM_ID         " CHAR      (2) ASCII    ,
"FWEN1_KEY         " CHAR      (12) ASCII    ,
"VERW_ZWECK_ID     " CHAR      (2) ASCII    ,
"KTO_ALT_ID        " CHAR      (10) ASCII    ,
"STUNDE            " FIXED     (2,0) ,
"UMSATZ_DM_S       " FIXED     (15,2) ,
"UMSATZ_DM_H       " FIXED     (15,2) ,
"UMSATZ_EURO_S     " FIXED     (15,2) ,
"UMSATZ_EURO_H     " FIXED     (15,2) ,
"GEBUEHR_DM        " FIXED     (5,2) ,
"GEBUEHR_EURO      " FIXED     (5,2) ,
"VALUTA            " FIXED     (8,0) ,
"KZ_FREMDKUNDE     " CHAR      (1) ASCII    ,
"TEXTSCHLUESSEL    " FIXED     (2,0) ,
"ZUSATZTEXT        " FIXED     (3,0) ,
"LEITWEG_AUSG_EINR " FIXED     (10,0) ,
"KZ_EIN_AUSGANG    " CHAR      (1) ASCII    ,
"PRIMANOTA         " CHAR      (8) ASCII    ,
"KONTO_NR          " FIXED     (10,0) ,
"GEGEN_KONTO_NR    " FIXED     (10,0) ,
"GEGEN_BIL_SCHL    " FIXED     (6,0) ,
"KZ_REFERENZ_NR    " FIXED     (1,0) ,
"ANZAHL_S          " FIXED     (1,0) ,
"ANZAHL_H          " FIXED     (1,0) ,
"UR_WAEHRUNG       " CHAR      (3) ASCII    ,
"VERWENDUNGSZWECK1 " VARCHAR   (27) ASCII    ,
"VERWENDUNGSZWECK2 " VARCHAR   (27) ASCII    
, PRIMARY KEY ("TAG_ID"            
,"LFD_NR"            
))
!
CREATE INDEX "UMSATZ.TAG_ID"
    ON UMSATZ( TAG_ID ASC ) !
insert into umsatz values (
'19970609',8391,'3304299000','AAAAAAAA69AQ','AA69AQ9831',
'ABABACAC200000','AD30020900','AE','ABABAI070000','..',
'0000000000',7,2000.00,0.00,0.00,0.00,0.00,0.00,19970609,'1',
20,75,0,'E','69AQ9831',3304299000,5049170097,0,0,1,0,'000',
'KARTENR.4917874297447007','')!
insert into umsatz values (
'19970609',8392,'3304299000','AAAAAAAA69AQ','AA69AQ9831',
'ABABACAC200000','AF20040000','AE','ABABAI070000','..',
'0000000000',7,91.00,0.00,0.00,0.00,0.00,0.00,19970609,'1',
20,75,0,'E','69AQ9831',3304299000,6181986,0,0,1,0,'000',
'100028060501433252208','BEITRAGDERSPIEGEL')!
insert into umsatz values (
'19970609',8617,'0883107000','AAAAAAAA10QR','AA10QR9831',
'ABABACAC200000','AH25162563','AE','ABABAB000004','..',
'0000000000',7,1002.00,0.00,0.00,0.00,0.00,0.00,19970609,'1',
20,75,0,'E','10QR9831',883107000,82044000,0,0,1,0,'000',
'RECHNUNG-NR.00019','SCHÜLER-NR.K112')!
insert into umsatz values (
'19970609',9565,'0881813000','AAAAAAAA11GI','AA11GI9831',
'ABABACAC200000','AK25090500','AE','ABABAB000004','..',
'0000000000',9,1000.00,0.00,0.00,0.00,0.00,0.00,19970609,'0',
20,75,0,'I','11GI9831',881813000,222232,2000,0,1,0,'000',
'UMBUCHUNGAUFKONTO222232','')!
insert into umsatz values (
'19970609',9567,'0222232000','AAAAAAAA11GI','AA11GI9831',
'ABABACAC200000','AK25090500','AE','ABABAH060002','..',
'0000000000',9,0.00,1000.00,0.00,0.00,0.00,0.00,19970609,'0',
51,0,0,'I','11GI9831',222232000,881813,2000,0,0,1,'000',
'UMBUCHUNGAUFKONTO222232','')!
insert into umsatz values (
'19970609',9581,'1882899000','AAAAACAG15AE','AA15AE9831',
'ABABACAC200000','AG29151731','AE','ABABAB000004','..',
'0000000000',9,677.95,0.00,0.00,0.00,0.00,0.00,19970609,'1',
20,75,0,'E','15AE9831',1882899000,2341,0,0,1,0,'000',
'-UR-NR.123/97','WENGORZ/TESTAMENT')!
insert into umsatz values (
'19970609',11530,'1880144000','AAAAAJAN10CI','AA10CI9831',
'ABABACAC200000','AK25090500','AE','ABABAB000004','..',
'0000000000',11,434.06,0.00,0.00,0.00,0.00,0.00,19970609,'0',
20,75,0,'I','10CI9831',1880144000,111880144,2400,0,1,0,'000',
'GEBURTSTAGSGELD','')!
insert into umsatz values (
'19970609',11531,'1880144000','AAAAAJAN10CI','AA10CI9831',
'ABAFAEAE240000','AK25090500','AE','ABABAB000004','..',
'0000000000',11,0.00,434.06,0.00,0.00,0.00,0.00,19970609,'0',
51,0,0,'I','10CI9831',1880144101,1880144,2000,0,0,1,'000',
'GEBURTSTAGSGELD','')!
insert into umsatz values (
'19970609',12533,'0999903000','AAAAACAG15AT','AA15AT9831',
'ABABACAC200000','AG25050000','AE','ABABAC010000','..',
'0000000000',12,683.00,0.00,0.00,0.00,0.00,0.00,19970609,'1',
20,75,0,'E','15AT9831',999903000,101313047,0,0,1,0,'000',
'5700820010902MFZ','57DEL-EC10/9')!
insert into umsatz values (
'19970609',12553,'1882538000','AAAAACAG15AT','AA15AT9831',
'ABABACAC200000','AC20010020','AE','ABABAB000004','..',
'0000000000',12,63.94,0.00,0.00,0.00,0.00,0.00,19970609,'1',
20,75,0,'E','15AT9831',1882538000,5392800201,0,0,1,0,'000',
'BAZ4221000706936','KTO422100070693')!
pars_then_ex !
data !
	select count(*)
		   from s_tg_me_pn
		  where tag_id = :suchkrit!
c 10 '19970609'

nodata !
	fetch into :anz_st!
data !
	select count(*)
		   from ( select count(*)
			    from umsatz 
		  	   where tag_id = :suchkrit
			   group by tag_id, medium_id, pn1_key )!
c 10 '19970609'

nodata!
	fetch into :anz_fk!
data !
	select u.tag_id, u.medium_id, u.pn1_key
		   from umsatz u
		   where tag_id = :suchkrit
		   group by tag_id, medium_id, pn1_key
		   having not exists ( select * from s_tg_me_pn st
					where st.tag_id = u.tag_id
					  and st.medium_id = u.medium_id
					  and st.pn1_key = u.pn1_key )!
c 10 '19970609'

nodata!
		fetch into 
			:tag_id, :medium_id, :pn1_key!
data !
	select st.tag_id, st.medium_id, st.pn1_key,
	                fk.uds, fk.udh, fk.ues, fk.ueh, fk.gd, fk.ge,
			fk.as, fk.ah,
			st.umsatz_dm_s, st.umsatz_dm_h,
			st.umsatz_euro_s, st.umsatz_euro_h,
			st.gebuehr_dm, st.gebuehr_euro, st.anzahl_s, 
                        st.anzahl_h
		   from s_tg_me_pn st,
			( select tag_id, medium_id, pn1_key,
				 sum(umsatz_dm_s) uds,sum(umsatz_dm_h) udh,
				 sum(umsatz_euro_s) ues,sum(umsatz_euro_h) ueh,
				 sum(gebuehr_dm) gd,sum(gebuehr_euro) ge,
				 sum(anzahl_s) as as,sum(anzahl_h) ah
			    from umsatz u
		   	   where u.tag_id = :suchkrit
		   	   group by u.tag_id, u.medium_id, u.pn1_key ) fk
		  where fk.tag_id = st.tag_id 
		    and fk.medium_id = st.medium_id
		    and fk.pn1_key = st.pn1_key
		    and (fk.uds != st.umsatz_dm_s or 
			 fk.udh != st.umsatz_dm_h or
			 fk.ues != st.umsatz_euro_s or
			 fk.ueh != st.umsatz_euro_h or
			 fk.gd  != st.gebuehr_dm or
			 fk.ge  != st.gebuehr_euro or
			 fk.as  != st.anzahl_s or
			 fk.ah  != st.anzahl_h)!
c 10 '19970609'

nodata!
		fetch into 
			:tag_id, :medium_id, :pn1_key,
	                :fk_uds, :fk_udh, :fk_ues, :fk_ueh, 
			:fk_gd, :fk_ge, :fk_as, :fk_ah,
	                :st_uds, :st_udh, :st_ues, :st_ueh, 
			:st_gd, :st_ge, :st_as, :st_ah!
pars_execute !
drop TABLE "S_TG_ME_PN"!
drop TABLE "UMSATZ"!
commit release !
* PTS 4831 !
* aus U1_5_14 erzeugt
file connect ( kern test !
*
CREATE TABLE TABX
( K FIXED(5),
  I FIXED(6),
  N FIXED(7),
  PRIMARY KEY( K,N ))!
INSERT TABX VALUES( 1, 1, 1)!
COMMIT!
*
CREATE TABLE TABY
( K FIXED(8),
  I FIXED(9),
  N FIXED(10),
  PRIMARY KEY( K,N ))!
INSERT TABY VALUES( 1, 1, 1)!
COMMIT!
*
CREATE TABLE TABZ
( K FIXED(11),
  I FIXED(12),
  N FIXED(13),
  PRIMARY KEY( K,N ))!
INSERT TABZ VALUES( 1, 1, 1)!
COMMIT!
select * from tabz !
fetch!
*
CREATE VIEW A1
AS SELECT K,I,N FROM TABY T2
   WHERE ( N, I) IN 
  (SELECT K, I FROM TABX WHERE N =  T2.K)!
select * from tabz !
fetch!

CREATE VIEW C1
AS SELECT K,I,N FROM TABY T0
WHERE (K,N) IN ( SELECT AVG(N),AVG(K) FROM TABZ
  GROUP BY I
  HAVING T0.I >= AVG(N))!
select * from tabz !
fetch!

SELECT * FROM A1!
FETCH!
select * from tabz !
fetch!
SELECT K,I,N FROM TABY T0
WHERE (K,N) IN ( SELECT AVG(N),AVG(K) FROM TABZ
  GROUP BY I
  HAVING T0.I >= AVG(N))!
FETCH!
commit!
select * from tabz !
fetch!
 
SELECT T1.K COL1, T1.N COL2 FROM TABZ T1
WHERE T1.K IN (SELECT T3.N FROM A1 T3)!
FETCH!
select * from tabz !
fetch!

SELECT K,I,N FROM C1 T0
WHERE (K,N) IN ( SELECT AVG(N),AVG(K) FROM TABY
  GROUP BY  I)!
FETCH!
 
*** hier kam -2010, obwohl gleiches Stmt. wie oben
SELECT T1.K COL1, T1.N COL2 FROM TABZ T1
WHERE T1.K IN (SELECT T3.N FROM A1 T3)!
FETCH!
drop table tabx !
drop table taby !
drop table tabz !
COMMIT WORK RELEASE!
* PTS 1000291 !
file connect ( kern test !
CREATE TABLE CH (
     START_LPNO       FIXED(10),
     STOP_LPNO        FIXED(10)
     )!
insert into ch values (123456, 123460)!
insert into ch values (234567, 234890)!
CREATE TABLE CR (
     CMD_ID          CHAR(15),
     REP_DATE        CHAR(5)  ASCII,
     REP_TIME        CHAR(8),
     LINE            CHAR(80) ASCII
     )!
insert cr values ('abcdefghij','12-12','17:30:00',
  'abcdefghijklmnopqrstuv 123456 zyxw')!
insert cr values ('abcdefghij','12-11','18:30:00',
  'abcdefghIJKLMNOPQRSTUV 123456 zyxw')!
insert cr values ('mnopqrstuv','12-10','19:30:00',
  'abcdefghIJKLMNOPQRSTUV 234567 zyxw')!
select rep_date, rep_time, line
from ch h, cr r
where r.cmd_id   = any (select cmd_id
                  from cr
                  where index(line,rtrim(chr(h.start_lpno)),20) > 0) !
fetch !
rollback release !
*PTS 1111975 Business warehouse !
file connect ( kern test !

CREATE TABLE RSUPDINFO
(
"UPDID"       Varchar (25) ASCII,
"OBJVERS"       Varchar (1) ASCII,
"INFOCUBE"       Varchar (30) ASCII,
"ISOURCE"       Varchar (30) ASCII,
"URPROGID"       Varchar (25) ASCII,
"WIPROGID"       Varchar (25) ASCII,
"TIMESTMPCUBE"       Fixed (15,0) DEFAULT 0,
"TSTPNM"       Varchar (12) ASCII,
"TIMESTMP"       Fixed (15,0)  DEFAULT 0,
"OBJSTAT"       Varchar (3) ASCII,
"CONTREL"       Varchar (6) ASCII,
"CONTTIMESTMP"       Fixed (15,0) DEFAULT 0,
"OWNER"       Varchar (12) ASCII,
"BWAPPL"       Varchar (10) ASCII,
"ACTIVFL"       Varchar (1) ASCII,
"EXPERTMODE"       Varchar (1) ASCII,
"STARTROUTINE"       Varchar (4) ASCII DEFAULT '0000',
"ODSPROGID"       Varchar (25) ASCII,
PRIMARY KEY ("UPDID", "OBJVERS")
)!

insert into RSUPDINFO values (
 '02KVB5MVIMSBCNFKCB0OK0J33',
 'D',
 '0QM_C03',
 '2LIS_07_S296',
 '06UBLTK29E9NMZSORMFQ8NTEN',
 '08Z1R5INMS0BS5Z8ZA592ZGKF',
 19990810174456,
 'SAP',
 19990810174457,
 'ACT',
 '2.0/01',
 0,
 'SAP',
 ' ',
 'X',
 ' ',
 '0000',
 ' ')!

SELECT                                                                                    
  *                                                                                       
FROM                                                                                      
  "RSUPDINFO" T_00                                                                        
WHERE                                                                                     
  T_00 . "OBJVERS" = 'D' AND  EXISTS ( 
     SELECT  T_100 . "INFOCUBE"  
     FROM  "RSUPDINFO" T_100  
     WHERE  T_100."INFOCUBE" = T_00."INFOCUBE" AND  
            T_100."ISOURCE" = T_00."ISOURCE" AND  
            T_100 . "OBJVERS" = 'D'                                 
     GROUP BY                                                                                  
       T_100 . "INFOCUBE" , T_100 . "ISOURCE"                                                  
     HAVING                                                                                    
      COUNT(*) >= 2 )                                                                         
ORDER BY                                                                                  
  "INFOCUBE" , "ISOURCE" !

fetch!

parsing !

SELECT                                                                                    
  *                                                                                       
FROM                                                                                      
  "RSUPDINFO" T_00                                                                        
WHERE                                                                                     
  T_00 . "OBJVERS" = ? AND  EXISTS ( 
     SELECT  T_100 . "INFOCUBE"  
     FROM  "RSUPDINFO" T_100  
     WHERE  T_100."INFOCUBE" = T_00."INFOCUBE" AND  
            T_100."ISOURCE" = T_00."ISOURCE" AND  
            T_100 . "OBJVERS" = ?                                  
     GROUP BY                                                                                  
       T_100 . "INFOCUBE" , T_100 . "ISOURCE"                                                  
     HAVING                                                                                    
      COUNT(*) >= ? )                                                                         
ORDER BY                                                                                  
  "INFOCUBE" , "ISOURCE" !

executing !
data !
select !
1
c 3 'D' c 3 'D' n 38 2

nodata !
pars_execute !
rollback work release !
* PTS 1116396-9 !

file connect ( sut sut !
create user mat2 password mat2 dba not exclusive!
commit release !

file connect ( mat2 mat2 !
create usergroup mm3 resource not exclusive!
create user mat3 password mat3 usergroup mm3!


CREATE TABLE "RABF_KOPF"          (
"AUFTRAG_ART       " CHAR      (1) ASCII    ,
"AUFTRAG_JAHR      " FIXED     (2,0) ,
"AUFTRAG_NR        " FIXED     (6,0) ,
"PROJEKT_STICHWORT " CHAR      (60) ASCII    ,
"PROJEKT_INHALT1   " CHAR      (60) ASCII    ,
"PROJEKT_INHALT2   " CHAR      (60) ASCII    ,
"DEBITOR_NR        " FIXED     (6,0) ,
"ANSCHRIFT_NR      " FIXED     (2,0) ,
"SPRACHE           " CHAR      (3) ASCII    ,
"WAEHRUNG          " CHAR      (3) ASCII    ,
"WECHSELKURS       " FLOAT     (8) ,
"REGION            " FIXED     (1,0) ,
"LAND              " CHAR      (3) ASCII    ,
"WERT_GUTS         " FIXED     (11,2) ,
"AUFTR_WERT_BRUTTO " FIXED     (11,2) ,
"AUFTR_WERT_NETTO  " FIXED     (11,2) ,
"AUFTR_FREW_NETTO  " FIXED     (14,2) ,
"POSITION_GESAMT   " FIXED     (5,0) ,
"POSITION_BERECHN  " FIXED     (5,0) ,
"L_AEND_INDEX      " FIXED     (2,0) ,
"AE_SACHBEARBEITER " CHAR      (8) ASCII    ,
"AE_DATUM          " DATE      ,
"AUFTR_BEREICH     " CHAR      (8) ASCII    ,
"AUFTR_ART_ABR     " CHAR      (1) ASCII    ,
"AUFTR_JAHR_ABR    " FIXED     (2,0) ,
"AUFTR_NR_ABR      " FIXED     (6,0) ,
"WECHSELKURS_ALT   " FLOAT     (16) ,
"AUFTR_WERT_BR_DM  " FIXED     (14,2) ,
"AUFTR_WERT_NE_DM  " FIXED     (14,2)
, PRIMARY KEY ("AUFTRAG_ART"
,"AUFTRAG_JAHR"
,"AUFTRAG_NR"
))!

CREATE TABLE "RABF_FKOPF"         (
"AUFTRAG_ART       " CHAR      (1) ASCII    ,
"AUFTRAG_JAHR      " FIXED     (2,0) ,
"AUFTRAG_NR        " FIXED     (6,0) ,
"AEND_INDEX        " FIXED     (2,0) ,
"AUFTR_ART_ANG     " CHAR      (1) ASCII    ,
"AUFTR_JAHR_ANG    " FIXED     (2,0) ,
"AUFTR_NR_ANG      " FIXED     (6,0) ,
"AEND_INDEX_ANG    " FIXED     (2,0) ,
"DATUM             " DATE      ,
"ZEICHEN           " CHAR      (15) ASCII    ,
"TEXT1             " CHAR      (50) ASCII    ,
"TEXT2             " CHAR      (50) ASCII    ,
"FKOPF_WERT_BRUTTO " FIXED     (11,2) ,
"FKOPF_WERT_NETTO  " FIXED     (11,2) ,
"FKOPF_FREW_NETTO  " FIXED     (14,2) ,
"GEWICHT           " FIXED     (11,3) ,
"VERTRETER_KZ      " CHAR      (1) ASCII    ,
"VERTRETER_NR      " FIXED     (6,0) ,
"PROV_SCHL         " FIXED     (3,0) ,
"PROV_WERT         " FIXED     (11,2) ,
"DATUM_ERFASSUNG   " DATE      ,
"DATUM_DRUCK       " DATE      ,
"AE_SACHBEARBEITER " CHAR      (8) ASCII    ,
"AE_DATUM          " DATE      ,
"FKOPF_WERT_BR_DM  " FIXED     (14,2) ,
"FKOPF_WERT_NE_DM  " FIXED     (14,2)
, PRIMARY KEY ("AUFTRAG_ART"
,"AUFTRAG_JAHR"
,"AUFTRAG_NR"
,"AEND_INDEX"
))!

CREATE TABLE "RABF_POS"           (
"AUFTRAG_ART       " CHAR      (1) ASCII    ,
"AUFTRAG_JAHR      " FIXED     (2,0) ,
"AUFTRAG_NR        " FIXED     (6,0) ,
"AEND_INDEX        " FIXED     (2,0) ,
"AUFTR_POSITION    " FIXED     (4,0) ,
"LFD_NR            " FIXED     (3,0) ,
"IDENT_NR          " FIXED     (10,0) ,
"ORDN_NR           " FIXED     (4,0) ,
"VERTEIL_SCHL      " FIXED     (4,0) ,
"PROD_GRP          " FIXED     (4,0) ,
"KUNDEN_POS        " CHAR      (15) ASCII    ,
"INTERNE_POS       " CHAR      (15) ASCII    ,
"BERECH_MODUS      " CHAR      (1) ASCII    ,
"SONSTIGES_KZ      " CHAR      (1) ASCII    ,
"SONSTIGES         " CHAR      (20) ASCII    ,
"ZEICH_NR          " CHAR      (20) ASCII    ,
"PREIS_BASIS       " FIXED     (4,0) ,
"MENGE_BESTELLT    " FIXED     (9,3) ,
"ART_WERT_BRUTTO   " FIXED     (11,2) ,
"ART_FREW_BRUTTO   " FIXED     (11,2) ,
"POS_WERT_BRUTTO   " FIXED     (11,2) ,
"POS_WERT_NETTO    " FIXED     (11,2) ,
"POS_WERT_VORAB    " FIXED     (11,2) ,
"POS_FREW_NETTO    " FIXED     (14,2) ,
"GEWICHT           " FIXED     (9,3) ,
"MENGE_LIEF        " FIXED     (9,3) ,
"MENGE_FAKT        " FIXED     (9,3) ,
"MENGE_GUTS        " FIXED     (9,3) ,
"LIEF_WERT_NETTO   " FIXED     (14,2) ,
"FAKT_WERT_NETTO   " FIXED     (14,2) ,
"GUTS_WERT_NETTO   " FIXED     (11,2) ,
"VERARBEITUNGS_KZ  " CHAR      (1) ASCII    ,
"WAREN_NR          " FIXED     (9,0) ,
"MENGEN_EINHEIT    " CHAR      (4) ASCII    ,
"AM_LEISTE         " FIXED     (6,0) ,
"BERECH_ART        " CHAR      (1) ASCII    ,
"AE_SACHBEARBEITER " CHAR      (8) ASCII    ,
"AE_DATUM          " DATE      ,
"WECHSELKURS       " FLOAT     (16) ,
"WECHSELKURS_ALT   " FLOAT     (16) ,
"ART_WERT_BRUTTO_DM" FIXED     (14,2) ,
"POS_WERT_BRUTTO_DM" FIXED     (14,2) ,
"POS_WERT_NETTO_DM " FIXED     (14,2) ,
"POS_WERT_VORAB_DM " FIXED     (14,2) ,
"LIEF_WERT_NETTO_DM" FIXED     (14,2) ,
"FAKT_WERT_NETTO_DM" FIXED     (14,2) ,
"GUTS_WERT_NETTO_DM" FIXED     (14,2) ,
"ZUORDNUNG" CHAR      (1) ASCII    ,
"PROD_BEREICH" CHAR      (1) ASCII    ,
"KURZ_PROD_GRP" FIXED     (2,0)
, PRIMARY KEY ("AUFTRAG_ART"
,"AUFTRAG_JAHR"
,"AUFTRAG_NR"
,"AEND_INDEX"
,"AUFTR_POSITION"
,"LFD_NR"
))!

grant all on rabf_kopf to mm3!
grant all on rabf_fkopf to mm3!
grant all on rabf_pos to mm3!

SELECT auftrag_art , auftrag_jahr , auftrag_nr , 
aend_index , auftr_art_ang , 
auftr_jahr_ang , auftr_nr_ang , aend_index_ang , 
datum , zeichen , text1 , text2 , 
fkopf_wert_brutto , 
fkopf_wert_netto , fkopf_frew_netto , gewicht , 
vertreter_kz , vertreter_nr , 
prov_schl , prov_wert , datum_erfassung , 
datum_druck , ae_sachbearbeiter , 
ae_datum FROM rabf_fkopf 
WHERE rabf_fkopf.auftrag_art = '6' AND auftrag_jahr = 2 AND 
auftrag_nr = 10 AND NOT EXISTS 
( SELECT RABF_POS.auftr_position FROM RABF_POS 
WHERE rabf_pos.auftrag_art = '6' AND 
auftrag_jahr = 2 AND auftrag_nr = 10 AND 
AEND_index = RABF_FKOPF.AEND_index )!

commit release !

file connect ( mat3 mat3 !
SELECT auftrag_art , auftrag_jahr , auftrag_nr , 
aend_index , auftr_art_ang , 
auftr_jahr_ang , auftr_nr_ang , aend_index_ang , 
datum , zeichen , text1 , text2 , 
fkopf_wert_brutto , 
fkopf_wert_netto , fkopf_frew_netto , gewicht , 
vertreter_kz , vertreter_nr , 
prov_schl , prov_wert , datum_erfassung , 
datum_druck , ae_sachbearbeiter , 
ae_datum FROM rabf_fkopf 
WHERE rabf_fkopf.auftrag_art = '6' AND auftrag_jahr = 2 AND 
auftrag_nr = 10 AND NOT EXISTS 
( SELECT RABF_POS.auftr_position FROM RABF_POS rabf_pos
WHERE rabf_pos.auftrag_art = '6' AND 
rabf_pos.auftrag_jahr = 2 AND rabf_pos.auftrag_nr = 10 AND 
rabf_pos.AEND_index = RABF_FKOPF.AEND_index )!

SELECT auftrag_art , auftrag_jahr , auftrag_nr , 
aend_index , auftr_art_ang , 
auftr_jahr_ang , auftr_nr_ang , aend_index_ang , 
datum , zeichen , text1 , text2 , 
fkopf_wert_brutto , 
fkopf_wert_netto , fkopf_frew_netto , gewicht , 
vertreter_kz , vertreter_nr , 
prov_schl , prov_wert , datum_erfassung , 
datum_druck , ae_sachbearbeiter , 
ae_datum FROM rabf_fkopf 
WHERE rabf_fkopf.auftrag_art = '6' AND auftrag_jahr = 2 AND 
auftrag_nr = 10 AND NOT EXISTS 
( SELECT RABF_POS.auftr_position FROM RABF_POS 
WHERE rabf_pos.auftrag_art = '6' AND 
auftrag_jahr = 2 AND auftrag_nr = 10 AND 
AEND_index = RABF_FKOPF.AEND_index )!

SELECT auftrag_art , auftrag_jahr , auftrag_nr , 
aend_index , auftr_art_ang , 
auftr_jahr_ang , auftr_nr_ang , aend_index_ang , 
datum , zeichen , text1 , text2 , 
fkopf_wert_brutto , 
fkopf_wert_netto , fkopf_frew_netto , gewicht , 
vertreter_kz , vertreter_nr , 
prov_schl , prov_wert , datum_erfassung , 
datum_druck , ae_sachbearbeiter , 
ae_datum FROM rabf_fkopf 
WHERE rabf_fkopf.auftrag_art = '6' AND auftrag_jahr = 2 AND 
auftrag_nr = 10 AND NOT EXISTS 
( SELECT RABF_POS.auftr_position FROM RABF_POS rabf_pos
WHERE rabf_pos.auftrag_art = '6' AND 
rabf_pos.auftrag_jahr = 2 AND rabf_pos.auftrag_nr = 10 AND 
rabf_pos.AEND_index = RABF_FKOPF.AEND_index )!
rollback release !

file connect ( mat2 mat2 !
drop user mat3 !
drop usergroup mm3 !
commit release !
file connect ( sut sut !
drop user mat2 !
commit release !