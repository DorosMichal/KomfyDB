SELECT * FROM table1;
SELECT b, a FROM table1;
SELECT * FROM table1 t1, table2 t2 WHERE t1.tab2_pk = t2.pk;
SELECT a, d FROM table1 t1, table2 t2 WHERE t1.tab2_pk = t2.pk;
SELECT a, d FROM table1 t1, table2 t2 WHERE t1.tab2_pk = t2.pk ORDER BY d;
SELECT COUNT(a) FROM table1;
SELECT d, MAX(a) FROM table1 t1, table2 t2 WHERE t1.tab2_pk = t2.pk GROUP BY d ORDER BY d DESC;
