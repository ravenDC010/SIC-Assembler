#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct symtab{
	char sym_label[10];
	char loc[5];
};
struct optab{
	char op_code[10];
	char obj[3];
};
int checkSymtab(char operand[], struct symtab s[], int n, char objCode[]){
	int i,buf=0,l,x;
	char temp[10],*p;
	l=strlen(operand);
	if(operand[l-1]=='X'){
		buf=1;
		strcpy(temp,operand);
		temp[l-2]='\0';
	}
	if(buf==0){
		for(i=0;i<n;i++){
			if(strcmp(operand,s[i].sym_label)==0){
				strcat(objCode,s[i].loc);
				return(1);
			}
		}	
		return(0);
	}
	if(buf==1){
		for(i=0;i<n;i++){
			if(strcmp(temp,s[i].sym_label)==0){
				x=strtol(s[i].loc,&p,10);
				x=x+8000;
				sprintf(temp,"%d",x);
				strcat(objCode,temp);
				return(1);
			}
		}	
		return(0);
	}
	
}
int checkOptab(char opcode[], struct optab op[], char objCode[]){
	int i;
	for(i=0;i<26;i++){
		if(strcmp(opcode,op[i].op_code)==0){
			strcpy(objCode,op[i].obj);
			return(1);
		}
	}
	return(0);
}
void readOptab(struct optab op[]){
	FILE *f;
	int word=0,line=0,i=0;
	char c;
	f=fopen("optab.txt", "r");
	while(!feof(f)){
		c=fgetc(f);
		if(c!='\t' && c!=' ' && c!='\n'){
			if(word==0){
				op[line].op_code[i++]=c;
			}
			else{
				op[line].obj[i++]=c;
			}
		}
		if(c=='\t'){
			if(word==0){
				op[line].op_code[i]='\0';
			}
			word++;
			i=0;
		}
		if(c=='\n'){
			if(word==1){
				op[line].obj[i]='\0';
			}
			word=0;
			i=0;
			line++;
		}
	}
}
int readSymtab(struct symtab s[]){
	FILE *f;
	int word=0,line=0,i=0;
	char c;
	f=fopen("symtab.txt", "r");
	while(!feof(f)){
		c=fgetc(f);
		if(c!='\t' && c!=' ' && c!='\n'){
			if(word==0){
				s[line].sym_label[i++]=c;
			}
			else{
				s[line].loc[i++]=c;
			}
		}
		if(c=='\t'){
			if(word==0){
				s[line].sym_label[i]='\0';
			}
			word++;
			i=0;
		}
		if(c=='\n'){
			if(word==1){
				s[line].loc[i]='\0';
			}
			word=0;
			i=0;
			line++;
		}
	}
	return(line);
}
int getConstant(char opcode[], char operand[], char objCode[]){
	int l,i,j=0,temp;
	char c[6],*p;
	if(strcmp(opcode,"WORD")==0){
		temp=strtol(operand,&p,10);
		sprintf(c,"%X",temp);
		l=strlen(c);
		for(i=0;i<6-l;i++){
			objCode[i]='0';
		}
		objCode[i]='\0';
		strcat(objCode,c);
		return(3);
	}
	if(strcmp(opcode,"BYTE")==0){
		if(operand[0]=='X'){
			for(i=2;i<strlen(operand)-1;i++){
				objCode[j++]=operand[i];
			}
			objCode[j]='\0';
			return(strlen(objCode)/2);
		}
		if(operand[0]=='C'){
			for(i=2;i<strlen(operand)-1;i++){
				sprintf(c,"%X",operand[i]);
				strcat(objCode,c);
				j=j+2;
			}
			objCode[j]='\0';
			return(strlen(objCode)/2);
		}
	}
}
void format(char s1[], char s2[]){
	int i,l;
	l=strlen(s1);
	for(i=0;i<6-l;i++){
		s2[i]='0';
	}
	s2[i]='\0';
	strcat(s2,s1);
}
int main(){
	char c,operand[10]={'\0'},label[10]={'\0'},opcode[10]={'\0'},address[10]={'\0'},objCode[10]={'\0'},*p;
	char word1[10]={'\0'}, word2[10]={'\0'}, word3[10]={'\0'}, word4[10]={'\0'},start_loc[10]={'\0'},l[10]={'\0'},start_add[10]={'\0'};
	int i=0,j,line=1,w=0,locctr,sym_count=0,comment=0,n,x;
	
	char objLine[71]={'\0'},startAdr[7];
	int objLength=0,brk=0;

	FILE *f1,*f2;
	f1=fopen("object_program.txt","w");
	f2=fopen("intermediate_file.txt", "r");
	struct symtab s[30];
	struct optab op[26];
	readOptab(op);
	n=readSymtab(s);

	fseek(f2,-5,SEEK_END);
	while(!feof(f2)){
		c=fgetc(f2);
		if(c!=-1 && c!='\n'){
			word1[i++]=c;
		}
	}
	word1[i]='\0';
	format(word1,l);
	rewind(f2);
	i=0;
	word1[i]='\0';
	while(!feof(f2)){
		c=fgetc(f2);
			if(c==-1)
				break;
			if(c=='\t'){
				if(w==0){
					break;
				}
				if(w==1){
					word1[i]='\0';
				}
				else if(w==2){
					word2[i]='\0';
				}
				else if(w==3){
					word3[i]='\0';
				}
				else if(w==4){
					word4[i]='\0';
				}
				w++;
				i=0;
			}
			else if(c!='\n' && c!=' ' && c!='\t'){
				if(w==0){
					w=1;
				}
				if(w==1)
					word1[i++]=c;
				else if(w==2)
					word2[i++]=c;
				else if(w==3)
					word3[i++]=c;
				else
					word4[i++]=c;
			}
		if(c=='\n'){
			word4[i]='\0';
			address[0]='\0';
			strcpy(address,word1);
			strcpy(label,word2);
			strcpy(opcode,word3);
			strcpy(operand,word4);
			if(strcmp(opcode,"START")==0){
				format(word1,address);
				printf("H^%-6s^%6s^%6s\n",label,address,l);
				fprintf(f1,"H^%-6s^%6s^%6s\n",label,address,l);
				strcpy(start_loc,address);
				format(address,start_add);
			}
			else{
				if(checkOptab(opcode,op,objCode)==1){
					if(strcmp(operand,"\0")!=0){//if label is not empty
						if(checkSymtab(operand,s,n,objCode)==0){
							printf("ERROR. %s not found in SYMTAB.\n",operand);
							return(0);
						}
						else{
							if(objLength+3>30 || brk==1){
								printf("T^%s^%X",start_add,objLength);
								printf("%s\n",objLine);
								fprintf(f1,"T^%s^%X",start_add,objLength);
								fprintf(f1,"%s\n",objLine);
								format(address,start_add);
								brk=0;
								objLength=0;
								objLine[0]='\0';
							}
							strcat(objLine,"^");
							strcat(objLine,objCode);
							objLength=objLength+3;
						}
					}
					else{
						if(objLength+3>30 || brk==1){
							printf("T^%s^%X",start_add,objLength);
							printf("%s\n",objLine);
							fprintf(f1,"T^%s^%X",start_add,objLength);
							fprintf(f1,"%s\n",objLine);
							format(address,start_add);
							brk=0;
							objLength=0;
							objLine[0]='\0';
						}
						strcat(objCode,"0000");
						strcat(objLine,"^");
						strcat(objLine,objCode);
						objLength=objLength+3;
					}
				}
				else if(strcmp(opcode,"BYTE")==0 || strcmp(opcode,"WORD")==0){
					x=getConstant(opcode,operand,objCode);
					if(objLength+x>30 || brk==1){
						printf("T^%s^%X",start_add,objLength);
						printf("%s\n",objLine);
						fprintf(f1,"T^%s^%X",start_add,objLength);
						fprintf(f1,"%s\n",objLine);
						format(address,start_add);
						brk=0;
						objLength=0;
						objLine[0]='\0';
					}
					strcat(objLine,"^");
					strcat(objLine,objCode);
					objLength=objLength+x;
				}
				else if(strcmp(opcode,"RESW")==0 || strcmp(opcode,"RESB")==0){
					brk=1;
				}	
			}
			line++;
			w=0;
			i=0;
			objCode[0]='\0';
			label[0]='\0';
			opcode[0]='\0';
			operand[0]='\0';
		}
	}
	printf("T^%s^%X",start_add,objLength);
	printf("%s\n",objLine);
	printf("E^%s\n",start_loc);
	fprintf(f1,"T^%s^%X",start_add,objLength);
	fprintf(f1,"%s\n",objLine);
	fprintf(f1,"E^%s\n",start_loc);
	fclose(f1);
	fclose(f2);
	return(0);
}
