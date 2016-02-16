#ifndef _H_SKEL
#define _H_SKEL

#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
using namespace std;

struct Sym;
struct Env {
	Env* next; Env(Env*);
	map<string,Sym*> iron;
	Sym* lookup(Sym*);
};
extern Env glob_env;
extern void glob_init();

struct Sym {
	string val;
	Sym(string);
	string dump(int depth=0); string tagval(); string pad(int);
	vector<Sym*> nest; void push(Sym*); void pop();
	virtual Sym* eval(Env*);
	virtual Sym* eq(Sym*);
	virtual Sym* div(Sym*);
};

extern void W(Sym*);
extern void W(string);

struct List: Sym { List(); Sym*div(Sym*); };

struct Op: Sym { Op(string); Sym*eval(Env*); };

extern int yylex();
extern int yylineno;
extern char* yytext;
#define TOC(C,X) { yylval.o = new C(yytext); return X; }
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_SKEL
