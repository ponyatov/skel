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
	static Env* copy(Env*);
	map<string,Sym*> iron;
	Sym* lookup(Sym*);
	string dump();
};
extern Env glob_env;
extern void glob_init();

struct Sym {
	string val;
	Sym(string); //Sym(Sym*);
	virtual Sym* copy();
	string dump(int depth=0); string pad(int);
	virtual string tagval(); string tagstr();
	vector<Sym*> nest; void push(Sym*); void pop();
	Env* local; void par(Sym*);
	virtual Sym* eval(Env*);
	virtual Sym* str();
	virtual Sym* eq(Sym*);
	virtual Sym* at(Sym*);
	virtual Sym* add(Sym*);
	virtual Sym* div(Sym*);
};

extern void W(Sym*);
extern void W(string);

struct Str: Sym { Str(string); Sym*copy(); Sym*add(Sym*); string tagval(); };

struct List: Sym { List(); Sym*add(Sym*); Sym*div(Sym*); Sym*str(); };

struct Op: Sym { Op(string); Sym*eval(Env*); };

struct Lambda: Sym { Lambda(); Sym*at(Sym*); };

extern int yylex();
extern int yylineno;
extern char* yytext;
#define TOC(C,X) { yylval.o = new C(yytext); return X; }
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_SKEL
