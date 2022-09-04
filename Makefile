main: main.c
	gcc main.c parser.c pager.c catalog.c btree.c lexer.c check.c execution_engine.c -o db
