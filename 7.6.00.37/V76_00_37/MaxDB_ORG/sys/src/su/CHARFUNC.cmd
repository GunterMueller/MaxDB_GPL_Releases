file connect ( kern test &2 &3 !

SELECT id, '''' || ALPHA     ( &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || ASCII     ( &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || CHR       ( &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id,  
     CASE &1 WHEN '' THEN '=empty' ELSE '<>empty' END e from t !
  fetch into :id, :a !
SELECT id,  
     CASE &1 WHEN ' ' THEN '=1space' ELSE '<>1space' END e from t !
  fetch into :id, :a !
SELECT id,  
     CASE WHEN &1 = '' THEN '=empty' ELSE '<>empty' END e from t !
  fetch into :id, :a !
SELECT id,  
     CASE WHEN &1 = ' ' THEN '=1space' ELSE '<>1space' END e from t !
  fetch into :id, :a !
SELECT id,  
     CASE '' WHEN &1 THEN '=empty' ELSE '<>empty' END e from t !
  fetch into :id, :a !
SELECT id,  
     CASE ' ' WHEN &1 THEN '=1space' ELSE '<>1space' END e from t !
  fetch into :id, :a !
SELECT id, 
     CASE WHEN '' = &1 THEN '=empty' ELSE '<>empty' END e from t !
  fetch into :id, :a !
SELECT id, 
     CASE WHEN ' ' = &1 THEN '=1space' ELSE '<>1space' END e from t !
  fetch into :id, :a !
SELECT id, '''' || 
     CASE WHEN '' = &1 THEN '' ELSE '<>empty' END || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || 
     CASE WHEN ' ' <> &1 THEN '<>1space' ELSE ' ' END || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || &1 || '''' e from t !
  fetch into :id, :a !
SELECT id, CONCAT ( CONCAT ('''', &1 ), '''') e from t !
  fetch into :id, :a !
SELECT id, DECODE    ( &1, '', 'empty', '<>empty' ) e from t !
  fetch into :id, :a !
SELECT id, DECODE    ( &1, ' ', '1space', '<>1space' ) e from t !
  fetch into :id, :a !
SELECT id, '''' || EXPAND    ( &1,44 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || GREATEST  ( &1, cr ) || '''' e from t, cr_t !
  fetch into :id, :a !
SELECT id, '''' || HEX       ( &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, x'C4' || HEXTORAW  ( &1 ) || x'DF' e from t !
  fetch into :id, :a !
SELECT id,         INDEX     ( &1, ' ' ) e from t !
  fetch into :id, :a !
SELECT id,         INDEX     ( &1, '' ) e from t !
  fetch into :id, :a !
SELECT id,         INDEX     ( '', &1 ) e from t !
  fetch into :id, :a !
SELECT id,         INDEX     ( 'abc efg', &1 ) e from t !
  fetch into :id, :a !
SELECT id, '''' || INITCAP   ( &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || LEAST     ( &1, cr ) || '''' e from t, cr_t !
  fetch into :id, :a !
SELECT id,         LENGTH    ( &1 ) e from t !
  fetch into :id, :a !
SELECT id, '''' || LFILL     ( &1, 'a', 3 ) || '''' e from t !
  fetch into :id, :a !
SELECT id,         LOCATE    ( &1, ' ' ) e from t !
  fetch into :id, :a !
SELECT id,         LOCATE    ( &1, '' ) e from t !
  fetch into :id, :a !
SELECT id,         LOCATE    ( &1, 'a b' ) e from t !
  fetch into :id, :a !
SELECT id,         LOCATE    ( '', &1 ) e from t !
  fetch into :id, :a !
SELECT id,         LOCATE    ( 'a', &1 ) e from t !
  fetch into :id, :a !
SELECT id, '''' || LOWER     ( &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || LPAD      ( &1, 3, 'a' ) || '''' e from t !  
  fetch into :id, :a !
SELECT id, '''' || LPAD      ( &1, 3, ' ' ) || '''' e from t !  
  fetch into :id, :a !
SELECT id, '''' || LTRIM     ( &1, 'ab' ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || LTRIM     ( &1, ' ' ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || LTRIM     ( ' a', &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || MAPCHAR   ( &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || REPLACE   ( &1, ' ', 'space' ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || REPLACE   ( 'a b', &1, 's' ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || REPLACE   ( 'a b', 'a', &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || RFILL     ( &1, 'a', 3 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || RPAD      ( &1, 3, 'a' ) || '''' e from t !  
  fetch into :id, :a !
SELECT id, '''' || RPAD      ( &1, 3, ' ' ) || '''' e from t !  
  fetch into :id, :a !
SELECT id, '''' || RTRIM     ( &1, 'a b' ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || RTRIM     ( &1, ' ' ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || RTRIM     ( 'a ', &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || SOUNDEX   ( &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || SUBSTR    ( &1, 1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || SUBSTR    ( &1, 1, 1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || SUBSTRING ( &1, 1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || SUBSTRING ( &1, 1, 1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || TRANSLATE ( &1, ' ', 's' ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || TRANSLATE ( 'a b', &1, 's' ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || TRANSLATE ( 'a b', 'a', &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || TRIM      ( &1, 'a b' ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || TRIM      ( &1, ' ' ) || '''' e from t !
  fetch into :id, :a ! 
SELECT id, '''' || TRIM      ( ' a ', &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || UPPER     ( &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || VALUE     ( &1, 'a' ) || '''' e from t !
  fetch into :id, :a !
rollback release !

 * file connect does not allow as many options !
CONNECT KERN IDENTIFIED BY TEST SQLMODE ORACLE &2 &3 !  
SELECT id,         ASCII     ( &1 ) e from t !
  fetch into :id, :a !
SELECT id,         INSTR     ( &1, ' ' ) e from t !
  fetch into :id, :a !
SELECT id,         INSTR     ( &1, '' ) e from t !
  fetch into :id, :a !
SELECT id,         INSTR     ( '', &1 ) e from t !
  fetch into :id, :a !
SELECT id,         INSTR     ( 'abc efg', &1 ) e from t !
  fetch into :id, :a !
SELECT id, '''' || LPAD      ( &1, 3, 'a' ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || NVL       ( &1, 'a' ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || RPAD      ( &1, 3, 'a' ) || '''' e from t !
  fetch into :id, :a !
SELECT id,         VSIZE     ( &1 ) e from t !
  fetch into :id, :a !
rollback release !

senderid ODB !
file connect ( kern test &2 &3 !
SELECT id, '''' || IFNULL    ( &1, 'null' ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || IFNULL    ( ascii(null), &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || LCASE     ( &1 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || LEFT      ( &1, 3 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || RIGHT     ( &1, 3 ) || '''' e from t !
  fetch into :id, :a !
SELECT id, '''' || UCASE     ( &1 ) || '''' e from t ! 
  fetch into :id, :a !
senderid XCI !
rollback release !
