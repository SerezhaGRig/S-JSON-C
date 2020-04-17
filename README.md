# S-JSON-C
C/C++ Lightweight and simple json parser.








How to use:
  struct json js;//Create js.

  fromJson("{\"name1\":\"val1\",\"name2\":\"val2\",\"name3\":[1,2]}", &js);
  /*Parse js if error it will return -1.
  To resize allocated array size use fromJson_resize("{\"name\":\"val\"}", &js, your (name,val) pair alloc array size);.*/


  //Use js.
  char* val1 = get_value(&js, "name1")//returns c_str (in this example "val1")
  char* arr_str = get_value(&js, "name3")
  
  /*If name doesnt exist in js it will return NULL else c_str ("[1,2]" in this example)*/

  int* ints = iarr_from_cstr(arr_str, your int arr size, &count);
  /*First arg arr_str, second argument alloc array size,  
  last arg returns ints count (2 in this example) function  returns null if error.*/

  //Use js.



  free_js(&js);//free allocated mamory
  free_arr(ints)//free allocated mamory
  
  struct json { char** names; char** values; int size; };
