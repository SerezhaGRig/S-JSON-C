/*
 Author: Serezha Grigoryan
 */
typedef struct{
	char** names;
	char** values;
	int size;
}Json;

int fromJson(char* from, Json* js);
char* get_value(Json* js, char* name);
int ex_in_js(Json* js, char* name, char* val);
int free_js(Json* js);
int find_in_arr(char** arr, int size, char* string);
int* iarr_from_cstr(char* from, int alloc_size, int* realsize);
char** csarr_from_cstr(char* from, int alloc_size, int* realsize);
double* farr_from_cstr(char* from, int alloc_size, int* realsize);
char** jsarr_from_cstr(char* from, int* realsize);
char* forJson_strf(char* from, char* to);
char* fromJson_strf(char* from, char* to);
int free_cstr_arr(char** arr, int size);
