#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define COMMAND_SIZE 1024
#define TOK_BUFFER_SIZE 64
#define TOK_DELITM " \t\a\r\n"


int shell_cd(char** args);
int shell_help(char** args);
int shell_exit(char** args);

char* builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &shell_cd,
    &shell_help,
    &shell_exit
};

int shell_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}




int shell_launch(char** args){
    pid_t pid , wpid;
    int status;

    pid = fork();

    if(pid == 0){
        //child process
        if(execvp(args[0],args) == -1){
            perror("shell");
        }
        exit(EXIT_FAILURE);
    }else{
        if(pid < 0){
            perror("shell");
        }
        else{
            // parent process
            do
            {
                wpid = waitpid(pid,&status,WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            
        }
    }
    return 1;
}

int shell_cd(char** args){
    if(args[1] == NULL){
        fprintf(stderr, "shell: expected argument to \"cd\"\n");
    }
    else{
        if(chdir(args[1]) != 0){
            perror("shell");
        }
    }
    return 1;
}


int shell_help(char** args){
    printf("Salah Shell\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in :\n");

    for(int i = 0 ; i < shell_num_builtins() ; i++){
        printf(" %s\n",builtin_str[i]);
    }
    printf("Use the man command for information on other programs.\n");
    return 1;
}

int shell_exit(char** args){
    return 0;
}



char* read_line(void){
    int buffer_size = COMMAND_SIZE;
    char* buffer = malloc(buffer_size*sizeof(char));
    int position = 0;
    int c; // EOF is an interger

    if(!buffer){
        fprintf(stderr,"shell: allocation error\n");
        exit(EXIT_FAILURE);
    }
    while(1){
        // Read a character
        c = getchar();

        if (c == EOF) {
            exit(EXIT_SUCCESS);
        } else if (c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        if(position >=  buffer_size){
            buffer_size += COMMAND_SIZE;
            buffer = realloc(buffer,buffer_size);
            if(!buffer){
                fprintf(stderr,"shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }

}



char** split_line(char* line){
    int buffer_size = TOK_BUFFER_SIZE , position = 0;
    char** tokens = malloc(buffer_size*sizeof(char*));
    char* token , **tokens_backup;  

    if(!tokens){
        fprintf(stderr,"shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line,TOK_DELITM);

    while (token != NULL)
    {
        tokens[position] = token;
        position += 1;

        if(position >= buffer_size){
            buffer_size += TOK_BUFFER_SIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, buffer_size * sizeof(char*));
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL,TOK_DELITM);
    }
    tokens[position] = NULL;
    return tokens;
}

int execute(char** args){
    if(args[0] == NULL){
        return 1;
    }
    for(int i = 0 ; i < shell_num_builtins() ; i++){
        if(strcmp(args[0],builtin_str[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }
    return shell_launch(args);
}



void shell_loop(void){
    
    char* line;
    char** args;
    int status;
    do
    {
        /* code */
        printf("> ");
        line = read_line();
        args = split_line(line);
        status = execute(args);

        free(line);
        free(args);
    } while (status);
    
}



// char* read_line(void){
//     char* line = NULL;
//     ssize_t buffer_size = 0;
//     if(getline(&line,buffer_size,stdin) == -1){
//         if(feof(stdin)){
//             exit(EXIT_FAILURE);
//         }else{
//             perror("readline");
//             exit(EXIT_FAILURE);
//         }
//     }
//     return line;
// }




int main(int argc , char* argv[]){
    shell_loop();

    return EXIT_SUCCESS;
}