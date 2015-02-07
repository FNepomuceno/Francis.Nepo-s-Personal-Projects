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
typedef void (*VoidFunc)(p_Void data);
typedef void (*SetFunc)(p_Void dst, p_Void src);
typedef void (*FuncFunc)(p_Tokn data);
typedef p_Void (*PtrFunc)(p_Void data);

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
	int type_id;
	p_Char name;
	size_t size;
	p_Type array;
	int index;
	PtrFunc extract;
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
		int struct_id, p_Aray array, PtrFunc func) {
	p_Type type = (p_Type)newEntry(array);
	type->struct_id = TYPE_ID;
	type->type_id = struct_id;
	type->name = name;
	type->size = size;
	type->array = (p_Type)array->data;
	type->index = indexOf(name, array);
	type->extract = func;
}

struct List {
	int struct_id;
	p_Void addr;
	p_List next;
};

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
	p_Void data;
};

void setSmrt(p_Smrt smrt, void *new_data, p_Type data_type) {
	smrt->data = new_data;
	smrt->type = data_type;
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

	//ensure loc cannot access any memory
	*loc = NULL;
}

void bindSmrt(pp_Smrt dst, p_Smrt src, p_Aray type_array) {
	if(*dst != NULL) {
		unbindSmrt(dst);
	}
	src->ref_locs = addListNode(dst, src->ref_locs, type_array);
	src->num_refs = numRefs(src->ref_locs);
	*dst = src;
}

void newSmrt(pp_Smrt smrt, p_Char data_type,
		p_Aray type_array) {
	if(*smrt != NULL) {
		unbindSmrt(smrt);
	}
	p_Smrt new_smrt = newData("smrt", type_array);
	p_Void new_data = newData(data_type, type_array);
	
	setSmrt(new_smrt, new_data, getType(data_type, type_array));
	bindSmrt(smrt, new_smrt, type_array);
}

p_Void extractSmrt(p_Smrt smrt, p_Char type) {
	if(strcmp(type, smrt->type->name) != 0) {
		printf("TYPE DOES NOT MATCH\n");
		return NULL;
	}
	return smrt->type->extract(smrt->data);
}

struct Ftyp {
	int struct_id;
	p_Char name;
	size_t size;
	p_Ftyp array;
	int index;
	p_Ftyp input;
	p_Ftyp output;
	VoidFunc print;
	SetFunc set;
};

void newDataType(p_Char name, size_t size,
		p_Aray array, VoidFunc print, SetFunc set) {
	p_Ftyp ftyp = (p_Ftyp)newEntry(array);
	ftyp->struct_id = FTYP_ID;
	ftyp->name = name;
	ftyp->size = size;
	ftyp->array = (p_Ftyp)array->data;
	ftyp->index = indexOf(name, array);
	ftyp->input = NULL;
	ftyp->output = NULL;
	ftyp->print = print;
	ftyp->set = set;
}

//typedef void (*VoidFunc)(p_Void data);

void printInt(p_Void data) {
	printf("Int: %d\n", *(int *)data);
}

void setInt(p_Void dst, p_Void src) {
	*(int *)dst = *(int *)src;
}

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

p_Void getData(p_Void data) {
	if(((p_Data)data)->struct_id == 0) {
		((p_Data)data)->struct_id = DATA_ID;
		return data;
	}
	if(((p_Data)data)->struct_id != DATA_ID) {
		printf("INVALID STRUCT ID\n");
		return NULL;
	}
	return data;
}

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
	TODO Set up printData() and setData()
	TODO Set up tokens
	TODO Set up unary and binary func type
	TODO Set up stack
	TODO Parse input already
*/
typedef void(*Prog)();
#define TYPE_CAP 20
#define FTYP_CAP 15
void program0();
void program1();
void program2();
void program3();
void program4();
Prog programs[] = {
	program0,
	program1,
	program2,
	program3,
	program4
};

int main(int argc, char *argv[]) {
	int index, max = sizeof programs / sizeof (Prog);
	for(index = 0; index < max; index += 1) {
		printf("Running program %d:\n", index);
		programs[index]();
		printf("\n");
	}
	return 0;
}

/*
	Testing ftyp_array data-type insertion
*/
void program4() {
	//Set up type_array
	p_Aray type_array = newArray(TYPE_CAP, sizeof(d_Type));
	addType("list", sizeof(d_List), LIST_ID, type_array, 0);
	addType("smrt", sizeof(d_Smrt), SMRT_ID, type_array, 0);
	addType("data", sizeof(d_Data), DATA_ID, type_array,
			getData);

	//Set up ftyp_array
	p_Aray ftyp_array = newArray(FTYP_CAP, sizeof(d_Ftyp));
	newDataType("int", sizeof(int), ftyp_array,
			printInt, setInt);

	//Clean arrays
	cleanArray(ftyp_array);
	cleanArray(type_array);
}

/*
	Extracting data from structs
*/
void program3() {
	p_Aray type_array = newArray(TYPE_CAP, sizeof(d_Type));
	addType("list", sizeof(d_List), LIST_ID, type_array, 0);
	addType("smrt", sizeof(d_Smrt), SMRT_ID, type_array, 0);
	addType("data", sizeof(d_Data), DATA_ID, type_array,
			getData);

	p_Smrt smrt1 = NULL;
	newSmrt(&smrt1, "data", type_array);

	p_Data data1 = (p_Data)extractSmrt(smrt1, "data");
	printf("data1 at %p\n", data1);
	p_Data data2 = (p_Data)extractSmrt(smrt1, "smrt");
	printf("data2 at %p\n", data2);
	
	unbindSmrt(&smrt1);
	
	cleanArray(type_array);
}

/*
	Making sure making smrts work
*/
void program2() {
	//Set up array
	p_Aray type_array = newArray(TYPE_CAP, sizeof(d_Type));
	addType("list", sizeof(d_List), LIST_ID, type_array, 0);
	addType("smrt", sizeof(d_Smrt), SMRT_ID, type_array, 0);
	addType("data", sizeof(d_Data), DATA_ID, type_array,
			getData);

	//Memory for smrts
	p_Smrt smrt1 = NULL, smrt2 = NULL;

	//Set up smrts
	newSmrt(&smrt1, "data", type_array);
	bindSmrt(&smrt2, smrt1, type_array);

	//Switch smrts
	newSmrt(&smrt1, "data", type_array);
	bindSmrt(&smrt2, smrt1, type_array);

	//Clean smrts
	unbindSmrt(&smrt2);
	unbindSmrt(&smrt1);

	//Clean array
	cleanArray(type_array);
}

/*
	Checking the type array
	makes sure it doesn't leak
*/
void program1() {
	p_Aray type_array = newArray(TYPE_CAP, sizeof(d_Type));
	addType("list", sizeof(d_List), LIST_ID, type_array, 0);
	addType("smrt", sizeof(d_Smrt), SMRT_ID, type_array, 0);
	cleanArray(type_array);
}

/*
	A literal hello world program
	to make sure the program works
*/
void program0() {
	printf("Hello world!\n");
}
