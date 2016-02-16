#include <hpp.hpp>
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cerr<<YYERR; cout<<YYERR; exit(-1); }
int main() { return yyparse(); }

void W(Sym*o) { cout<<o->dump(); }
void W(string s) { cout<<s; }

Sym::Sym(string V)		{ val=V; local = new Env(&glob_env); }

string Sym::pad(int n) { string S; for(int i=0;i<n;i++) S+='\t'; return S; }
string Sym::tagval() { return "<"+val+">"; }
string Sym::tagstr() { return "<'"+val+"'>"; }
string Sym::dump(int depth) {
	string S = "\n"+pad(depth)+tagval()+local->dump();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) S+=(*it)->dump(depth+1);
	return S; }

void Sym::push(Sym*o) { nest.push_back(o); }
void Sym::pop() { nest.pop_back(); }

void Sym::par(Sym*o) { local->iron[o->val]=o; }

Sym* Sym::eval(Env*env) {
	Sym*E = env->lookup(this); if (E) return E;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) (*it)=(*it)->eval(env);
	return this; 
}

Sym* Sym::str()	{ return new Str(val); }

Sym* Sym::eq(Sym*o) { glob_env.iron[val]=o; return o; }

Sym* Sym::add(Sym*o) { Sym*R=new Op("+");
	R->push(this); R->push(o); return R; }
Sym* Sym::div(Sym*o) { Sym*R=new Op("/");
	R->push(this); R->push(o); return R; }

Str::Str(string V):Sym(V) {}
string Str::tagval() { return tagstr(); }
Sym* Str::add(Sym*o) { return new Str(val+o->str()->val); }

List::List():Sym("") {}

Sym* List::str() { string S;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) S+=(*it)->str()->val;
	return new Str(S); }

Sym* List::add(Sym*o) {
	List*L = new List();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
			L->push(*it);
	if (o->val!="") L->push(o); else
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

Op::Op(string V):Sym(V) {}
Sym* Op::eval(Env*env) {
	if (val=="~") return nest[0]; else Sym::eval(env);
	if (val=="=") return nest[0]->eq(nest[1]);
	if (val=="+") return nest[0]->add(nest[1]);
	if (val=="/") return nest[0]->div(nest[1]);
	return this;
}

Lambda::Lambda():Sym("^") {}

Env::Env(Env*X) { next=X; }
Sym* Env::lookup(Sym*o) {
	auto it=iron.find(o->val); if (it!=iron.end()) return it->second;
	else if (next) return next->lookup(o);
	else return NULL;
}
string Env::dump() { string S;
	for (auto it=iron.begin(),e=iron.end();it!=e;it++)
		S += ","+it->first +":"+ it->second->tagval();
	return S; }
Env glob_env(NULL);
void glob_init() {
	glob_env.iron["MODULE"] = new Sym(MODULE);
}

