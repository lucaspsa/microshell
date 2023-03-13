/*
 UFSM
 Sistemas Operacionais 2018/2 
 Aluno:  Lucas Pittella Siqueira
 lucas.siqueira@ecomp.ufsm.br
 Disciplina: Sistemas Operacionais 2018/2 
 
 Como o codigo funciona?

 Em loop, shell analisa o input, separa cada elemento e salva em um array de ponteiros.

 Logo o shell analisa o argumento salvo na primeira posicao e procede para a funcao de acordo.

 Apos a tarefa, shell volta para o loop inicial. 
 
 Caso a funcao precise executar um novo programa, ele passa para a funcao onde se criara um novo processo filho
 para ser substituido atraves da system call execvp.
	
 execvp, variacao de exec, "v" espera um array de ponteiros, "p" deixa o sistema operacional
 procurar o path do arquivo se nao houver "/", logo realiza a troca de contexto enquanto o processo pai aguarda.
 Se execvp nao encontrar o arquivo, por default executa /bin/sh.		
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define BUFFER_SIZE 128
#define DELIM " \t\n"
char **split_line(char *line){
	int buffersize = BUFFER_SIZE;
	int i = 0;
	char *token;
	char **token_ptrs = malloc(buffersize * sizeof(char*));
	if(!token_ptrs){
		fprintf(stderr, "Error func split_line malloc");
		exit(EXIT_FAILURE);
	}
	token = strtok(line, DELIM);
	while (token != NULL){
		token_ptrs[i] = token;
		i++;
		if (i >= buffersize) {
			buffersize += BUFFER_SIZE;
			token_ptrs = realloc(token_ptrs, buffersize * sizeof(char*));
			if (!token_ptrs){
				fprintf(stderr, "Error func split_line malloc");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, DELIM); //strtok same string, str must be NULL
	}
	token_ptrs[i] = NULL;
	return token_ptrs;
}

char *read_line(void){
	char *line = NULL;
	ssize_t buffersize = 0;	
	getline(&line,&buffersize,stdin);
	return line;
}

int msh_launcher(char **args){
	pid_t pid, wpid;
	int status;
	pid = fork();
	switch(pid){
		case -1:
			printf("Erro!\n");
			exit(EXIT_FAILURE);
		case 0:
			execvp(args[1],args+1);
			exit(status);
			break;
		default:
			waitpid(-1,&status,0);
			break;
	}
	return 1;	
}

int msh_lista(char **args){
	args[2]=args[1];
	args[1] = "/bin/ls"; 
	return msh_launcher(args);	
}

int msh_help(char **args){
	printf("Command List: lista arg, mostra arg, espera int, sair, imprime args, tempo,executa args.\n");
	return 1;
}

int msh_mostra(char **args){
	args[2]=args[1];
	args[1] = "/bin/cat";	
	return msh_launcher(args);
}

int msh_espera(char **args){
	sleep(atoi(args[1]));
	return 1;
}

int msh_sair(char **args){
	exit(EXIT_SUCCESS);
	return 0;
}

int msh_imprime(char **args){
	int i=1;
	while(args[i]!=NULL){
		printf("%s ",args[i]);i++;	
	}
	printf("\n");
	return 1;	
}

int msh_tempo(char **args){
	args[1]="/bin/date";
	args[2]=NULL;
	return msh_launcher(args);
}

int msh_executa(char **args){
	return msh_launcher(args);
}

char *comandos_str[] = {"lista","help","mostra","espera","sair","imprime","tempo","executa"};

int (*comandos_func[])(char **)={&msh_lista,&msh_help,&msh_mostra,&msh_espera,&msh_sair,&msh_imprime,&msh_tempo,&msh_executa};

int numero_de_comandos(){
	return sizeof(comandos_str)/sizeof(char *);
}

int args_parser(char **args){
	int i;	

	if(args[0] == NULL)
		return 1;

	for(i=0;i<numero_de_comandos();i++){
		if(strcmp(args[0],comandos_str[i]) == 0){
		return (*comandos_func[i])(args);
		}
	}
	printf("Command not found. Type help for list of commands.\n");
	return 1;
}

void msh_loop(void){
	int status;	
	char *line;
	char **args;
	
	
	do{
		printf("msh> ");
		line = read_line();
		args = split_line(line);	
		status = args_parser(args);		
		free(line);
		free(args);
		}while(status); 
}

int main(int argc, char **argv)
{
	msh_loop();	
}
