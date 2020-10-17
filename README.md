# S-JSON-C
C/C++ Lightweight and simple json parser.








How to use:


  Json js;//Create js.

  Json* js=fromJson("{\"name1\":\"val1\",\"name2\":\"val2\",\"name3\":[1,2]}");
  /*Parse json file if error it will return null elso Json structure.*/

  //Use Json object.
  char* val1 = get_value(js, "name1")//returns c_str (in this example "val1")
  char* arr_str = get_value(js, "name3")
  
  /*If name doesnt exist in js it will return NULL else c_str func return("[1,2]" in this example)*/
  /*js.size return size of json pairs*/ 


  char* arr_str="[{\"name1\":\"val1\"},\"some string\"]"
  JsArr* js_arr = from_jsarr(arr_str);
  /*Returned value js_arr was JsArr structure which elements are strings from json-array 
  to get elements for use  */
  /*First arg is a sring which is array from json file,  
  returns null if error.*/
  //Use JsArr object.
  char* elem1 = get_ellement(js_arr, 0);
  char* elem2 = get_ellement(js_arr, 1);
  char* elem3 = get_ellement(js_arr, 2);

  /*In this exemple get element returns 
  elem1->"{\"name1\":\"val1\"}" 
  elem2->"\"some string\""
  elem3->NULL*/  
  /*js_arr.size return size of array elments*/ 
 



  free_js(js);//free allocated mamory
  free_jsarr(js_arr)//free allocated mamory
  
  typedef struct{JsonPair *pairs;int size;}Json;
  typedef struct{JsonPair *pairs;int size;}Json;
  typedef struct{Element* elems;int size;}JsArr;
  typedef struct{char* value;}Element;
