*   *ID* DERASE   DBCMD    changed on 1994-03-24-19.40.00 by heinz     *
 *** Bereichs-DELETE fuer Sekundaerschluessel wird getestet.
 ***********************************************************!
DECLARE SEL_MON CURSOR FOR
 SELECT DESCRIPTION,
        FIXED(VALUE,11,0) "VALUE"
   FROM SYSDBA.MONITOR_ROW 
  WHERE NOT (DESCRIPTION LIKE '%temp%') !
FETCH SEL_MON INTO :A, :B !
CLOSE SEL_MON !
commit release!
dbmcli superdba,colduser !
sql_connect kern,test !
sql_execute diagnose index namedind on kern.RanDelTst!
sql_release !
bye !
file connect ( kern test!
file DERA2!
