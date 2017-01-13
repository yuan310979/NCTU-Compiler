#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

SymbolTable* BuildSymbolTable(){
	SymbolTable* new = (SymbolTable*) malloc(sizeof(SymbolTable));
	new->current_level = 0;
	new->current_size = 0;
	new->capacity = 4;
	new->Entries = (TableEntry**) malloc(sizeof(TableEntry*) * 4);

	new->next_local_num = 0;

	return new;
}

void PrintSymbolTable(SymbolTable* t)
{
    if (!Opt_Symbol)
        return;
    int i;
    TableEntry* ptr;

    printf("=======================================================================================");
    printf("\n");
    printf("%-33s%-11s%-12s%-19s%-24s\n", "Name", "Kind", "Level", "Type", "Attribute");
    printf("---------------------------------------------------------------------------------------");
    printf("\n");
    for (i = 0; i < t->current_size; i++) {
        ptr = t->Entries[i];
        if (ptr->level == t->current_level) {
            printf("%-33s%-11s", ptr->name, ptr->kind);
            PrintLevel(ptr->level);
            /*if(ptr->type == NULL)
            	fprintf(stderr, "TYPE IS NULL");*/
            //printf("%d", ptr->type->array_signature->capacity);
            printf("%-19s", PrintType(ptr->type,0));
            /*if(ptr->attribute != NULL)
            	printf("%d",ptr->attribute->val->ival);*/
            if(!strcmp(ptr->kind, "constant") || !strcmp(ptr->kind, "function"))
            	PrintAttribute(ptr->attribute);
            if(t->current_level != 0 && !strcmp(ptr->kind, "variable"))
            	printf(" %d",ptr->local_num);
            printf("\n");
        }
    }
    printf("=======================================================================================");
    printf("\n");
}

void PopTableEntry(SymbolTable* s)
{
    TableEntry* ptr;
    for (int i = 0; i < s->current_size; i++) {
        ptr = s->Entries[i];
        if (ptr->level == s->current_level) {
            free(ptr);
            if(s->next_local_num > 1 && s->current_level != 0) 	// except for the entry function, other function's local will start at 1
            	s->next_local_num--;
            if (i < (s->current_size) - 1){			//delete and move around(if not the end of the entries)
                s->Entries[i] = s->Entries[--(s->current_size)];  // move the last one to the free space
                i--;   								//and check the moved one                                 		
                continue;                               		
            } 
            else{
                s->current_size--;
            }
        }
    }
}

void PrintLevel(int level){
	if(!level){
		printf("%d%-11s", level, "(global)");
	}
	else{
		printf("%d%-11s", level, "(local)");
	}
}

char* PrintType(Type* type, int dim){
	char* output_buf = (char*) malloc(sizeof(char) * 19);
	//memset(output_buf, 0 ,18);
	char tmp_buf[8];
	int name_len = strlen(type->name) + 1;
	snprintf(output_buf, name_len, "%s", type->name);
	ArraySig* tmp = type->array_signature;
	int current_dim = 0;

	/*if(type->array_signature!=NULL)
		PrintTmpArraySig(type->array_signature);*/
	
	while(tmp != NULL){
		if(!dim){
			snprintf(tmp_buf, 7, "[%d]", tmp->capacity);
			strcat(output_buf, tmp_buf);
		}
		if(dim != 0){
			dim--;
		}
		tmp = tmp->next_dimension;
	}

	return output_buf;
}

void PrintAttribute(Attribute* a){
	if(a == NULL)
		return;
	else if(a->val != NULL){
		if(!strcmp(a->val->type->name, "string")){
			printf("%-24s", a->val->sval);
		}
		else if(!strcmp(a->val->type->name, "float")){
			printf("%-24f", a->val->dval);
		}
		else if(!strcmp(a->val->type->name, "int")){
			printf("%-24d", a->val->ival);
			//printf("%-24s ", a->val->type->name);
		}
		else if(!strcmp(a->val->type->name, "bool")){
			printf("%-24s", a->val->sval);
		}
		else if(!strcmp(a->val->type->name, "scientific")){
			printf("%-24f", a->val->dval);
		}
	}
	else if(a->type_list != NULL){
		TypeList* tmp = a->type_list;
		for(int i = 0; i < tmp->current_size; i++){
			if(i == tmp->current_size-1){
				printf("%s", PrintType(tmp->types[i],0));
			}
			else{
				printf("%s,", PrintType(tmp->types[i],0));
			}
		}
	}
}


void InsertTableEntryFromList(SymbolTable* t, IdList* l, const char* kind, Type* type){
    for (int i = 0; i < l->current_size; i++) {
    	//for debug
    	//printf("%s: %s\n", l->ids[i], kind);
    	

    	Type* tmp_type = (Type*) malloc(sizeof(Type));
    	strcpy(tmp_type->name, type->name);
    	ArraySig* tmp1 = l->arraysigs[i];
    	if(tmp1 != NULL){
    		tmp_type->array_signature = (ArraySig*) malloc(sizeof(ArraySig));
    		ArraySig* tmp2 = tmp_type->array_signature;
    		while(tmp1->next_dimension != NULL){
    			tmp2->capacity = tmp1->capacity;
    			tmp2->next_dimension = (ArraySig*) malloc(sizeof(ArraySig));
    			tmp1 = tmp1->next_dimension;
    			tmp2 = tmp2->next_dimension;
    		}
    		tmp2->capacity = tmp1->capacity;
    		tmp2->next_dimension = NULL;
    	}
    	else{
    		tmp_type->array_signature = NULL;
    	}


    	//tmp_type->array_signature = l->arraysigs[i];
   
    	//for debug
    	/*
    	ArraySig* tmp = tmp_type->array_signature;
		printf("%s: ", l->ids[i]);
    	if(tmp_type->array_signature != NULL){
    		while(tmp->next_dimension != NULL){
    			printf("%d ", tmp->capacity);
    			tmp = tmp->next_dimension;
    		}
    		printf("%d ", tmp->capacity);
    	}
    	printf("\n");
		*/
		Attribute* tmp_attribute;
		if(l->attributes[i] == NULL){
			tmp_attribute = NULL;
		}
		else{
			tmp_attribute = (Attribute*) malloc(sizeof(Attribute));
			tmp_attribute->val = l->attributes[i]->val;
			//tmp_attribute->val = l->attributes[i]->val;
		}
        TableEntry* new_entry = BuildTableEntry(l->ids[i], kind, t->current_level, tmp_type, tmp_attribute);
        
		//if(l->attributes[i]!=NULL && l->attributes[i]->val!=NULL)
		//	printf("%s %s\n", PrintType(l->attributes[i]->val->type, 0), PrintType(type,0));

		int mismatch = -1;
		int array_mismatch = -1;
    	if(!strcmp(kind, "constant") || !strcmp(kind, "variable")){
    		//printf("%s %s\n", PrintType(l->attributes[i]->val->type, 0), PrintType(type,0));
    		if(l->attributes[i] != NULL && l->attributes[i]->val != NULL && strcmp(PrintType(l->attributes[i]->val->type, l->attributes[i]->current_dim), PrintType(type,0)) != 0){
    			if(!strcmp(PrintType(type,0), "float") && !strcmp(PrintType(l->attributes[i]->val->type, l->attributes[i]->current_dim), "int"))
    				mismatch = 0;
    			else if(!strcmp(PrintType(type,0), "double") && !strcmp(PrintType(l->attributes[i]->val->type, l->attributes[i]->current_dim), "int"))
    				mismatch = 0;
    			else if(!strcmp(PrintType(type,0), "double") && !strcmp(PrintType(l->attributes[i]->val->type, l->attributes[i]->current_dim), "float"))
    				mismatch = 0;
    			else if(!strcmp(PrintType(type,0), "float") && !strcmp(PrintType(l->attributes[i]->val->type, l->attributes[i]->current_dim), "scientific"))
    				mismatch = 0;
    			else if(!strcmp(PrintType(type,0), "double") && !strcmp(PrintType(l->attributes[i]->val->type, l->attributes[i]->current_dim), "scientific"))
    				mismatch = 0;
    			else
    				mismatch = 1;
    		}
    		else if(l->attributes[i] != NULL && l->attributes[i]->type_list != NULL){
    			int num = GetArrayNum(l->arraysigs[i]);
    			//printf("Line#%d: Num: %d %d\n",linenum, l->attributes[i]->type_list->current_size , num);
    			if(l->attributes[i]->type_list->current_size > num){
    				printf("Error at Line#%d: too many initializer for array initialization\n", linenum);
					error = 1;
					continue;
    			}
    			for(int j = 0; j < l->attributes[i]->type_list->current_size; j++){
    				if(strcmp(PrintType(type,0),PrintType(l->attributes[i]->type_list->types[j], 0)) != 0){
    					if(!strcmp(PrintType(type,0), "float") && !strcmp(PrintType(l->attributes[i]->type_list->types[j], 0), "int")){
    						array_mismatch= 0;
	    				}
		    			else if(!strcmp(PrintType(type,0), "double") && !strcmp(PrintType(l->attributes[i]->type_list->types[j], 0), "int")){
		    				array_mismatch= 0;
		    			}
		    			else if(!strcmp(PrintType(type,0), "double") && !strcmp(PrintType(l->attributes[i]->type_list->types[j], 0), "float")){
		    				array_mismatch= 0;
		    			}
		    			else if(!strcmp(PrintType(type,0), "float") && !strcmp(PrintType(l->attributes[i]->type_list->types[j], 0), "scientific")){
		    				array_mismatch= 0;
		    			}
		    			else if(!strcmp(PrintType(type,0), "double") && !strcmp(PrintType(l->attributes[i]->type_list->types[j], 0), "scientific")){
		    				array_mismatch= 0;
		    			}
		    			else{
		    				array_mismatch= 1;
		    			}
    				}
    				else{
    					array_mismatch= 0;
    				}
	    			if(array_mismatch){
			    		printf("Error at Line#%d: %s type array initialize mismatch, LHS: %s, RHS: %s\n", linenum, kind, PrintType(type,0), PrintType(l->attributes[i]->type_list->types[j], 0));
						error = 1;
						break;
			    	}
    			}
    		}
    		else{
    			mismatch = 0;
    			array_mismatch= 0;
    		}
    	}
    	if(array_mismatch == 1){
    		continue;
    	}
    	
    	if(mismatch == 1){
    		printf("Error at Line#%d: %s type initialize mismatch, LHS: %s, RHS: %s\n", linenum, kind, PrintType(type,0), PrintType(l->attributes[i]->val->type, l->attributes[i]->current_dim));
			error = 1;
			continue;
    	}

    	if(t->current_level != 0){
    		new_entry->local_num = t->next_local_num;
    		t->next_local_num++;
    	}
    	
        /*ArraySig* tmp = new_entry->type->array_signature;
		printf("%s: ", new_entry->name);
    	if(tmp_type->array_signature != NULL){
    		while(tmp->next_dimension != NULL){
    			printf("%d ", tmp->capacity);
    			tmp = tmp->next_dimension;
    		}
    		printf("%d ", tmp->capacity);
    	}
    	printf("\n");*/
        //printf("%d\n", t->current_size);
        InsertTableEntry(t, new_entry);
    }
}

void InsertTableEntryFromTypeList(SymbolTable* t, IdList* l, const char* kind, TypeList* tl){
	for (int i = 0; i < l->current_size; i++) {

    	Type* tmp_type = (Type*) malloc(sizeof(Type));
    	strcpy(tmp_type->name, tl->types[i]->name);
    	ArraySig* tmp1 = l->arraysigs[i];
    	if(tmp1 != NULL){
    		tmp_type->array_signature = (ArraySig*) malloc(sizeof(ArraySig));
    		ArraySig* tmp2 = tmp_type->array_signature;
    		while(tmp1->next_dimension != NULL){
    			tmp2->capacity = tmp1->capacity;
    			tmp2->next_dimension = (ArraySig*) malloc(sizeof(ArraySig));
    			tmp1 = tmp1->next_dimension;
    			tmp2 = tmp2->next_dimension;
    		}
    		tmp2->capacity = tmp1->capacity;
    		tmp2->next_dimension = NULL;
    	}
    	else{
    		tmp_type->array_signature = NULL;
    	}
    	
		Attribute* tmp_attribute;
		if(l->attributes[i] == NULL){
			tmp_attribute = NULL;
		}
		else{
			tmp_attribute = (Attribute*) malloc(sizeof(Attribute));
			tmp_attribute->val = l->attributes[i]->val;
			tmp_attribute->val = l->attributes[i]->val;
		}

        TableEntry* new_entry = BuildTableEntry(l->ids[i], kind, t->current_level, tmp_type, tmp_attribute);
        InsertTableEntry(t, new_entry);
    }
}

void InsertTableEntry(SymbolTable* t, TableEntry* e){
	if(FindEntryInScope(t,e->name) != NULL){
		printf("Error at Line#%d: %s '%s' is redeclared\n", linenum, e->kind, e->name);
		error = 1;
		return;
	}

	if(t->current_size == t->capacity){
		t->capacity *= 2;
		TableEntry** tmp = t->Entries;
		t->Entries = (TableEntry**) malloc(sizeof(TableEntry*) * t->capacity);
		for(int i = 0; i < t->current_size; i++){
			(t->Entries)[i] = tmp[i];
		}
		free(tmp);
	}
	
	//printf("%d", t->current_size);
	t->Entries[t->current_size++] = e;
	//printf("%d\n", t->current_size);

	//for debug
	/*
	printf("%s: ", t->Entries[0]->name);
	ArraySig* tmp = t->Entries[0]->type->array_signature;
	if(tmp != NULL){
		while(tmp->next_dimension != NULL){
			printf("%d ", tmp->capacity);
			tmp = tmp->next_dimension;
		}
		printf("%d ", tmp->capacity);
	}
	printf("\n");
	*/
}

TableEntry* BuildTableEntry(const char* name, const char* kind, int level, Type* type, Attribute* attribute){
	TableEntry* new = (TableEntry*) malloc(sizeof(TableEntry));
	strcpy(new->name, name);
	strcpy(new->kind, kind);
	new->level = level;
	new->type = type;
	new->attribute = attribute;
	new->has_def = 0;

	return new;
}

Type* BuildType(const char* typename){
	Type* new = (Type*) malloc(sizeof(Type));
	strcpy(new->name, typename);
	new->array_signature = NULL; //先給NULL, 等等再判斷有沒有array

	//for debug 
	/*
	printf("%s\n", new->name);	
	*/

	return new;
}

IdList* BuildIdList()
{
    IdList* new = (IdList*)malloc(sizeof(IdList));
    new->current_size = 0;
    new->capacity = 4;
    new->ids = (char**)malloc(sizeof(char*) * 4);
    new->arraysigs = (ArraySig**) malloc(sizeof(ArraySig*) * 4);
    new->attributes = (Attribute**) malloc(sizeof(Attribute*) * 4);
    return new;
}

//ArraySig will have one more meaningless capacity
void InsertIdList(IdList* i ,const char* idname, ArraySig* arraysig, Attribute* attribute){
	char* tmp_idname = strdup(idname); 

	/*if(arraysig!=NULL)
		printf("%d\n",arraysig->capacity);*/  //OK

	if(i->current_size == i->capacity){
		i->capacity *= 2;
		char** tmp_ids = i->ids;
		ArraySig** tmp_arraysigs = i->arraysigs;	//printf("%d\n",tmp_arraysig->capacity);
		Attribute** tmp_attributes = i->attributes;

		i->ids = (char**) malloc(sizeof(char*) * (i->capacity));
		i->arraysigs = (ArraySig**) malloc(sizeof(ArraySig*) * (i->capacity));
		i->attributes = (Attribute**) malloc(sizeof(Attribute*) * (i->capacity));

		for(int m = 0; m < i->current_size; m++){
			i->ids[m] = tmp_ids[m];
			i->arraysigs[m] = tmp_arraysigs[m];
			i->attributes[m] = tmp_attributes[m];
		}
		free(tmp_ids);
		free(tmp_arraysigs);
		free(tmp_attributes);
	}
	/*for(int m = 0; m<i->current_size; m++){
		if(i->attributes[m] != NULL){
			printf("%s\n", i->attributes[m]->val->type->name);
			if(!strcmp(i->attributes[m]->val->type->name, "string")) printf("%s\n", i->attributes[m]->val->sval);
		}
	}*/
	
	i->ids[i->current_size] = tmp_idname;
	i->arraysigs[i->current_size] = arraysig;
	i->attributes[i->current_size] = attribute;
	/*if(i->attributes[i->current_size] != NULL)
		printf("%d", i->attributes[i->current_size]->val->ival);*/
	i->current_size++;

	//for debug
	/*
	for(int m = 0; m < i->current_size; m++){
		printf("%s ",i->ids[m]);
		if(i->arraysigs[m] != NULL){
			ArraySig* tmp = i->arraysigs[m];
			while(tmp->next_dimension != NULL){
				printf("%d ", tmp->capacity);
				tmp = tmp->next_dimension;
			}
			printf("%d ", tmp->capacity);
			printf("\n");
		}
		else{
			printf("\n");
		}
	}
	printf("\n");
	*/
}

void ResetIdList(IdList* l){
	for(int i = 0; i < l->current_size; i++){
		free(l->ids[i]);
		free(l->arraysigs[i]);
		free(l->attributes[i]);
	}
	l->current_size = 0;
	l->capacity = 4;
	l->ids = (char**)malloc(sizeof(char*) * 4);
    l->arraysigs = (ArraySig**) malloc(sizeof(ArraySig*) * 4);
    l->attributes = (Attribute**) malloc(sizeof(Attribute*) * 4);
}

Value* BuildValue(const char* typename, char* val){
	Type* tmp_type = BuildType(typename);
	Value* tmp_value = (Value*) malloc(sizeof(Value));
	tmp_value->type = tmp_type;
	tmp_value->sval = NULL;

	if(!strcmp(typename, "int")){
		tmp_value->ival = atoi(val);
	}
	else if(!strcmp(typename, "float")){
		tmp_value->dval = atof(val);
		tmp_value->sval = strdup(val);  //prevent scientific expression
	}
	else if(!strcmp(typename, "scientific")){
		//strcpy(tmp_value->type->name, "float");
		char* tmp = strdup(val);
		char* pch;
		char* num;
		char* pownum;
		pch = strtok(tmp, "e");
		num = strdup(pch);
		pch = strtok(NULL, "e");
		pownum = strdup(pch);
		double f = atof(num);
		int i = atoi(pownum);
		while(i!=0){
			if(i>0){
				f *= 10;
				i--;
			}
			else if(i < 0){
				f /= 10;
				i++;
			}
		}
		tmp_value->dval = f;
		tmp_value->sval = strdup(val);  //prevent scientific expression
	}
	else if(!strcmp(typename, "bool")){
		tmp_value->sval = strdup(val);
	}
	else if(!strcmp(typename, "string")){
		tmp_value->sval = strdup(val);
	}

	//for debug
	/*printf("%s: ", tmp_value->type->name);
	if(!strcmp(tmp_value->type->name, "int")){
		printf("%d\n", tmp_value->ival);
	}
	else if(!strcmp(tmp_value->type->name, "float")){
		printf("%f\n", tmp_value->dval);
	}
	else if(!strcmp(tmp_value->type->name, "bool")){
		printf("%s\n", tmp_value->sval);
	}
	else if(!strcmp(tmp_value->type->name, "string")){
		printf("%s\n", tmp_value->sval);
	}
	else if(!strcmp(tmp_value->type->name, "scientific")){
		printf("%s\n", tmp_value->sval);
	}*/

	return tmp_value;
}

Value* BuildNegValue(const char* typename,char* val){
	Type* tmp_type = BuildType(typename);
	Value* tmp_value = (Value*) malloc(sizeof(Value));
	tmp_value->type = tmp_type;
	tmp_value->sval = NULL;

	if(!strcmp(typename, "int")){
		tmp_value->ival = atoi(val)*(-1);
	}
	else if(!strcmp(typename, "float")){
		tmp_value->dval = atof(val) * (-1);
		tmp_value->sval = strdup(val);  //prevent scientific expression
	}
	else if(!strcmp(typename, "scientific")){
		char* tmp = strdup(val);
		char* pch;
		char* num;
		char* pownum;
		pch = strtok(tmp, "e");
		num = strdup(pch);
		pch = strtok(NULL, "e");
		pownum = strdup(pch);
		double f = atof(num);
		int i = atoi(pownum);
		while(i!=0){
			if(i>0){
				f *= 10;
				i--;
			}
			else if(i < 0){
				f /= 10;
				i++;
			}
		}
		printf("%f\n", f);
		tmp_value->dval = f;
		tmp_value->sval = strdup(val); 

		int len = strlen(val) + 2;
		tmp_value->sval = (char*) malloc(sizeof(char)*len);
		strcpy(tmp_value->sval, "-");
		strcat(tmp_value->sval, val);
		//strcpy(tmp_value->type->name, "float");
		//tmp_value->sval = strdup(val);  //prevent scientific expression
	}

	return tmp_value;
}

ArraySig* BuildTmpArraySig(){
	ArraySig* tmp = (ArraySig*) malloc(sizeof(ArraySig));
	tmp->next_dimension = NULL;
	return tmp;
}

void ExtendTmpArraySig(ArraySig* arraysig, const int size, int* new_bit){
	//printf("%d\n", size);
	//printf("%d\n", *new_bit);
	if(arraysig->next_dimension == NULL && *new_bit == 1){
		arraysig->capacity = size;
		arraysig->next_dimension = NULL;
		*new_bit = 0;
	}
	else if(arraysig->next_dimension == NULL && *new_bit == 0){	
		arraysig->next_dimension = (ArraySig*) malloc(sizeof(ArraySig));
		arraysig->next_dimension->capacity = size;
		arraysig->next_dimension->next_dimension = NULL;
	}
	else{
		ArraySig* tmp = arraysig;
		while(tmp->next_dimension != NULL){
			tmp = tmp->next_dimension;
		}
		tmp->next_dimension = (ArraySig*) malloc(sizeof(ArraySig));
		tmp->next_dimension ->capacity = size;
		tmp->next_dimension->next_dimension = NULL;
	}
}

/*
ArraySig* ExtendTmpArraySig(ArraySig* arraysig, const int size){
	if(arraysig == NULL){
		arraysig = BuildTmpArraySig();
		arraysig->capacity = size;
		//printf("%d ",size);
	}
	else{
		ArraySig* tmp = arraysig;
		while(tmp->next_dimension != NULL){
			tmp = tmp->next_dimension;
		}
		tmp->next_dimension = (ArraySig*) malloc(sizeof(ArraySig));
		tmp->next_dimension->capacity = size;
		tmp->next_dimension->next_dimension = NULL;
	}
	//PrintTmpArraySig(arraysig);
	return arraysig;
}
*/
void PrintTmpArraySig(ArraySig* arraysig){
	if(arraysig == NULL){
		fprintf(stderr,"Print Error: arraysig is NULL!\n");
	}
	ArraySig* tmp = arraysig;
	while(tmp->next_dimension != NULL){
		printf("%d ", tmp->capacity);
		tmp = tmp->next_dimension;
	}
	printf("%d ", tmp->capacity);
	printf("\n");
}

/*
void ResetTmpArraySig(ArraySig* arraysig){
	ArraySig* current = arraysig;
	ArraySig* next;
	while(current->next_dimension != NULL){
		next = current->next_dimension;
		free(current);
		current = next;
	}
	free(current);
	arraysig = (ArraySig*) malloc(sizeof(ArraySig));
}
*/

Attribute* BuildConstAttribute(Value* v){
	Attribute* new = (Attribute*) malloc(sizeof(Attribute));
	new->val = v;
	new->type_list = NULL;

	return new;
}


Attribute* BuildFunctAttribute(TypeList* t){
	Attribute* new = (Attribute*) malloc(sizeof(Attribute));
	new->val = NULL;
	new->type_list = t;

	return new;
}

Attribute* BuildExprAttribute(Expr* e){
	Attribute* new = (Attribute*) malloc(sizeof(Attribute));
	if(!strcmp(e->kind, "error"))
		return NULL;
	//printf("%s\n", e->type->name);
	new->val = (Value*) malloc(sizeof(Value));
	new->val->type = e->type;
	new->type_list = NULL;
	new->current_dim = e->current_dimension;
	return new;
}

Attribute* BuildExprListAttribute(ExprList* l){
	Attribute* new = (Attribute*) malloc(sizeof(Attribute));
	new->type_list = (TypeList*) malloc(sizeof(TypeList));
	new->type_list->types = (Type**) malloc(sizeof(Type*) * l->current_size);
	new->val = NULL;
	for(int i = 0; i < l->current_size; i++){
		new->type_list->types[i] = l->exprs[i]->type;
	}
	new->type_list->current_size = l->current_size;
	new->type_list->capacity = l->current_size;

	return new;
}


TypeList* AddTypeToListFromExprList(TypeList* l, Type* t){
	if(l == NULL){
		l = (TypeList*) malloc(sizeof(TypeList));
		l->types = (Type**) malloc(sizeof(Type*) * 4);
		l->capacity = 4;
		l->current_size = 0;
	}
	if(l->current_size == l->capacity){
		l->capacity *= 2;
		TypeList* tmp = l;
		l->types = (Type**) malloc(sizeof(Type*) * l->capacity);
		for(int i = 0; i < l->current_size; i++){
			l->types[i] = tmp->types[i];
		}
		free(tmp);
	}
	l->types[l->current_size++] = t;

	return l;
}

TypeList* AddTypeToList(TypeList* l, Type* t, IdList* idlist){
	if(l == NULL){
		l = (TypeList*) malloc(sizeof(TypeList));
		l->current_size = 0;
		l->capacity = 4;
		l->types = (Type**) malloc(sizeof(Type*) * 4);
	}
	if(l->current_size == l->capacity){
		l->capacity *= 2;
		TypeList* tmp = l;
		l->types = (Type**) malloc(sizeof(Type*) * l->capacity);
		for(int i = 0; i < l->current_size; i++){
			l->types[i] = tmp->types[i];
		}
		free(tmp);
	}
	if(idlist->arraysigs[idlist->current_size-1] != NULL){
		ArraySig* tmp1 = idlist->arraysigs[idlist->current_size-1];
		t->array_signature = (ArraySig*) malloc(sizeof(ArraySig));
		ArraySig* tmp2 = t->array_signature;
		while(tmp1->next_dimension != NULL){
			tmp2->capacity = tmp1->capacity;
			tmp1 = tmp1->next_dimension;
			tmp2->next_dimension = (ArraySig*) malloc(sizeof(ArraySig));
			tmp2 = tmp2->next_dimension;
		}
		tmp2->capacity = tmp1->capacity;
		tmp2->next_dimension = NULL;
	}

	l->types[l->current_size++] = t;

	return l;
}

TypeList* ExtendTypeList(TypeList* dest, TypeList* src){
	if(dest->capacity - dest->current_size < src->current_size){
		while(dest->capacity - dest->current_size < src->current_size){
			dest->capacity *= 2;
		}
		Type** tmp = dest->types;
		dest->types = (Type**) malloc(sizeof(Type*) * dest->capacity);
		for(int i = 0; i < dest->current_size; i++){
			dest->types[i] = tmp[i];
		}
		free(tmp);
 	}
 	for(int i = 0; i < src->current_size; i++){
 		dest->types[dest->current_size++] = src->types[i];
 	}
 	free(src);
 	return dest;
}

TableEntry* FindEntryInScope(SymbolTable* st, const char* name){
	for(int i = 0; i < st->current_size; i++){
		TableEntry* it = st->Entries[i];
		if(!strcmp(name, it->name) && it->level == st->current_level){
			return it;
		}
	}

	return NULL;
}

TableEntry* FindEntryInGlobal(SymbolTable* st, const char* name)
{
    for (int i = 0; i < st->current_size; i++) {
        TableEntry* it = st->Entries[i];
        if (!strcmp(name, it->name) && it->level == 0) {
            return it;
        }
    }
    return NULL;
}

TableEntry* FindEntryInLocalLoop(SymbolTable* st, const char* name)
{
    for (int i = 0; i < st->current_size; i++) {
        TableEntry* it = st->Entries[i];
        if (!strcmp(name, it->name) && it->level != 0) {
            return it;
        }
    }
    return NULL;
}

Expr* FunctionCall(const char* name, ExprList* l, SymbolTable* st){
	Expr* e = (Expr*)malloc(sizeof(Expr));
	strcpy(e->kind, "function");
    strcpy(e->name, name);
    e->current_dimension = 0;
	e->entry = FindEntryInGlobal(st, name);
	if(e->entry == NULL){
		printf("Error at Line#%d: function %s is not declared\n", linenum, name);
		error = 1;
        strcpy(e->kind, "error");
        e->para = NULL;
        return e;	
	}
	else{
		e->type = e->entry->type;
	}
	if(e->entry->has_def != 1){
		printf("Error at Line#%d: function %s has no definition\n", linenum, name);
		error = 1;
		return e;
	}
	if(l == NULL){
		e->para = NULL;
	}
	else{
		TypeList* para = AddTypeToListFromExprList(NULL, l->exprs[0]->type);
		//printf("%d\n", l->current_size);
        for (int i = 1; i < l->current_size; i++) {
            AddTypeToListFromExprList(para, l->exprs[i]->type);
        }
        e->para = para;
	}
	if(!CheckFuncParNum(e)){
		if(e->para == NULL)
			return e;
		for(int i = 0; i < e->para->current_size; i++){
			if(strcmp(PrintType(e->entry->attribute->type_list->types[i], 0), PrintType(l->exprs[i]->type, l->exprs[i]->current_dimension)) != 0){
				if(strstr(PrintType(e->entry->attribute->type_list->types[i], 0), "float") != NULL && strstr(PrintType(l->exprs[i]->type, l->exprs[i]->current_dimension),"int")!=NULL){
					char* t1 = strstr(PrintType(e->entry->attribute->type_list->types[i], 0), "float");
					char* t2 = strstr(PrintType(l->exprs[i]->type, l->exprs[i]->current_dimension),"int");
					printf("%s %s\n", t1+5,t2+3);
					if(!strcmp(t1+5,t2+3))
						continue;
				}
				else if(strstr(PrintType(e->entry->attribute->type_list->types[i], 0), "double") != NULL && strstr(PrintType(l->exprs[i]->type, l->exprs[i]->current_dimension),"int")!=NULL){
					char* t1 = strstr(PrintType(e->entry->attribute->type_list->types[i], 0), "double");
					char* t2 = strstr(PrintType(l->exprs[i]->type, l->exprs[i]->current_dimension),"int");
					printf("%s %s\n", t1+6,t2+3);
					if(!strcmp(t1+6,t2+3))
						continue;
				}
				else if(strstr(PrintType(e->entry->attribute->type_list->types[i], 0), "double") != NULL && strstr(PrintType(l->exprs[i]->type, l->exprs[i]->current_dimension),"float")!=NULL){
					char* t1 = strstr(PrintType(e->entry->attribute->type_list->types[i], 0), "double");
					char* t2 = strstr(PrintType(l->exprs[i]->type, l->exprs[i]->current_dimension),"float");
					printf("%s %s\n", t1+6,t2+5);
					if(!strcmp(t1+6,t2+5))
						continue;
				}
				else if(!strcmp(PrintType(e->entry->attribute->type_list->types[i], 0), "float") && !strcmp(PrintType(l->exprs[i]->type, l->exprs[i]->current_dimension),"scientific")){
					continue;
				}
				else if(!strcmp(PrintType(e->entry->attribute->type_list->types[i], 0), "double") && !strcmp(PrintType(l->exprs[i]->type, l->exprs[i]->current_dimension),"scientific")){
					continue;
				}
				printf("Error at Line#%d: parameter type mismatch\n", linenum);
				error = 1;
                return e;
			}
			//printf("%d",e->para->current_size);
		}
	}
	return e;
}


Expr* ConstExpr(Value* v){
	//printf("%s\n", v->type->name);
	Expr* new = (Expr*) malloc(sizeof(Expr));
	strcpy(new->kind, "const");
	new->current_dimension = 0;
	new->entry = NULL;
	new->type = v->type;
	if(!strcmp(v->type->name, "scientific"))
		strcpy(new->type->name, "float");

	return new;
}


Expr* FindVarRef(SymbolTable* st, const char* name){
	Expr* e = (Expr*) malloc(sizeof(Expr));
	TableEntry* tmp = FindEntryInScope(st, name);
	if(tmp == NULL){
		tmp = FindEntryInLocalLoop(st, name);
	}
	if(tmp == NULL){
		tmp = FindEntryInGlobal(st, name);
	}
	if(tmp == NULL){
		strcpy(e->kind, "error");
        strcpy(e->name, name);
        e->current_dimension = 0;
        e->entry = NULL;
        e->para = NULL;
        printf("Error at Line#%d: symbol %s is not declared\n", linenum, name);
        error = 1;
        return e;
	}
	strcpy(e->kind, "var");
    strcpy(e->name, name);
    e->current_dimension = 0;
    e->entry = tmp;
    e->type = e->entry->type;
    return e;
}

ExprList* BuildExprList(ExprList* el, Expr* e){
	if(el == NULL){
		el = (ExprList*) malloc(sizeof(ExprList));
		el->current_size = 0;
		el->capacity = 4;
		el->exprs = (Expr**) malloc(sizeof(Expr*) * el->capacity);  //since we want multiple pointer point to the expr
	}
	else if(el->current_size == el->capacity){
		el->capacity *= 2;
		Expr** tmp = el->exprs;
		el->exprs = (Expr**) malloc(sizeof(Expr*) * el->capacity);
		for(int i = 0; i < el->current_size; i++){
			el->exprs[i] = tmp[i];
		}
		free(tmp);
	}
	el->exprs[el->current_size++] = e;
	return el;
}

Expr* ArithOp(Expr* LHS, Expr* RHS, char op){
	if (LHS == NULL || RHS == NULL)
        return NULL;
    if (!strcmp(LHS->kind, "error") || !strcmp(RHS->kind, "error"))
        return 0;
    Expr* tmp = (Expr*) malloc(sizeof(Expr));
    tmp->current_dimension = 0;
    tmp->entry = NULL;
    strcpy(tmp->kind, "var");
    if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
    	tmp->type = BuildType("int");
    	return tmp;
    }
    else if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "float")){
    	tmp->type = BuildType("float");
    	return tmp;
    }
    else if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "double")){
    	tmp->type = BuildType("double");
    	return tmp;
    }
    else if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "float") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
    	tmp->type = BuildType("float");
    	return tmp;
    }
    else if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "float") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "float")){
    	tmp->type = BuildType("float");
    	return tmp;
    }
    else if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "float") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "double")){
    	tmp->type = BuildType("double");
    	return tmp;
    }
    else if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "double") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
    	tmp->type = BuildType("double");
    	return tmp;
    }
    else if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "double") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "float")){
    	tmp->type = BuildType("double");
    	return tmp;
    }
    else if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "double") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "double")){
    	tmp->type = BuildType("double");
    	return tmp;
    }
    else{
    	printf("Error at Line#%d: between %c are not integer/float/double\n", linenum, op);
    	error = 1;
        strcpy(tmp->kind, "error");
        tmp->type = BuildType(LHS->type->name);
        return tmp;
    }
}

Expr* ModOp(Expr* LHS, Expr* RHS){
	if (LHS == NULL || RHS == NULL)
        return NULL;
    if (!strcmp(LHS->kind, "error") || !strcmp(RHS->kind, "error"))
        return 0;
    Expr* tmp = (Expr*) malloc(sizeof(Expr));
    tmp->current_dimension = 0;
    tmp->entry = NULL;
    strcpy(tmp->kind, "var");
    if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "int")){
    	tmp->type = BuildType("int");
    	return tmp;
    }
    else{
    	printf("Error at Line#%d: between MOD are not integer\n", linenum);
    	error = 1;
        strcpy(tmp->kind, "error");
        tmp->type = BuildType(LHS->type->name);
        return tmp;
    }
}

Expr* LogicOp(Expr* LHS, Expr* RHS, char* op){
	//printf("%s %s\n",PrintType(LHS->type, LHS->current_dimension), PrintType(RHS->type, RHS->current_dimension));
	if (LHS == NULL || RHS == NULL)
        return NULL;
    if (!strcmp(LHS->kind, "error") || !strcmp(RHS->kind, "error"))
        return 0;
    Expr* tmp = (Expr*) malloc(sizeof(Expr));
    tmp->current_dimension = 0;
    tmp->entry = NULL;
    strcpy(tmp->kind, "var");
    if(!strcmp(PrintType(LHS->type, LHS->current_dimension), "bool") && !strcmp(PrintType(RHS->type, RHS->current_dimension), "bool")){
    	tmp->type = BuildType("bool");
    	return tmp;
    }
    else{
    	printf("Error at Line#%d: between %s are not bool\n", linenum, op);
    	error = 1;
        strcpy(tmp->kind, "error");
        tmp->type = BuildType(LHS->type->name);
        return tmp;
    }
}

Expr* NotOp(Expr* RHS){
	if (RHS == NULL)
        return NULL;
    if (!strcmp(RHS->kind, "error"))
        return 0;
    Expr* tmp = (Expr*) malloc(sizeof(Expr));
    tmp->current_dimension = 0;
    tmp->entry = NULL;
    strcpy(tmp->kind, "var");
    if(!strcmp(PrintType(RHS->type, RHS->current_dimension), "bool")){
    	tmp->type = BuildType("bool");
    	return tmp;
    }
    else{
    	printf("Error at Line#%d: RHS of NOT is nor bool\n", linenum);
    	error = 1;
        strcpy(tmp->kind, "error");
        tmp->type = BuildType(RHS->type->name);
        return tmp;
    }
}

Expr* RelOp(Expr* LHS, Expr* RHS, char* op){
	if (LHS == NULL || RHS == NULL)
        return NULL;
    if (!strcmp(LHS->kind, "error") || !strcmp(RHS->kind, "error"))
        return 0;
    Expr* tmp = (Expr*) malloc(sizeof(Expr));
    tmp->current_dimension = 0;
    tmp->entry = NULL;
    strcpy(tmp->kind, "var");
    if((!strcmp(PrintType(LHS->type, LHS->current_dimension), "int") || !strcmp(PrintType(LHS->type, LHS->current_dimension), "float") || !strcmp(PrintType(LHS->type, LHS->current_dimension), "double")) &&
    	(!strcmp(PrintType(RHS->type, RHS->current_dimension), "int") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "float") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "double"))
      ){
    	tmp->type = BuildType("bool");
    	return tmp;
    }
    else{
    	printf("Error at Line#%d: between %s are not integer/float/double\n", linenum, op);
    	error = 1;
        strcpy(tmp->kind, "error");
        tmp->type = BuildType(LHS->type->name);
        return tmp;
    }
}

Expr* NegativeOp(Expr* RHS){
	if((!strcmp(PrintType(RHS->type, RHS->current_dimension), "int") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "float") || !strcmp(PrintType(RHS->type, RHS->current_dimension), "double"))){
    	return RHS;
    }
    else{
    	printf("Error at Line#%d: RHS of NEGATIVE are not integer/float/double\n", linenum);
    	error = 1;
        strcpy(RHS->kind, "error");
        return RHS;
    }
}

int CheckFuncParNum(Expr* e){
	if(e == NULL)
		return 0;
	if(e-> para == NULL)
		return 0;
	if (strcmp(e->kind, "error") == 0)
        return 0;
    if (strcmp(e->kind, "function"))
        return 0;
    //printf("%d %d\n",e->para->current_size,e->entry->attribute->type_list->current_size);
    if (e->entry->attribute->type_list->current_size != 0 && e->para == NULL) {
        printf("Error at Line#%d: too few arguments to function '%s'\n", linenum, e->name);
        error = 1;
        return 1;
    } 
    else if (e->entry->attribute->type_list->current_size == 0 && e->para == NULL) {
        return 0;
    } 
    else if (e->para->current_size > e->entry->attribute->type_list->current_size) {
        printf("Error at Line#%d: too many arguments to function '%s'\n", linenum, e->name);
        error = 1;
        return 1;
    } 
    else if (e->para->current_size < e->entry->attribute->type_list->current_size) {
        printf("Error at Line#%d: too few arguments to function '%s'\n", linenum, e->name);
        error = 1;
        return 1;
    }
    return 0;
}

int CheckFuncReturn(Type* t, Expr* e){
	if(t == NULL){
		return 0;
	}
	if(!strcmp(e->kind, "error")){
		return 0;
	}
	if(!strcmp(PrintType(t,0), "void")){
		printf("Error at Line#%d: void function cannot be returned \n", linenum);
        error = 1;
        return 1;
	}
	if(!strcmp(PrintType(t,0), "float")&&!strcmp(PrintType(e->type, e->current_dimension), "int")){
		return 0;
	}
	if(!strcmp(PrintType(t,0), "double")&&!strcmp(PrintType(e->type, e->current_dimension), "int")){
		return 0;
	}
	if(!strcmp(PrintType(t,0), "double")&&!strcmp(PrintType(e->type, e->current_dimension), "int")){
		return 0;
	}

	if(strcmp(PrintType(t,0), PrintType(e->type, e->current_dimension))){
		printf("Error at Line#%d: return type mismatch, ", linenum);
        printf("should return %s, got %s \n", PrintType(t, 0), PrintType(e->type, e->current_dimension));
        error = 1;
        return 1;
	}
	return 0;
}

void CheckReturnLastLine(int r){
	if(r == 0){
		printf("Error at Line#%d: last line of function has no return\n", linenum);
		error = 1;
	}
	return;
}

int CheckType(Expr* LHS, Expr* RHS){
	if(LHS == NULL || RHS == NULL)
		return 0;
	if (!strcmp(LHS->kind, "error") || strcmp(RHS->kind, "err") == 0)
        return 0;
    if(!strcmp(LHS->entry->kind, "constant")){
    	printf("Error at Line#%d: Re-assign to constants are not allowed\n", linenum);
    	error = 1;
    	return 0;
    }
	if(strcmp(PrintType(LHS->type, LHS->current_dimension),PrintType(RHS->type, RHS->current_dimension)) != 0){
		if(GetDim(LHS) != LHS->current_dimension || GetDim(RHS) != RHS->current_dimension){
			printf("Error at Line#%d: array assignment\n", linenum);
			error = 1;
            return 1;
		}
		if(!CanCoerce(LHS, RHS)){
			printf("Error at Line#%d: type mismatch, LHS= %s, RHS= %s\n",
                linenum, PrintType(LHS->type, LHS->current_dimension), PrintType(RHS->type, RHS->current_dimension));
			error = 1;
            return 1;
		}
	}
	return 0;
}

int CheckRW(Expr* RHS){
	if(!strcmp(RHS->kind, "var")){
		if(!strcmp(PrintType(RHS->type, RHS->current_dimension), "int") || 
		   !strcmp(PrintType(RHS->type, RHS->current_dimension), "float") ||
		   !strcmp(PrintType(RHS->type, RHS->current_dimension), "double") ||
		   !strcmp(PrintType(RHS->type, RHS->current_dimension), "string") ||
		   !strcmp(PrintType(RHS->type, RHS->current_dimension), "bool")
		){
			return 1;
		}
		else{
			printf("Error at Line#%d: Print/Read can not be used on array type\n",linenum);
			error = 1;
	        return 0;
		}	
	}
	
}

void CheckHasDef(int is_def){
	if(is_def){
		printf("Error at Line#%d: function has already been defined\n",linenum);
		error = 1;
	}
	return;
}

int CanCoerce(Expr* LHS, Expr* RHS)
{
    if (LHS == NULL || RHS == NULL)
        return 0;
    if (strstr(PrintType(LHS->type, LHS->current_dimension), "float") != NULL && strstr(PrintType(RHS->type, RHS->current_dimension), "int") != NULL) {
        char* t1 = strstr(PrintType(LHS->type, LHS->current_dimension), "float");
        char* t2 = strstr(PrintType(RHS->type, RHS->current_dimension), "int");
        if(!strcmp(t1+5, t2+3))
        	return 1;
    }
    else if (strstr(PrintType(LHS->type, LHS->current_dimension), "double") != NULL && strstr(PrintType(RHS->type, RHS->current_dimension), "int") != NULL) {
        char* t1 = strstr(PrintType(LHS->type, LHS->current_dimension), "double");
        char* t2 = strstr(PrintType(RHS->type, RHS->current_dimension), "int");
        if(!strcmp(t1+6, t2+3))
        	return 1;
    }
    else if (strstr(PrintType(LHS->type, LHS->current_dimension), "double") != NULL && strstr(PrintType(RHS->type, RHS->current_dimension), "float") != NULL) {
        char* t1 = strstr(PrintType(LHS->type, LHS->current_dimension), "double");
        char* t2 = strstr(PrintType(RHS->type, RHS->current_dimension), "float");
        if(!strcmp(t1+6, t2+5))
        	return 1;
    }
    return 0; //can not coerce
}

int CheckBool(Expr* e){
	if(!strcmp(PrintType(e->type, e->current_dimension), "bool")){
		return 0;
	}
	else{
		printf("Error at Line#%d: conditional expression in if/while should be boolean type\n",linenum);
		error = 1;
		return 1;
	}
}

int GetArrayNum(ArraySig* a){
	int tmp = 1;
	ArraySig* arraysig = a;
	while(arraysig != NULL){
		tmp *= arraysig->capacity;
		arraysig = arraysig->next_dimension;
	}
	//printf("%d\n", tmp);
	return tmp;
}

int GetDim(Expr* e){
	int tmp = 0;
	ArraySig* a = e->type->array_signature;
	if(a == NULL)
		return 0;
	while(a != NULL){
		tmp++;
		a = a->next_dimension;
	}
	//printf("Line#%d: %d\n", linenum, tmp);
	return tmp;
}

int CheckForWhile(int is_forwhile){
	if(is_forwhile){
		return 0;
	}
	else{
		printf("Error at Line#%d: break and continue can only appear in loop statements.\n",linenum);
		error = 1;
		return 1;
	}
}