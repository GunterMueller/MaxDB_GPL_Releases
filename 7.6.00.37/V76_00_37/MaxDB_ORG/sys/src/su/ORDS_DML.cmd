*   *ID* ORDS-DML CMD      changed on 1994-02-08-12.15.57 by KRISCHAN  *

file CONNECT ( kern test SQLMODE ORACLE!

* Nun kommen ein paar Einfuegungen in den Artikelstamm!

INSERT INTO artikel (nummer, name) VALUES (1, 'Kopfartikel')!
INSERT INTO artikel (nummer, name) VALUES (2, 'Posartikel 1')!
INSERT INTO artikel (nummer, name) VALUES (3, 'Posartikel 2')!
INSERT INTO artikel (nummer)       VALUES (4)!
UPDATE artikel SET wert = 10 WHERE nummer=2!
UPDATE artikel SET wert = 20 WHERE nummer=3!

* Nun kommen ein paar Einfuegungen in die st_liste!
INSERT INTO st_liste (bauk, position, stueck) VALUES (5, 1, 1)!
INSERT INTO st_liste (bauk, position, stueck) VALUES (1, 1, 1)!
INSERT INTO st_liste (bauk, position, stueck) VALUES (1, 1, 2)!
INSERT INTO st_liste (bauk, position, stueck) VALUES (1, 1, 3)!
INSERT INTO st_liste (bauk, position, stueck) VALUES (1, 2, 3)!
UPDATE st_liste SET anzahl=1 WHERE bauk=1 AND position=1!
UPDATE st_liste SET anzahl=2 WHERE bauk=1 AND position=2!

* Ein paar Einfuegungen per Join-View in die Tabellen!
* Die gehen leider nicht in Oracle 6.0, deshalb sind sie hier auskommentiert
* und als Einfuegungen in die Basistabellen realisiert!
*INSERT INTO bauk VALUES (10, 'Kopf-Test1', 1, 11, 'Pos-Test1')!
*INSERT INTO bauk VALUES (10, 'Kopf-Test1', 2, 12, 'Pos-Test2')!
*INSERT INTO bauk VALUES (10, 'Kopf-Test1', 2, 12, 'Pos-Test2')!
*INSERT INTO bauk VALUES (10, 'Kopf-Test1', 3, 12, 'Pos-Test3')!
INSERT INTO artikel (nummer, name) VALUES (10, 'Kopf-Test1')!
INSERT INTO artikel (nummer, name) VALUES (11, 'Pos-Test1')!
INSERT INTO artikel (nummer, name) VALUES (12, 'Pos-Test2')!
INSERT INTO st_liste (bauk, position, stueck) VALUES (10, 1, 11)!
INSERT INTO st_liste (bauk, position, stueck) VALUES (10, 2, 12)!

UPDATE st_liste SET anzahl=2 WHERE bauk=10 AND position=1!
COMMIT WORK!

* Und nun ein paar Loeschungen!
DELETE FROM artikel     WHERE nummer = 2!
DELETE FROM st_liste WHERE stueck = 2!
DELETE FROM artikel     WHERE nummer = 2!

ROLLBACK WORK RELEASE!

