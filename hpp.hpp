#ifndef _H_SKEL
#define _H_SKEL

#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
using namespace std;

struct Sym {
	string tag,val;
	Sym(string,string); Sym(string); virtual Sym* copy();
	string dump(int depth=0); string pad(int);
	virtual string tagval(); string tagstr();
	vector<Sym*> nest; void push(Sym*); void pop();
	map<string,Sym*> pars; void par(Sym*);
	Sym* replace(Sym*,Sym*); virtual bool match(Sym*);
	virtual Sym* eval();
	virtual Sym* str();
	virtual Sym* inher(Sym*);
	virtual Sym* member(Sym*);
	virtual Sym* eq(Sym*);
	virtual Sym* at(Sym*);
	virtual Sym* add(Sym*);
	virtual Sym* div(Sym*);
	virtual Sym* ins(Sym*);
};

extern map<string,Sym*> env;
extern void env_init();

extern void W(Sym*);
extern void W(string);

struct Directive: Sym { Directive(string); Sym*eval(); };

struct Str: Sym { Str(string); Sym*copy(); 
	Sym*add(Sym*); string tagval(); Sym*eval(); };

struct List: Sym { List(); Sym*add(Sym*); Sym*div(Sym*); Sym*str(); };

struct Cons: Sym { Cons(Sym*,Sym*); };

struct Op: Sym { Op(string); Sym*eval(); Sym*copy(); };

typedef Sym*(*FN)(Sym*);
struct Fn:Sym { Fn(string,FN); FN fn; Sym*at(Sym*);
	static Sym* evaluate(Sym*);
};

struct Lambda: Sym { Lambda(); Sym*eval(); Sym*at(Sym*); };

//struct Class: Sym { Class(Sym*); static Sym* cclass(Sym*); };

extern int yylex();
extern int yylineno;
extern char* yytext;
#define TOC(C,X) { yylval.o = new C(yytext); return X; }
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_SKEL
