#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<string.h>

# define FILE_NAME "users.txt"

struct User{
	int id;
	char name[100];
	int age;
};

void create(){ 
	FILE *file = fopen(FILE_NAME,"r");
	int id =0;
	printf("Enter user id:\n");
	scanf("%d",&id);
	User u; 
	while(fscanf(file,"%d %s %d",&u.id,&u.name,&u.age)==3){
		if(u.id == id){
			printf("\nUser with this id alreaddy exist\n");
			return;
		}
	}
	fclose(file);
	file = fopen(FILE_NAME, "a");
	u.id = id; 
	printf("\nEnter user name:\n"); 
	scanf("%s",&u.name); 
	printf("\nEnter user age:\n"); 
	scanf("%d",&u.age); 
	fprintf(file, "%d %s %d \n",u.id,u.name,u.age); 
	fclose(file); 
	printf("A new user added"); 
}
void read(){
	FILE * file = fopen(FILE_NAME,"r");
	if(!file){
		printf("\n This file doesn't exist yet\n");
		return;
	}
	User u;
	printf("\n Users data \n");
	while(fscanf(file,"%d %s %d",&u.id,&u.name,&u.age)==3){
		printf("ID: %d, Name: %s, Age: %d\n", u.id, u.name, u.age);
	}
	fclose(file);
}
void update(){
	FILE *file = fopen(FILE_NAME,"r");
	FILE *temp = fopen("temp.txt","w");
	if(!file || !temp){
		printf("Error opening file\n");
		return;
	}
	int id=0,found=0;
	printf("Enter id to update\n");
	scanf("%d",&id);
	User u;
	while(fscanf(file,"%d %s %d",&u.id,&u.name,&u.age)==3){
		if(u.id == id){
			found = 1;
			printf("Enter new name: \n");
			scanf("%s",&u.name);
			printf("\nEnter new age: \n");
			scanf("%d",&u.age);
		}
		fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
	}
	fclose(file);
	fclose(temp);

	remove(FILE_NAME);
	rename("temp.txt",FILE_NAME);
	if(found){
		printf("User updated successfully \n");
	}
	else{
		printf("User with id %d not found \n",id);
	}
}
void Delete(){
	FILE *file = fopen(FILE_NAME,"r");
	FILE *temp = fopen("temp.txt","w");
	if(!file || !temp){
		printf("Error opening file\n");
		return;
	}
	int id = 0,found=0;
	printf("Enter id to delete\n");
	scanf("%d",&id);
	User u;
	while(fscanf(file,"%d %s %d",&u.id,&u.name,&u.age)==3){
		if(id == u.id){
			found = 1;
		}
		else{
			fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
		}
	}
	fclose(file);
	fclose(temp);

	remove(FILE_NAME);
	rename("temp.txt",FILE_NAME);
	if(found){
		printf("User deleted successfully\n");
	}
	else{
		printf("User not found \n");
	}
}
int main(){
	int task;
	while(true){
		printf("\nSelect one of these CURD operations \n");
		printf("1. Create new user\n");
		printf("2. Read all users\n");
		printf("3. Update existing user\n");
		printf("4. Delete existing user \n");
		printf("0. To exit the program\n");
		scanf("%d",&task);
		if(task==0){
			break;
		}
		else if(task==1){
			create();
		}
		else if(task==2){
			read();
		}
		else if(task==3){
			update();
		}
		else if(task==4){
			Delete();
		}
		else{
			printf("Select a valid operation\n");
		}
	}
return 0;
}