/*
 Author: Serezha Grigoryan
 */
typedef struct{
	char* name;
	char* value;
}JsonPair;
typedef struct{
	JsonPair *pairs;
	int size;
}Json;
typedef struct
{
	char* value;
}Element;
typedef struct
{
	Element* elems;
	int size;
}JsArr;
//Parsing Methods
Json* from_json(char* data);
JsArr* from_jsarr(char* from);
char** csarr_from_cstr(char* from, int alloc_size, int* realsize);
int* iarr_from_cstr(char* from, int alloc_size, int* realsize);
double* farr_from_cstr(char* from, int alloc_size, int* realsize);
//Object Methods
char* get_value(Json* js, char* name);
int ex_in_js(Json* js, char* name, char* val);
char* get_element(JsArr* arr,int i);
int find_in_jsarr(JsArr* arr, char* string);
char* forJson_strf(char* from, char* to);
char* fromJson_strf(char* from, char* to);
int find_in_js(Json* js,char* string);
//Memory free Methods
int free_js(Json* js);
int free_jsarr(JsArr* arr);
int free_cstr_arr(char** arr, int size);
int free_arr(void* arr);

