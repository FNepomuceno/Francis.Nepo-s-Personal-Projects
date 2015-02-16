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
typedef void *p_Void, **pp_Void;
typedef void (*VoidFunc)(p_Void data);
typedef void (*SetFunc)(p_Void dst, p_Void src);
typedef void (*FuncFunc)(p_Tokn data);
typedef p_Void (*PtrFunc)(p_Void data);
typedef p_Ftyp (*FtypFunc)(p_Void data);

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
	VoidFunc set_type;
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
	type_array[index].set_type(new_data);
	return new_data;
}

void addType(p_Char name, size_t size, int struct_id,
		p_Aray array, PtrFunc func, VoidFunc setter) {
	p_Type type = (p_Type)newEntry(array);
	type->struct_id = TYPE_ID;
	type->type_id = struct_id;
	type->name = name;
	type->size = size;
	type->array = (p_Type)array->data;
	type->index = indexOf(name, array);
	type->extract = func;
	type->set_type = setter;
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

void setListType(void *data) {
	if(((p_List)data)->struct_id != 0) {
		printf("INVALID SET\n");
	}
	((p_List)data)->struct_id = LIST_ID;
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

void setSmrtType(void *data) {
	if(((p_Smrt)data)->struct_id != 0) {
		printf("INVALID SET\n");
	}
	((p_Smrt)data)->struct_id = SMRT_ID;
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

p_Ftyp getFtyp(p_Char type, p_Aray array) {
	int index = indexOf(type, array);
	return &((p_Ftyp)array->data)[index];	
}

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
	if(data != NULL) {
		printf("Int: %d\n", *(int *)data);
	}
	else {
		printf("ERROR: DATA IS NULL\n");
	}
}

void setInt(p_Void dst, p_Void src) {
	if(src != NULL && dst != NULL) {
		*(int *)dst = *(int *)src;
	}
	else {
		printf("ERROR: DATA IS NULL\n");
	}
}

struct Tokn {
	int struct_id;
	char *name;
	void *data;
	p_Type data_type;
	p_Ftyp func_type;
	p_Smrt input;
	p_Smrt output;
	VoidFunc clean_data;
};

void unsetTokn(p_Smrt smrt) {
	if(smrt == NULL) {return;}
	p_Tokn tokn = (p_Tokn)extractSmrt(smrt, "tokn");
	if(tokn == NULL) {return;}

	//Deallocate memory and set it to null
	tokn->clean_data(&(tokn->data));
	tokn->data = NULL;
}

void setDataTokn(p_Void data_ref, p_Char type,
		p_Aray type_array, pp_Void src) {
	if(strcmp("data", type) == 0) {
		pp_Smrt smrt_ref = (pp_Smrt)data_ref;
		bindSmrt(smrt_ref, *(pp_Smrt)src, type_array);
		unbindSmrt((pp_Smrt)src);
	}
	else {
		*(pp_Void)data_ref = *src;
	}
}

void setTokn(pp_Smrt smrt_ref, p_Char name, p_Char data_type,
		VoidFunc cleaner, p_Aray type_array, FtypFunc get_ftyp,
		pp_Void src) {
	//makes the smrt container for the Tokn
	//and extracts the Tokn to insert its contents
	newSmrt(smrt_ref, "tokn", type_array);
	p_Tokn tokn = (p_Tokn)extractSmrt(*smrt_ref, "tokn");

	//setting its contents
	p_Type type = getType(data_type, type_array);
	tokn->name = name;
	tokn->data_type = type;
	tokn->func_type = get_ftyp(*src);
	tokn->input = NULL;
	tokn->output = NULL;
	tokn->clean_data = cleaner;
	setDataTokn(&(tokn->data), data_type, type_array, src);
}

p_Void getTokn(p_Void data) {
	if(((p_Tokn)data)->struct_id != TOKN_ID) {
		printf("INVALID STRUCT ID: \n");
		printf("EXPECTED %d\n", TOKN_ID);
		printf("RECEIVED %d\n", ((p_Tokn)data)->struct_id);
		return NULL;
	}
	return data;
}

void setToknType(void *data) {
	if(((p_Tokn)data)->struct_id != 0) {
		printf("INVALID SET\n");
	}
	((p_Tokn)data)->struct_id = TOKN_ID;
}

struct Data {
	int struct_id;
	void *data;
	p_Ftyp type;
};

void printData(p_Smrt smrt, p_Char data_type) {
	p_Data data = (p_Data)extractSmrt(smrt, "data");
	if(data == NULL) {
		printf("ERROR: DATA NULL\n");
		return;
	}
	p_Ftyp type = data->type;
	if(strcmp(type->name, data_type) != 0) {
		printf("ERROR: Incompatible data type\n");
	}

	type->print(data->data);
}

void unsetData(p_Smrt smrt) {
	if(smrt == NULL) {return;}
	p_Data data = (p_Data)extractSmrt(smrt, "data");
	if(data == NULL) {return;}

	free(data->data);
	data->data = NULL;
}

void setNewData(pp_Smrt smrt_ref, p_Char data_type,
		p_Aray ftyp_array, p_Aray type_array, p_Void src) {
	//makes the smrt container for the Data
	//and extracts the Data to insert its contents
	newSmrt(smrt_ref, "data", type_array);
	p_Data data = (p_Data)extractSmrt(*smrt_ref, "data");

	//inserting its contents
	p_Ftyp type = getFtyp(data_type, ftyp_array);
	size_t size = type->size;
	data->type = type;
	data->data = calloc(1, size);

	//setting the contents
	type->set(data->data, src);
}

void setData(pp_Smrt smrt_ref, p_Char data_type,
		p_Aray ftyp_array, p_Aray type_array, p_Void src) {
	if(*smrt_ref == NULL) {
		setNewData(smrt_ref, data_type, ftyp_array,
				type_array, src);
	}
	else {
		p_Data data = (p_Data)extractSmrt(*smrt_ref, "data");
		p_Ftyp type = data->type;
		type->set(data->data, src);
	}
}

p_Void getData(p_Void data) {
	if(((p_Data)data)->struct_id != DATA_ID) {
		printf("INVALID STRUCT ID\n");
		return NULL;
	}
	return data;
}

void setDataType(p_Void data) {
	if(((p_Data)data)->struct_id != 0) {
		printf("INVALID SET\n");
	}
	((p_Data)data)->struct_id = DATA_ID;
}

p_Ftyp dataFtyp(p_Void data) {
	p_Data stuf = (p_Data)extractSmrt((p_Smrt)data, "data");
	return stuf->type;
}

void cleanData(p_Void data) {
	pp_Smrt smrt_ref = (pp_Smrt)data;
	unsetData(*smrt_ref);
	unbindSmrt(smrt_ref);
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
	TODO Set up unary and binary func type
	TODO Extract data from tokens
	TODO Set up stack
	TODO Parse input already
*/
typedef void(*Prog)();
#define TYPE_CAP 20
#define FTYP_CAP 15
void program0(), program1(), program2(),
	 program3(), program4(), program5();
Prog programs[] = {
	program0, program1, program2,
	program3, program4, program5
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
void program5() {
	//Set up type_array
	p_Aray type_array = newArray(TYPE_CAP, sizeof(d_Type));
	addType("list", sizeof(d_List), LIST_ID, type_array,
			0, setListType);
	addType("smrt", sizeof(d_Smrt), SMRT_ID, type_array,
			0, setSmrtType);
	addType("data", sizeof(d_Data), DATA_ID, type_array,
			getData, setDataType);
	addType("tokn", sizeof(d_Tokn), TOKN_ID, type_array,
			getTokn, setToknType);

	//Set up ftyp_array
	p_Aray ftyp_array = newArray(FTYP_CAP, sizeof(d_Ftyp));
	newDataType("int", sizeof(int), ftyp_array,
			printInt, setInt);

	//Sample data
	p_Smrt sdat = NULL, stok = NULL;
	int test = 4;
	setData(&sdat, "int", ftyp_array, type_array, &test);
	printData(sdat, "int");
	setTokn(&stok, "4", "data", cleanData, type_array,
			dataFtyp, (pp_Void)&sdat);
	printData(((p_Smrt)
			((p_Tokn)extractSmrt(stok, "tokn"))->data), "int");

	//Clean up data
	unsetTokn(stok);
	unbindSmrt(&stok);

	//Clean arrays
	cleanArray(ftyp_array);
	cleanArray(type_array);
}

/*
	Testing ftyp_array data-type insertion
*/
void program4() {
	//Set up type_array
	p_Aray type_array = newArray(TYPE_CAP, sizeof(d_Type));
	addType("list", sizeof(d_List), LIST_ID, type_array,
			0, setListType);
	addType("smrt", sizeof(d_Smrt), SMRT_ID, type_array,
			0, setSmrtType);
	addType("data", sizeof(d_Data), DATA_ID, type_array,
			getData, setDataType);

	//Set up ftyp_array
	p_Aray ftyp_array = newArray(FTYP_CAP, sizeof(d_Ftyp));
	newDataType("int", sizeof(int), ftyp_array,
			printInt, setInt);

	//Sample data
	p_Smrt smrt1 = NULL;
	int test = 4;
	setData(&smrt1, "int", ftyp_array, type_array, &test);
	printData(smrt1, "int");

	test = 5;
	setData(&smrt1, "int", ftyp_array, type_array, &test);
	printData(smrt1, "int");

	//Clean up data
	unsetData(smrt1);
	unbindSmrt(&smrt1);

	//Clean arrays
	cleanArray(ftyp_array);
	cleanArray(type_array);
}

/*
	Extracting data from structs
*/
void program3() {
	p_Aray type_array = newArray(TYPE_CAP, sizeof(d_Type));
	addType("list", sizeof(d_List), LIST_ID, type_array,
			0, setListType);
	addType("smrt", sizeof(d_Smrt), SMRT_ID, type_array,
			0, setSmrtType);
	addType("data", sizeof(d_Data), DATA_ID, type_array,
			getData, setDataType);

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
	addType("list", sizeof(d_List), LIST_ID, type_array,
			0, setListType);
	addType("smrt", sizeof(d_Smrt), SMRT_ID, type_array,
			0, setSmrtType);
	addType("data", sizeof(d_Data), DATA_ID, type_array,
			getData, setDataType);

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
	addType("list", sizeof(d_List), LIST_ID, type_array,
			0, setListType);
	addType("smrt", sizeof(d_Smrt), SMRT_ID, type_array,
			0, setSmrtType);
	cleanArray(type_array);
}

/*
	A literal hello world program
	to make sure the program works
*/
void program0() {
	printf("Hello world!\n");
}
