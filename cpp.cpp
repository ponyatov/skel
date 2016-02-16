#include <hpp.hpp>
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cerr<<YYERR; cout<<YYERR; exit(-1); }
int main() { env_init(); return yyparse(); }

void W(Sym*o) { cout<<o->dump(); }
void W(string s) { cout<<s; }

Sym::Sym(string V)		{ val=V; }

string Sym::pad(int n) { string S; for(int i=0;i<n;i++) S+='\t'; return S; }
string Sym::tagval() { return "<"+val+">"; }
string Sym::tagstr() { return "<'"+val+"'>"; }
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
string Str::tagval() { return tagstr(); }
Sym* Str::eval() { return this; }
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
Sym* Op::eval() {
	if (val=="~") return nest[0]; else Sym::eval();
	if (val=="=") return nest[0]->eq(nest[1]);
	if (val=="@") return nest[0]->at(nest[1]);
	if (val=="+") return nest[0]->add(nest[1]);
	if (val=="/") return nest[0]->div(nest[1]);
	return this;
}

Lambda::Lambda():Sym("^") {}

Sym* Lambda::at(Sym*o) {			// FUCKEN APPLY
	return this; }
	/*
	Sym* R = this->copy();
	R->local->iron["x"]=new Str("y");
	R->local->iron.begin()->second = o;	// set parameter value
//	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
//		(*it)=(*it)->eval(&glob_env);
//	R->eval(local);
	return R;//->eval(&glob_env);
}*/

map<string,Sym*> env;
void env_init() {
	env["MODULE"] = new Str(MODULE);
}

