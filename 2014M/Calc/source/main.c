#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARAY_ID 89
#define TYPE_ID 45
#define LIST_ID 17
#define SMRT_ID 80
#define FTYP_ID 26
#define TOKN_ID 19
#define DATA_ID 97
#define FUNC_ID 64
#define STAK_ID 33
typedef struct Aray d_Aray, *p_Aray, **pp_Aray;
typedef struct Type d_Type, *p_Type, **pp_Type;
typedef struct List d_List, *p_List, **pp_List;
typedef struct Smrt d_Smrt, *p_Smrt, **pp_Smrt;
typedef struct Ftyp d_Ftyp, *p_Ftyp, **pp_Ftyp;
typedef struct Tokn d_Tokn, *p_Tokn, **pp_Tokn;
typedef struct Data d_Data, *p_Data, **pp_Data;
typedef struct Func d_Func, *p_Func, **pp_Func;
typedef struct Stak d_Stak, *p_Stak, **pp_Stak;

typedef char *p_Char;
typedef void *p_Void;
typedef void(*VoidFunc)(p_Void data);
typedef void(*FuncFunc)(p_Tokn data);

/*
	araytype.h
	smrtlist.h
	functype.h
	tokendat.h
	datatype.h
	function.h
	stacking.h
*/

struct Aray {
	int struct_id;
	size_t num_objs;
	size_t max_objs;
	size_t data_size;
	p_Void data;
};

p_Aray newArray(size_t array_size, size_t data_size) {
	p_Aray new_array = calloc(1, sizeof(d_Aray));
	new_array->struct_id = ARAY_ID;
	new_array->data_size = data_size;
	new_array->max_objs = array_size;
	new_array->num_objs = 0;
	new_array->data = calloc(array_size, data_size);
	return new_array;
}
void cleanArray(p_Aray array) {
	free(array->data);
	free(array);
}
p_Void newEntry(p_Aray array) {
	p_Void data = array->data +
			array->num_objs * array->data_size;
	array->num_objs += 1;
	return data;
}

struct Type {
	int struct_id;
	p_Type array;
	int index;
	int type_id;
	p_Char name;
	size_t size;
};

int indexOf(p_Char type, p_Aray array) {
	size_t index, size = array->num_objs;
	p_Type type_array = (p_Type)array->data;
	for(index = 0; index < size; index++) {
		if(strcmp(type, type_array[index].name) == 0) {
			return (int)index;
		}
	}
	return -1;
}

p_Type getType(p_Char type, p_Aray array) {
	int index = indexOf(type, array);
	return &((p_Type)array->data)[index];	
}

p_Void newData(p_Char type, p_Aray array) {
	int index = indexOf(type, array);
	p_Type type_array = (p_Type)array->data;
	size_t data_size;
	p_Void new_data;
	if(index < 0) {
		return NULL;
	}

	data_size = type_array[index].size;
	new_data = calloc(1, data_size);
	return new_data;
}

void addType(p_Char name, size_t size,
		int struct_id, p_Aray array) {
	p_Type type = (p_Type)newEntry(array);
	type->name = name;
	type->size = size;
	type->type_id = struct_id;
	type->struct_id = TYPE_ID;
	type->array = (p_Type)array->data;
	//type->index = indexOf()
}

struct List {
	int struct_id;
	p_Void addr;
	p_List next;
};

p_List newListNode(void *loc);
p_List addListNode(p_Void loc, p_List other_refs,
		p_Aray type_array) {
	p_List new_node;
	if(loc == NULL) {
		return NULL;
	}
	new_node = newData("list", type_array);
	new_node->addr = loc;
	new_node->next = other_refs;
	return new_node;
}

int numRefs(p_List first) {
	if(first == NULL) {
		return 0;
	}
	return 1 + numRefs(first->next);
}

p_List removeAddr(void *loc, p_List first) {
	p_List cur = first;
	p_List nxt;
	if(cur == NULL) {
		return NULL;
	}

	nxt = cur->next;
	if(cur->addr == loc) {
		free(cur);
		return nxt;
	}
	
	while(nxt != NULL) {
		if(nxt->addr == loc) {
			cur->next = nxt->next;
			free(nxt);
			return first;
		}
		cur = nxt;
		nxt = nxt->next;
	}
	return NULL;
}

struct Smrt {
	int struct_id;
	int num_refs;
	p_List ref_locs;
	p_Type type;
	void *data;
};

void setSmrt(p_Smrt smrt, void *new_data, p_Type data_type) {
	smrt->data = new_data;
	smrt->type = data_type;
}

void bindSmrt(pp_Smrt dst, p_Smrt src, p_Aray type_array) {
	src->ref_locs = addListNode(dst, src->ref_locs, type_array);
	src->num_refs = numRefs(src->ref_locs);
	*dst = src;
}

void newSmrt(pp_Smrt smrt, void *new_data,
		char *data_type, p_Aray type_array) {
	p_Smrt new_smrt = newData("smrt", type_array);
	setSmrt(new_smrt, new_data, getType(data_type, type_array));
	bindSmrt(smrt, new_smrt, type_array);
}

void unbindSmrt(pp_Smrt loc) {
	p_Smrt smrt;
	p_List first;
	if(loc == NULL) {
		return;
	}
	smrt = *loc;
	first = removeAddr(loc, smrt->ref_locs);
	smrt->ref_locs = first;
	smrt->num_refs = numRefs(smrt->ref_locs);
	
	//Case where no connections are left
	//to the smart pointer and therefore its data
	if(smrt->num_refs == 0) {
		free(smrt->data);
		free(smrt);
	}
}

struct Ftyp {
	int struct_id;
	p_Ftyp array;
	int index;
	char *name;
	int size;
	p_Ftyp input;
	p_Ftyp output;
	VoidFunc print;
};
struct Tokn {
	int struct_id;
	char *name;
	void *data;
	p_Type data_type;
	p_Ftyp func_type;
	p_Smrt input;
	p_Smrt output;
};
struct Data {
	int struct_id;
	void *data;
	p_Ftyp type;
	int size;
};
struct Func {
	int struct_id;
	p_Func array;
	int index;
	p_Ftyp type;
	FuncFunc execute;
};
struct Stak {
	int struct_id;
};

/*
	TODO Set up smrt binding and unbinding
	TODO Set up data_smrt and int ftyp
*/
typedef void(*Prog)();
void program0();
void program1();
Prog program = program1;
#define TYPE_CAP 20

int main(int argc, char *argv[]) {
	program();
	return 0;
}

/*
	Checking the type array
	makes sure it doesn't leak
*/
void program1() {
	p_Aray type_array = newArray(TYPE_CAP, sizeof(d_Type));
	addType("list", sizeof(d_List), LIST_ID, type_array);
	addType("smrt", sizeof(d_Smrt), SMRT_ID, type_array);
	cleanArray(type_array);
}

/*
	A literal hello world program
	to make sure the program works
*/
void program0() {
	printf("Hello world!\n");
}
