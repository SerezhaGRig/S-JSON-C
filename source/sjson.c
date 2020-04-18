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



int parse_pair(char* from, int size, char** name, char** val);



//ok
int parse_pair(char* from, int size, char** name, char** val)
{
	char* start = from;
	int lenght = 0;
	for (from += 2; *from != '\"' && *from; lenght++, from++)
		;
	if (!(*from) || start[1] != '\"' || from[1] != ':')
		return -1;
	from += 2;
	lenght++;
	(*name) = malloc(lenght * sizeof(char));
	if (*name) {
		strncpy(*name, start + 2, lenght);
		(*name)[lenght - 1] = '\0';
	}
	size = size - (int)(from - start) - 1;
	if ((from[size] != '\"' || *(from) != '\"') && (*from != '{' || from[size] != '}')
		&& (*from != '[' || from[size] != ']') && (!isdigit(*from) || !isdigit(from[size])) && (strncmp("True", from, size + 1) || size + 1 != 4) && (strncmp("False", from, size + 1) || size + 1 != 5)) {
		free(*name);
		return -1;
	}
	if (*from == '\"')
	{
		(*val) = malloc(size * sizeof(char));
		if (*val) {
			strncpy(*val, ++from, size);
			(*val)[size - 1] = '\0';
		}
	}
	else {

		size++;
		(*val) = malloc((size + 1) * sizeof(char));

		if (*val) {
			strncpy((*val), from, size);
			(*val)[size] = '\0';
		}
	}
	return 0;
}





//ok
int fromJson(char* from, struct json* js)
{
	return fromJson_resize(from, js, OBJ_COUNT_DEF);
}




//ok
int fromJson_resize(char* from, struct json* js, int alloc_resize)
{
	if (from == NULL)
		return NULL;
	js->names = malloc(alloc_resize * sizeof(char*));
	js->values = malloc(alloc_resize * sizeof(char*));
	int	count;
	int open_brecks = 0;

	int quot_closed = true;
	char* start = from++;
	for (count = 0; *(from); from++)
	{
		if (quot_closed) {
			if ((*from == '{' || *from == '['))
				open_brecks++;
			if (*from == '\"')
			{

				quot_closed = false;
				from++;
				continue;

			}
			if ((*from == ',' || *from == '}') && !open_brecks)
			{
				int size = from - start;
				if (js->names && js->values) {
					if (parse_pair(start, size, &(js->names[count]), &(js->values[count]))) {
						free_cstr_arr(js->names, count);
						free_cstr_arr(js->values, count);
						return -1;
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
		return 0;
	free(js->names);
	free(js->values);
	return -1;
}



//ok
int find_in_arr(char** arr, int size, char* string)
{
	int i;
	for (i = 0; i < size; i++)
		if (!strcmp(string, arr[i]))
			return i;
	return -1;
}




//ok
int ex_in_js(struct json* js, char* name, char* val)
{
	return strcmp(js->values[find_in_arr(js->names, js->size, name)], val);
}



//ok
char* get_value(struct json* js, char* name)
{
	int i = find_in_arr(js->names, js->size, name);
	if (i != -1)
		return js->values[i];
	return 0;
}



//ok
int free_js(struct json* js)
{
	int i;
	for (i = 0; i < js->size; i++)
	{
		free(js->names[i]);
		free(js->values[i]);
	}
	free(js->names);
	free(js->values);
	return 0;
}



//ok
int* iarr_from_cstr(char* from, int alloc_size, int* realsize)
{

	if (from == NULL)
		return NULL;
	int* int_arr = malloc((alloc_size) * sizeof(int));
	int a, i, entered;
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
			entered = true;
			a = a * 10 + (*from) - '0';

		}
		if (*from != ',' && from != ']')
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
char** csarr_from_cstr(char* from, int alloc_size, int* realsize)
{
	if (from == NULL)
		return NULL;
	char* start;
	char** carr = malloc(alloc_size * sizeof(char*));


	int i;
	int quot_closed = true;
	int breck_count = 0;
	int lenght = 0;
	int error;

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
char** multiarr_from_cstr(char* from, int alloc_size, int* realsize)
{
	if (from == NULL)
		return NULL;
	char** carr = malloc(alloc_size * sizeof(char*));
	char* start;
	int breck_count = 0, lenght, quot_closed = true, i = 0;

	for (lenght = 0; *from != ']'; lenght = 0, i++) {
		if (*from != '[' && *from != ',') {
			free_cstr_arr(carr, i);
			return NULL;
		}

		from++;
		if (!(*from))
		{
			free_cstr_arr(carr, i);
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
		if (!((*start == '\"' && *(from - 1) == '\"') || (*start == '{' && *(from - 1) == '}')
			|| (*start == '[' && *(from - 1) == ']') || (isdigit(*start) && isdigit(*(from - 1))) || (!strncmp(start, "True", from - start) && from - start == 4) || (!strncmp(start, "False", from - start) && from - start == 4)))
		{
			free_cstr_arr(carr, i);
			return NULL;
		}

		if (*start == '\"') {
			start++;
			lenght -= 2;
		}

		if (carr) {

			i = i % alloc_size;
			carr[i] = malloc((lenght + 1) * sizeof(char));
			if (carr[i]) {
				strncpy(carr[i], start, lenght);
				(carr[i])[lenght] = '\0';
			}
		}
	}
	if (*from == ']')
	{
		if (realsize != NULL)
			* realsize = i;
		return carr;
	}
	free_cstr_arr(carr, i);
	return NULL;
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
char* forJson_strf(char* from, char* to)
{
	for (; *to = *from; to++, from++)
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
	for (; *to = *from; to++, from++)
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


/*int boolcmp(int val, int boolean);
//ok
int boolcmp(int val, int boolean) {
	if (boolean)
	{
		return val == 0;
	}
	return val != 0;
}*/
int corrector(char* from, char* to)
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
			if (*from == '\"')
				if (from > start + 1) {
					if (*(from - 1) != '\\' || (*(from - 1) == '\\' && *(from - 2) == '\\')) {
						quot_closed = true;
					}
				}
				else quot_closed = false;
		}

		*to = *from;
		to++;
	}
	*to = '\0';
}