
#include<stdio.h> 
#include<string.h> 
#include<sys/wait.h> 
#include<stdlib.h>
#include<sys/types.h> 
#include<unistd.h> 


  
#define MAXCOM 3000 
#define MAXLIST 500 
  
//clear the shell
#define clear() printf("\033[H\033[J") 

  

  // function for finding pipe 
int Pipe_parser(char* str, char** strpiped) 
{ 
    int i; 
    for (i = 0; i < 2; i++) { 
        strpiped[i] = strsep(&str, "|"); 
        if (strpiped[i] == NULL) 
            break; 
    } 
  
    if (strpiped[1] == NULL) 
        return 0; // returns zero if no pipe is found. 
    else { 
        return 1; 
    } 
} 

  

  
//THIS WILL CHECK FOR BUILTIN COMMANDS ENTERED
int check_for_builtin_cmd(char** parsed) 
{ 
    int NoOfOwnCmds = 4, i, switchOwnArg = 0; 
    char* ListOfOwnCmds[NoOfOwnCmds]; 
    char* username; 
  
    ListOfOwnCmds[0] = "exit"; 
    ListOfOwnCmds[1] = "cd"; 
    ListOfOwnCmds[2] = "help"; 
    ListOfOwnCmds[3] = "hello"; 
  
    for (i = 0; i < NoOfOwnCmds; i++) { 
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) { 
            switchOwnArg = i + 1; 
            break; 
        } 
    } 
  
    switch (switchOwnArg) { 
    case 1: 
        printf("\nGoodbye\n"); 
        exit(0); 
    case 2: 
        chdir(parsed[1]); 
        return 1; 
    case 3: 
        username = getenv("USER"); 
        printf("\nHello %s", 
            username); 
        return 1; 
    default: 
        break; 
    } 
  
    return 0; 
} 
  

  
// this function will check space for space parses
void space_parser(char* str, char** parsed) 
{ 
    int i; 
	
  
    for (i = 0; i < MAXLIST; i++) { 
        parsed[i] = strsep(&str, " "); 
  
     
        if (strlen(parsed[i]) == 0) 
            i--; 
		if (parsed[i] == NULL) 
            break; 
    } 
} 
  
int go_thorugh_string(char* str, char** parsed, char** parsedpipe) 
{ 
  
    char* strpiped[2]; 
    int check_pipe = 0; 
  
    check_pipe = Pipe_parser(str, strpiped); 
  
    if (check_pipe) { 
        space_parser(strpiped[0], parsed); 
        space_parser(strpiped[1], parsedpipe); 
  
    } 
	else { 
  
        space_parser(str, parsed); 
    } 
  
    if (check_for_builtin_cmd(parsed)) 
        return 0; 
    else
        return 1 + check_pipe; 
} 
  
int main() 
{ 
    char str[MAXCOM], *parsing_argus[MAXLIST]; 
    char* parsing_argusPiped[MAXLIST]; 
    int check = 0; 
    clear();

	

  
    while (1) { 

      //THIS PART WILL PRINT THE DIR FOR THE SHELL
				char cwd[1024]; 
				getcwd(cwd, sizeof(cwd)); 
				printf("\nDir: %s", cwd); 
        
		
		
		//here it will take the string
						char* buf; 
						scanf("%s",buf); //read the buffer
						 
						if (strlen(buf) != 0) 
						{ 
							 
							strcpy(str, buf); 
							continue;  
						}

						else { 
							break;
						} 
		
		
		
		
		
        // process 
        if( go_thorugh_string(str,  parsing_argus, parsing_argusPiped)==1)  //this will check for a builtin command
		{
				// Forking a child 
					pid_t pid = fork();  
				  
					if (pid == -1) { 
						printf("\nFailed forking child.."); 
						return; 
					} else if (pid == 0) { 
						if (execvp(parsing_argus[0], parsing_argus) < 0) { 
							printf("\nCould not execute command.."); 
						} 
						exit(0); 
					} 
					else 
					{ 
						// waiting for child to terminate 
						wait(NULL);  
						return; 
					} 
			
		
		}		
		
		
		 if( go_thorugh_string(str,  parsing_argus, parsing_argusPiped)==2) //this will check for if the command includes a pipe 
		{
			
							
					int pipefd[2];  
					pid_t pidno1, pidno2; 
				  
					if (pipe(pipefd) < 0) { 
						printf("\nunintialized pipe"); 
						return; 
					} 
					pidno1 = fork(); 
					if (pidno1 < 0) { 
						printf("\nFORk failed"); 
						return; 
					} 
				  
					if (pidno1 == 0) { 
						// Child 1 executing.. 
						// It only needs to write at the write end 
						close(pipefd[0]); 
						dup2(pipefd[1], STDOUT_FILENO); 
						close(pipefd[1]); 
				  
						if (execvp(parsing_argus[0], parsing_argus) < 0) { 
							printf("\nfailed to execute cmd"); 
							exit(0); 
						} 
					} else { 
						// Parent executing 
						pidno2 = fork(); 
				  
						if (pidno2 < 0) { 
							printf("\n fork failed"); 
							return; 
						} 
				  
						//read end data read here
						if (pidno2 == 0) { 
							close(pipefd[1]); 
							dup2(pipefd[0], STDIN_FILENO); 
							close(pipefd[0]); 
							if (execvp(parsing_argus[0], parsing_argus) < 0) { 
								printf("\ncommand 2 failed"); 
								exit(0); 
							} 
						} else { 
						
						
							
							wait(NULL); 
						
						wait(NULL); 

						} 
					} 
		}		
		
       
    } 
    return 0; 
}
