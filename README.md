
## Installation

download the repo & make user the files are in same folder and import eqn.h into your project
```c
#include "eqn.h"
```
## Quick Example
```c
#include <stdio.h>
#include "eqn.h"
int main(){
    Error error;
    Expression exp;
    init_expression(&exp);
    float result=expression_eval(&exp,"2+2",NULL,&error);
    printf("2+2=%f\n",result);
    /* output:
        2+2=4.000000
    */
    return 0;
}

```
## Struct
### Expression:
An Expression instance holds all the information about the expression being execuated. It consis of:
+ opcode_stack: stack used fo storing  opcode_stac
+ data_stack: satck used for storing data during evlation
+ funciton_dict: dictionary used for storing predefined function 
+ constant_dict: dictionary used for storing constants.
+ variable_dict: dictionary used for storing variable.

### Error:
```c
typedef struct{
	Status status; // __sucess__,__failed__,__pending__
	char msg[100];
} Error;
```
Error is used to hold the state fo evaluation process. 
+ status: holds either of 3 enum value ( \__sucess__,\__failed__,\__pending__) represeting the state of eval process.
+ msg: msg is set usually when status in \__failed__. Stores the description on why the eval process may have failed.

### Function_struct:
```c
typedef struct{
	char *name;
	float (*func)(float*,int);
	int parms;
} Function_struct;
```
Used for defining funciton.
+ name: the function name (used in evaluation strnig)
+ func: pointer to the actual function in c.
+ parms: the number of paramers that funciton can accept.
***!!! It is important that u  define function in c with return type float and parameters (flot\*,int) !!!***

***flaot\* \-> contains list of argemets that is passed***

***int-> length of parameters u have defined in funciton struct (passed to help avoid segfault)***

#### Example
```c
#include <stdio.h>
#include "eqn.h"
float power(float *f,int len){
    float p=1;
    for(int i=0;i<f[1];i++)p*=f[0];
    return p;
}
int main(){
    Error error; Expression exp;

    init_expression(&exp);
    Function_struct func[]={{"pow",&power,2},{NULL,NULL,0}}; /*pow(x,y) so parms=2*/
    register_functions(&exp,func);/* registering fncton in exp*/

    float result=expression_eval(&exp,"pow(2)",NULL,&error);
    if(error.status==__failed__)printf("1st exp  error: %s\n",error.msg);
    else printf("1st exp resut:%f\n",result);
    
    result=expression_eval(&exp,"pow(2,2)",NULL,&error);
    if(error.status==__failed__)printf("2nd exp  error: %s\n",error.msg);
    else printf("2nd exp resut:%f\n",result);

    /* output:
        1st exp  error: Error in function call  "pow":arguments required->2, provide->1
        2nd exp resut:4.000000

    */
    return 0;
}
```

### Variable_struct:
A key value pair used to represent variable in process. 
```c
typedef struct{
	char *name;
	float value;
} Variable_struct;
```



## API
###  void init_expression(Expression *exp):
initilalize the expression to a new/ready state before used. If u calling init_expression() after certain used make sure u call flush_expression() before.

### void register_functions(Expression *e,Function_struct *function):
stores function into given expression e. 

### void register_constants(Expression *e,Variable_struct *c):
store constats into given expression e.

### float expression_eval(Expression *exp,char *string,Variable_struct *variable,Error *error):
Evaluate the string under the environment of Expression. Variable_struct *variable must contain all the variable thats are subjected in the char \*string, failing to do so would result in Variable_error.

#### Example:
```c
#include <stdio.h>
#include "eqn.h"

int main(){
    Error error;
    Expression exp;
    init_expression(&exp);

    Variable_struct var1[]={{"x",2},{NULL,0}};
    float result=expression_eval(&exp,"x+y",var1,&error);
    if(error.status==__failed__)printf("1st exp  error: %s\n",error.msg);
    else printf("1st exp resut:%f\n",result);
    
    Variable_struct var2[]={{"x",2},{"y",5},{NULL,0}};
    result=expression_eval(&exp,"x+y",var2,&error);
    if(error.status==__failed__)printf("2nd exp  error: %s\n",error.msg);
    else printf("2nd exp resut:%f\n",result);



    /* output:
        1st exp  error: Variable Error: variable->"y" is not provided
        2nd exp resut:7.000000

    */
    return 0;
}
```

Pending .......
