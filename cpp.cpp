#include <hpp.hpp>
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cerr<<YYERR; cout<<YYERR; exit(-1); }
int main() { return yyparse(); }

void W(Sym*o) { cout<<o->dump(); }
void W(string s) { cout<<s; }

Sym::Sym(string V)		{ val=V; }

string Sym::pad(int n) { string S; for(int i=0;i<n;i++) S+='\t'; return S; }
string Sym::tagval() { return "<"+val+">"; }
string Sym::dump(int depth) { string S = "\n"+pad(depth)+tagval();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) S+=(*it)->dump(depth+1);
	return S; }

void Sym::push(Sym*o) { nest.push_back(o); }
void Sym::pop() { nest.pop_back(); }

Sym* Sym::eval(Env*env) {
	Sym*E = env->lookup(this); if (E) return E;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++) (*it)=(*it)->eval(env);
	return this; 
}

Sym* Sym::eq(Sym*o) { glob_env.iron[val]=o; return o; }

Sym* Sym::div(Sym*o) { Sym*R=new Op("/");
	R->push(this); R->push(o); return R; }

List::List():Sym("") {}

Sym* List::div(Sym*o) {
	List*L = new List();
	if (nest.size()) {
		for (auto it=nest.begin(),e=nest.end();it!=e;it++) {
			L->push(*it); L->push(o); }
		L->pop(); }
	return L; }

Op::Op(string V):Sym(V) {}
Sym* Op::eval(Env*env) { Sym::eval(env);
	if (val=="=") return nest[0]->eq(nest[1]);
	if (val=="/") return nest[0]->div(nest[1]);
	return this;
}

Env::Env(Env*X) { next=X; }
Sym* Env::lookup(Sym*o) {
	auto it=iron.find(o->val); if (it!=iron.end()) return it->second;
	else if (next) return next->lookup(o);
	else return NULL;
}
Env glob_env(NULL);
void glob_init() {
	glob_env.iron["MODULE"] = new Sym(MODULE);
}

