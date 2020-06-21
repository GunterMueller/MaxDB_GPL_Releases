
explain 
select /*+ordered*/ * from t1, t2 where b1 = 'value' and b2 = s2 and sl = bl !
select /*+ordered*/ * from t1, t2 where b1 = 'value' and b2 = s2 and sl = bl !
fetch !
explain 
select /*+ordered*/ * from t1, t2 where b2 = s2 and b1 = 'value' and sl = bl !
select /*+ordered*/ * from t1, t2 where b2 = s2 and b1 = 'value' and sl = bl !
fetch !
 * In den Isolationlevels 2 und 3 liefert der Join "nur" Tablescan anstatt
   "JOIN VIA MULTIPLE INDEXED COLUMNS". Aufgrund einer bestehenden 
   Tabellensperre erfolgen keine Eintragungen in die SYSUPDSTAT - Tabelle,
   die implizit ueber update statistics ausgewertet wuerden. !
   
explain 
select /*+ordered*/ * from t1, t2 where b1 = s1 and b2 = 'value' and sl = bl !
select /*+ordered*/ * from t1, t2 where b1 = s1 and b2 = 'value' and sl = bl !
fetch !
explain 
select /*+ordered*/ * from t1, t2 where b1 = s1 and bl = 'value' and s2 = b2 !
select /*+ordered*/ * from t1, t2 where b1 = s1 and bl = 'value' and s2 = b2 !
fetch !
explain 
select /*+ordered*/ * from t1, t2 where b1 = s1 and 'value' = b2 and sl = bl !
select /*+ordered*/ * from t1, t2 where b1 = s1 and 'value' = b2 and sl = bl !
FETCH !
