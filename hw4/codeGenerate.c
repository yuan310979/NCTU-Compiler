#include "symbolTable.h"
#include "codeGenerate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void Initialization(){
	true_index = 0;
	false_index = 0;
	fprintf(fpout, ".class public output\n");
	fprintf(fpout, ".super java/lang/Object\n");
}

void GenGlobalVarFromList(IdList* l){
	for(int i = 0; i < l->current_size; i++){
		Expr* e = FindVarRef(symbol_table, l->ids[i]);
		if(!strcmp(e->kind, "error")){
			continue;
		}
		if(e->entry == NULL){
			continue;
		}
		else{
			if(e->entry->level == 0){
				if(!strcmp(PrintType(e->entry->type,0), "int")){
					fprintf(fpout, ".field public static %s I\n", e->entry->name);
				}
				else if(!strcmp(PrintType(e->entry->type,0), "float")){
					fprintf(fpout, ".field public static %s F\n", e->entry->name);
				}
				else if(!strcmp(PrintType(e->entry->type,0), "double")){
					fprintf(fpout, ".field public static %s D\n", e->entry->name);
				}
				else if(!strcmp(PrintType(e->entry->type,0), "bool")){
					fprintf(fpout, ".field public static %s Z\n", e->entry->name);
				}
			}
		}
	}
}

// variable(local or global) or constant variable
void GenVarRef(Expr* e){
	if(!strcmp(e->kind, "error")){
		return;
	}
	else if(!strcmp(e->entry->kind, "variable") || !strcmp(e->entry->kind, "parameter")){
		if(e->entry->level == 0){
  			if(!strcmp(PrintType(e->entry->type,0), "int")){
  				fprintf(fpout, "\tgetstatic output/%s I\n", e->entry->name);
  			}
  			else if(!strcmp(PrintType(e->entry->type,0), "float")){
  				fprintf(fpout, "\tgetstatic output/%s F\n", e->entry->name);
  			}
  			else if(!strcmp(PrintType(e->entry->type,0), "double")){
  				fprintf(fpout, "\tgetstatic output/%s D\n", e->entry->name);
  			}
  			else if(!strcmp(PrintType(e->entry->type,0), "bool")){
  				fprintf(fpout, "\tgetstatic output/%s Z\n", e->entry->name);
  			}
		}
		else{
			fprintf(fpout, "\tiload %d\n", e->entry->local_num);
		}
	}
	else if(!strcmp(e->entry->kind, "constant")){
		if(!strcmp(PrintType(e->entry->type,0), "int")){
			fprintf(fpout, "\tsipush %d\n", e->entry->attribute->val->ival);
		}
		else if(!strcmp(PrintType(e->entry->type,0), "float")){
			fprintf(fpout, "\tldc %f\n", e->entry->attribute->val->dval);
		}
		else if(!strcmp(PrintType(e->entry->type,0), "double")){
			fprintf(fpout, "\tldc %lf\n", e->entry->attribute->val->dval);
		}
		else if(!strcmp(PrintType(e->entry->type,0), "bool")){
			if(!strcmp(e->entry->attribute->val->sval, "true"))
				fprintf(fpout, "\ticonst_1\n");
			else if(!strcmp(e->entry->attribute->val->sval, "false"))
				fprintf(fpout, "\ticonst_0\n");
		}
		else if(!strcmp(PrintType(e->entry->type,0), "string")){
			fprintf(fpout, "\tldc \"%s\"\n", e->entry->attribute->val->sval);
		}
	}
}

void GenLiteralConstant(Value* v){
	if(v == NULL){
		return;
	}
	else{
		if(!strcmp(v->type->name, "int")){
			fprintf(fpout, "\tsipush %d\n", v->ival);
		}
		else if(!strcmp(v->type->name, "float")){
			fprintf(fpout, "\tldc %f\n", v->dval);
		}
		else if(!strcmp(v->type->name, "double")){
			fprintf(fpout, "\tldc %f\n", v->dval);
		}
		else if(!strcmp(v->type->name, "bool")){
			if(!strcmp(v->sval, "true"))
				fprintf(fpout, "\ticonst_1\n");
			else if(!strcmp(v->sval, "false"))
				fprintf(fpout, "\ticonst_0\n");
		}
		else if(!strcmp(v->type->name, "string")){
			fprintf(fpout, "\tldc \"%s\"\n", v->sval);
		}
	}
}

void GenArithOp(Expr* LHS, Expr* RHS, const char op){
	if(!strcmp(LHS->kind, "error") || !strcmp(RHS->kind, "error"))
		return;
	switch(op){
		case '+':
			if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		    	fprintf(fpout, "\tiadd\n");
		    }
		    else{
		    	fprintf(fpout, "\tfadd\n");
		    }
			break;
		case '-':
			if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		    	fprintf(fpout, "\tisub\n");
		    }
		    else{
		    	fprintf(fpout, "\tfsub\n");
		    }
			break;
		case '*':
			if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		    	fprintf(fpout, "\timul\n");
		    }
		    else{
		    	fprintf(fpout, "\tfmul\n");
		    }
			break;
		case '/':
			if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		    	fprintf(fpout, "\tidiv\n");
		    }
		    else{
		    	fprintf(fpout, "\tfdiv\n");
		    }
			break;
	}
}

void GenModOp(){
	fprintf(fpout, "\tirem\n");
}

void GenNotOp(){
	fprintf(fpout, "\tixor\n");
}

void GenLogicOp(const char* op){
	if(!strcmp(op, "||")){
		fprintf(fpout, "\tior\n");
	}
	else if(!strcmp(op, "&&")){
		fprintf(fpout, "\tiand\n");
	}
}

void GenNegativeOp(Expr* RHS){
	if(!strcmp(RHS->kind, "error"))
		return;
	if(!strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		fprintf(fpout, "\tineg\n");
	}
	else if(!strcmp(PrintType(RHS->type, RHS->current_dimension), "float") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "double")){
		fprintf(fpout, "\tfneg\n");
	}
}

void GenRelOp(Expr* LHS, Expr* RHS, const char* op){
	if(!strcmp(LHS->kind, "error") || !strcmp(RHS->kind, "error"))
		return;

	if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		fprintf(fpout, "\tisub\n");
	}
	else{
		fprintf(fpout, "\tfcmpl\n");
	}

	if(!strcmp(op, "<")){
		fprintf(fpout, "\tiflt ");
	}
	else if(!strcmp(op, "<=")){
		fprintf(fpout, "\tifle ");
	}
	else if(!strcmp(op, "!=")){
		fprintf(fpout, "\tifne ");
	}
	else if(!strcmp(op, ">=")){
		fprintf(fpout, "\tifge ");
	}
	else if(!strcmp(op, ">")){
		fprintf(fpout, "\tifgt ");
	}
	else if(!strcmp(op, "==")){
		fprintf(fpout, "\tifeq ");
	}
	fprintf(fpout, "\tLtrue_%d\n", true_index);
	fprintf(fpout, "\ticonst_0\n");
	fprintf(fpout, "\tgoto Lfalse_%d\n", false_index);
	fprintf(fpout, "Ltrue_%d:\n", true_index++);
	fprintf(fpout, "\ticonst_1\n");
	fprintf(fpout, "Lfalse_%d:\n", false_index++);
}

void GenVarStore(Expr* e){
	if(!strcmp(e->kind, "error")){
		return;
	}
	if(e->entry->level != 0){
		fprintf(fpout, "\tistore %d\n", e->entry->local_num);
	}
	else{
		if(!strcmp(PrintType(e->type, e->current_dimension), "int")){
			fprintf(fpout, "\tputstatic output/%s I\n", e->entry->name);
		}
		else if(!strcmp(PrintType(e->type, e->current_dimension), "float")){
			fprintf(fpout, "\tputstatic output/%s F\n", e->entry->name);
		}
		else if(!strcmp(PrintType(e->type, e->current_dimension), "double")){
			fprintf(fpout, "\tputstatic output/%s D\n", e->entry->name);
		}
		else if(!strcmp(PrintType(e->type, e->current_dimension), "bool")){
			fprintf(fpout, "\tputstatic output/%s Z\n", e->entry->name);
		}
	}
}

void GenInitialStore(const char* name, Type* t){
	int size;
	if(symbol_table->current_level != 0){
		size = tmp_idlist->current_size;
		fprintf(fpout, "\tistore %d\n", size+symbol_table->next_local_num-1);
		return;
	}
	else{
		if(!strcmp(PrintType(t,0), "int")){
			fprintf(fpout, "\tputstatic output/%s I\n", name);
		}
		else if(!strcmp(PrintType(t,0), "float")){
			fprintf(fpout, "\tputstatic output/%s F\n", name);
		}
		else if(!strcmp(PrintType(t,0), "double")){
			fprintf(fpout, "\tputstatic output/%s D\n", name);
		}
		else if(!strcmp(PrintType(t,0), "bool")){
			fprintf(fpout, "\tputstatic output/%s Z\n", name);
		}
		return;
	}
}