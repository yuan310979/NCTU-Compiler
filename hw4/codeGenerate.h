#include <stdio.h>

extern FILE* fpout;
extern SymbolTable* symbol_table;
extern IdList* tmp_idlist;
extern int error;

int true_index;
int false_index;
int if_index;

void Initialization();
void GenGlobalVarFromList(IdList*);
void GenVarRef(Expr*);
void GenLiteralConstant(Value*);
void GenArithOp(Expr*, Expr*, const char);
void GenModOp();
void GenNotOp();
void GenLogicOp(const char*);
void GenNegativeOp(Expr*);
void GenRelOp(Expr*, Expr*, const char*);
void GenVarStore(Expr*);
void GenInitialStore(const char*, Type*);
void GenPrintInitialization();
void GenPrintInvoke(Expr*);
void GenReadInvoke(Expr*);
void GenFuncInitialization(const char*);
void GenFuncEnd(const char*);
void GenReturn(Expr*);
void GenFuncInvoke(const char*);
void GenIfStatement();
void GenIfElse();
void GenIfEnd();
void GenIfWithoutElse();
void GenControlStart();
void GenControlFlag();
void GenForEnd();