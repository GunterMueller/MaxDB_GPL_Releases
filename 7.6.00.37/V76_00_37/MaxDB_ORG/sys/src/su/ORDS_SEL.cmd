*   *ID* ORDS-SEL CMD      changed on 1994-02-08-12.32.57 by KRISCHAN  *

file CONNECT ( kern test SQLMODE ORACLE!

CREATE TABLE datatest
(
  a_char             CHAR                       NOT NULL,
  a_character        CHARACTER     DEFAULT 'a',
  a_char_2           CHAR      (2) DEFAULT 'b'  NOT NULL,
  a_character_2      CHARACTER (2),
  a_date             DATE,
  a_number           NUMBER,
  a_number_ast       NUMBER (*)  DEFAULT 1 NOT NULL,
  a_number_18        NUMBER (18),
  a_integer          INTEGER     DEFAULT 3 NOT NULL,
  a_int              INT         DEFAULT 4 NOT NULL,
  a_smallint         SMALLINT    DEFAULT 5,
  a_dec              DEC         DEFAULT 6,
  a_dec_ast          DEC (*)     NOT NULL,
  a_dec_8            DEC (8)     NOT NULL,
  a_decimal          DECIMAL     DEFAULT 6,
  a_decimal_ast      DECIMAL (*) NOT NULL,
  a_decimal_8        DECIMAL (8) NOT NULL,
  a_double_precision DOUBLE PRECISION,
  a_real             REAL,
  a_float            FLOAT,
  a_float_ast        FLOAT (*),
  a_float_9          FLOAT (9),
  a_number_ast_2     NUMBER  (*,2) default 1 not null,
  a_number_6_2       NUMBER  (6,2) DEFAULT 2 NOT NULL,
  a_dec_ast_2        DEC     (*,2) DEFAULT 33 NOT NULL,
  a_dec_6_2          DEC     (6,2) DEFAULT 44 NOT NULL,
  a_decimal_ast_2    DECIMAL (*,2) DEFAULT 555 NOT NULL,
  a_decimal_6_2      DECIMAL (6,2) DEFAULT 666 NOT NULL,
  a_numeric_6_2      NUMERIC (6,2) DEFAULT 7777 NOT NULL,
  a_raw_2            RAW (2) NOT NULL,
  a_varchar_2        VARCHAR (2) DEFAULT 6 NOT NULL
)!

ALTER TABLE datatest MODIFY a_char CHAR!

SELECT        SUBSTR (column_name,    1,20) column_name,
        SUBSTR (data_type,      1,9)  type,
                data_length           length,
                data_precision        prec,
                data_scale            scale,
                nullable,
                column_id             id,
                default_length        dlg
        FROM all_tab_columns
        WHERE table_name = 'DATATEST'!
FETCH INTO :a, :b, :c, :d, :e, :f, :g, :h!

DROP TABLE datatest!
COMMIT WORK!

* A little information about all existent users and more infos about myself.
* We should avoid to select the creation date, because this will give
* differences every night. We also don't select user_id, since in a
* distributed test this is also different - and *please* no user DUMMY!
SELECT SUBSTR (username, 1, 30) username
  FROM all_users
  WHERE username NOT LIKE 'S%'
    AND username <> 'DUMMY'
  ORDER BY 1!
FETCH INTO :a!
SELECT SUBSTR (username, 1, 30) username,
       default_tablespace, temporary_tablespace
FROM user_users!
FETCH INTO :a, :b, :c!

* A alphabetical list of all columns in the form <tableName>.<columnName> !
SELECT SUBSTR (RTRIM (table_name) || '.' ||
               RTRIM (column_name), 1, 64) column_name
  FROM all_tab_columns
  WHERE owner = 'KERN'
  ORDER BY table_name, column_name!
FETCH INTO :a!

* All tables of kern with its column count!
SELECT SUBSTR (table_name, 1, 30) table_name,
       COUNT  (column_name)       column_count
  FROM all_tab_columns
  WHERE owner = 'KERN'
  GROUP BY table_name
  ORDER BY table_name!
FETCH INTO :a, :b!

* All tables of kern without index (there are none)!
SELECT COUNT (table_name) FROM all_tables
  WHERE table_name NOT IN (SELECT table_name FROM all_indexes)
    AND owner = 'KERN'!
FETCH INTO :a!

* Just to be sure that it's switched off!
SELECT * FROM all_def_audit_opts!
FETCH INTO :a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k,:l,:m!

* Although auditing is switched off (look above), in this table there are
* rows, which describe the meaning of the action codes.
SELECT name FROM audit_actions WHERE action = 1!
FETCH INTO :a!

* Setting of the comments for a columns or table is easy, but querying isn't!
SELECT table_name, '--- Table ----' column_name, comments
  FROM user_tab_comments WHERE comments IS NOT NULL
UNION ALL
SELECT table_name, column_name, comments
  FROM user_col_comments WHERE comments IS NOT NULL!
FETCH INTO :a, :b, :c!

* It's just a simple view, so here comes a little more complex condition!
SELECT SUBSTR (owner,      1, 30) owner,
       SUBSTR (table_name, 1, 30) table_name,
       table_type
  FROM all_catalog
  WHERE table_name LIKE 'ART%'
     OR (table_type = 'VIEW' AND owner = 'KERN')!
FETCH INTO :a, :b, :c!

* 'cat' is a view for 'user_catalog' is a view for 'all_catalog' is a 
* view for 'sysdd.tables', and that is still not really a table...!
SELECT SUBSTR (table_name, 1, 30) table_name, table_type 
  FROM cat WHERE table_type = 'SEQUENCE'!
FETCH INTO :a, :b!

* Here too we better not select the date, since this will yield
* differences for sure!
SELECT grantee, owner, table_name, column_name, grantor,
       privilege, grantable
  FROM all_col_privs_made
  WHERE owner = 'KERN'!
FETCH INTO :a, :b, :c, :d, :e, :f, :g!

* In SQL-DB there is no need for this stuff, so let's let it emtpy!
SELECT COUNT (*) FROM all_db_links!
FETCH INTO :a!

SELECT owner, index_name, table_owner, table_name, table_type,
       uniqueness, distinct_keys
  FROM all_indexes WHERE owner = 'KERN' ORDER BY index_name!
FETCH INTO :a, :b, :c, :d, :e, :f, :g!

* The following simple looking query was a killer for a long time, because
* the view contains a join and tons of function calls (like decode or rpad)!
SELECT * FROM all_ind_columns WHERE index_owner = 'KERN'!
FETCH INTO :a, :b, :c, :d, :e, :f, :g, :h!

* We must be sure, that the synonyms doesn't sneek into this view!
SELECT count (*) FROM all_tables
  WHERE owner = 'KERN' AND table_name LIKE 'TEIL%'!
FETCH INTO :a!

* And now some simple queries for our own database objects!
SELECT * FROM user_sequences!
FETCH INTO :a, :b, :c, :d, :e, :f, :g, :h!
SELECT synonym_name, table_owner, table_name FROM user_synonyms!
FETCH INTO :a, :b, :c!

SELECT * FROM all_views WHERE owner = 'KERN'!
FETCH INTO :a, :b, :c, :d, :e, :f, :g, :h, :i, :j!

* Remember: No date!
SELECT grantee, owner, table_name, column_name, grantor,
       update_priv, references_priv
  FROM column_privileges
  WHERE owner = 'KERN'!
FETCH INTO :a, :b, :c, :d, :e, :f!

* A question for all DBAs: A list of all users with the number of tables
* they own, ordered by this number; the point is that we have to list the
* users without tables too!
SELECT SUBSTR (owner, 1, 30) "User",
       COUNT (*)             "Table count"
  FROM sys.dba_tables
  WHERE owner NOT LIKE 'S%' AND owner <> 'DUMMY'
  GROUP BY owner
UNION ALL
SELECT SUBSTR (username, 1, 30) "User",
       0                        "Table count"
  FROM sys.dba_users
  WHERE username NOT IN (SELECT owner FROM sys.dba_tables)
  ORDER BY 2 DESC!
FETCH INTO :a, :b!

* A typical hacker-query: Which table can i crack? But please shut up with
* the dd-views!
SELECT DISTINCT table_name FROM all_col_privs_recd
UNION ALL
SELECT table_name FROM all_tab_privs_recd
MINUS
SELECT table_name FROM dict!
FETCH INTO :a!

COMMIT WORK!

* Here we are not interested in the data dictionary views, which also
* appeared in this views. This select is really slow in oracle 6.0!
SELECT SUBSTR (grantee,    1, 30) grantee,
       SUBSTR (owner,      1, 30) owner,
       SUBSTR (table_name, 1, 30) table_name,
       '--- Table ---'            column_name,
       SUBSTR (grantor,    1, 30) grantor,
       privilege, grantable
  FROM all_tab_privs_recd
  WHERE table_name NOT IN (SELECT table_name FROM dict)
    AND owner <> 'DUMMY'
UNION ALL
SELECT SUBSTR (grantee,     1, 30) grantee,
       SUBSTR (owner,       1, 30) owner,
       SUBSTR (table_name,  1, 30) table_name,
       SUBSTR (column_name, 1, 30) column_name,
       SUBSTR (grantor,     1, 30) grantor,
       privilege, grantable
  FROM all_col_privs_recd
  WHERE table_name NOT IN (SELECT table_name FROM dict)
    AND owner <> 'DUMMY'
ORDER BY 3, 4, 1, 2!
FETCH INTO :a, :b, :c, :d, :e, :f, :g!

* Now we look for the table with the most columns, ignoring the dd-views.
* Because a from-select isn't allowed in oracle mode, we have to create
* a temporary result table with a column count of the tables!
CREATE TABLE col_counts AS
  SELECT owner, table_name, COUNT (column_name) col_count
    FROM all_tab_columns
    WHERE owner NOT LIKE 'S%' AND owner <> 'DUMMY'
    GROUP BY owner, table_name!
SELECT owner, table_name, COUNT (column_name) col_count
  FROM all_tab_columns
  WHERE owner NOT LIKE 'S%' AND owner <> 'DUMMY'
  GROUP BY owner, table_name
  HAVING COUNT (column_name) = (SELECT MAX (col_count) FROM col_counts)!
FETCH INTO :a, :b, :c!
DROP TABLE col_counts!

* And now a list of all the indexes with some additional information
* about its components!
SELECT ind.index_name,   ind.uniqueness,       ind.table_name,
       icol.column_name, icol.column_position,
       tcol.data_type,   tcol.data_precision,  tcol.data_scale
  FROM user_indexes ind, user_ind_columns icol, user_tab_columns tcol
  WHERE (ind.table_name,  ind.table_name,  ind.index_name, icol.column_name)
    IN ((icol.table_name, tcol.table_name, icol.index_name, tcol.column_name))
  ORDER BY 3, 1, 5!
FETCH INTO :a, :b, :c, :d, :e, :f, :g, :h!

* The following query looks for primary key columns of all tables of the
* connected user. But it doesn't find any (at least in SQL-DB), because
* the primary keys aren't listed yet in *_constraints. Sorry for that.
* In SQL-DB you should better query
*   SELECT tablename, columnname, keycolumnno FROM sysdd.columns
*     WHERE keycolumnno IS NOT NULL ORDER BY 1, 3!
* and that works even faster!
SELECT cols.table_name, cols.column_name, cols.position
  FROM user_cons_columns cols, user_constraints cons
  WHERE (cons.constraint_name, cons.constraint_type)
    IN ((cols.constraint_name, 'P'))
  ORDER BY 1, 3!

* Let's create a nice list of all checks for columns!
SELECT cols.table_name, cols.column_name,
       cons.constraint_name, cons.search_condition
  FROM user_constraints cons, user_cons_columns cols
  WHERE (cons.table_name, cons.constraint_name)
    IN ((cols.table_name, cols.constraint_name))
    AND cons.search_condition IS NOT NULL
  ORDER BY 1, 2!
FETCH INTO :a, :b, :c, :d!

* Are there differences in all_catalog and all_objects? Since I looked in
* the oracle 6.0 view, I know that in all_objects are indixes but in
* all_catalog not! We compute the union of both tables and then subtract
* the intersection of both, so that only the differences come through!
(SELECT table_name FROM all_catalog
   UNION     SELECT object_name FROM all_objects)
MINUS
(SELECT table_name FROM all_catalog
   INTERSECT SELECT object_name FROM all_objects)!
FETCH INTO :a!

* And here a select with a where condition containing an oracle special
* comparison of a value list with a subselect!
SELECT owner, table_name, column_name,
       data_type, data_length, data_precision, data_scale,
       nullable, column_id, default_length, data_default
  FROM all_tab_columns
  WHERE (owner, table_name, column_name) IN
    (SELECT table_owner, table_name, column_name FROM all_ind_columns)!
FETCH INTO :a,:b,:c,:d,:e,:f,:g,:h,:i,:j,:k!

COMMIT WORK!

* And now some selects for a commented description of the ddview
* SYS.USER_SNAPSHOT_LOG and SYS.DBA_SNAPSHOT_LOG!
CREATE TABLE dictcols (
  table_name  CHAR   (64),
  column_no   NUMBER ( 3),
  column_name CHAR   (64),
  data_type   CHAR   ( 9),
  data_length NUMBER ( 4),
  comments    CHAR   (254),
  PRIMARY KEY (table_name, column_no))!
INSERT INTO dictcols 
  SELECT d.table_name, 0, NULL, NULL, NULL, d.comments
    FROM dictionary d!
INSERT INTO dictcols 
  SELECT d.table_name, c.column_id,   d.column_name,
         c.data_type,  c.data_length, d.comments
    FROM all_tab_columns c, dict_columns d
    WHERE c.table_name  = d.table_name
      AND c.column_name = d.column_name!
SELECT DECODE (column_no, 0, table_name, '-->'||column_name),
       data_type, data_length, comments
  FROM dictcols WHERE table_name LIKE '%_SNAPSHOT_LOGS'!
FETCH INTO :a, :b, :c, :d!
DROP TABLE dictcols!

* Fehlermeldung Michael/4 und Rocco-SAP/21.4.95!
SELECT ROWNUM FROM user_tables!
FETCH INTO :A!
SELECT * FROM user_tables WHERE ROWNUM <= 1!
FETCH INTO ?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,
           ?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?!
PARS_THEN_EX!
SELECT table_name INTO :a FROM user_tables WHERE table_name = 'GibtsNicht'!
PARS_EXECUTE!

SELECT 'ich darf eine Tabelle anlegen'
  FROM session_privs
  WHERE privilege = 'CREATE TABLE'!
SELECT 'ich darf ueberall eine Tabelle anlegen'
  FROM session_privs
  WHERE privilege = 'CREATE ANY TABLE'!
COMMIT WORK RELEASE!

file CONNECT ( normalo pet SQLMODE ORACLE!
SELECT 'ich darf eine Tabelle anlegen'
  FROM session_privs
  WHERE privilege = 'CREATE TABLE'!
SELECT 'ich darf ueberall eine Tabelle anlegen'
  FROM session_privs
  WHERE privilege = 'CREATE ANY TABLE'!
COMMIT WORK RELEASE!
