#ifndef _EQN_SOLVER_
#define _EQN_SOLVER_ value


#define is_unary_opcode(ch)((ch)=='+' || (ch)=='-'||(ch)=='!')
#define is_binary_opcode(ch) ((ch)=='*' || (ch)=='/')
#define is_bracket(ch)((ch)=='(' || (ch)==')')
#define is_opcode(ch)(is_unary_opcode((ch))||is_binary_opcode((ch))||is_bracket((ch))||(ch)==',')
#define is_blank(ch)((ch)==' ' || (ch)=='\t')
#define is_end(ch)((ch)=='\n' || (ch)=='\0')
#define is_char(ch) (((ch)>='a' && (ch)<='z')||((ch)>='A' && (ch)<='Z'))
//#define is_num(ch) (((ch)<='a' || (ch)>='z'))

#define true 1
#define false 0


typedef struct{
	char *name;
	float (*func)(float*,int);
	int parms;
} Function_struct;

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "calc_stack.h"
#include "dict.h"

void print_string(char *c,int len){
	printf("--string:");
	for(int i=0;i<len;i++)printf("%c",c[i]);
	printf("\n");
};


typedef struct{
	Stack opcode_stack;
	Stack data_stack;
	Dict function_dict;
	Dict varible_dict;
	Dict constant_dict;
	char *raw_equation;
	char new_scope;
} Expression;

/* !!!Decleared at top!!!
typedef struct{
	char *name;
	float (*func)(float*,int);
	int parms;
} Function_struct;

*/


typedef struct{
	char *name;
	float value;
} Variable_struct;

void init_expression(Expression *e){
	init_stack(&e->opcode_stack,10);
	init_stack(&e->data_stack,10);
	init_dict(&e->function_dict);
	init_dict(&e->varible_dict);
	init_dict(&e->constant_dict);
	e->raw_equation=NULL;
}
//void free()

void register_functions(Expression *e,Function_struct *function){
	for(Function_struct* f=function;f->name!=NULL;f++)insert_dict(&e->function_dict,f->name,f);

}
void register_constants(Expression *e,Variable_struct *c){
	for(;c->name!=NULL;c++)insert_dict(&e->constant_dict,c->name,&c->value);
}
void register_expression(Expression *e,char *string){
	e->raw_equation=string;
	e->new_scope=true;
}
float parse_string(Expression *e,char*str,Variable_struct *variable,Error *error);

float flush_variables(Expression *exp){
	/*delete key,value and the stack*/
	free_dict(&exp->varible_dict);
	init_dict(&exp->varible_dict);

}

float proceed_next_calculation(Expression *e,Variable_struct *variable,Error *error){
	float res;
	if(e->raw_equation==NULL){
		//error->status=__failed__;
		//sprintf(&error->msg,"Error in Expression string (Null Detected): possibel fix -> call register_expression();");
		set_error(error,__failed__,"Error in Expression string (Null Detected): possibel fix -> call register_expression();");
		return 0;
	};
	if(variable->name!=NULL){
		float *f=malloc(sizeof(float));
		*f=variable->value;
		insert_dict(&e->varible_dict,variable->name,f);
		variable->name=NULL;
	};
	res=parse_string(e,e->raw_equation,variable,error);
	if(error->status==__failed__ || error->status==__sucess__)e->raw_equation=NULL;
	/*
	 variable_dict free garnu parxa 
	 !!Note free key value pair plus the stack (:call flush_variable(exp):) !!!
	 we are letting user input value as process caries on 
	 flush variable inside this func if error occired
	 else let give it in user hand;
	 !!Note intial variable->name must be Null!!
	*/ 
	return res;
}

float calculate_expression(Expression *e,Variable_struct *variable,Error *error){
	float res=0;
	res=parse_string(e,e->raw_equation,variable,error);
	////printf("1searching for->%d\n",variable->value);
	return res;
}


float expression_eval(Expression *e,char *string,Variable_struct *variable,Error *error){
	float res=0;
	register_expression(e,string);
	Variable_struct request={NULL,1};
	while(true){
		res=calculate_expression(e,&request,error);
		if(error->status==__failed__ || error->status==__sucess__){
			/*flush variable dict */;
			e->raw_equation=NULL;
			free(e->varible_dict.stack);	
			init_dict(&e->varible_dict);
			return res;
		}
		if(error->status==__pending__ && request.name!=NULL){
			for(Variable_struct *v=variable;;v++){
				if(v->name==NULL){
					error->status=__failed__;
					sprintf(error->msg,"Variable Error: variable->\"%s\" is not provided",request.name);
					free(request.name);
					goto return_res;
					return 0;
				};
				if(strcmp(request.name,v->name)==0){
					insert_dict(&e->varible_dict,v->name,&v->value);
					free(request.name);
					break;
				}
			}
		}
	};
	/*
	 varible_dict free garnu parxa 
	 !!Note dont free key value just the stack !!!
	 we are assuming static variable struct;
	*/ 
	return_res:
	e->raw_equation=NULL;
	free(e->varible_dict.stack);
	init_dict(&e->varible_dict);
	return res;

}



float parse_string(Expression *exp,char *string,Variable_struct *variable,Error *error){
	Stack *opcode_stack=&exp->opcode_stack;
	Stack *data_stack=&exp->data_stack;
	Dict *function_dict=&exp->function_dict;

	//Error error={__sucess__};
	//char new_scope=true;

	error->status=__sucess__;
	char *ch=string;
	float Number=0;
	__ignore__:
	for(;;ch++){
		if(is_opcode(*ch))goto __opcode__;
		if(*ch==' '||*ch=='\t')continue;
		if(*ch=='\n' || *ch=='\0'){
			//print_stack(data_stack);
			//print_stack(opcode_stack);
			push_opcode(opcode_stack,create_node(__char__,ch,1),data_stack,error);
			/* !!Check length of clac stack and op stack baki xa!!*/
			if(error->status==__failed__)goto __error__;/*if ERROR*/
			Node *temp=pop_stack(data_stack);/*compiled sucessfully*/
			//print_node(temp);
			return *(float*)temp->data;
		}
		break;
	}
	__int__:
	Number=0;
	if(*ch=='.'){
		//err //printf("[!]wtf . ?\n");
		set_error(error,__failed__,"Error while Prsing: need a decimal before\'.\'? ");
		return 0;
	}
	if(*ch<'0' || *ch>'9')goto __variable__;
	for(;;ch++){
		if(is_end(*ch)||is_blank(*ch)){
			//printf("-Numberb: %f\n",Number);
			push_stack(data_stack,create_node(__number__,&Number,sizeof(float)));
			exp->new_scope=false;
			goto __ignore__;
		}
		if(is_opcode(*ch)){
			//printf("-Number: %f\n",Number);
			push_stack(data_stack,create_node(__number__,&Number,sizeof(float)));
			exp->new_scope=false;
			goto __opcode__;
		}
		if(*ch=='.'){ch++;goto __float__;};
		if(*ch<'0' || *ch>'9'){
			//set_err error //printf("[!]Err string inside int\n");
			error->status=__failed__;
			sprintf(error->msg,"Error while Parsing: unknow token \'%c\' used in defining a number",*ch);
			return 0;
		}
		Number=Number*10+(*ch-'0');

	}
	__float__:
	int temp=1;
	for(;;ch++){
		if(is_end(*ch)||is_blank(*ch)){
			//printf("-Number: %f\n",Number);
			push_stack(data_stack,create_node(__number__,&Number,sizeof(float)));
			exp->new_scope=false;
			goto __ignore__;
		}
		if(is_opcode(*ch)){
			//printf("-Number: %f\n",Number);
			push_stack(data_stack,create_node(__number__,&Number,sizeof(float)));
			exp->new_scope=false;
			goto __opcode__;
		};
		if(*ch=='.'){
			//err //printf("[!]Err two . found\n");
			set_error(error,__failed__,"Parsing Error:  unknow token \'..\'");
			return 0;
		}
		if(*ch<'0' || *ch>'9'){
			//set_err error //printf("[!]Err string inside int\n");
			error->status=__failed__;
			sprintf(error->msg,"Error while Parsing: unknow token \'%c\' used in defining a number",*ch);
			return 0;
		}
		Number+=(*ch-'0')/pow(10,temp);
		temp++;

	}

	__variable__:
	char *start=ch;
	int size=0;
	for(;;ch=ch++){
		if(*ch=='('){
			/*function detected*/
			//printf("**IS A FUNCTION");
			//print_string(start,size);
			char *key=malloc(size+1);
			memcpy(key,start,size);
			key[size]='\0';
			Function_struct *function=get_dict(function_dict,key);
			if(function==NULL){
				//set_error(&error,__failed__,"Name Error: No function Name called \"");
				error->status=__failed__;
				sprintf(error->msg,"Name Error: No function name called \"%s\"",key);
				free(key);
				goto __error__;

			}
			else{
				//float a[]={9,9};
				////printf("[*]fOUND %s:%f\n",function->name,(*function->func)(a,1));
				Node *temp=malloc(sizeof(Node));
				temp->type=__function__;
				temp->data=function;
				push_opcode(opcode_stack,temp,data_stack,error);
				goto __opcode__;
			}
			//push_opcode(create_node(__function__,))
			goto __opcode__;
		}
		if(!is_char(*ch)){
			/*variable detected*/
			char *key=malloc(size+1);
			memcpy(key,start,size);
			key[size]='\0';
			float *val=get_dict(&exp->constant_dict,key);
			if(val==NULL)val=get_dict(&exp->varible_dict,key);
			if(val==NULL){
				/*take snapshot to ask for input*/
				exp->raw_equation=start;
				error->status=__pending__;
				variable->name=key;
				variable->value=99;
				return 0;
			};
			free(key);
			push_stack(data_stack,create_node(__number__,val,sizeof(float)));
			exp->new_scope=false;
			//print_string(start,size);
			goto __opcode__;
		};
		ch++;
		size++;
	}

	__opcode__:
	if(is_blank(*ch)||is_end(*ch))goto __ignore__;
	if(is_opcode(*ch)){
		if(*ch=='('||is_binary_opcode(*ch)||*ch==',')exp->new_scope=true;
		//printf("- opcode:%c\n",*ch);
		char op=*ch;
		if(exp->new_scope){
			if(*ch=='+'){op=UNARY_ADD;exp->new_scope=false;}
			else if(*ch=='-'){op=UNARY_SUB;exp->new_scope=false;}
		}
		push_opcode(opcode_stack,create_node(__char__,&op,sizeof(char)),data_stack,error);
		if(error->status==__failed__)goto __error__;
	}
	else{
		error->status=__failed__;
		sprintf(error->msg,"Error while Parsing: unknow token found \'%c\'",*ch);
		goto __error__;
		////printf("[!] Unknow token %c\n",*ch);
		return 0;
	}
	ch++;
	goto __ignore__;

	__error__:
		//printf("Status:%d\n%s\n",error->status,error->msg);
		return 0;
}

#endif