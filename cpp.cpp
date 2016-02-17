#include <hpp.hpp>
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cerr<<YYERR; cout<<YYERR; exit(-1); }
int main() { env_init(); return yyparse(); }

void W(Sym*o) { cout<<o->dump(); }
void W(string s) { cout<<s; }

Sym::Sym(string T,string V)		{ tag=T; val=V; }
Sym::Sym(string V):Sym("",V)	{}

string Sym::pad(int n) { string S; for(int i=0;i<n;i++) S+='\t'; return S; }
string Sym::tagval() { return "<"+tag+":"+val+">"; }
string Sym::tagstr() { return "<"+tag+":'"+val+"'>"; }
string Sym::dump(int depth) {
	string S = "\n"+pad(depth)+tagval();				// <T:V>
	for (auto pr=pars.begin(),e=pars.end();pr!=e;pr++)	// par{}ameters
		S+=","+pr->first+":"+pr->second->tagval();		
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)	// nest[]ed
		S+=(*it)->dump(depth+1);
	return S; }

void Sym::push(Sym*o) { nest.push_back(o); }
void Sym::pop() { nest.pop_back(); }

void Sym::par(Sym*o) { pars[o->val]=o; }

Sym* Sym::eval() {
	auto E = env.find(val); if (E!=env.end()) return E->second;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) (*it)=(*it)->eval();
	return this; }

Sym* Sym::str()	{ return new Str(val); }

Sym* Sym::eq(Sym*o) { env[val]=o; return o; }
Sym* Sym::at(Sym*o) { push(o); return this; }

Sym* Sym::add(Sym*o) { Sym*R=new Op("+");
	R->push(this); R->push(o); return R; }
Sym* Sym::div(Sym*o) { Sym*R=new Op("/");
	R->push(this); R->push(o); return R; }

Str::Str(string V):Sym(V) {}
Sym* Str::copy() { return new Str(val); }
string Str::tagval() { return tagstr(); }
Sym* Str::eval() { return this; }
Sym* Str::add(Sym*o) { return new Str(val+o->str()->val); }

List::List():Sym("[","]") {}

Sym* List::str() { string S;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) S+=(*it)->str()->val;
	return new Str(S); }

Sym* List::add(Sym*o) {
	List*L = new List();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
			L->push(*it);
	if (o->val!="]") L->push(o); else
		for (auto it=o->nest.begin(),e=o->nest.end();it!=e;it++)
			L->push(*it);
	return L; }

Sym* List::div(Sym*o) {
	List*L = new List();
	if (nest.size()) {
		for (auto it=nest.begin(),e=nest.end();it!=e;it++) {
			L->push(*it); L->push(o); }
		L->pop(); }
	return L; }

Op::Op(string V):Sym("op",V) {}
Sym* Op::copy() { Sym* R = new Op(val);
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) R->push(*it);
	return R; }
Sym* Op::eval() {
	if (val=="~") return nest[0]; else Sym::eval();
	if (val==":") return nest[0]->inher(nest[1]);
	if (val=="=") return nest[0]->eq(nest[1]);
	if (val=="@") return nest[0]->at(nest[1]);
	if (val=="+") return nest[0]->add(nest[1]);
	if (val=="/") return nest[0]->div(nest[1]);
	return this; }

Fn::Fn(string V, FN F):Sym("V") { fn=F; }
Sym* Fn::at(Sym*o) { return fn(o); }

Sym* Fn::evaluate(Sym*o) { return o->eval(); }

Lambda::Lambda():Sym("^","^") {}
Sym* Lambda::eval() { return this; } // block eval

// f**ken magic: apply by rewrite, not apply by eval in environment
// WARNING: RECURSION NOT APPLICABLE (???)

Sym* Sym::copy() { Sym*C = new Sym(tag,val);
	for (auto pr=pars.begin(),e=pars.end();pr!=e;pr++)	// par{}ameters
		C->pars[pr->first]=pr->second;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)	// nest[]ed
		C->push((*it)->copy());
	return C; }

bool Sym::match(Sym*o) { return o->str()->val==val; }

Sym* Sym::replace(Sym*A,Sym*B) {
	if (match(A)) return B;
	for (auto pr=pars.begin(),e=pars.end();pr!=e;pr++)
		pars[pr->first]= pr->second->replace(A,B);
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		(*it) = (*it)->replace(A,B);
	return this; }

Sym* Lambda::at(Sym*o) {								// lambda apply
	Sym* R = copy()->replace(pars.begin()->second,o);	// via rewrite
	if (R->nest.size()!=1) yyerror("multibody lambda:"+R->dump());
	else return (R->nest[0])->eval(); } // with eval
//	else return (R->nest[0])        ; } // as symbolic expression

//Class::Class(Sym*o):Sym("class",o->str()->val) {}
//Sym* Class::cclass(Sym*o) { return new Class(o); }

Sym* Sym::inher(Sym*o) { return new Sym(val,o->val); }

map<string,Sym*> env;
void env_init() {
	env["MODULE"] = new Str(MODULE);
	env["eval"] = new Fn("eval",Fn::evaluate);
//	env["class"] = new Fn("class",Class::cclass);
}

