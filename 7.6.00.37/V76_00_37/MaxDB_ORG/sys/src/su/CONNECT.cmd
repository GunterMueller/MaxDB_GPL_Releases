*   *ID* CONNECT  DBCMD    changed on 1992-09-28-13.22.57 by ADMIN     *
*
*  fuer den Test der Isolation Levels ungleich '1' wird die '1'
*  jeweils vor dem Testlauf automatisch geaendert.
*
*  Bitte deshalb diese Datei nicht veraendern !!!!!
*
CONNECT &1 IDENTIFIED BY &2 &3 &4 ISOLATION LEVEL 1 !
*
*  Isolation Level 15 gibt es nur fuer SQLMODE INTERNAL, deshalb wird
*  ein erneutes Connect versucht. So laufen dann die meisten Tests
*  wenigstens einigermassen fehlerfrei durch.
*
IF &rc <> 0 THEN BEGIN
   IF &rc = -3017 THEN BEGIN
*
 *---> es erfolgt ein erneutes Connect, das den durch falschen
       ISOLATION LEVEL hervorgerufenen Fehler vertuschen soll !
CONNECT &1 IDENTIFIED BY &2 &3 &4 ISOLATION LEVEL 1 !
*
   END
END
