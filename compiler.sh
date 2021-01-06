rm assembly.txt
clear
lex lexer.l
yacc -d parser.y
gcc y.tab.c lex.yy.c graph.c opti_icg.c unopti_icg.c assembly.c
rm y.tab.c lex.yy.c
./a.out < sample.php
python3 opti_icg2.py
cat assembly.txt