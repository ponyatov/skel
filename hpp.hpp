#ifndef _H_SKEL
#define _H_SKEL

#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
using namespace std;

struct Sym {
	string val;
	Sym(string);
	string dump(int depth=0); string pad(int);
	virtual string tagval(); string tagstr();
	vector<Sym*> nest; void push(Sym*); void pop();
	map<string,Sym*> pars; void par(Sym*);
	virtual Sym* eval();
	virtual Sym* str();
	virtual Sym* eq(Sym*);
	virtual Sym* at(Sym*);
	virtual Sym* add(Sym*);
	virtual Sym* div(Sym*);
};

extern map<string,Sym*> env;
extern void env_init();

extern void W(Sym*);
extern void W(string);

struct Str: Sym { Str(string); Sym*add(Sym*); string tagval(); Sym*eval(); };

struct List: Sym { List(); Sym*add(Sym*); Sym*div(Sym*); Sym*str(); };

struct Op: Sym { Op(string); Sym*eval(); };

struct Lambda: Sym { Lambda(); Sym*at(Sym*); };

extern int yylex();
extern int yylineno;
extern char* yytext;
#define TOC(C,X) { yylval.o = new C(yytext); return X; }
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_SKEL
