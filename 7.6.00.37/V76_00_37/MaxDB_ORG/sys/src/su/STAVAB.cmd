*** STAVAB.cmd ***
* Aufruf in STAVA.cmd
*
* Anlegen der Stapel-VIEWs
CREATE VIEW VE&1 AS SELECT K,N FROM VE&2!
*
CREATE VIEW VW&1 AS SELECT K,N FROM VW&2
WHERE K = 1 AND K = N AND SQRT(N) = 1 AND K - N = 0 AND K * N = 1!
*
CREATE VIEW VS&1 AS SELECT K,N FROM VS&2
WHERE K = (SELECT MIN(N) FROM VS&2)!
*
CREATE VIEW VX&1 AS SELECT T1.K,T1.N FROM VE&1 T1,VW&1 T2,VS&1 T3 
WHERE T1.K = T2.K AND T1.N = T2.N 
  AND T3.N = T2.N AND T3.K = T2.K!
*
CREATE VIEW VY&1 AS SELECT N,K FROM VE&1
                    UNION ALL
                    SELECT N,K FROM VW&1
                    UNION ALL
                    SELECT N,K FROM VS&1!
*
CREATE VIEW VZ&1 AS SELECT T1.K,T1.N 
                    FROM VE&1 T1,VW&1 T2,VS&1 T3 
                    WHERE T1.K = T2.K OR T1.N = T2.N 
                    OR T3.N = T2.N OR T3.K = T2.K
                    OR T2.K = T3.N OR T1.N = T2.K!
*
CREATE VIEW VA&1 AS SELECT K,N 
                    FROM VE&1 
                    WHERE K IN 
                          (SELECT N FROM VW&1
                           WHERE K IN 
                                (SELECT N FROM VS&1
                                 WHERE (K = 2 * N) OR (K = 1)))!
*
CREATE VIEW VB&1 AS SELECT K,N 
                    FROM VE&1 
                    WHERE (K IN 
                             (SELECT N FROM VW&1
                              WHERE K = 1))
                      AND (N IN    
                             (SELECT K FROM VS&1
                              WHERE K = 1))!
*
CREATE VIEW VC&1 AS SELECT N FROM VE&1 T0
WHERE T0.K IN
              (SELECT N FROM VW&1 T1
               WHERE K = T0.N)
  AND T0.N IN
              (SELECT K FROM VS&1 T2
               WHERE N = T0.K)!
*
CREATE VIEW VD&1 AS SELECT N FROM VE&1 T0
WHERE T0.K IN
              (SELECT N FROM VW&1 T1
               WHERE K = T0.N
                         AND T0.N IN
                                    (SELECT N FROM VS&1 T2
                                     WHERE K = T0.K))!
*
CREATE VIEW VF&1 AS SELECT N FROM VE&1 T0
WHERE T0.K IN
              (SELECT N FROM VW&1 T1
               WHERE K = T0.N
                         AND T0.K IN
                                    (SELECT N FROM VS&1 T2
                                     WHERE N = T1.K))!
*
CREATE VIEW VG&1 AS SELECT N FROM VE&1 T0
WHERE T0.K IN
              (SELECT N FROM VW&1 T1
               WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS&1 T2
                                     WHERE N = T0.K))!
*
CREATE VIEW VH&1 AS SELECT N FROM VE&1 T0
WHERE T0.K IN
              (SELECT N FROM VW&1 T1
               WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS&1 T2
                                     WHERE N = T1.K))!
*
CREATE VIEW VI&1 AS SELECT N FROM VE&1 T0
WHERE K = N AND NOT EXISTS
               (SELECT N FROM VW&1 T1
                WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS&1 T2
                                     WHERE N = T1.K AND K IS NULL))!
*
CREATE VIEW VJ&1 AS SELECT N FROM VE&1 T0
WHERE K = N AND NOT EXISTS
               (SELECT N FROM VW&1 T1
                WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS&1 T2
                                     WHERE N = T1.K AND T0.K IS NULL))!
*
CREATE VIEW VK&1 AS SELECT T0.N FROM VE&1 T0
WHERE T0.N IN
              (SELECT T1.N FROM VW&1 T1, VS&1 T2
               WHERE T1.N = T0.N
                 AND T2.N = T0.K)!
FILE STAVAS( &1!
COMMIT!
