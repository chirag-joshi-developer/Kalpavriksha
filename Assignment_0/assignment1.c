#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<string.h>
#include<ctype.h>

char opstack[100000];
int opctr= 0;

int valstack[100000];
int valctr = 0;


void pushop(char op){
	opstack[opctr] = op;
	opctr++;
}
void popop(){
	opctr--;
}
char topop(){
	return opstack[opctr-1];
}
bool isempty(){
	return opctr == 0;
}

void pushval(int val){
	valstack[valctr] = val;
	valctr++;
}
void popval(){
	valctr--;
}
int topval(){
	return valstack[valctr-1];
}

int precedence(char op){
	if(op=='-'||op=='+'){
		return 1;
	}
	if(op=='/'||op=='*'){
		return 2;
	}
	return 0;
}

int calculate(int a,int b,char op,int *error){
	if(op == '+'){
		return a+b;
	}
	if(op== '-'){
		return a-b;
	}
	if(op == '*'){
		return a*b;
	}
	if(op == '/'){
		if(b==0){
			*error = 1;
			return 0;
		}
		return a/b;
	}
	return 0;
}
int eval(char* str,int *error){
	int i=0;
	while(str[i]!= '\0'){
		if(isspace(str[i])){
			i++;
			continue;
		}
		else if(isdigit(str[i])){
			int val =0;
			while(i<strlen(str) && isdigit(str[i])){
				val = val*10 + (str[i]-'0');
				i++;
			}
			pushval(val);
		}
		else if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/'){
			while(!isempty() && precedence(topop())>=precedence(str[i])){
				int b = topval();
				popval();
				int a = topval();
				popval();
				char op =  topop();
				popop();
				int res = calculate(a,b,op,error);
				if(*error) return 0;
				pushval(res);
			}
			pushop(str[i]);
			i++;
		}
		else{
			*error = 2;
			return 0;
		}
	}
	while(!isempty()){
			int b = topval();
			popval();
			int a = topval();
			popval();
			char op =  topop();
			popop();
			int res = calculate(a,b,op,error);
			if(*error) return 0;
			pushval(res);
		}
	return topval();
}

int main(){
	char str[100000];
	scanf("%[^\n]s",&str);
	int error = 0;
	int res = eval(str,&error);
	if (error == 1) {
        printf("Error: Division by zero.\n");
    } else if (error == 2) {
        printf("Error: Invalid expression.\n");
    } else {
        printf("%d\n", res);
    }
    return 0;
}