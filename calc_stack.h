/*
***** Storing data in Node ******* \n

	__number__ ==> to float type; used to store numbers;
	__char__ ==> to char* type; used to store opcode;
	__function__ ==> is of char* type, first byte i.e [0] stores the number of paramaters which is < thean sizeof(char) & rest of the byte is a string i.e strting from [1]
					... data[0]=>number of parameters, data[1]..data[n]=>string(name of functon);
	__var__==>is of char*, stores string;(name of variable)

**** How does the stack work ***** \n

*/
#ifndef _CALC_STACK_
#define _CALC_STACK_ value

#define UNARY_SUB '~'
#define UNARY_ADD '#'
#define is_unary_opcode_stack(ch) ((ch)==UNARY_ADD || (ch)==UNARY_SUB)

/* !!!chaeged to type enum!!!
#define __sucess__ 0
#define __failed__ 1
*/

typedef enum{
	__sucess__=0,
	__failed__,
	__pending__
} Status;

typedef struct{
	int status;
	char msg[100];
} Error;

void set_error(Error *e,int status,char *msg){
	e->status=status;
	strcpy(e->msg,msg);
}

typedef enum{
	__number__,
	__char__,
	__function__,
	__var__
} Type;

typedef struct 
{
	Type type;
	void* data;			
}Node;

typedef struct{
	int size;
	int start;
	Node** data;
}Stack;

void init_stack(Stack *stack,int size){
	stack->size=size;
	stack->start=0;
	stack->data=malloc(size*sizeof(Node*));
};
void free_stack(Stack *stack){
	for(int i=0;i<stack->size;i++){
		free(stack->data[i]->data);
		free(stack->data[i]);
	}
	free(stack->data);

};
void push_stack(Stack *stack,Node *data){
	if(stack->start==stack->size)return;
	stack->data[stack->start]=data;
	stack->start++;
};
Node* pop_stack(Stack *stack){
	if(stack->start==0)return NULL;
	stack->start--;
	Node*res=stack->data[stack->start];
	stack->data[stack->start]=NULL;
	return res;

}
Node* lookup_stack(Stack *stack,int pos){
	if(pos>stack->size || stack->start==0)return NULL;
	if(pos<0)return stack->data[stack->start-1];
	return stack->data[pos];
}
Node* create_node(Type  type,void*data,int size){
	Node* res=malloc(sizeof(Node));
	res->type=type;
	res->data=malloc(size);
	memcpy(res->data,data,size);
	return res;
}


void print_node(Node *node){
	switch(node->type){
		case __number__:
			printf("[*]Printing number: %f\n",*(float*)node->data);
			break;
		case __var__:
			printf("[*]Printinng string %s\n",(char*)node->data);
			break;
		case __function__:
			printf("[*]Printinng function parms: %d,",((char*)node->data)[0]);
			printf("name:%s\n",((char*)node->data)+1);
			break;
		case __char__:
			printf("[*]Printinng opcode: %c\n",*(char*)node->data);
	}	
}
void print_stack(Stack*stack){
	printf("[!]Length:%d\n",stack->start);
	for(int i=0;i<stack->start;i++){
		//printf("[!]%d...%d\n",i,stack->data[i]->type);
		print_node(stack->data[i]);
	}
};

char get_precedence(char c){

	if(c==')')
		return 9;
	if(c==UNARY_SUB || c==UNARY_ADD)
		return 4;
	if(c=='*' || c=='/')
		return 3;
	else if(c=='+' || c=='-')
		return 2;
	else
		return 1;
}

Node *calc_functon(Node *function,Stack *data_stack,int parms,Error *error){
	Function_struct *func=function->data;
	//printf("[*]From function->%s\t-parms:[%d,%d]\n",func->name,func->parms,parms);
	if(func->parms!=parms){
		//set_error(error,__failed__,"Error in function call: argeumets Length donut match");
		error->status=__failed__;
		sprintf(error->msg,"Error in function call  \"%s\":arguments required->%d, provide->%d",func->name,func->parms,parms);
		return NULL;
	}
	Node *temp;
	float *res=malloc(sizeof(float)*parms);
	for(int i=0;i<parms;i++){
		temp=pop_stack(data_stack);
		if(temp==NULL){
			set_error(error,__failed__,"Error in function call (Null detected): Please provide valid formated input");
			return NULL;
		}
		if(temp->type!=__number__){
			set_error(error,__failed__,"Error in function call (NAN detected): Please provide number as parms");
			return NULL;
		}
		//printf("\t-arg[%d]:%f\n",i,*(float*)temp->data);
		res[i]=*(float*)temp->data;
		free(temp->data);
		free(temp);
	}
	res[0]=(*func->func)(res,parms);
	temp=create_node(__number__,res,sizeof(float));
	free(res);
	return temp;

}

Node *calc(Node *n1,char op,Node *n2,Error *error){
	/*if(n1==NULL){
		if(op=='-')*(float*)n2->data=-*(float*)n2->data; //yo hataunu parxa!!
		return n2;
	}*/
	if(n2==NULL){
		set_error(error,__failed__,"Error in Calculation (Null dected): Please provide promated input");
		return NULL;
	}
	if(op==UNARY_SUB){
		*(float*)n2->data=-*(float*)n2->data;
		return n2;
	}else if(op==UNARY_ADD)return n2;

	if(n1==NULL){
		//printf("----------%d\n",op);
		set_error(error,__failed__,"Error in Calculation (Null dected): Please provide promated input");
		return NULL;
	}


	
	float temp_n1=*(float*)n1->data;
	float temp_n2=*(float*)n2->data;
	switch(op){
		case '+':
			temp_n1+=temp_n2;
			break;
		case '-':
			temp_n1-=temp_n2;
			break;
		case '*':
			temp_n1*=temp_n2;
			break;
		case '/':
			//n2==0 ko case handel garna xa;
			temp_n1/=temp_n2;
			break;
	}
	//printf("[*]From calc:%f\n",temp_n1);

	*(float*)n1->data=temp_n1;
	free(n2);
	return n1;
}
/*
Dtaa stack null handel garn abaki xa;
*/
void push_opcode(Stack *opcode_stack,Node *node,Stack*data_stack,Error *error){
	if(node->type==__function__){
		push_stack(opcode_stack,node);
		return;
	}
	char op1=*(char*)node->data;
	Node *temp=lookup_stack(opcode_stack,-1);
	if(temp==NULL || op1=='(' || op1==',' || temp->type==__function__){
		push_stack(opcode_stack,node);
		return;
	}
	char op2=*(char*)temp->data;

	if(get_precedence(op2)>=get_precedence(op1)){
		while(get_precedence(op2)>=get_precedence(op1)){
			temp=pop_stack(opcode_stack);//opcode node lai free garna baki xa:
			free(temp->data);
			free(temp);//mYBE FREE VAYO HOLA;MAY BE PORN TO ERROR;
			if(is_unary_opcode_stack(op2))temp=calc(NULL,op2,pop_stack(data_stack),error);
			else temp=calc(pop_stack(data_stack),op2,pop_stack(data_stack),error);
			if(error->status==__failed__)return;
			push_stack(data_stack,temp);
			temp=lookup_stack(opcode_stack,-1);
			if(temp==NULL || temp->type==__function__ )break;
			if(temp->type==__char__)op2=*(char*)temp->data;

		}
		if(is_end(op1))free(node);
		else push_stack(opcode_stack,node);
		return;
	}

	if(op1==')' || is_end(op1)){
		int parms=1;
		while(temp!=NULL){
			temp=pop_stack(opcode_stack);
			if(temp->type==__char__ && *(char*)temp->data=='('){
				free(temp->data);
				free(temp);
				temp=lookup_stack(opcode_stack,-1);
				if(temp!=NULL && temp->type==__function__){
					void *temp1=calc_functon(pop_stack(opcode_stack),data_stack,parms,error);
					free(temp);
					if(error->status==__failed__)return;
					push_stack(data_stack,temp1);
					//paxi..
				};
				return;
			}
			if(temp->type==__function__){
				//err;
			}
			op2=*(char*)temp->data;
			free(temp->data);
			free(temp);//MAYBE FREE VAYO HOLA ; PORN TO ERROR
			if(op2==','){
				parms++;
				temp=lookup_stack(opcode_stack,-1);
				continue;
			}
			if(is_unary_opcode_stack(op2))temp=calc(NULL,op2,pop_stack(data_stack),error);
			else temp=calc(pop_stack(data_stack),op2,pop_stack(data_stack),error);
			if(error->status==__failed__)return;
			push_stack(data_stack,temp);
			temp=lookup_stack(opcode_stack,-1);
		}
		return;
	}
	if(is_end(op1))free(node);
	else push_stack(opcode_stack,node);

};

void push_data(Stack *stack,Node *node){
	push_stack(stack,node);
};

#endif