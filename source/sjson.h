#define OBJ_COUNT_DEF 30

struct json {
	char** names;
	char** values;
	int size;
};

int fromJson(char* from, struct json* js);
int fromJson_resize(char* from, struct json* js, int resize);
int find_in_arr(char** arr, int size, char* string);
char* get_value(struct json* js, char* name);
int ex_in_js(struct json* js, char* name, char* val);
int free_js(struct json* js);
int* iarr_from_cstr(char* from, int alloc_size, int* realsize);
char** csarr_from_cstr(char* from, int alloc_size, int* realsize);
double* farr_from_cstr(char* from, int alloc_size, int* realsize);
char** multiarr_from_cstr(char* from, int alloc_size, int* realsize);
char* forJson_strf(char* from, char* to);
char* fromJson_strf(char* from, char* to);
int corrector(char* from, char* to);