*   *ID* N99A31   DBCMD    changed on 1992-09-28-13.27.21 by ADMIN     *
file connect ( kern test !
CREATE TABLE obj
(OBJTYPE        CHAR(18) ASCII KEY,
 "OWNER"        CHAR(64) ASCII KEY DEFAULT USER,
 NAME1          CHAR(64) ASCII KEY DEFAULT ' ',
 NAME2          CHAR(64) ASCII KEY DEFAULT ' ',
 NAME3          CHAR(18) ASCII KEY DEFAULT ' ',
 SUBTYPE        CHAR(12) ASCII KEY DEFAULT ' ',
 "VERSION"      CHAR(4)  ASCII KEY DEFAULT ' ',
 DATE_1         DATE DEFAULT DATE,
 DATE_2         DATE DEFAULT DATE,
 DATE_3         DATE DEFAULT DATE,
 TIME_1         TIME DEFAULT TIME,
 TIME_2         TIME DEFAULT TIME,
 TIME_3         TIME DEFAULT TIME,
 FIXED3_1       FIXED(3),
 FIXED3_2       FIXED(3),
 FIXED3_3       FIXED(3),
 FIXED3_4       FIXED(3),
 FIXED3_5       FIXED(3),
 FIXED18_1      FIXED(18),
 FIXED18_2      FIXED(18),
 CHAR1_1        CHAR(1) ASCII,
 CHAR1_2        CHAR(1) ASCII,
 CHAR2_1        CHAR(2) ASCII,
 CHAR3_1        CHAR(3) ASCII,
 CHAR3_2        CHAR(3) ASCII,
 CHAR6_1        CHAR(6) ASCII,
 CHAR6_2        CHAR(6) ASCII,
 CHAR8_1        CHAR(8) ASCII,
 CHAR8_2        CHAR(8) ASCII,
 CHAR10_1       CHAR(10) ASCII,
 CHAR10_2       CHAR(10) ASCII,
 CHAR10_3       CHAR(10) ASCII,
 CHAR10_4       CHAR(10) ASCII,
 CHAR10_5       CHAR(10) ASCII,
 CHAR12_1       CHAR(12) ASCII,
 CHAR18_1       CHAR(18) ASCII,
 CHAR40_1       CHAR(40) ASCII,
 CHAR64_1       CHAR(64) ASCII,
 CHAR80_1       CHAR(80) ASCII,
 CHAR80_2       CHAR(80) ASCII,
 CHAR254_1      CHAR(254) ASCII,
 CHAR254_2      CHAR(254) ASCII,
 STRING_1       LONG ASCII,
 STRING_2       LONG ASCII,
 STRING_3       LONG ASCII)!
CREATE VIEW obj_column
  (OBJTYPE, "OWNER", TABLENAME, COLUMNNAME, NAME3, SUBTYPE, "VERSION",
  LONGNAME, FRAC, "LENGTH", "MODE", DATATYPE, CODETYPE,
   "DEFAULT", "RANGE", CREATEDATE, CREATETIME, UPDDATE, UPDTIME)
AS SELECT
   OBJTYPE, "OWNER", NAME1, NAME2, NAME3, SUBTYPE, "VERSION",
   CHAR40_1, FIXED3_1, FIXED3_2, CHAR3_1, CHAR10_1, CHAR6_2,
   CHAR254_1, CHAR254_2, DATE_1, TIME_1, DATE_2, TIME_2
FROM obj
WHERE OBJTYPE = 'COLUMN'
WITH CHECK OPTION!
alter table obj add (xxx char (10), ccc char (20),
xxxxxx char (1))!
pars_then_ex !
data !
INSERT obj_column
     SET
     objtype ='COLUMN', owner =:authid, tablename =:name,
     columnname =:cname(i), name3  =' ', version =' ',
     frac =:dec(i), length =:len(i), mode =:cmod(i),
     datatype =:datatype(i), codetype =:codetype(i),
     default =:dbdefault(i), range =:dbrange(i)!
c 64 'elke' c 64 'test' c 64 'firstcol' n 3 0
n 3 8 c 3 'opt' c 10 'char' c 6 'ascii '
c 254
c 254

nodata !
pars_execute !
alter table obj add (newcol char (8), newfixed fixed (12))!
select * from obj !
fetch !
drop table obj !
commit release !
