#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4996)  // More specific than _CRT_SECURE_NO_WARNINGS
#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>
#include "sjson.h"

#define MAX_NESTING_DEPTH 1000

// Forward declarations
static int parse_pair(const char* from, int size, JsonPair* js);
static char* corrector(const char* from, char* to);
static char* safe_strncpy(char* dest, const char* src, size_t n);

// Safe string copy helper
static char* safe_strncpy(char* dest, const char* src, size_t n) {
    if (!dest || !src || n == 0) return dest;
    
    size_t i;
    for (i = 0; i < n - 1 && src[i]; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return dest;
}

Json* from_json(const char* dirty) {
    if (dirty == NULL) {
        return NULL;
    }
    
    size_t dirty_len = strlen(dirty);
    if (dirty_len > INT_MAX - 1) {
        return NULL;  // String too long
    }
    
    char* clear = malloc(dirty_len + 1);
    if (clear == NULL) {
        return NULL;
    }
    
    corrector(dirty, clear);
    
    Json* js = calloc(1, sizeof(Json));
    if (js == NULL) {
        free(clear);
        return NULL;
    }
    
    int count = 0;
    int open_brecks = 0;
    bool quot_closed = true;
    char* from = clear;
    char* start = from++;
    
    for (count = 0; *from; from++) {
        if (quot_closed) {
            if (*from == '{' || *from == '[') {
                open_brecks++;
                if (open_brecks > MAX_NESTING_DEPTH) {
                    free_js(js);
                    free(clear);
                    return NULL;
                }
            }
            if (*from == '"') {
                quot_closed = false;
                if (*(from + 1) != '"') {
                    from++;
                }
                continue;
            }
            if ((*from == ',' || *from == '}') && !open_brecks) {
                int size = from - start;
                if (size < 0) {
                    free_js(js);
                    free(clear);
                    return NULL;
                }
                
                JsonPair* tmp = realloc(js->pairs, (count + 1) * sizeof(JsonPair));
                if (tmp == NULL) {
                    free_js(js);
                    free(clear);
                    return NULL;
                }
                js->pairs = tmp;
                
                // Initialize new pair
                js->pairs[count].name = NULL;
                js->pairs[count].value = NULL;
                
                if (parse_pair(start, size, &(js->pairs[count])) != 0) {
                    js->size = count + 1;  // Include failed pair for cleanup
                    free_js(js);
                    free(clear);
                    return NULL;
                }
                count++;
                start = from;
            }
            if (*from == '}' || *from == ']') {
                open_brecks--;
            }
        } else {
            if (*from == '"') {
                if (from > clear && (*(from - 1) != '\\' || 
                    (from > clear + 1 && *(from - 1) == '\\' && *(from - 2) == '\\'))) {
                    quot_closed = true;
                }
            }
        }
    }
    
    js->size = count;
    if (count && *(from - 1) == '}' && open_brecks == -1 && quot_closed) {
        free(clear);
        return js;
    }
    
    free_js(js);
    free(clear);
    return NULL;
}

JsArr* from_jsarr(const char* from) {
    if (from == NULL || *from != '[') {
        return NULL;
    }
    
    JsArr* jsarr = calloc(1, sizeof(JsArr));
    if (jsarr == NULL) {
        return NULL;
    }
    
    const char* start;
    int breck_count = 0, lenght;
    bool quot_closed = true;
    int i = 0;
    
    from++;  // Skip initial '['
    
    while (*from && *from != ']') {
        // Skip commas and whitespace
        while (*from && (*from == ',' || isspace(*from))) {
            from++;
        }
        
        if (*from == ']') {
            break;
        }
        
        start = from;
        lenght = 0;
        
        // Parse one element
        while (*from) {
            if ((*from == ']' || *from == ',') && breck_count <= 0 && quot_closed) {
                break;
            }
            
            if (quot_closed) {
                if (*from == '{' || *from == '[') {
                    breck_count++;
                } else if (*from == '}' || *from == ']') {
                    breck_count--;
                } else if (*from == '"') {
                    quot_closed = false;
                }
            } else {
                if (*from == '"' && from > start) {
                    if (*(from - 1) != '\\' || 
                        (from > start + 1 && *(from - 1) == '\\' && *(from - 2) == '\\')) {
                        quot_closed = true;
                    }
                }
            }
            
            lenght++;
            from++;
        }
        
        if (lenght == 0) {
            free_jsarr(jsarr);
            return NULL;
        }
        
        // Reallocate array
        Element* tmp = realloc(jsarr->elems, (i + 1) * sizeof(Element));
        if (tmp == NULL) {
            free_jsarr(jsarr);
            return NULL;
        }
        jsarr->elems = tmp;
        
        // Allocate and copy element
        jsarr->elems[i].value = malloc((lenght + 1) * sizeof(char));
        if (jsarr->elems[i].value == NULL) {
            jsarr->size = i + 1;
            free_jsarr(jsarr);
            return NULL;
        }
        
        safe_strncpy(jsarr->elems[i].value, start, lenght + 1);
        i++;
        jsarr->size = i;
    }
    
    if (*from == ']') {
        return jsarr;
    }
    
    free_jsarr(jsarr);
    return NULL;
}

char** csarr_from_cstr(const char* from, int* realsize) {
    if (from == NULL || *from != '[' || realsize == NULL) {
        return NULL;
    }
    
    int capacity = 10;  // Initial capacity
    char** carr = malloc(capacity * sizeof(char*));
    if (carr == NULL) {
        return NULL;
    }
    
    const char* start;
    int i = 0;
    bool quot_closed = true;
    int lenght = 0;
    
    from++;  // Skip '['
    
    while (*from && *from != ']') {
        // Skip comma and whitespace
        while (*from && (*from == ',' || isspace(*from))) {
            from++;
        }
        
        if (*from != '"') {
            free_cstr_arr(carr, i);
            return NULL;
        }
        
        from++;  // Skip opening quote
        start = from;
        lenght = 0;
        
        // Find closing quote
        while (*from) {
            if (*from == '"' && (from == start || *(from - 1) != '\\' || 
                (from > start + 1 && *(from - 1) == '\\' && *(from - 2) == '\\'))) {
                break;
            }
            lenght++;
            from++;
        }
        
        if (*from != '"') {
            free_cstr_arr(carr, i);
            return NULL;
        }
        
        // Resize array if needed
        if (i >= capacity) {
            capacity *= 2;
            char** tmp = realloc(carr, capacity * sizeof(char*));
            if (tmp == NULL) {
                free_cstr_arr(carr, i);
                return NULL;
            }
            carr = tmp;
        }
        
        // Allocate and copy string
        carr[i] = malloc((lenght + 1) * sizeof(char));
        if (carr[i] == NULL) {
            free_cstr_arr(carr, i);
            return NULL;
        }
        
        safe_strncpy(carr[i], start, lenght + 1);
        i++;
        from++;  // Skip closing quote
    }
    
    if (*from == ']') {
        *realsize = i;
        return carr;
    }
    
    free_cstr_arr(carr, i);
    return NULL;
}

int* iarr_from_cstr(const char* from, int* realsize) {
    if (from == NULL || *from != '[' || realsize == NULL) {
        return NULL;
    }
    
    int capacity = 10;
    int* int_arr = malloc(capacity * sizeof(int));
    if (int_arr == NULL) {
        return NULL;
    }
    
    int i = 0;
    from++;  // Skip '['
    
    while (*from && *from != ']') {
        // Skip comma and whitespace
        while (*from && (*from == ',' || isspace(*from))) {
            from++;
        }
        
        if (*from == ']') {
            break;
        }
        
        // Parse number
        char* endptr;
        long val = strtol(from, &endptr, 10);
        
        if (endptr == from || val > INT_MAX || val < INT_MIN) {
            free(int_arr);
            return NULL;
        }
        
        // Resize if needed
        if (i >= capacity) {
            capacity *= 2;
            int* tmp = realloc(int_arr, capacity * sizeof(int));
            if (tmp == NULL) {
                free(int_arr);
                return NULL;
            }
            int_arr = tmp;
        }
        
        int_arr[i++] = (int)val;
        from = endptr;
    }
    
    if (*from == ']') {
        *realsize = i;
        return int_arr;
    }
    
    free(int_arr);
    return NULL;
}

double* farr_from_cstr(const char* from, int* realsize) {
    if (from == NULL || *from != '[' || realsize == NULL) {
        return NULL;
    }
    
    int capacity = 10;
    double* double_arr = malloc(capacity * sizeof(double));
    if (double_arr == NULL) {
        return NULL;
    }
    
    int i = 0;
    from++;  // Skip '['
    
    while (*from && *from != ']') {
        // Skip comma and whitespace
        while (*from && (*from == ',' || isspace(*from))) {
            from++;
        }
        
        if (*from == ']') {
            break;
        }
        
        // Parse number
        char* endptr;
        double val = strtod(from, &endptr);
        
        if (endptr == from) {
            free(double_arr);
            return NULL;
        }
        
        // Resize if needed
        if (i >= capacity) {
            capacity *= 2;
            double* tmp = realloc(double_arr, capacity * sizeof(double));
            if (tmp == NULL) {
                free(double_arr);
                return NULL;
            }
            double_arr = tmp;
        }
        
        double_arr[i++] = val;
        from = endptr;
    }
    
    if (*from == ']') {
        *realsize = i;
        return double_arr;
    }
    
    free(double_arr);
    return NULL;
}

int find_in_jsarr(JsArr* arr, const char* string) {
    if (arr == NULL || string == NULL) {
        return -1;
    }
    
    for (int i = 0; i < arr->size; i++) {
        if (arr->elems[i].value && strcmp(string, arr->elems[i].value) == 0) {
            return i;
        }
    }
    return -1;
}

int find_in_js(Json* js, const char* string) {
    if (js == NULL || string == NULL) {
        return -1;
    }
    
    for (int i = 0; i < js->size; i++) {
        if (js->pairs[i].name && strcmp(string, js->pairs[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

int cmp_in_js(Json* js, const char* name, const char* val) {
    if (js == NULL || name == NULL || val == NULL) {
        return SJSON_NULL_PARAMETER;
    }
    
    int idx = find_in_js(js, name);
    if (idx == -1) {
        return SJSON_KEY_NOT_FOUND;
    }
    
    if (js->pairs[idx].value == NULL) {
        return SJSON_ERROR;
    }
    
    return strcmp(js->pairs[idx].value, val);
}

char* get_value(Json* js, const char* name) {
    if (js == NULL || name == NULL) {
        return NULL;
    }
    
    int i = find_in_js(js, name);
    if (i != -1 && js->pairs[i].value != NULL) {
        return js->pairs[i].value;
    }
    return NULL;
}

char* get_element(JsArr* arr, int i) {
    if (arr == NULL || i < 0 || i >= arr->size) {
        return NULL;
    }
    return arr->elems[i].value;
}

char* forJson_strf(const char* from, char* to) {
    if (from == NULL || to == NULL) {
        return to;
    }
    
    char* start = to;
    for (; *from != '\0'; from++) {
        if (*from == '}' || *from == '{' || *from == '[' || 
            *from == ']' || *from == '"' || *from == '\\') {
            *to++ = '\\';
        }
        *to++ = *from;
    }
    *to = '\0';
    return start;
}

char* fromJson_strf(const char* from, char* to) {
    if (from == NULL || to == NULL) {
        return to;
    }
    
    char* start = to;
    for (; *from != '\0'; from++) {
        if (*from == '\\') {
            if (from[1] == '{' || from[1] == '}' || from[1] == '[' || 
                from[1] == ']' || from[1] == '"' || from[1] == '\\') {
                from++;
                *to++ = *from;
            } else {
                *to++ = *from;
            }
        } else {
            *to++ = *from;
        }
    }
    *to = '\0';
    return start;
}

int free_js(Json* js) {
    if (js != NULL) {
        if (js->pairs != NULL) {
            for (int i = 0; i < js->size; i++) {
                free(js->pairs[i].name);
                free(js->pairs[i].value);
            }
            free(js->pairs);
        }
        free(js);
    }
    return 0;
}

int free_jsarr(JsArr* arr) {
    if (arr != NULL) {
        if (arr->elems != NULL) {
            for (int i = 0; i < arr->size; i++) {
                free(arr->elems[i].value);
            }
            free(arr->elems);
        }
        free(arr);
    }
    return 0;
}

int free_arr(void* arr) {
    free(arr);
    return 0;
}

int free_cstr_arr(char** arr, int size) {
    if (arr != NULL) {
        for (int i = 0; i < size; i++) {
            free(arr[i]);
        }
        free(arr);
    }
    return 0;
}

static char* corrector(const char* from, char* to) {
    if (from == NULL || to == NULL) {
        return to;
    }
    
    bool quot_closed = true;
    const char* start = from;
    char* to_start = to;
    
    for (; *from; from++) {
        if (quot_closed) {
            if (*from == ' ' || *from == '\n' || *from == '\t' || *from == '\r') {
                continue;
            }
            if (*from == '"') {
                quot_closed = false;
            }
        } else {
            if (*from == '"') {
                if (from > start && (*(from - 1) != '\\' || 
                    (from > start + 1 && *(from - 1) == '\\' && *(from - 2) == '\\'))) {
                    quot_closed = true;
                }
            }
        }
        
        *to++ = *from;
    }
    
    if (to > to_start && *(to - 1) == ';') {
        to--;
    }
    *to = '\0';
    return to_start;
}

static int parse_pair(const char* from, int size, JsonPair* js) {
    if (from == NULL || js == NULL || size < 7) {  // Minimum: {"a":1}
        return -1;
    }
    
    // Skip initial comma or opening brace
    if (*from == ',' || *from == '{') {
        from++;
        size--;
    }
    
    // Check for opening quote
    if (*from != '"') {
        return -1;
    }
    
    from++;
    size--;
    
    // Find name
    const char* name_start = from;
    int name_len = 0;
    
    while (size > 0 && *from != '"') {
        name_len++;
        from++;
        size--;
    }
    
    if (*from != '"' || size < 3 || from[1] != ':') {
        return -1;
    }
    
    // Allocate name
    js->name = malloc(name_len + 1);
    if (js->name == NULL) {
        return -1;
    }
    safe_strncpy(js->name, name_start, name_len + 1);
    
    // Skip quote and colon
    from += 2;
    size -= 2;
    
    // Parse value
    if (*from == '"') {
        // String value
        from++;
        size--;
        const char* val_start = from;
        int val_len = 0;
        
        while (size > 0 && *from != '"') {
            val_len++;
            from++;
            size--;
        }
        
        if (*from != '"') {
            free(js->name);
            js->name = NULL;
            return -1;
        }
        
        js->value = malloc(val_len + 1);
        if (js->value == NULL) {
            free(js->name);
            js->name = NULL;
            return -1;
        }
        safe_strncpy(js->value, val_start, val_len + 1);
        
    } else {
        // Non-string value (number, boolean, object, array)
        js->value = malloc(size + 1);
        if (js->value == NULL) {
            free(js->name);
            js->name = NULL;
            return -1;
        }
        safe_strncpy(js->value, from, size + 1);
    }
    
    return 0;
}

#ifdef _WIN32
#pragma warning(pop)
#endif