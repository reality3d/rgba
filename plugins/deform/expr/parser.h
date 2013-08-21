#ifndef BISON_PARSER_H
# define BISON_PARSER_H

# ifndef YYSTYPE
#  define YYSTYPE int
#  define YYSTYPE_IS_TRIVIAL 1
# endif
# define	NUMBER	257
# define	UNARY_FUNCTION	258
# define	BINARY_FUNCTION	259
# define	FUNCTION3D	260
# define	FUNCTION4D	261
# define	FUNCTION5D	262
# define	IDENTIFIER	263
# define	POW	264
# define	EXIT	265
# define	ELEMENTARY_FUNCTION	266
# define	UMINUS	267
# define	UPLUS	268


extern YYSTYPE yylval;

#endif /* not BISON_PARSER_H */
