# S-JSON-C
C/C++ lightweight json parser



How to use:
  struct json js;//create js

  fromJson("{'name1':val1,'name2':val2,'name3':[1,2]}", &js);
  /*parse js if error it will return -1
  to resize allocated array size use fromJson_resize("{'name':val}", &js, your (name,val)pair arr size);*/


  //use js

  char* arr_str = get_value(&js, "name3")
  /*if name doesnt exist in js it will return NULL else c_str ("[1,2]" in this example)*/

  int* ints = iarr_from_cstr(arr_str, your int arr size, &count);
  /*first arg arr_str second argument alloc array size  
  last arg returns ints count (2 in this example) function  returns null if error*/

  //use js



  free_js(&js);//free allocated mamory
  free_arr(ints)//free allocated mamory
