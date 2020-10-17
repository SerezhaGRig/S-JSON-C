#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif // WIN
#define true 1
#define false 0



#include <stdlib.h>
#include<string.h>
#include<ctype.h>
#include "sjson.h"
#include<math.h>



int parse_pair(char* from, int size, JsonPair* js);
char* corrector(char* from, char* to);







//ok
Json* from_json(char* dirty)
{
	if (dirty == NULL)
			return 0;
	char clear[strlen(dirty)+1];
	corrector(dirty, clear);
	Json* js = malloc(sizeof(Json));
	js->pairs=NULL;
	js->size=0;
	int	count;
	int open_brecks = 0;
	int quot_closed = true;
	char* from = clear;
	char* start = from++;
	for (count = 0; *(from); from++)
	{
		if (quot_closed) {
			if ((*from == '{' || *from == '['))
				open_brecks++;
 			if (*from == '\"')
			{

				quot_closed = false;
				if(*(from+1)!='\"')
					from++;
				continue;

			}
			if ((*from == ',' || *from == '}') && !open_brecks)
			{
				int size = from - start;
				js->pairs = realloc(js->pairs,(count+1) * sizeof(JsonPair));
				if (js) {
					if (parse_pair(start, size, &(js->pairs[count]))) {
						js->size=count;
						free_js(js);
						return NULL;
					}
				}
				count++;
				start = from;
			}
			if ((*from == '}' || *from == ']'))
				open_brecks--;
		}
		else {
			if (*(from) == '\"')
			{
				if (*(from - 1) != '\\' || (*(from - 1) == '\\' && *(from - 2) == '\\')) {
					quot_closed = true;
				}
			}
		}

	}
	js->size = count;
	if (count && *(from - 1) == '}' && open_brecks == -1 && quot_closed)
		return js;
	free_js(js);
	return NULL;
}


//ok
JsArr* from_jsarr(char* from)
{
	if (from == NULL)
		return NULL;
	JsArr* jsarr=malloc(sizeof(JsArr)); //= malloc(alloc_size * sizeof(char*));
	jsarr->elems=NULL;
	jsarr->size=0;
	char* start;
	int breck_count = 0, lenght, quot_closed = true, i = 0;

	for (lenght = 0; *from != ']'; lenght = 0, i++) {
		if (*from != '[' && *from != ',') {
			free_jsarr(jsarr);
			return NULL;
		}

		from++;
		if (!(*from))
		{
			free_jsarr(jsarr);
			return NULL;
		}
		for (start = from; true; lenght++, from++) {

			if ((*from == ']' || *from == ',') && breck_count <= 0)
				break;
			if (quot_closed) {
				if ((*from == '{' || *from == '[') && quot_closed) {
					breck_count++;
				}

				if (*(from) == '\"')
				{

					quot_closed = false;
					breck_count++;
					continue;
				}
				if ((*from == '}' || *from == ']'))
					breck_count--;
			}
			else {
				if (*(from) == '\"')
				{

					if (*(from - 1) != '\\' || (*(from - 1) == '\\' && *(from - 2) == '\\')) {
						quot_closed = true;
						breck_count--;
					}



				}
			}
		}
	jsarr->elems = realloc(jsarr->elems,(i+1)*sizeof(Element));
		if (!((*start == '\"' && *(from - 1) == '\"') || (*start == '{' && *(from - 1) == '}')
			|| (*start == '[' && *(from - 1) == ']') || (isdigit(*start) && isdigit(*(from - 1))) || (!strncmp(start, "true", from - start) && from - start == 4) || (!strncmp(start, "false", from - start) && from - start == 4)))
		{
			free_jsarr(jsarr);
			return NULL;
		}

		if(jsarr){
			jsarr->elems[i].value = malloc((lenght + 1) * sizeof(char));
			if (jsarr->elems[i].value) {
				strncpy(jsarr->elems[i].value, start, lenght);
				(jsarr->elems[i].value)[lenght] = '\0';
			}
			if (*start == '\"') {
				start++;
				lenght -= 2;
			}
		}
	}
	if (*from == ']')
	{
		jsarr->size = i;
		return jsarr;
	}
	free_jsarr(jsarr);
	return NULL;
}

//ok
char** csarr_from_cstr(char* from, int alloc_size, int* realsize)
{
	if (from == NULL)
		return NULL;
	char* start;
	char** carr = malloc(alloc_size * sizeof(char*));


	int i;
	int quot_closed = true;
	int lenght = 0;

	for (i = 0; *from != ']'; from++, i++, lenght = 0) {
		if (*from != ',' && *from != '[')
		{
			free_cstr_arr(carr, i);
			return NULL;
		}
		from++;
		if (*from != '\"')
		{
			free_cstr_arr(carr, i);
			return NULL;
		}
		for (start = from + 1; *from; lenght++, from++) {


			if (quot_closed) {
				if ((*from == ']' || *from == ','))
					break;
				if (*(from) == '\"')
				{

					quot_closed = false;
					continue;
				}
			}
			else {
				if (*(from) == '\"')
				{

					if (*(from - 1) != '\\' || (*(from - 1) == '\\' && *(from - 2) == '\\')) {
						quot_closed = true;

						break;
					}


				}
			}
		}
		if (carr) {
			i = i % alloc_size;
			carr[i] = malloc((lenght) * sizeof(char));
			if ((carr)[i]) {
				if (lenght > 0) {
					strncpy(carr[i], start, (lenght - 1));
				}
				((carr)[i])[lenght - 1] = '\0';
			}
		}



	}
	if (realsize != NULL)
		* realsize = i;
	return carr;
}


//ok
int* iarr_from_cstr(char* from, int alloc_size, int* realsize)
{

	if (from == NULL)
		return NULL;
	int* int_arr = malloc((alloc_size) * sizeof(int));
	int a, i;
	int sign = 1;
	for (i = 0; *(from++) != ']'; i++) {
		if (!isdigit(*from) && *from != '-' && *from)
		{
			free(int_arr);
			return NULL;
		}
		if (*from == '-') {
			sign = -1;
			from++;
		}
		if (!isdigit(*from))
		{
			free(int_arr);
			return NULL;
		}
		for (a = 0; isdigit(*from); from++) {
			a = a * 10 + (*from) - '0';

		}
		if (*from != ',' && *from != ']')
		{
			free(int_arr);
			return NULL;
		}
		a *= sign;
		if (int_arr) {
			(int_arr)[i % alloc_size] = a;
		}

	}
	if (realsize != NULL)
		* realsize = i;
	return int_arr;
}


//ok
double* farr_from_cstr(char* from, int alloc_size, int* realsize)
{
	if (from == NULL)
		return NULL;
	double* double_arr = malloc((alloc_size) * sizeof(double));
	int i, sign = 1;
	double power = 1.0;
	double val = 0;
	for (i = 0; *from != ']'; i++) {
		from++;
		if (!isdigit(*from) && *(from) == '-' && *(from)) {
			free(double_arr);
			return NULL;
		}

		if (*(from) == '-') {
			sign = -1;
			from++;
		}
		if (!isdigit(*from)) {
			free(double_arr);
			return NULL;
		}
		for (val = 0; isdigit(*from); from++) {
			val = val * 10 + (*from) - '0';
		}
		if (*from == '.') {
			from++;
			if (!isdigit(*from)) {
				free(double_arr);
				return NULL;
			}
			for (power = 1.0; isdigit(*from); from++) {
				double a = ((double)* from - (double)'0');
				val = 10.0 * val + a;
				power *= 10;
			}
			val = val / power;
		}
		if (*from != ',' && *from != ']')
		{
			free(double_arr);
			return NULL;
		}

		val *= sign;
		if (double_arr) {
			i = i % alloc_size;
			(double_arr)[i] = val;
		}
	}
	if (realsize != NULL)
		* realsize = i;
	return double_arr;
}


//ok
int find_in_jsarr(JsArr* arr, char* string)
{
	int i;
	for (i = 0; i < arr->size; i++)
		if (!strcmp(string, arr->elems[i].value))
			return i;
	return -1;
}

//ok
int find_in_js(Json* js,char* string)
{
	int i;
	for (i = 0; i < js->size; i++)
		if (!strcmp(string, js->pairs[i].name))
			return i;
	return -1;
}


//ok
int ex_in_js(Json* js, char* name, char* val)
{
	return strcmp(js->pairs[find_in_js(js,name)].value, val);
}



//ok
char* get_value(Json* js, char* name)
{
	int i = find_in_js(js,name);
	if (i != -1)
		return js->pairs[i].value;
	return 0;
}


/*int boolcmp(int val, int boolean);
//ok
int boolcmp(int val, int boolean) {
	if (boolean)
	{
		return val == 0;
	}
	return val != 0;
}*/
//ok
char* get_element(JsArr* arr,int i)
{
	if(i<arr->size)
		return arr->elems[i].value;
	return NULL;
}





//ok
char* forJson_strf(char* from, char* to)
{
	for (; (*to = *from)!=0; to++, from++)
	{
		if (from[1] == '}' || from[1] == '{' || from[1] == '[' || from[1] == ']' || from[1] == '\"' || from[1] == '\\') {
			*(++to) = '\\';


		}

	}
	return to;
}




//ok
char* fromJson_strf(char* from, char* to)
{
	for (; (*to = *from)!=0; to++, from++)
	{
		if (*from == '\\') {
			if (from[1] == '{' || from[1] == '}' || from[1] == '[' || from[1] == ']' || from[1] == '\"' || from[1] == '\\') {
				from++;
				*to = *from;
			}
		}

	}
	return to;
}


//ok
int free_js(Json* js)
{
	int i;
	for (i = 0; i < js->size; i++)
	{
		free(js->pairs[i].name);
		free(js->pairs[i].value);
	}
	free(js->pairs);
	free(js);
	return 0;
}





//ok
int free_jsarr(JsArr* arr)
{
	int i;
	for (i = 0; i < arr->size; i++)
	{
		free(arr->elems[i].value);
	}
	free(arr->elems);
	free(arr);
	return 0;
}
//ok
int free_arr(void* arr)
{
	free(arr);
	return 0;
}



//ok
int free_cstr_arr(char** arr, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		free(arr[i]);
	}
	free(arr);
	return 0;
}



//ok
char* corrector(char* from, char* to)
{
	int quot_closed = true;
	char* start = from;
	for (; *from; from++)
	{
		if (quot_closed) {
			if (*from == ' ' || *from == '\n') {
				continue;
			}
			if (*from == '\"')
				quot_closed = false;
		}
		else {
			if (*from == '\"'){
				if (from > start + 1) {
					if (*(from - 1) != '\\' || (*(from - 1) == '\\' && *(from - 2) == '\\')) {
						quot_closed = true;
					}
				}
				else quot_closed = false;
			}
		}

		*to = *from;
		to++;
	}
	*to = '\0';
	return to;
}

int parse_pair(char* from, int size, JsonPair* js)
{
	char* start = from;
	int lenght = 0;
	for (from += 2; *from != '\"' && *from; lenght++, from++)
		;
	if (!(*from) || start[1] != '\"' || from[1] != ':')
		return -1;
	from += 2;
	lenght++;
	js->name = malloc(lenght * sizeof(char));
	if (js->name) {
		strncpy(js->name, start + 2, lenght);
		(js->name)[lenght - 1] = '\0';
	}
	size = size - (int)(from - start) - 1;
	if ((from[size] != '\"' || *(from) != '\"') && (*from != '{' || from[size] != '}')
		&& (*from != '[' || from[size] != ']') && (!isdigit(*from) || !isdigit(from[size])) && (strncmp("true", from, size + 1) || size + 1 != 4) && (strncmp("false", from, size + 1) || size + 1 != 5)) {
		free(js->name);
		return -1;
	}
	if (*from == '\"')
	{
		(js->value) = malloc(size * sizeof(char));
		if (js->value) {

			strncpy(js->value, ++from, size);
			(js->value)[size - 1] = '\0';
		}
	}
	else {
		size++;
		(js->value) = malloc((size + 1) * sizeof(char));
		if (js->value) {
			strncpy((js->value), from, size);
			(js->value)[size] = '\0';
		}
	}
	return 0;
}
