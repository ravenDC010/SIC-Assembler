#include<stdio.h>
#include<string.h>
#include<stdlib.h>
struct symtab{
	char sym_label[10];
	char locctr[4];
};
struct optab{
	char op_code[10];
	char obj[3];
};
void format(char loc[],int locctr){
	int i;
	char temp[5];
	sprintf(temp,"%X",locctr);
	for(i=0;i<4-strlen(temp);i++){
		loc[i]='0';
	}
	loc[i]='\0';	
	strcat(loc,temp);
}

int symbol(FILE *f3, struct symtab s[], char label[], int lc, int *sym_count){
	char loc[5]={'\0'};
	int i;
	for(i=0;i<(*sym_count);i++){
		if(strcmp(label,s[i].sym_label)==0){//search label in symtab
			return(0);
		}
		else{//if not found, insert into symtab
			strcpy(s[*sym_count].sym_label,label);
			format(loc,lc);
			strcpy(s[*sym_count].locctr,loc);
			fprintf(f3,"%s\t%s\n",s[*sym_count].sym_label,s[*sym_count].locctr);//insert label and locctr in symtab
			*sym_count=(*sym_count)+1;
			break;
		}
	}
	if(*sym_count==0){
		strcpy(s[*sym_count].sym_label,label);
		format(loc,lc);
		strcpy(s[*sym_count].locctr,loc);
		fprintf(f3,"%s\t%s\n",s[*sym_count].sym_label,s[*sym_count].locctr);
		*sym_count=(*sym_count)+1;
	}
	return(1);
}
int checkOptab(char opcode[], char operand[], int *lc, struct optab op[]){
	int num,i,z=0,valid=1;
	char x='X';
	for(i=0;i<26;i++){
		if(strcmp(opcode,op[i].op_code)==0){
			valid=0;
			break;
		}
	}
	if(valid==0){
		*lc=(*lc)+3;
		return(valid);
	}
	else{
		if(strcmp(opcode,"WORD")==0){
			*lc=(*lc)+3;
		}
		else if(strcmp(opcode,"RESW")==0){
			num=atoi(operand);
			*lc=(*lc)+(num*3);
		}
		else if(strcmp(opcode,"RESB")==0){
			num=atoi(operand);
			*lc=(*lc)+num;
		}
		else if(strcmp(opcode,"BYTE")==0){
			num=0;
			for(i=0;i<strlen(operand)-1;i++){
				if(z==1){
					num++;
				}
				if(operand[i]=='\''){
					z=1;
				}
			}
			if(operand[0]=='X'){
				num=num/2;
			}
			*lc=(*lc)+num;
		}
		else{
			printf("ERROR. OPCODE \"%s\" not valid\n",opcode);
			return(valid);
		}
		return(0);
	}
}
void readOptab(struct optab op[]){
	FILE *f4;
	char c;
	f4=fopen("optab.txt", "r");
	int word=0,line=0,i=0;
	while(!feof(f4)){
		c=fgetc(f4);
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
int main(int argc, char* argv[]){
	char c,operand[10]={'\0'},label[10]={'\0'},opcode[10]={'\0'},zero[4]="0000",*p;
	char word1[10]={'\0'}, word2[10]={'\0'}, word3[10]={'\0'},loc[5]={'\0'};
	int i=0,j,line=1,w=0,locctr,sym_count=0,z2=1,z3=1,comment=0,tab=0,start_loc,length;
	FILE *f,*f2,*f3;
	if(argc!=2){
		printf("Enter the source code txt file as command line argument\n");
		return(-1);
	}
	f=fopen(argv[1], "r");
	f2=fopen("intermediate_file.txt", "w");	
	f3=fopen("symtab.txt", "w");
	struct symtab s[30];
	struct optab op[26];
	readOptab(op);
	while(!feof(f) && (strcmp(opcode,"END")!=0)){
		
		c=fgetc(f);
		if(c=='.'){
			comment=1;
			z2=0;
		}
		if(comment==0){
			if(c==-1)
				break;
			if(c=='\t' && tab==0){
				tab=1;
				if(w>0 && w<4){
					if(w==1){
						word1[i]='\0';
					}
					else if(w==2){
						word2[i]='\0';
					}
					else if(w==3){
						word3[i]='\0';
					}
					w++;
					i=0;
				}
			}
			else if(c!='\n' && c!=' ' && c!='\t'){
				tab=0;
				if(w==0){
					w=1;
				}
				if(w==1)
					word1[i++]=c;
				else if(w==2)
					word2[i++]=c;
				else
					word3[i++]=c;
				z2=0;
			}
		}
		if(c=='\n' && z2==0){
			if(comment==1){
				comment=0;
				z2=1;
			}
			else{
				if(w==1){
					word1[i]='\0';
					strcpy(opcode,word1);
				}
				else if(w==2){
					word2[i]='\0';
					strcpy(opcode,word1);
					strcpy(operand,word2);

				}
				else if(w==3){
					word3[i]='\0';
					strcpy(label,word1);
					strcpy(opcode,word2);
					strcpy(operand,word3);

				}
				else if(w>3){
					printf("ERROR. There are %d words in line %d\n",w,line);
					return(0);
				}

				if(line==1){
					if(strcmp(opcode,"START")==0){
						locctr=strtol(operand,&p,16);
					}
					else{
						locctr=strtol(zero,&p,16);
					}
					format(loc,locctr);
					start_loc=locctr;
					fprintf(f2,"%s\t%s\t%s\t%s\n",loc,label,opcode,operand);
					printf("%s\t%s\t%s\t%s\n",loc,label,opcode,operand);
				}
				else{
					if(strcmp(label,"\0")!=0){//if label is not empty
						if(symbol(f3,s,label,locctr,&sym_count)==0){
							printf("Duplicate symbol %s found\n",label);
							return(0);
						}
					}
					if(strcmp(word1,"END")!=0){
						format(loc,locctr);
						fprintf(f2,"%s\t%s\t%s\t%s\n",loc,label,opcode,operand);
						printf("%s\t%s\t%s\t%s\n",loc,label,opcode,operand);
						if(checkOptab(opcode,operand,&locctr,op)==1){
							return(0);
						}
					}
					else{
						fprintf(f2,"\t%s\t%s\t%s\n",label,opcode,operand);
						printf("\t%s\t%s\t%s\n",label,opcode,operand);
					}
				}
				line++;
				w=0;
				i=0;
				z2=1;
				label[0]='\0';
				opcode[0]='\0';
				operand[0]='\0';
			}
		}

	}
	length=locctr-start_loc;
	format(loc,length);
	printf("%s\n",loc);
	fprintf(f2,"%s\n",loc);
	fclose(f3);
	fclose(f2);
	fclose(f);
	return(0);
}
