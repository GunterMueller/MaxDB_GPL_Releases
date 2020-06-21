file CONNECT ( KERN TEST !
CREATE TABLE T ( A CHAR (8), B CHAR (9), C CHAR (10), D CHAR (11))!
INSERT T VALUES ('aaa', 'bbb', 'ccc', 'ddd')!
CREATE TABLE T1 (E CHAR (8), F CHAR(9), G CHAR (10))!
INSERT T1 VALUES ('eee', 'fff', 'ggg')!
CREATE INDEX I1 ON T (C)!
CREATE INDEX I2 ON T (A,B,C)!
CREATE INDEX I3 ON T (D) !
commit !
explain SELECT * FROM <T1, T> WHERE E=A AND F = B !
SELECT * FROM <T1, T> WHERE E=A AND F = B !
fetch !
insert t values ('eee', 'fff', '', 'ddd')!
SELECT * FROM T, T1 WHERE E=A AND F = B !
fetch !
insert t values ('eee', 'fff', 'hhh', 'ddd')!
SELECT * FROM T, T1 WHERE E=A AND F = B !
fetch !
drop table t !
drop table t1 !
commit work !
CREATE TABLE T ( A CHAR (30), B CHAR (30), C CHAR (30), D CHAR (30))!
INSERT T VALUES ('aaa', 'bbb', 'ccc', 'ddd')!
CREATE TABLE T1 (E CHAR (30), F CHAR(30), G CHAR (32))!
INSERT T1 VALUES ('eee', 'fff', 'ggg')!
COMMIT !
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', '          nein', '            nein', '        nein')!
INSERT T VALUES ('eee', 'fff', 'hhh', 'dd1')!
INSERT T VALUES ('eee', 'fff', 'hhh', 'dd2')!
INSERT T VALUES ('eee', 'fff', 'hhh', 'dd3')!
INSERT T VALUES ('eee', 'fff', 'hhh', '001')!
INSERT T VALUES ('eee', 'fff', 'hhh', '002')!
INSERT T VALUES ('eee', 'fff', 'hhh', '003')!
INSERT T VALUES ('eee', 'fff', 'hhh', '004')!
INSERT T VALUES ('eee', 'fff', 'hhh', '005')!
INSERT T VALUES ('eee', 'fff', 'hhh', '006')!
INSERT T VALUES ('eee', 'fff', 'hhh', '007')!
INSERT T VALUES ('eee', 'fff', 'hhh', '008')!
INSERT T VALUES ('eee', 'fff', 'hhh', '009')!
INSERT T VALUES ('eee', 'fff', 'hhh', '010')!
INSERT T VALUES ('eee', 'fff', 'hhh', '011')!
INSERT T VALUES ('eee', 'fff', 'hhh', '012')!
INSERT T VALUES ('eee', 'fff', 'hhh', '013')!
INSERT T VALUES ('eee', 'fff', 'hhh', '014')!
INSERT T VALUES ('eee', 'fff', 'hhh', '015')!
INSERT T VALUES ('eee', 'fff', 'hhh', '016')!
INSERT T VALUES ('eee', 'fff', 'hhh', '017')!
INSERT T VALUES ('eee', 'fff', 'hhh', '018')!
INSERT T VALUES ('eee', 'fff', 'hhh', '019')!
INSERT T VALUES ('eee', 'fff', 'hhh', '020')!
INSERT T VALUES ('eee', 'fff', 'hhh', '021')!
INSERT T VALUES ('eee', 'fff', 'hhh', '022')!
INSERT T VALUES ('eee', 'fff', 'hhh', '023')!
INSERT T VALUES ('eee', 'fff', 'hhh', '024')!
INSERT T VALUES ('eee', 'fff', 'hhh', '025')!
INSERT T VALUES ('eee', 'fff', 'hhh', '026')!
INSERT T VALUES ('eee', 'fff', 'hhh', '027')!
INSERT T VALUES ('eee', 'fff', 'hhh', '028')!
INSERT T VALUES ('eee', 'fff', 'hhh', '029')!
INSERT T VALUES ('eee', 'fff', 'hhh', '030')!
INSERT T VALUES ('eee', 'fff', 'hhh', '031')!
INSERT T VALUES ('eee', 'fff', 'hhh', '032')!
INSERT T VALUES ('eee', 'fff', 'hhh', '033')!
INSERT T VALUES ('eee', 'fff', 'hhh', '034')!
INSERT T VALUES ('eee', 'fff', 'hhh', '035')!
INSERT T VALUES ('eee', 'fff', 'hhh', '036')!
INSERT T VALUES ('eee', 'fff', 'hhh', '037')!
INSERT T VALUES ('eee', 'fff', 'hhh', '038')!
INSERT T VALUES ('eee', 'fff', 'hhh', '039')!
INSERT T VALUES ('eee', 'fff', 'hhh', '040')!
INSERT T VALUES ('eee', 'fff', 'hhh', '041')!
INSERT T VALUES ('eee', 'fff', 'hhh', '042')!
INSERT T VALUES ('eee', 'fff', 'hhh', '043')!
INSERT T VALUES ('eee', 'fff', 'hhh', '044')!
INSERT T VALUES ('eee', 'fff', 'hhh', '045')!
INSERT T VALUES ('eee', 'fff', 'hhh', '046')!
INSERT T VALUES ('eee', 'fff', 'hhh', '047')!
INSERT T VALUES ('eee', 'fff', 'hhh', '048')!
INSERT T VALUES ('eee', 'fff', 'hhh', '049')!
INSERT T VALUES ('eee', 'fff', 'hhh', '050')!
INSERT T VALUES ('eee', 'fff', 'hhh', '051')!
INSERT T VALUES ('eee', 'fff', 'hhh', '052')!
INSERT T VALUES ('eee', 'fff', 'hhh', '053')!
INSERT T VALUES ('eee', 'fff', 'hhh', '054')!
INSERT T VALUES ('eee', 'fff', 'hhh', '055')!
INSERT T VALUES ('eee', 'fff', 'hhh', '056')!
INSERT T VALUES ('eee', 'fff', 'hhh', '057')!
INSERT T VALUES ('eee', 'fff', 'hhh', '058')!
INSERT T VALUES ('eee', 'fff', 'hhh', '059')!
INSERT T VALUES ('eee', 'fff', 'hhh', '060')!
INSERT T VALUES ('eee', 'fff', 'hhh', '061')!
INSERT T VALUES ('eee', 'fff', 'hhh', '062')!
INSERT T VALUES ('eee', 'fff', 'hhh', '063')!
INSERT T VALUES ('eee', 'fff', 'hhh', '064')!
INSERT T VALUES ('eee', 'fff', 'hhh', '065')!
INSERT T VALUES ('eee', 'fff', 'hhh', '066')!
INSERT T VALUES ('eee', 'fff', 'hhh', '067')!
INSERT T VALUES ('eee', 'fff', 'hhh', '068')!
INSERT T VALUES ('eee', 'fff', 'hhh', '069')!
INSERT T VALUES ('eee', 'fff', 'hhh', '070')!
INSERT T VALUES ('eee', 'fff', 'hhh', '071')!
INSERT T VALUES ('eee', 'fff', 'hhh', '072')!
INSERT T VALUES ('eee', 'fff', 'hhh', '073')!
INSERT T VALUES ('eee', 'fff', 'hhh', '074')!
INSERT T VALUES ('eee', 'fff', 'hhh', '075')!
INSERT T VALUES ('eee', 'fff', 'hhh', '076')!
INSERT T VALUES ('eee', 'fff', 'hhh', '077')!
INSERT T VALUES ('eee', 'fff', 'hhh', '078')!
INSERT T VALUES ('eee', 'fff', 'hhh', '079')!
INSERT T VALUES ('eee', 'fff', 'hhh', '080')!
INSERT T VALUES ('eee', 'fff', 'hhh', '081')!
INSERT T VALUES ('eee', 'fff', 'hhh', '082')!
INSERT T VALUES ('eee', 'fff', 'hhh', '083')!
INSERT T VALUES ('eee', 'fff', 'hhh', '084')!
INSERT T VALUES ('eee', 'fff', 'hhh', '085')!
INSERT T VALUES ('eee', 'fff', 'hhh', '086')!
INSERT T VALUES ('eee', 'fff', 'hhh', '087')!
INSERT T VALUES ('eee', 'fff', 'hhh', '088')!
INSERT T VALUES ('eee', 'fff', 'hhh', '089')!
INSERT T VALUES ('eee', 'fff', 'hhh', '090')!
INSERT T VALUES ('eee', 'fff', 'hhh', '091')!
INSERT T VALUES ('eee', 'fff', 'hhh', '092')!
INSERT T VALUES ('eee', 'fff', 'hhh', '093')!
INSERT T VALUES ('eee', 'fff', 'hhh', '094')!
INSERT T VALUES ('eee', 'fff', 'hhh', '095')!
INSERT T VALUES ('eee', 'fff', 'hhh', '096')!
INSERT T VALUES ('eee', 'fff', 'hhh', '097')!
INSERT T VALUES ('eee', 'fff', 'hhh', '098')!
INSERT T VALUES ('eee', 'fff', 'hhh', '099')!
INSERT T VALUES ('eee', 'fff', 'hhh', '100')!
INSERT T VALUES ('eee', 'fff', 'hhh', 'zz2')!
INSERT T VALUES ('eee', 'ffz', 'hhh', 'zzz')!
INSERT T VALUES ('eez', 'fff', 'hhh', 'zzz')!
commit work !
declare res cursor for
SELECT * FROM T, T1 WHERE E=A AND F = B !
CREATE INDEX I1 ON T (C)!
CREATE INDEX I2 ON T (A,B,C)!
CREATE INDEX I3 ON T (D) !
commit work !
explain SELECT * FROM T, T1 WHERE E=A AND F = B !
SELECT * FROM T, T1 WHERE E=A AND F = B 
except all
SELECT * from res !
fetch !
drop table t !
drop table t1 !
commit work !
CREATE TABLE T ( A CHAR (30), B CHAR (30), C CHAR (30), D CHAR (30))!
INSERT T VALUES ('aaa', 'bbb', 'ccc', 'ddd')!
CREATE TABLE T1 (E CHAR (30), F CHAR(31), G CHAR (32))!
INSERT T1 VALUES ('eee', 'fff', 'ggg')!
COMMIT !
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', '          nein', '            nein', '        nein')!
INSERT T VALUES ('eee', 'fff', 'hhh', 'dd1')!
INSERT T VALUES ('eee', 'fff', 'hhh', 'dd2')!
INSERT T VALUES ('eee', 'fff', 'hhh', 'dd3')!
INSERT T VALUES ('eee', 'fff', 'hhh', '001')!
INSERT T VALUES ('eee', 'fff', 'hhh', '002')!
INSERT T VALUES ('eee', 'fff', 'hhh', '003')!
INSERT T VALUES ('eee', 'fff', 'hhh', '004')!
INSERT T VALUES ('eee', 'fff', 'hhh', '005')!
INSERT T VALUES ('eee', 'fff', 'hhh', '006')!
INSERT T VALUES ('eee', 'fff', 'hhh', '007')!
INSERT T VALUES ('eee', 'fff', 'hhh', '008')!
INSERT T VALUES ('eee', 'fff', 'hhh', '009')!
INSERT T VALUES ('eee', 'fff', 'hhh', '010')!
INSERT T VALUES ('eee', 'fff', 'hhh', '011')!
INSERT T VALUES ('eee', 'fff', 'hhh', '012')!
INSERT T VALUES ('eee', 'fff', 'hhh', '013')!
INSERT T VALUES ('eee', 'fff', 'hhh', '014')!
INSERT T VALUES ('eee', 'fff', 'hhh', '015')!
INSERT T VALUES ('eee', 'fff', 'hhh', '016')!
INSERT T VALUES ('eee', 'fff', 'hhh', '017')!
INSERT T VALUES ('eee', 'fff', 'hhh', '018')!
INSERT T VALUES ('eee', 'fff', 'hhh', '019')!
INSERT T VALUES ('eee', 'fff', 'hhh', '020')!
INSERT T VALUES ('eee', 'fff', 'hhh', '021')!
INSERT T VALUES ('eee', 'fff', 'hhh', '022')!
INSERT T VALUES ('eee', 'fff', 'hhh', '023')!
INSERT T VALUES ('eee', 'fff', 'hhh', '024')!
INSERT T VALUES ('eee', 'fff', 'hhh', '025')!
INSERT T VALUES ('eee', 'fff', 'hhh', '026')!
INSERT T VALUES ('eee', 'fff', 'hhh', '027')!
INSERT T VALUES ('eee', 'fff', 'hhh', '028')!
INSERT T VALUES ('eee', 'fff', 'hhh', '029')!
INSERT T VALUES ('eee', 'fff', 'hhh', '030')!
INSERT T VALUES ('eee', 'fff', 'hhh', '031')!
INSERT T VALUES ('eee', 'fff', 'hhh', '032')!
INSERT T VALUES ('eee', 'fff', 'hhh', '033')!
INSERT T VALUES ('eee', 'fff', 'hhh', '034')!
INSERT T VALUES ('eee', 'fff', 'hhh', '035')!
INSERT T VALUES ('eee', 'fff', 'hhh', '036')!
INSERT T VALUES ('eee', 'fff', 'hhh', '037')!
INSERT T VALUES ('eee', 'fff', 'hhh', '038')!
INSERT T VALUES ('eee', 'fff', 'hhh', '039')!
INSERT T VALUES ('eee', 'fff', 'hhh', '040')!
INSERT T VALUES ('eee', 'fff', 'hhh', '041')!
INSERT T VALUES ('eee', 'fff', 'hhh', '042')!
INSERT T VALUES ('eee', 'fff', 'hhh', '043')!
INSERT T VALUES ('eee', 'fff', 'hhh', '044')!
INSERT T VALUES ('eee', 'fff', 'hhh', '045')!
INSERT T VALUES ('eee', 'fff', 'hhh', '046')!
INSERT T VALUES ('eee', 'fff', 'hhh', '047')!
INSERT T VALUES ('eee', 'fff', 'hhh', '048')!
INSERT T VALUES ('eee', 'fff', 'hhh', '049')!
INSERT T VALUES ('eee', 'fff', 'hhh', '050')!
INSERT T VALUES ('eee', 'fff', 'hhh', '051')!
INSERT T VALUES ('eee', 'fff', 'hhh', '052')!
INSERT T VALUES ('eee', 'fff', 'hhh', '053')!
INSERT T VALUES ('eee', 'fff', 'hhh', '054')!
INSERT T VALUES ('eee', 'fff', 'hhh', '055')!
INSERT T VALUES ('eee', 'fff', 'hhh', '056')!
INSERT T VALUES ('eee', 'fff', 'hhh', '057')!
INSERT T VALUES ('eee', 'fff', 'hhh', '058')!
INSERT T VALUES ('eee', 'fff', 'hhh', '059')!
INSERT T VALUES ('eee', 'fff', 'hhh', '060')!
INSERT T VALUES ('eee', 'fff', 'hhh', '061')!
INSERT T VALUES ('eee', 'fff', 'hhh', '062')!
INSERT T VALUES ('eee', 'fff', 'hhh', '063')!
INSERT T VALUES ('eee', 'fff', 'hhh', '064')!
INSERT T VALUES ('eee', 'fff', 'hhh', '065')!
INSERT T VALUES ('eee', 'fff', 'hhh', '066')!
INSERT T VALUES ('eee', 'fff', 'hhh', '067')!
INSERT T VALUES ('eee', 'fff', 'hhh', '068')!
INSERT T VALUES ('eee', 'fff', 'hhh', '069')!
INSERT T VALUES ('eee', 'fff', 'hhh', '070')!
INSERT T VALUES ('eee', 'fff', 'hhh', '071')!
INSERT T VALUES ('eee', 'fff', 'hhh', '072')!
INSERT T VALUES ('eee', 'fff', 'hhh', '073')!
INSERT T VALUES ('eee', 'fff', 'hhh', '074')!
INSERT T VALUES ('eee', 'fff', 'hhh', '075')!
INSERT T VALUES ('eee', 'fff', 'hhh', '076')!
INSERT T VALUES ('eee', 'fff', 'hhh', '077')!
INSERT T VALUES ('eee', 'fff', 'hhh', '078')!
INSERT T VALUES ('eee', 'fff', 'hhh', '079')!
INSERT T VALUES ('eee', 'fff', 'hhh', '080')!
INSERT T VALUES ('eee', 'fff', 'hhh', '081')!
INSERT T VALUES ('eee', 'fff', 'hhh', '082')!
INSERT T VALUES ('eee', 'fff', 'hhh', '083')!
INSERT T VALUES ('eee', 'fff', 'hhh', '084')!
INSERT T VALUES ('eee', 'fff', 'hhh', '085')!
INSERT T VALUES ('eee', 'fff', 'hhh', '086')!
INSERT T VALUES ('eee', 'fff', 'hhh', '087')!
INSERT T VALUES ('eee', 'fff', 'hhh', '088')!
INSERT T VALUES ('eee', 'fff', 'hhh', '089')!
INSERT T VALUES ('eee', 'fff', 'hhh', '090')!
INSERT T VALUES ('eee', 'fff', 'hhh', '091')!
INSERT T VALUES ('eee', 'fff', 'hhh', '092')!
INSERT T VALUES ('eee', 'fff', 'hhh', '093')!
INSERT T VALUES ('eee', 'fff', 'hhh', '094')!
INSERT T VALUES ('eee', 'fff', 'hhh', '095')!
INSERT T VALUES ('eee', 'fff', 'hhh', '096')!
INSERT T VALUES ('eee', 'fff', 'hhh', '097')!
INSERT T VALUES ('eee', 'fff', 'hhh', '098')!
INSERT T VALUES ('eee', 'fff', 'hhh', '099')!
INSERT T VALUES ('eee', 'fff', 'hhh', '100')!
INSERT T VALUES ('eee', 'fff', 'hhh', 'zz2')!
INSERT T VALUES ('eee', 'ffz', 'hhh', 'zzz')!
INSERT T VALUES ('eez', 'fff', 'hhh', 'zzz')!
commit work !
declare res cursor for
SELECT * FROM T, T1 WHERE E=A AND F = B !
CREATE INDEX I1 ON T (C)!
CREATE INDEX I2 ON T (A,B,C)!
CREATE INDEX I3 ON T (D) !
commit work !
explain SELECT * FROM T, T1 WHERE E=A AND F = B !
SELECT * FROM T, T1 WHERE E=A AND F = B 
except all
SELECT * from res !
fetch !
drop table t !
drop table t1 !
commit work !
CREATE TABLE T ( A CHAR (30), B CHAR (31), C CHAR (30), D CHAR (30))!
INSERT T VALUES ('aaa', 'bbb', 'ccc', 'ddd')!
CREATE TABLE T1 (E CHAR (30), F CHAR(30), G CHAR (32))!
INSERT T1 VALUES ('eee', 'fff', 'ggg')!
COMMIT !
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', 'fff', '', 'ddd')!
INSERT T VALUES ('eee', '          nein', '            nein', '        nein')!
INSERT T VALUES ('eee', 'fff', 'hhh', 'dd1')!
INSERT T VALUES ('eee', 'fff', 'hhh', 'dd2')!
INSERT T VALUES ('eee', 'fff', 'hhh', 'dd3')!
INSERT T VALUES ('eee', 'fff', 'hhh', '001')!
INSERT T VALUES ('eee', 'fff', 'hhh', '002')!
INSERT T VALUES ('eee', 'fff', 'hhh', '003')!
INSERT T VALUES ('eee', 'fff', 'hhh', '004')!
INSERT T VALUES ('eee', 'fff', 'hhh', '005')!
INSERT T VALUES ('eee', 'fff', 'hhh', '006')!
INSERT T VALUES ('eee', 'fff', 'hhh', '007')!
INSERT T VALUES ('eee', 'fff', 'hhh', '008')!
INSERT T VALUES ('eee', 'fff', 'hhh', '009')!
INSERT T VALUES ('eee', 'fff', 'hhh', '010')!
INSERT T VALUES ('eee', 'fff', 'hhh', '011')!
INSERT T VALUES ('eee', 'fff', 'hhh', '012')!
INSERT T VALUES ('eee', 'fff', 'hhh', '013')!
INSERT T VALUES ('eee', 'fff', 'hhh', '014')!
INSERT T VALUES ('eee', 'fff', 'hhh', '015')!
INSERT T VALUES ('eee', 'fff', 'hhh', '016')!
INSERT T VALUES ('eee', 'fff', 'hhh', '017')!
INSERT T VALUES ('eee', 'fff', 'hhh', '018')!
INSERT T VALUES ('eee', 'fff', 'hhh', '019')!
INSERT T VALUES ('eee', 'fff', 'hhh', '020')!
INSERT T VALUES ('eee', 'fff', 'hhh', '021')!
INSERT T VALUES ('eee', 'fff', 'hhh', '022')!
INSERT T VALUES ('eee', 'fff', 'hhh', '023')!
INSERT T VALUES ('eee', 'fff', 'hhh', '024')!
INSERT T VALUES ('eee', 'fff', 'hhh', '025')!
INSERT T VALUES ('eee', 'fff', 'hhh', '026')!
INSERT T VALUES ('eee', 'fff', 'hhh', '027')!
INSERT T VALUES ('eee', 'fff', 'hhh', '028')!
INSERT T VALUES ('eee', 'fff', 'hhh', '029')!
INSERT T VALUES ('eee', 'fff', 'hhh', '030')!
INSERT T VALUES ('eee', 'fff', 'hhh', '031')!
INSERT T VALUES ('eee', 'fff', 'hhh', '032')!
INSERT T VALUES ('eee', 'fff', 'hhh', '033')!
INSERT T VALUES ('eee', 'fff', 'hhh', '034')!
INSERT T VALUES ('eee', 'fff', 'hhh', '035')!
INSERT T VALUES ('eee', 'fff', 'hhh', '036')!
INSERT T VALUES ('eee', 'fff', 'hhh', '037')!
INSERT T VALUES ('eee', 'fff', 'hhh', '038')!
INSERT T VALUES ('eee', 'fff', 'hhh', '039')!
INSERT T VALUES ('eee', 'fff', 'hhh', '040')!
INSERT T VALUES ('eee', 'fff', 'hhh', '041')!
INSERT T VALUES ('eee', 'fff', 'hhh', '042')!
INSERT T VALUES ('eee', 'fff', 'hhh', '043')!
INSERT T VALUES ('eee', 'fff', 'hhh', '044')!
INSERT T VALUES ('eee', 'fff', 'hhh', '045')!
INSERT T VALUES ('eee', 'fff', 'hhh', '046')!
INSERT T VALUES ('eee', 'fff', 'hhh', '047')!
INSERT T VALUES ('eee', 'fff', 'hhh', '048')!
INSERT T VALUES ('eee', 'fff', 'hhh', '049')!
INSERT T VALUES ('eee', 'fff', 'hhh', '050')!
INSERT T VALUES ('eee', 'fff', 'hhh', '051')!
INSERT T VALUES ('eee', 'fff', 'hhh', '052')!
INSERT T VALUES ('eee', 'fff', 'hhh', '053')!
INSERT T VALUES ('eee', 'fff', 'hhh', '054')!
INSERT T VALUES ('eee', 'fff', 'hhh', '055')!
INSERT T VALUES ('eee', 'fff', 'hhh', '056')!
INSERT T VALUES ('eee', 'fff', 'hhh', '057')!
INSERT T VALUES ('eee', 'fff', 'hhh', '058')!
INSERT T VALUES ('eee', 'fff', 'hhh', '059')!
INSERT T VALUES ('eee', 'fff', 'hhh', '060')!
INSERT T VALUES ('eee', 'fff', 'hhh', '061')!
INSERT T VALUES ('eee', 'fff', 'hhh', '062')!
INSERT T VALUES ('eee', 'fff', 'hhh', '063')!
INSERT T VALUES ('eee', 'fff', 'hhh', '064')!
INSERT T VALUES ('eee', 'fff', 'hhh', '065')!
INSERT T VALUES ('eee', 'fff', 'hhh', '066')!
INSERT T VALUES ('eee', 'fff', 'hhh', '067')!
INSERT T VALUES ('eee', 'fff', 'hhh', '068')!
INSERT T VALUES ('eee', 'fff', 'hhh', '069')!
INSERT T VALUES ('eee', 'fff', 'hhh', '070')!
INSERT T VALUES ('eee', 'fff', 'hhh', '071')!
INSERT T VALUES ('eee', 'fff', 'hhh', '072')!
INSERT T VALUES ('eee', 'fff', 'hhh', '073')!
INSERT T VALUES ('eee', 'fff', 'hhh', '074')!
INSERT T VALUES ('eee', 'fff', 'hhh', '075')!
INSERT T VALUES ('eee', 'fff', 'hhh', '076')!
INSERT T VALUES ('eee', 'fff', 'hhh', '077')!
INSERT T VALUES ('eee', 'fff', 'hhh', '078')!
INSERT T VALUES ('eee', 'fff', 'hhh', '079')!
INSERT T VALUES ('eee', 'fff', 'hhh', '080')!
INSERT T VALUES ('eee', 'fff', 'hhh', '081')!
INSERT T VALUES ('eee', 'fff', 'hhh', '082')!
INSERT T VALUES ('eee', 'fff', 'hhh', '083')!
INSERT T VALUES ('eee', 'fff', 'hhh', '084')!
INSERT T VALUES ('eee', 'fff', 'hhh', '085')!
INSERT T VALUES ('eee', 'fff', 'hhh', '086')!
INSERT T VALUES ('eee', 'fff', 'hhh', '087')!
INSERT T VALUES ('eee', 'fff', 'hhh', '088')!
INSERT T VALUES ('eee', 'fff', 'hhh', '089')!
INSERT T VALUES ('eee', 'fff', 'hhh', '090')!
INSERT T VALUES ('eee', 'fff', 'hhh', '091')!
INSERT T VALUES ('eee', 'fff', 'hhh', '092')!
INSERT T VALUES ('eee', 'fff', 'hhh', '093')!
INSERT T VALUES ('eee', 'fff', 'hhh', '094')!
INSERT T VALUES ('eee', 'fff', 'hhh', '095')!
INSERT T VALUES ('eee', 'fff', 'hhh', '096')!
INSERT T VALUES ('eee', 'fff', 'hhh', '097')!
INSERT T VALUES ('eee', 'fff', 'hhh', '098')!
INSERT T VALUES ('eee', 'fff', 'hhh', '099')!
INSERT T VALUES ('eee', 'fff', 'hhh', '100')!
INSERT T VALUES ('eee', 'fff', 'hhh', 'zz2')!
INSERT T VALUES ('eee', 'ffz', 'hhh', 'zzz')!
INSERT T VALUES ('eez', 'fff', 'hhh', 'zzz')!
commit work !
declare res cursor for
SELECT * FROM T, T1 WHERE E=A AND F = B !
CREATE INDEX I1 ON T (C)!
CREATE INDEX I2 ON T (A,B,C)!
CREATE INDEX I3 ON T (D) !
commit work !
explain SELECT * FROM T, T1 WHERE E=A AND F = B !
SELECT * FROM T, T1 WHERE E=A AND F = B 
except all
SELECT * from res !
fetch !
drop table t !
drop table t1 !
commit release !
