# S-JSON-C

A lightweight and simple JSON parser for C/C++ with minimal dependencies.

## Features

- **Lightweight**: Minimal memory footprint and simple implementation
- **No external dependencies**: Uses only standard C libraries
- **Easy to use**: Simple API for parsing and accessing JSON data
- **Support for various data types**: Objects, arrays, strings, numbers, and booleans
- **Memory management**: Built-in functions for proper memory cleanup

## Installation

Simply include the header and source files in your project:

```c
#include "sjson.h"
```

Compile with:
```bash
gcc -o your_program your_program.c sjson.c
```

## Quick Start

### Parsing JSON Objects

```c
#include "sjson.h"
#include <stdio.h>

int main() {
    // Parse a JSON string
    Json* js = from_json("{\"name\":\"John\",\"age\":30,\"city\":\"New York\"}");
    
    if (js != NULL) {
        // Access values by key
        char* name = get_value(js, "name");
        char* age = get_value(js, "age");
        
        printf("Name: %s\n", name);  // Output: John
        printf("Age: %s\n", age);    // Output: 30
        
        // Clean up
        free_js(js);
    }
    
    return 0;
}
```

### Working with JSON Arrays

```c
// Parse a JSON array
char* arr_str = "[{\"id\":1,\"name\":\"Item1\"},\"some string\",42]";
JsArr* js_arr = from_jsarr(arr_str);

if (js_arr != NULL) {
    // Access array elements by index
    char* elem1 = get_element(js_arr, 0);  // Returns: {"id":1,"name":"Item1"}
    char* elem2 = get_element(js_arr, 1);  // Returns: "some string"
    char* elem3 = get_element(js_arr, 2);  // Returns: 42
    
    printf("Array size: %d\n", js_arr->size);
    
    // Clean up
    free_jsarr(js_arr);
}
```

## API Reference

### Main Functions

#### JSON Object Functions

| Function | Description | Returns |
|----------|-------------|---------|
| `Json* from_json(char* data)` | Parse a JSON string into a Json structure | Json* or NULL on error |
| `char* get_value(Json* js, char* name)` | Get value for a given key | Value string or NULL if not found |
| `int find_in_js(Json* js, char* string)` | Find index of a key in JSON object | Index or -1 if not found |
| `int cmp_in_js(Json* js, char* name, char* val)` | Compare a value in JSON with given string | strcmp result or -1 |
| `void free_js(Json* js)` | Free allocated memory for Json structure | - |

#### JSON Array Functions

| Function | Description | Returns |
|----------|-------------|---------|
| `JsArr* from_jsarr(char* from)` | Parse a JSON array string | JsArr* or NULL on error |
| `char* get_element(JsArr* arr, int i)` | Get element at index i | Element string or NULL |
| `int find_in_jsarr(JsArr* arr, char* string)` | Find index of a string in array | Index or -1 if not found |
| `void free_jsarr(JsArr* arr)` | Free allocated memory for JsArr structure | - |

#### Specialized Array Parsing Functions

| Function | Description |
|----------|-------------|
| `char** csarr_from_cstr(char* from, int alloc_size, int* realsize)` | Parse array of strings |
| `int* iarr_from_cstr(char* from, int alloc_size, int* realsize)` | Parse array of integers |
| `double* farr_from_cstr(char* from, int alloc_size, int* realsize)` | Parse array of floats |

#### Utility Functions

| Function | Description |
|----------|-------------|
| `char* forJson_strf(char* from, char* to)` | Escape special characters for JSON |
| `char* fromJson_strf(char* from, char* to)` | Unescape JSON special characters |

### Data Structures

```c
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
```

## Examples

### Complex JSON Parsing

```c
// Parse nested JSON
char* complex_json = "{\"user\":{\"name\":\"Alice\"},\"items\":[1,2,3]}";
Json* js = from_json(complex_json);

if (js != NULL) {
    // Get nested object as string
    char* user_obj = get_value(js, "user");  // Returns: {"name":"Alice"}
    
    // Get array as string
    char* items = get_value(js, "items");    // Returns: [1,2,3]
    
    // Parse the nested structures if needed
    Json* user = from_json(user_obj);
    JsArr* items_arr = from_jsarr(items);
    
    // Don't forget to free memory
    free_js(user);
    free_jsarr(items_arr);
    free_js(js);
}
```

### Parsing Integer Arrays

```c
char* int_array = "[10,20,30,40,50]";
int realsize;
int* numbers = iarr_from_cstr(int_array, 10, &realsize);

if (numbers != NULL) {
    for (int i = 0; i < realsize; i++) {
        printf("Number %d: %d\n", i, numbers[i]);
    }
    free_arr(numbers);
}
```

### Error Handling

```c
Json* js = from_json("{invalid json}");
if (js == NULL) {
    printf("Error: Invalid JSON format\n");
} else {
    // Process valid JSON
    free_js(js);
}
```

## Important Notes

1. **Memory Management**: Always free allocated memory using the appropriate free functions (`free_js()`, `free_jsarr()`, etc.)

2. **Return Values**: Most parsing functions return NULL on error, always check return values

3. **String Values**: The library returns string representations for all values, including numbers and booleans. Parse them as needed in your application.

4. **Nested Structures**: Nested objects and arrays are returned as strings. Parse them separately if you need to access their contents.

5. **Thread Safety**: The library is not thread-safe. Use appropriate synchronization if accessing from multiple threads.

## Limitations

- No support for JSON validation beyond basic structure
- Limited Unicode support
- Numbers are returned as strings and need manual conversion
- No pretty-printing functionality
- No JSON generation/serialization functions (parsing only)

## Building and Testing

### Basic compilation:
```bash
gcc -c sjson.c -o sjson.o
gcc -o test_program test.c sjson.o
```

### With debugging:
```bash
gcc -g -Wall -c sjson.c -o sjson.o
gcc -g -Wall -o test_program test.c sjson.o
```

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## License

[Add your license information here]

## Author

Serezha Grigoryan

## Version History

- 1.0.0 - Initial release with basic JSON parsing functionality
