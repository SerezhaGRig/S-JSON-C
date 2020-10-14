# S-JSON-C
C/C++ Lightweight and simple json parser.








How to use:


  Json js;//Create js.

  fromJson("{\"name1\":\"val1\",\"name2\":\"val2\",\"name3\":[1,2]}", &js);
  /*Parse js if error it will return -1.*/

  //Use js.
  char* val1 = get_value(&js, "name1")//returns c_str (in this example "val1")
  char* arr_str = get_value(&js, "name3")
  
  /*If name doesnt exist in js it will return NULL else c_str func return("[1,2]" in this example)*/



  char* arr_str="[{\"name1\":\"val1\"},\"some string\"]"
  char** js_arr = jsarr_from_cstr(arr_str, &count);
  /*Returned value js_arr was array which elements are strings from json-array 
  for example arr_str[0]->"{\"name1\":\"val1\"}"  
  */

  /*First arg is a sring which is array,  
  last arg returns cstring array count (2 in this example) function  returns null if error.*/

  //Use js.



  free_js(&js);//free allocated mamory
  free_arr(ints)//free allocated mamory
  
  typedef struct{ char** names; char** values; int size; } Json;
