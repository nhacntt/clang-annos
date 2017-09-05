#include "FuncDecl.hpp"

using namespace std;

string FuncDecl::toString(){
	string s = FuncName + ":\n\t" + SourceFile + "\n\t" + FuncType + "\t" + MangledName +"(";
	for (auto i = Parameters.begin(); i != Parameters.end(); ++i){
		s += i->first + " "+ i->second + ",";
	}
	s += ")";
	return s;
}


void FuncDeclList::addDecl(FuncDecl *FD){
	//TODO
}

void FuncDeclList::print(){
	//TODO
}


void FuncDeclList::writeToYAML(string FileName){
	//TODO
}

void FuncDeclList::readFromYAML(string FileName){
	//TODO
}

