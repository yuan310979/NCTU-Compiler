extern int linenum;             /* declared in lex.l */
extern char *yytext;            /* declared by lex */
extern int Opt_Symbol;
extern int error;
extern int is_entryfunc;

typedef struct SymbolTable SymbolTable;
typedef struct TableEntry TableEntry;
typedef struct Type Type;
typedef struct Attribute Attribute;
typedef struct ArraySig ArraySig;
typedef struct TypeList TypeList;
typedef struct Value Value;
typedef struct IdList IdList;
typedef struct Expr Expr;
typedef struct ExprList ExprList;


struct SymbolTable{
	int current_level;
	int current_size;
	int capacity;
	TableEntry** Entries;	//index in which level and in which entry

	int next_local_num;
};

struct TableEntry {
	char name[33];	// most 32 words
	char kind[12];	// variable function constant parameter
	int level;	
	Type* type;	//only a type name or an array(will have array signature)
	Attribute* attribute;
	int has_def;
	int local_num;  // index
};

struct Type{
	char name[12];
	ArraySig* array_signature;
};

struct Attribute{
	Value* val;
	TypeList* type_list;
	int current_dim;
};

struct ArraySig{
	int capacity;
	ArraySig* next_dimension;	// array may have multiple dimention
};

struct TypeList{
	int current_size;	//utilize current_size to be types' index
	int capacity;	// current_size < capacity
	Type** types;	//	function parameter symbol may have multiple parameter
};

struct Value{
	Type* type;
	int ival;
	double dval;
	char* sval;
};
	
struct IdList{
	int current_size;
	int capacity;
	char** ids;
	ArraySig** arraysigs;
	Attribute** attributes;
};

struct Expr{
	char kind[16]; //var,func,const or error
	char name[33];
	Type* type; 	//used for not which kind is not var
	TableEntry* entry;
	int current_dimension;
	TypeList* para;
};

struct ExprList{
	int current_size;
	int capacity;
	Expr** exprs;
};

SymbolTable* BuildSymbolTable();
void PrintSymbolTable(SymbolTable*);
void PopTableEntry(SymbolTable*);
void PrintLevel(int);
char* PrintType(Type*, int);
void PrintAttribute(Attribute*);
void InsertTableEntryFromList(SymbolTable*, IdList*, const char*, Type*);
void InsertTableEntryFromTypeList(SymbolTable*, IdList*, const char*, TypeList*);
void InsertTableEntry(SymbolTable*, TableEntry*);
TableEntry* BuildTableEntry(const char*, const char*, int , Type*, Attribute*);

Type* BuildType(const char*);

IdList* BuildIdList();
void InsertIdList(IdList*, const char*, ArraySig*, Attribute*);
void ResetIdList(IdList*);

Value* BuildValue(const char*, char*);
Value* BuildNegValue(const char*, char*);

ArraySig* BuildTmpArraySig();
void ExtendTmpArraySig(ArraySig*, const int, int*);
void ResetTmpArraySig(ArraySig*);
//for debug
//ArraySig* ExtendTestArraySig(ArraySig*, const int);
void PrintTmpArraySig(ArraySig*);

Attribute* BuildConstAttribute(Value*);
Attribute* BuildFunctAttribute(TypeList*);
Attribute* BuildExprAttribute(Expr*);
Attribute* BuildExprListAttribute(ExprList*);

TypeList* AddTypeToList(TypeList*, Type*, IdList*);
TypeList* ExtendTypeList(TypeList*, TypeList*);
TypeList* AddTypeToListFromExprList(TypeList*, Type*);

TableEntry* FindEntryInScope(SymbolTable*, const char*);
TableEntry* FindEntryInLocalLoop(SymbolTable*,const char*);
TableEntry* FindEntryInGlobal(SymbolTable*, const char*);

Expr* ConstExpr(Value*);
Expr* FindVarRef(SymbolTable*, const char*);
Expr* FunctionCall(const char*, ExprList*, SymbolTable*);
Expr* ArithOp(Expr*, Expr*, char);
Expr* ModOp(Expr*, Expr*);
Expr* LogicOp(Expr*, Expr*, char*);
Expr* NotOp(Expr*);
Expr* RelOp(Expr*, Expr*, char*);
Expr* NegativeOp(Expr*);

ExprList* BuildExprList(ExprList*,Expr*);

int CheckFuncParNum(Expr*);
int CheckFuncReturn(Type*, Expr*);
void CheckReturnLastLine(int);
int CheckType(Expr*, Expr*);
int CanCoerce(Expr*, Expr*);
int CheckRW(Expr*);
void CheckHasDef(int);
int CheckBool(Expr*);

int GetArrayNum(ArraySig*);
int GetDim(Expr*);
int CheckForWhile(int);