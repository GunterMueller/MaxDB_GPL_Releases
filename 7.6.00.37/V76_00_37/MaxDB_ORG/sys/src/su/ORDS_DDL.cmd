*   *ID* ORDS-DDL CMD      changedfon 1994-02-08-12.15.57 by KRISCHAN  *

* DDL (Data-Definition-Language)!

file CONNECT ( kern test SQLMODE ORACLE!

CREATE TABLE artikel
(
  nummer    NUMBER(6) PRIMARY KEY,
  name      CHAR(20)  DEFAULT 'Kein Name' CHECK (name <> 'Leerteil'),
  zeichnung CHAR(20)  DEFAULT 'Ohne Zeichnung'
)!
ALTER TABLE artikel ADD wert NUMBER (5)!
CREATE INDEX zeichnung ON artikel (zeichnung, nummer)!

CREATE TABLE st_liste
(
  bauk     NUMBER(6),
  position INTEGER,
  stueck   NUMBER(6) CONSTRAINT st_n_null NOT NULL,
  CONSTRAINT st_n_bauk CHECK (stueck <> bauk),
  CHECK (position < 999),
  PRIMARY KEY (bauk, position),
  FOREIGN KEY (bauk)   REFERENCES artikel (nummer),
  FOREIGN KEY (stueck) REFERENCES artikel (nummer)
)!
ALTER TABLE st_liste ADD anzahl NUMBER (5)!

CREATE UNIQUE INDEX verwendung ON st_liste (stueck, bauk, position)!

CREATE VIEW bauk (bauknr, baukname,
                       posnr, posinr, posiname)
  AS SELECT kopf.nummer, kopf.name,
          position, posi.nummer, posi.name
       FROM artikel kopf, st_liste, artikel posi
       WHERE st_liste.bauk   = kopf.nummer
         AND st_liste.stueck = posi.nummer
  WITH CHECK OPTION!

COMMENT ON TABLE  artikel                   IS 'Bauteile'!
COMMENT ON COLUMN artikel.nummer            IS 'Eindeutige Artikelnummer'!

CREATE SYNONYM teil FOR artikel!

GRANT UPDATE (nummer, name) ON artikel     TO PUBLIC!
GRANT UPDATE (nummer, name) ON artikel     TO PUBLIC!
GRANT SELECT                ON st_liste    TO PUBLIC!

CREATE SEQUENCE teilenummer!

COMMIT WORK RELEASE!

* Let's create a second user and connect as it to have the possibility of
* testing the all_*_grant_recd tables!
file CONNECT ( sut sut!
CREATE USER kern2 PASSWORD test2 DBA!
COMMIT WORK RELEASE!

file CONNECT ( kern2 test2 SQLMODE ORACLE!
CREATE TABLE t2 (a2 CHAR (2), b2 INT)!
GRANT INSERT      ON t2 TO kern!
GRANT SELECT      ON t2 TO PUBLIC!
GRANT UPDATE (b2) ON t2 TO kern!
GRANT UPDATE (a2) ON t2 TO PUBLIC!
COMMIT WORK RELEASE!
