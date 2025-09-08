/*
 Author: Serezha Grigoryan
 */
#ifndef SJSON_H
#define SJSON_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    char* name;
    char* value;
} JsonPair;

typedef struct {
    JsonPair* pairs;
    int size;
} Json;

typedef struct {
    char* value;
} Element;

typedef struct {
    Element* elems;
    int size;
} JsArr;

// Error codes
typedef enum {
    SJSON_OK = 0,
    SJSON_ERROR = -1,
    SJSON_INVALID_JSON = -2,
    SJSON_OUT_OF_MEMORY = -3,
    SJSON_KEY_NOT_FOUND = -4,
    SJSON_INDEX_OUT_OF_BOUNDS = -5,
    SJSON_NULL_PARAMETER = -6
} sjson_error_t;

// Parsing Methods
Json* from_json(const char* data);
JsArr* from_jsarr(const char* from);
char** csarr_from_cstr(const char* from, int* realsize);
int* iarr_from_cstr(const char* from, int* realsize);
double* farr_from_cstr(const char* from, int* realsize);

// Object Methods
char* get_value(Json* js, const char* name);
int cmp_in_js(Json* js, const char* name, const char* val);
char* get_element(JsArr* arr, int i);
int find_in_jsarr(JsArr* arr, const char* string);
char* forJson_strf(const char* from, char* to);
char* fromJson_strf(const char* from, char* to);
int find_in_js(Json* js, const char* string);

// Memory free Methods
int free_js(Json* js);
int free_jsarr(JsArr* arr);
int free_cstr_arr(char** arr, int size);
int free_arr(void* arr);

#endif // SJSON_H