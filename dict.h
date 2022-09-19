#ifndef BIG_DICK
#define BIG_DICK value

typedef struct Dict_Node{
	char *key;
	void *value;
	struct Dict_Node *next;
} Dict_Node;

typedef struct 
{
	Dict_Node *stack;
	int size;
	int occupied;
} Dict;

void init_dict(Dict *dict){
	dict->stack=malloc(sizeof(Dict_Node)*7);
	dict->size=7;
	dict->occupied=0;
}
void free_dict(Dict *dict){
	for(int i=0;i<dict->size;i++){
		if(dict->stack[i].key==NULL)continue;
		free(dict->stack[i].key);
		free(dict->stack[i].value);
	}	
	free(dict->stack);
}
int hash(char *key,int size){
	int res=0;
	for(char *ch=key;*ch!='\0';ch++)res+=*ch;
	return res%size;
}

void insert_dict(Dict *dict,char *key,void*value){
	if(dict->occupied==dict->size){
		//size full;
		return;
	}
	int pos=hash(key,dict->size);
	if(dict->stack[pos].key==NULL){
		dict->stack[pos].key=key;
		dict->stack[pos].value=value;

	}else if(strcmp(dict->stack[pos].key,key)==0){
		free(key);
		free(dict->stack[pos].value);
		dict->stack[pos].value=value;
	}else{
		for(int i=0;i<dict->size;i++){
			if(dict->stack[i].key==NULL){
				dict->stack[i].key=key;
				dict->stack[i].value=value;
				Dict_Node *stack;
				for(stack=&dict->stack[pos];stack->next!=NULL;stack=stack->next);
				stack->next=&dict->stack[i];
				break;
			}
		}
	}
	dict->occupied++;

}
void* get_dict(Dict *dict,char *key){
	int pos=hash(key,dict->size);
	if(dict->stack[pos].key==NULL)return NULL;
	Dict_Node *stack=&dict->stack[pos];
	while(stack!=NULL){
		if(strcmp(stack->key,key)==0)return stack->value;
		stack=stack->next;
	}
	return NULL;
}



void print_dict(Dict *dict){
	printf("**Dict**\nsize:%d\noccupied:%d\n********\n",dict->size,dict->occupied);
	for(int i=0;i<dict->size;i++){
		if(dict->stack[i].key==NULL)printf("\t- _______\n");
		else printf("\t- %s\n",dict->stack[i].key);
	}
}

#endif