
file CONNECT ( kern test SQLMODE ORACLE!

* Here come some more tables for the articles: orders and stores!

UPDATE artikel SET wert = nummer+20 WHERE wert IS NULL!

CREATE TABLE auftrag
(
  auftnum   NUMBER (6) PRIMARY KEY,
  kunde     CHAR (20),
  termin    DATE
)!
INSERT INTO auftrag VALUES (940001, 'Mueller', '16-MAY-94')!
INSERT INTO auftrag VALUES (940002, 'Mayer',   '16-JUN-94')!
INSERT INTO auftrag VALUES (940003, 'Schulze', '27-MAY-94')!

CREATE TABLE auftpos
(
  auftnum   NUMBER(6),
  auftpos   NUMBER(4),
  artikel   NUMBER(6) NOT NULL,
  menge     NUMBER(5) DEFAULT 1,
  PRIMARY KEY (auftnum, auftpos),
  FOREIGN KEY (auftnum) REFERENCES auftrag (auftnum),
  FOREIGN KEY (artikel) REFERENCES artikel (nummer)
)!
* Order 940001 by Mueller!
INSERT INTO auftpos VALUES (940001, 0001, 000001, 10)!
INSERT INTO auftpos VALUES (940001, 0002, 000002, 11)!
INSERT INTO auftpos VALUES (940001, 0003, 000003, 12)!
INSERT INTO auftpos VALUES (940001, 0004, 000004, 13)!
* Order 940002 by Mayer!
INSERT INTO auftpos VALUES (940002, 0001, 000010, 1)!
INSERT INTO auftpos VALUES (940002, 0002, 000011, 2)!
INSERT INTO auftpos VALUES (940002, 0003, 000012, 3)!
* Order 940003 by Schulze!
INSERT INTO auftpos VALUES (940003, 0001, 000001, 20)!

CREATE TABLE lagerfach
(
  lager   NUMBER(4),
  auftnum NUMBER(6),
  auftpos NUMBER(4),
  artikel NUMBER(6) NOT NULL,
  menge   NUMBER(5) NOT NULL,
  PRIMARY KEY (lager, artikel),
  FOREIGN KEY (auftnum, auftpos) REFERENCES auftpos (auftnum, auftpos),
  FOREIGN KEY (artikel)          REFERENCES artikel (nummer)
)!
* Auftragsneutrale Lagerrung!
INSERT INTO lagerfach VALUES (0005, NULL,   NULL, 000010,  15)!
INSERT INTO lagerfach VALUES (0006, NULL,   NULL, 000011, 200)!
INSERT INTO lagerfach VALUES (0007, NULL,   NULL, 000012,  11)!
* Auftragsbezogene Lagerung!
INSERT INTO lagerfach VALUES (0011, 940001, 0001, 000001,  10)!
INSERT INTO lagerfach VALUES (0021, 940003, 0001, 000001,  20)!
INSERT INTO lagerfach VALUES (0002, 940001, 0002, 000002,   5)!
INSERT INTO lagerfach VALUES (0003, 940001, 0003, 000003,   6)!
INSERT INTO lagerfach VALUES (0004, 940001, 0004, 000004,   7)!

COMMIT WORK RELEASE!
