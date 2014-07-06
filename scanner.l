%{
enum yytokentype {
	SYM =	258,
	REG =   259,
	IMM =	260,
	LBL =	271,
	EOL =	262
};
%}

%%

^[a-zA-Z][a-zA-Z0-9]*:	{ return LBL; }
[$][a-zA-Z]+		{ return REG; }
[#][0-9a-f]+		{ return IMM; }
[a-zA-Z]+		{ return SYM; }
\n			{ return EOL; }

%%

main(int argc, char **argv)
{
	int tok;
	while(tok = yylex()) {
		printf("%d", tok);
	}
}
