#include <string>
#include <vector>

using namespace std;

struct FuncDecl {
//	private:
	public:
	const string SourceFile;
	const string FuncType;
	const string FuncName;
	const string MangledName;
	const string Annotation; //Annotation applied to function
	vector<pair<string,string>> Parameters; //Pair of (parameter type, annotation )
	bool HasAnnotation;
	string toString();

//	public:
	//Constructor
	FuncDecl(string SourceFile, string FuncType, string FuncName, string MangledName, string Annotation):
					SourceFile(SourceFile),
					FuncType(FuncType),
					FuncName(FuncName),
					MangledName(MangledName),
					Annotation(Annotation){}
/*
		//Accessors
		string getSourceFile(){ return SourceFile;}
		string getFuncName(){ return FuncName;}
		string getFuncType(){ return FuncType;}
		string getAnnotation(){ return Annotation;}
		bool hasAnnotation(){ return HasAnnotation;}
		bool hasParameters(){ return Parameters.empty();}
		vector<pair<string,string>> getParameters(){ return Parameters;}
		void setParameters(vector<pair<string,string>> Params){
			Parameters=Params;
		}
*/
};
 
class FuncDeclList{
	private:
		vector<FuncDecl*> FDList;

	public:
		FuncDeclList(){}
		~FuncDeclList(){}
		void print();
		void addDecl(FuncDecl *FD);
		void writeToYAML(string FileName);
		void readFromYAML(string FileName);
};
