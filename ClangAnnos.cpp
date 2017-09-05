#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/Mangle.h"
#include "FuncDecl.hpp"

using namespace llvm;
using namespace clang;  
using namespace clang::tooling;


/*
FindFuncDeclVisitor
*/

class FindFuncDeclVisitor : public RecursiveASTVisitor<FindFuncDeclVisitor> {  
	public:
		explicit FindFuncDeclVisitor(ASTContext *Context,FuncDeclList *FuncDecls) : //Context(Context),
															MContext(Context->createMangleContext()),
															SourceMgr(Context->getSourceManager()),
															FuncDecls(FuncDecls){}

    	virtual bool VisitFunctionDecl(FunctionDecl *f) {
//    		if (!isa<CXXMethodDecl>(f)) { //Find the functions which are not class member
				//get filename
				FullSourceLoc fullLoc(f->getNameInfo().getBeginLoc(), SourceMgr);
				const std::string FileName = SourceMgr.getFilename(fullLoc);
				const unsigned lineNum = fullLoc.getSpellingLineNumber();
				const unsigned lineCol = fullLoc.getSpellingColumnNumber();

      			//Function typemvim
      			QualType QT = f->getReturnType();
      			std::string TypeStr = QT.getAsString();

      			// Function name
      			DeclarationName DeclName = f->getNameInfo().getName();
      			std::string FuncName = DeclName.getAsString();

				//Mangled name
				std::string MangledName;
				
  				if (!MContext->shouldMangleDeclName(f)) {
    				MangledName=FuncName;
  				} else {

  					llvm::raw_string_ostream ostream(MangledName);
  					MContext->mangleName(f, ostream);
					ostream.flush();
				}
				//Function annotation
				std::string FuncAnno;
				//llvm::errs()<< fileName<<"\t"<<TypeStr <<"\t"<<FuncName<<"\t"<<MangledName<<"\n";
				FuncDecl *FD = new FuncDecl(FileName, TypeStr, FuncName, MangledName, FuncAnno);
				if (f->hasAttr<AnnotateAttr>()) errs()<<f->getAttr<AnnotateAttr>()->getAnnotation() + "\t";
				//Get parameters
				llvm::ArrayRef<ParmVarDecl*> params = f->parameters();
				for (auto i = params.begin(); i!=params.end();i++){
					std::string Annotation;
					if ((*i)->hasAttr<AnnotateAttr>()){
						AnnotateAttr* attr = (*i)->getAttr<AnnotateAttr>();
						Annotation = attr->getAnnotation();
					}

					FD->Parameters.push_back(std::pair<string,string>((*i)->getType().getAsString(),Annotation));

					//llvm::errs()<<(*i)->getType().getAsString()<<"\t"<<Annotation<<"\n";
					
				}
				errs()<<FD->toString() + "\n";
				delete FD;

//    		}			
		return true;
		}
		virtual ~FindFuncDeclVisitor(){}
	private:
		//ASTContext *Context;
		MangleContext *MContext;
		SourceManager &SourceMgr;
		FuncDeclList *FuncDecls;
};

/*
FindFuncDeclConsumer
*/

class FindFuncDeclConsumer : public clang::ASTConsumer {  
	public:
		explicit FindFuncDeclConsumer(ASTContext *Context,FuncDeclList *FuncDecls) : Visitor(Context,FuncDecls) {}

		virtual void HandleTranslationUnit(clang::ASTContext &Context) {
			
			Visitor.TraverseDecl(Context.getTranslationUnitDecl());
		}

	private:
		FindFuncDeclVisitor Visitor;
};

/*
FindFuncDeclCallAction
*/
class FindFuncDeclCallAction : public clang::ASTFrontendAction {  
	public:
		FindFuncDeclCallAction() {}
		FuncDeclList *FuncDecls = new FuncDeclList;

		virtual std::unique_ptr<ASTConsumer> CreateASTConsumer( CompilerInstance &Compiler, llvm::StringRef InFile) {
				return std::unique_ptr<ASTConsumer>(new FindFuncDeclConsumer(&Compiler.getASTContext(),FuncDecls));
			}
};

/*
main
*/
  struct FileFormat {
    enum class Kind { Dot, JSON, YAML };

    virtual string Filename(StringRef Prefix) const = 0;
    virtual void Write(raw_ostream&, const FuncDeclList&)
      const = 0;

    static std::unique_ptr<FileFormat> Create(Kind);
  };
static cl::OptionCategory MyToolCategory("clang-annos options");
//cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
//static cl::opt<string> InputFilename(cl::Positional, cl::desc("<input file>"), cl::Required);
static cl::opt<string> OutputFilename("o", cl::desc("Specify output filename"), cl::value_desc("filename"),cl::Optional);
/*static cl::opt<FileFormat::Kind> CGFormat("format", cl::desc("output format"),cl::Optional,
			cl::values(
				clEnumValN(FileFormat::Kind::Dot,  "dot",  "GraphViz dot"),
				clEnumValN(FileFormat::Kind::JSON, "json", "JavaScript Object Notation"),
				clEnumValN(FileFormat::Kind::YAML, "yaml", "Yet Another Markup Language")
			)
);*/
int main(int argc, const char **argv) {  

	CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
	ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
	errs()<<OutputFilename;
	std::unique_ptr<FrontendActionFactory> FrontendFactory = newFrontendActionFactory<FindFuncDeclCallAction>();
	int result = Tool.run(FrontendFactory.get());
	return result;
}

