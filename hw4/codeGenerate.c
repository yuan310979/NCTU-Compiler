#include "symbolTable.h"
#include "codeGenerate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void Initialization(){
	true_index = 0;
	false_index = 0;
	if_index = 0;
	fprintf(fpout, ".class public output\n");
	fprintf(fpout, ".super java/lang/Object\n");
	fprintf(fpout, ".field public static _sc Ljava/util/Scanner;\n");
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
			if(!strcmp(PrintType(e->entry->type,0), "int") || !strcmp(PrintType(e->entry->type,0), "bool"))
				fprintf(fpout, "\tiload %d\n", e->entry->local_num);
			else if(!strcmp(PrintType(e->entry->type,0), "float") || !strcmp(PrintType(e->entry->type,0), "double"))
				fprintf(fpout, "\tfload %d\n", e->entry->local_num);
		}
	}
	else if(!strcmp(e->entry->kind, "constant")){
		if(!strcmp(PrintType(e->entry->type,0), "int")){
			fprintf(fpout, "\tldc %d\n", e->entry->attribute->val->ival);
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
			fprintf(fpout, "\tldc %d\n", v->ival);
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
		    else if((!strcmp(PrintType(LHS->type, LHS->current_dimension), "float") || !strcmp(PrintType(LHS->type, LHS->current_dimension), "double")) && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		    	fprintf(fpout, "\ti2f\n");
		    	fprintf(fpout, "\tfadd\n");
		    }
		    else if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && (!strcmp(PrintType(RHS->type, RHS->current_dimension), "float") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "double"))){	
		    	fprintf(fpout, "\tfstore 99\n");
		    	fprintf(fpout, "\ti2f\n");
		    	fprintf(fpout, "\tfload 99\n");
		    	fprintf(fpout, "\tfadd\n");
		    }
		    else if((!strcmp(PrintType(LHS->type, LHS->current_dimension), "float") || !strcmp(PrintType(LHS->type, LHS->current_dimension), "double")) && 
		    		(!strcmp(PrintType(RHS->type, RHS->current_dimension), "float") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "double"))){
		    	fprintf(fpout, "\tfadd\n");
		    }
			break;
		case '-':
			if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		    	fprintf(fpout, "\tisub\n");
		    }
		    else if((!strcmp(PrintType(LHS->type, LHS->current_dimension), "float") || !strcmp(PrintType(LHS->type, LHS->current_dimension), "double")) && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		    	fprintf(fpout, "\ti2f\n");
		    	fprintf(fpout, "\tfsub\n");
		    }
		    else if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && (!strcmp(PrintType(RHS->type, RHS->current_dimension), "float") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "double"))){	
		    	fprintf(fpout, "\tfstore 99\n");
		    	fprintf(fpout, "\ti2f\n");
		    	fprintf(fpout, "\tfload 99\n");
		    	fprintf(fpout, "\tfsub\n");
		    }
		    else if((!strcmp(PrintType(LHS->type, LHS->current_dimension), "float") || !strcmp(PrintType(LHS->type, LHS->current_dimension), "double")) && 
		    		(!strcmp(PrintType(RHS->type, RHS->current_dimension), "float") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "double"))){
		    	fprintf(fpout, "\tfsub\n");
		    }
			break;
		case '*':
			if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		    	fprintf(fpout, "\timul\n");
		    }
		    else if((!strcmp(PrintType(LHS->type, LHS->current_dimension), "float") || !strcmp(PrintType(LHS->type, LHS->current_dimension), "double")) && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		    	fprintf(fpout, "\ti2f\n");
		    	fprintf(fpout, "\tfmul\n");
		    }
		    else if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && (!strcmp(PrintType(RHS->type, RHS->current_dimension), "float") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "double"))){	
		    	fprintf(fpout, "\tfstore 99\n");
		    	fprintf(fpout, "\ti2f\n");
		    	fprintf(fpout, "\tfload 99\n");
		    	fprintf(fpout, "\tfmul\n");
		    }
		    else if((!strcmp(PrintType(LHS->type, LHS->current_dimension), "float") || !strcmp(PrintType(LHS->type, LHS->current_dimension), "double")) && 
		    		(!strcmp(PrintType(RHS->type, RHS->current_dimension), "float") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "double"))){
		    	fprintf(fpout, "\tfmul\n");
		    }
			break;
		case '/':
			if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		    	fprintf(fpout, "\tidiv\n");
		    }
		    else if((!strcmp(PrintType(LHS->type, LHS->current_dimension), "float") || !strcmp(PrintType(LHS->type, LHS->current_dimension), "double")) && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
		    	fprintf(fpout, "\ti2f\n");
		    	fprintf(fpout, "\tfdiv\n");
		    }
		    else if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && (!strcmp(PrintType(RHS->type, RHS->current_dimension), "float") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "double"))){	
		    	fprintf(fpout, "\tfstore 99\n");
		    	fprintf(fpout, "\ti2f\n");
		    	fprintf(fpout, "\tfload 99\n");
		    	fprintf(fpout, "\tfdiv\n");
		    }
		    else if((!strcmp(PrintType(LHS->type, LHS->current_dimension), "float") || !strcmp(PrintType(LHS->type, LHS->current_dimension), "double")) && 
		    		(!strcmp(PrintType(RHS->type, RHS->current_dimension), "float") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "double"))){
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

void GenVarStore(Expr* e, Expr* e2){
	if(!strcmp(e->kind, "error") || !strcmp(e2->kind, "error")){
		return;
	}
	if(e->entry->level != 0){
		if((!strcmp(PrintType(e->entry->type,0), "int") || !strcmp(PrintType(e->entry->type,0), "bool")) && 
		   (!strcmp(PrintType(e2->type,0), "int") || !strcmp(PrintType(e2->type,0), "bool"))){
			fprintf(fpout, "\tistore %d\n", e->entry->local_num);
		}
		else if((!strcmp(PrintType(e->entry->type,0), "float") || !strcmp(PrintType(e->entry->type,0), "double")) && !strcmp(PrintType(e2->type,0), "int")){
			fprintf(fpout, "\ti2f\n");
			fprintf(fpout, "\tfstore %d\n", e->entry->local_num);
		}
		else if(!strcmp(PrintType(e->entry->type,0), "float") || !strcmp(PrintType(e2->type,0), "float")){
			fprintf(fpout, "\tfstore %d\n", e->entry->local_num);
		}
		else if(!strcmp(PrintType(e->entry->type,0), "double") || !strcmp(PrintType(e2->type,0), "float")){
			fprintf(fpout, "\tfstore %d\n", e->entry->local_num);
		}
		else if(!strcmp(PrintType(e->entry->type,0), "double") || !strcmp(PrintType(e2->type,0), "double")){
			fprintf(fpout, "\tfstore %d\n", e->entry->local_num);
		}
		/*if(!strcmp(PrintType(e->entry->type,0), "int") || !strcmp(PrintType(e->entry->type,0), "bool")){
			fprintf(fpout, "\tistore %d\n", e->entry->local_num);
		}
		else if(!strcmp(PrintType(e->entry->type,0), "float") || !strcmp(PrintType(e->entry->type,0), "double")){
			fprintf(fpout, "\tfstore %d\n", e->entry->local_num);
		}*/
	}
	else{
		if(!strcmp(PrintType(e->type, e->current_dimension), "int") && !strcmp(PrintType(e2->type, e2->current_dimension), "int")){
			fprintf(fpout, "\tputstatic output/%s I\n", e->entry->name);
		}
		else if((!strcmp(PrintType(e->entry->type,0), "float") || !strcmp(PrintType(e->entry->type,0), "double")) && !strcmp(PrintType(e2->type, e2->current_dimension), "int")){
			fprintf(fpout, "\ti2f\n");
			fprintf(fpout, "\tputstatic output/%s F\n", e->entry->name);
		}
		else if(!strcmp(PrintType(e->type, e->current_dimension), "float") && !strcmp(PrintType(e2->type, e2->current_dimension), "float")){
			fprintf(fpout, "\tputstatic output/%s F\n", e->entry->name);
		}
		else if(!strcmp(PrintType(e->type, e->current_dimension), "double") && !strcmp(PrintType(e2->type, e2->current_dimension), "float")){
			fprintf(fpout, "\tputstatic output/%s D\n", e->entry->name);
		}
		else if(!strcmp(PrintType(e->type, e->current_dimension), "double") && !strcmp(PrintType(e2->type, e2->current_dimension), "double")){
			fprintf(fpout, "\tputstatic output/%s D\n", e->entry->name);
		}
		else if(!strcmp(PrintType(e->type, e->current_dimension), "bool")){
			fprintf(fpout, "\tputstatic output/%s Z\n", e->entry->name);
		}
	}
}

void GenInitialStore(const char* name, Type* t, Expr* e){
	int size;
	//printf("%s %s\n",PrintType(t,0));
	if(symbol_table->current_level != 0){
		size = tmp_idlist->current_size;
		if(!strcmp(PrintType(t,0), "int") && !strcmp(PrintType(e->type,0), "int")){
			fprintf(fpout, "\tistore %d\n", size+symbol_table->next_local_num-1);
		}
		else if(!strcmp(PrintType(t,0), "float") && !strcmp(PrintType(e->type,0), "int")){
			fprintf(fpout, "\ti2f\n");
			fprintf(fpout, "\tfstore %d\n", size+symbol_table->next_local_num-1);
		}
		else if(!strcmp(PrintType(t,0), "double") && !strcmp(PrintType(e->type,0), "int")){
			fprintf(fpout, "\ti2f\n");
			fprintf(fpout, "\tfstore %d\n", size+symbol_table->next_local_num-1);
		}
		else if(!strcmp(PrintType(t,0), "float") && !strcmp(PrintType(e->type,0), "float")){
			fprintf(fpout, "\tfstore %d\n", size+symbol_table->next_local_num-1);
		}
		else if(!strcmp(PrintType(t,0), "double") && !strcmp(PrintType(e->type,0), "float")){
			fprintf(fpout, "\tfstore %d\n", size+symbol_table->next_local_num-1);
		}
		else if(!strcmp(PrintType(t,0), "double") && !strcmp(PrintType(e->type,0), "double")){
			fprintf(fpout, "\tfstore %d\n", size+symbol_table->next_local_num-1);
		}
		else if(!strcmp(PrintType(t,0), "bool")){
			fprintf(fpout, "\tistore %d\n", size+symbol_table->next_local_num-1);
		}
		//fprintf(fpout, "\tistore %d\n", size+symbol_table->next_local_num-1);
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

void GenPrintInitialization(){
	fprintf(fpout, "\tgetstatic java/lang/System/out Ljava/io/PrintStream;\n");
}

void GenPrintInvoke(Expr* e){
	if(!strcmp(e->kind, "error")){
		return;
	}
	if(!strcmp(PrintType(e->type,0), "int"))
		fprintf(fpout, "\tinvokevirtual java/io/PrintStream/print(I)V\n");
	else if(!strcmp(PrintType(e->type,0), "float"))
		fprintf(fpout, "\tinvokevirtual java/io/PrintStream/print(F)V\n");
	else if(!strcmp(PrintType(e->type,0), "double"))
		fprintf(fpout, "\tinvokevirtual java/io/PrintStream/print(D)V\n");
	else if(!strcmp(PrintType(e->type,0), "bool"))
		fprintf(fpout, "\tinvokevirtual java/io/PrintStream/print(Z)V\n");
	else if(!strcmp(PrintType(e->type,0), "string"))
		fprintf(fpout, "\tinvokevirtual java/io/PrintStream/print(Ljava/lang/String;)V\n");
}

void GenReadInvoke(Expr* e){
	fprintf(fpout, "\tgetstatic output/_sc Ljava/util/Scanner;\n");

	if(!strcmp(PrintType(e->type, 0), "int")){
		fprintf(fpout, "\tinvokevirtual java/util/Scanner/nextInt()I\n");
	}
	else if(!strcmp(PrintType(e->type, 0), "float")){
		fprintf(fpout, "\tinvokevirtual java/util/Scanner/nextFloat()F\n");
	}
	else if(!strcmp(PrintType(e->type, 0), "double")){
		fprintf(fpout, "\tinvokevirtual java/util/Scanner/nextDouble()D\n");
	}
	else if(!strcmp(PrintType(e->type, 0), "bool")){
		fprintf(fpout, "\tinvokevirtual java/util/Scanner/nextBoolean()Z\n");
	}

	//GenVarStore(e);
	if(!strcmp(e->kind, "error")){
		return;
	}
	if(e->entry->level != 0){
		if(!strcmp(PrintType(e->entry->type,0), "int") || !strcmp(PrintType(e->entry->type,0), "bool")){
			fprintf(fpout, "\tistore %d\n", e->entry->local_num);
		}
		else if(!strcmp(PrintType(e->entry->type,0), "float") || !strcmp(PrintType(e->entry->type,0), "double")){
			fprintf(fpout, "\tfstore %d\n", e->entry->local_num);
		}
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

void GenFuncInitialization(const char* name){
	Expr* e = FindVarRef(symbol_table, name);
	fprintf(fpout, ".method public static %s(", name);
	if(e->entry->attribute->type_list != NULL){
		for(int i = 0; i < e->entry->attribute->type_list->current_size; i++){
			if(!strcmp(PrintType(e->entry->attribute->type_list->types[i], 0), "int")){
				fprintf(fpout, "I");
			}
			else if(!strcmp(PrintType(e->entry->attribute->type_list->types[i], 0), "float")){
				fprintf(fpout, "F");
			}
			else if(!strcmp(PrintType(e->entry->attribute->type_list->types[i], 0), "double")){
				fprintf(fpout, "D");
			}
			else if(!strcmp(PrintType(e->entry->attribute->type_list->types[i], 0), "bool")){
				fprintf(fpout, "Z");
			}
		}
	}
	else if(is_entryfunc){
		fprintf(fpout, "[Ljava/lang/String;");
	}
	fprintf(fpout, ")");
	if(is_entryfunc){
		fprintf(fpout, "V\n");
	}
	else if(!strcmp(PrintType(e->entry->type,0), "int")){
		fprintf(fpout, "I\n");
	}
	else if(!strcmp(PrintType(e->entry->type,0), "float")){
		fprintf(fpout, "F\n");
	}
	else if(!strcmp(PrintType(e->entry->type,0), "double")){
		fprintf(fpout, "D\n");
	}
	else if(!strcmp(PrintType(e->entry->type,0), "bool")){
		fprintf(fpout, "Z\n");
	}
	else if(!strcmp(PrintType(e->entry->type,0), "void")){
		fprintf(fpout, "V\n");
	}

	fprintf(fpout, ".limit stack 100\n");
	fprintf(fpout, ".limit locals 100\n");

	fprintf(fpout, "\tnew java/util/Scanner\n");
	fprintf(fpout, "\tdup\n");
	fprintf(fpout, "\tgetstatic java/lang/System/in Ljava/io/InputStream;\n");
	fprintf(fpout, "\tinvokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V\n");
	fprintf(fpout, "\tputstatic output/_sc Ljava/util/Scanner;\n");
}

void GenFuncEnd(const char* return_type){
	if(!strcmp(return_type, "void"))
		fprintf(fpout, "\treturn\n");
	fprintf(fpout, ".end method\n");
}

void GenReturn(Expr* e){
	if(!strcmp(e->kind, "error"))
		return;
	if(is_entryfunc){
		fprintf(fpout, "\treturn\n");
	}
	else if(!strcmp(PrintType(e->type, 0), "int")){
		fprintf(fpout, "\tireturn\n");
	}
	else if(!strcmp(PrintType(e->type, 0), "float")){
		fprintf(fpout, "\tfreturn\n");
	}
	else if(!strcmp(PrintType(e->type, 0), "double")){
		fprintf(fpout, "\tdreturn\n");
	}
	else if(!strcmp(PrintType(e->type, 0), "bool")){
		fprintf(fpout, "\tireturn\n");
	}
}

void GenFuncInvoke(const char* name){
	TableEntry* t = FindEntryInGlobal(symbol_table, name);
	if(t == NULL){
		return;
	}
	fprintf(fpout, "\tinvokestatic output/%s(", t->name);
	if(t->attribute->type_list != NULL){
		for(int i = 0; i < t->attribute->type_list->current_size; i++){
			if(!strcmp(PrintType(t->attribute->type_list->types[i], 0), "int")){
				fprintf(fpout, "I");
			}
			else if(!strcmp(PrintType(t->attribute->type_list->types[i], 0), "float")){
				fprintf(fpout, "F");
			}
			else if(!strcmp(PrintType(t->attribute->type_list->types[i], 0), "double")){
				fprintf(fpout, "D");
			}
			else if(!strcmp(PrintType(t->attribute->type_list->types[i], 0), "bool")){
				fprintf(fpout, "Z");
			}
		}
	}
	fprintf(fpout, ")");
	if(!strcmp(PrintType(t->type,0), "int")){
		fprintf(fpout, "I\n");
	}
	else if(!strcmp(PrintType(t->type,0), "float")){
		fprintf(fpout, "F\n");
	}
	else if(!strcmp(PrintType(t->type,0), "double")){
		fprintf(fpout, "D\n");
	}
	else if(!strcmp(PrintType(t->type,0), "bool")){
		fprintf(fpout, "Z\n");
	}
	else if(!strcmp(PrintType(t->type,0), "void")){
		fprintf(fpout, "V\n");
	}
}

void GenIfStatement(){
	fprintf(fpout, "\tifeq Lelse_%d\n", fwbuf->value[fwbuf->current_size-1]);
}

void GenIfElse(int index){
	fprintf(fpout, "\tgoto Lexit_%d\n", fwbuf->value[fwbuf->current_size-1]);
	fprintf(fpout, "Lelse_%d:\n", fwbuf->value[fwbuf->current_size-1]);
}

void GenIfEnd(){
	fprintf(fpout, "Lexit_%d:\n", fwbuf->value[fwbuf->current_size-1]);
}

void GenIfWithoutElse(){
	fprintf(fpout, "\tgoto Lexit_%d\n", fwbuf->value[fwbuf->current_size-1]);
	fprintf(fpout, "Lelse_%d:\n", fwbuf->value[fwbuf->current_size-1]);
	fprintf(fpout, "Lexit_%d:\n", fwbuf->value[fwbuf->current_size-1]);
}

void GenControlStart(){
	fprintf(fpout, "Lbegin_%d:\n", fwbuf->value[fwbuf->current_size-1]);
}

void GenControlFlag(){
	fprintf(fpout, "\tifeq Lexit_%d\n", fwbuf->value[fwbuf->current_size-1]);
	fprintf(fpout, "\tgoto Lexec_%d\n", fwbuf->value[fwbuf->current_size-1]);
	fprintf(fpout, "Lincre_%d:\n", fwbuf->value[fwbuf->current_size-1]);
}

void GenWhileControlFlag(){
	fprintf(fpout, "\tifeq Lexit_%d\n", fwbuf->value[fwbuf->current_size-1]);
}

void GenWhileEnd(){
	fprintf(fpout, "\tgoto Lbegin_%d\n", fwbuf->value[fwbuf->current_size-1]);
	fprintf(fpout, "Lexit_%d:\n", fwbuf->value[fwbuf->current_size-1]);
}

void GenForEnd(){
	fprintf(fpout, "\tgoto Lincre_%d\n", fwbuf->value[fwbuf->current_size-1]);
	fprintf(fpout, "Lexit_%d:\n", fwbuf->value[fwbuf->current_size-1]);
}

void GenExecFlag(){
	fprintf(fpout, "Lexec_%d:\n", fwbuf->value[fwbuf->current_size-1]);
}

void GenIncreEnd(){
	fprintf(fpout, "\tgoto Lbegin_%d\n", fwbuf->value[fwbuf->current_size-1]);
}

// for fwbuf
void forwhile_buf_initialize(forwhile_buf* buf){
	buf->current_size = 0;
}

void push_buf(forwhile_buf* buf, int num){
	buf->value[buf->current_size++] = num;
}

void pop_buf(forwhile_buf* buf){
	buf->current_size--;
}