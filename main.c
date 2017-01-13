#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

char cwd[1024];
int ct;
char str[70];
int ground;
char string[100], *pointer;
int position, length;
int i=1;
int background; /* equals 1 if a command is followed by '&' */
char inputBuffer[MAX_LINE]; /*buffer to hold command entered */
char *args[MAX_LINE/2 + 1]; /*command line arguments */

struct listOfCommand{
    char command[70];
    struct listOfCommand *nextPtr;
};
typedef struct listOfCommand listOfCommand;
typedef struct listOfCommand** listOfCommandPtrPtr;
typedef struct listOfCommand* listOfCommandPtr;
void addCommand(char []);
void removeListOfCommand();
void showLastTenCommands(listOfCommandPtr);
void getCommand(int i);
void checkString(char *);
int linkedListLength();
listOfCommandPtr d,hdr=NULL,p;
listOfCommandPtrPtr hdrr=NULL;
char *getList;
char input[70];
char *argscontrol[4];
char *strToCharArray[70];
char input_control[70];
char addLink[70];
char pipeArgs[70];
static sigjmp_buf jmpbuf;
static volatile sig_atomic_t jumpok = 0;
int in,out,err;
void addCommand(char []);
void myPipes(char*[]);
void setup(char [],char* [],int*);
void myprogio(char*[]);
char* deblank(char*);
void pipes(char*,char[]);
pid_t exec_redirect(const char *,char *[],int,int,int);

/* The setup function below will not return any value, but it will just: read
in the next command line; separate it into distinct arguments (using blanks as
delimiters), and set the args array entries to point to the beginning of what
will become null-terminated, C-style strings. */
void my_handler(int s){
    printf("Ctrl+C Yakalandı %d\n",s);
    pid_t child=fork();
    if(child ==0)
    showLastTenCommands(hdr);
    else if(wait(NULL)!=child)
        perror("waitnull ctrl c");
    setup(inputBuffer, args, &background);
    char *s1=strchr(args[0],'!');

    if(s1!=NULL){
        if(strcmp(s1,"!0")==0){
            getCommand(0);
            checkString(getList);
        }
        else if(strcmp(s1,"!1")==0){
            getCommand(1);
            checkString(getList);
        }
        else if(strcmp(s1,"!2")==0){
            getCommand(2);
            checkString(getList);
        }
        else if(strcmp(s1,"!3")==0){
            getCommand(3);
            checkString(getList);
        }
        else if(strcmp(s1,"!4")==0){
            getCommand(4);
            checkString(getList);
        }
        else if(strcmp(s1,"!5")==0){
            getCommand(5);
            checkString(getList);
        }
        else if(strcmp(s1,"!6")==0){
            getCommand(6);
            checkString(getList);
        }
        else if(strcmp(s1,"!7")==0){
            getCommand(7);
            checkString(getList);
        }
        else if(strcmp(s1,"!8")==0){
            getCommand(8);
            checkString(getList);
        }
        else if(strcmp(s1,"!9")==0){
            getCommand(9);
            checkString(getList);
        }
        else if(strcmp(s1,"!!")==0){
            getCommand(linkedListLength()-1);
            checkString(getList);
        }

    }


    if (jumpok == 0) return;
    siglongjmp(jmpbuf, 1);
    //exit(1);
}

void setup(char inputBuffer[], char *args[],int *background)
{

    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start;  /* index where beginning of next command parameter is */
            /* index of where to place the next parameter into args[] */

    ct = 0;

    /* read what the user enters on the command line */
    length = read(STDIN_FILENO,inputBuffer,MAX_LINE);

    /* 0 is the system predefined file descriptor for stdin (standard input),
       which is the user's screen in this case. inputBuffer by itself is the
       same as &inputBuffer[0], i.e. the starting address of where to store
       the command that is read, and length holds the number of characters
       read in. inputBuffer is not a null terminated C-string. */

    start = -1;
    if (length == 0) {
	printf("Bir ctrl+d sinyali algılandı!");
	perror("exit");
	exit(0);
	}         /* ^d was entered, end of user command stream */

/* the signal interrupted the read system call */
/* if the process is in the read() system call, read returns -1
  However, if this occurs, errno is set to EINTR. We can check this  value
  and disregard the -1 value */
    if ( (length < 0) && (errno != EINTR) ) {
        perror("error reading the command");
	exit(-1);           /* terminate with error code of -1 */
    }
    strcpy(addLink,inputBuffer);
    strcpy(pipeArgs,inputBuffer);
	printf(">>%s",inputBuffer);
    for (i=0;i<length;i++){ /* examine every character in the inputBuffer */

        switch (inputBuffer[i]){
	    case ' ':
	    case '\t' :               /* argument separators */
		if(start != -1){
                    args[ct] = &inputBuffer[start];    /* set up pointer */
		    ct++;
		}
                inputBuffer[i] = '\0'; /* add a null char; make a C string */
		start = -1;
		break;

            case '\n':                 /* should be the final char examined */
		if (start != -1){
                    args[ct] = &inputBuffer[start];
		    ct++;
		}
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
		break;

	    default :             /* some other character */
		if (start == -1)
		    start = i;
                if (inputBuffer[i] == '&'){
		    *background  = 1;
                    inputBuffer[i-1] = '\0';
		}
	} /* end of switch */
     }    /* end of for */
     args[ct] = NULL; /* just in case the input line was > 80 */

	for (i = 0; i <= ct; i++)
		printf("args %d = %s\n",i,args[i]);
} /* end of setup routine */
char *substring(char *string, int position, int length)
{
   char *pointer;
   int c;

   pointer = malloc(length+1);

   if (pointer == NULL)
   {
      printf("Unable to allocate memory.\n");
      exit(EXIT_FAILURE);
   }

   for (c = 0 ; c < position -1 ; c++)
      string++;

   for (c = 0 ; c < length ; c++)
   {
      *(pointer+c) = *string;
      string++;
   }

   *(pointer+c) = '\0';

   return pointer;
}

void whereis(char *args[],int q) {
    char a[100]="whereis ";


    if(strcmp(args[0],"where")==0 && q==0){
            if(args[1] != NULL) {
                if(args[0] != NULL) {
                strcat(a,args[1]);
		strcat(a,"| awk '{print $2}'");
                strcat(a,">text.txt");
                system(a);
                FILE *p;
                if((p=fopen("text.txt","r"))==NULL) {
                    printf("\nUnable t open file text.txt");
                    exit(1);
                }
                if(fgets(str,12,p)!=NULL)
                    puts(str);
                    fclose(p);

                }
            }
    }
    else if (q==1){
            if(args[0] != NULL) {
                strcat(a,args[0]);
		strcat(a,"| awk '{print $2}'");
                strcat(a,">text.txt");
                system(a);
                FILE *p;
                if((p=fopen("text.txt","r"))==NULL) {
                    printf("\nUnable t open file text.txt");
                    exit(1);
                }
                if(fgets(str,12,p)!=NULL)
                    puts(str);
                    fclose(p);
                }
            }
    }

int forkAndExecvWorks (char* args[]) {
	whereis(args,1);
	int pid;
	int status;

	if( (pid = fork()) == 0 ) {
		/* child code starts here */
		execv(str, args);
	}
	else {
		/* If the parent doesn't call wait (or waitpid), then both parent and child will
		run at the same time. (child runs in the background) */
		if(background != 1) {
			wait(&status);
		}
		/* Parent Code Starts here */
		if(i % 2 ==0){
			startPage(args);
		}
	}
	ground=0;
	return 0;
}

int forkAndExeclWorks (char* args[]) {
	int pid;
	int status;
	whereis(args,1);

        int k =0;
    	while(strstr(args[k],"/")==NULL){
        	argscontrol[k]=args[k];
        	printf("\nargs %d = %s",k,argscontrol[k]);
        	k++;
    	}


	if( (pid = fork()) == 0 ) {
		/* child code starts here */
		if(background == 1)	{
			execl(args[ct - 2], argscontrol[0],argscontrol[1],argscontrol[2],argscontrol[3],argscontrol[4],NULL);
		}
		if(background ==0){
			execl(args[ct - 1], argscontrol[0],argscontrol[1],argscontrol[2],argscontrol[3],argscontrol[4],NULL);
		}
	}

	else {
		/* If the parent doesn't call wait, then both parent and child will
		run at the same time. (child runs in the background) */
		if(background != 1) {
			args[ct-1] = NULL;
			wait(&status);
		}
		/* Parent Code Starts here */
		if(i % 2 ==0){
			startPage(args);
		}
	}
	ground=0;
	return 0;
}

int getPwd() {
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	fprintf(stdout, "Current working dir: %s\n", cwd);
        else
        perror("getcwd() error");
	return 0;
}

void changeDirectory(char *args[]) {
	if(strcmp(args[0],"cd")==0){
		if(args[1] != NULL) {
			if(args[2] == NULL) {
				chdir(args[1]);
				getPwd();
			}
		}
        }
}

void clearScreen(char *args[]) {
	if(strcmp(args[0],"clr")==0){
		if(args[1] == NULL) {
				system("clear");
		}
        }
}

void printEnv(char *args[]) {
	char a[100]="printenv ";

	if(strcmp(args[0],"print")==0){
		if(args[1] != NULL) {
			strcat(a,args[1]);
			if(args[2] == NULL) {
				system(a);
			}
		}
		else if(args[1] == NULL) {
			system("printenv");
		}
    	}
}


void setEnv(char *args[]) { 
	perror("C12");
	char b[100]="export ";

	if(strcmp(args[0],"set")==0){
		perror("chdir");
		if(args[1] != NULL) {
			strcat(b,"");
			strcat(b,args[1]);
			if(strcmp(args[2],"=")==0) {
				if(args[3] != NULL) {
					strcat(b,"=");
					strcat(b,args[3]);
					system(b);
				}
			}
		}
       	 }
}

void exitMethod(char *args[]) {
	if(strcmp(args[0],"exit")==0){
		if(args[1] == NULL) {
			if(background == 1)
				printf("There are background processes still running!");
			if (background ==0)
				exit(0);
		}
	}
}

int pathMethod(char *args[]) {
	char a[100]="PATH=$PATH:";
	char b[100]="export PATH=${PATH%:";

	if(strcmp(args[0],"path")==0){
		if(args[1] == NULL) {
			system("$PATH");
			return(1);
		}
	}

	if(strcmp(args[0],"path")==0){
		if(strcmp(args[1],"+")==0) {
			if(args[2] != NULL) {
				strcat(a,args[2]);
				system(a);
				return(1);
			}
		}
	}

	if(strcmp(args[0],"path")==0){ 
		if(strcmp(args[1],"-")==0) {
			if(args[2] != NULL) {
				strcat(b,args[2]);
				strcat(b,"}");
				system(b);
				return(1);
			}
		}
	}
    return 1;
}

void substringForExecBack(char *args[]) {
	char str[80];
	strcpy(str,args[ct-2]);

	char search[10]="/";
	int count1=0;
	int count2=0;
	int i;
	int j;
	int flag;

	while (str[count1]!='\0')
	count1++;

	while (search[count2]!='\0')
	count2++;

	for(i=0;i<=count1-count2;i++){
		for(j=i;j<i+count2;j++){
			flag=1;
			if (str[j]!=search[j-i]) {
				flag=0;
				break;
			}
		}
		if (flag==1)
		break;
	}
	if (flag==1) {
	puts("SEARCH SUCCESSFUL!");
	//execl ye gönder
	forkAndExeclWorks (args);
	}
	else {
	puts("SEARCH UNSUCCESSFUL!");
	//execv ye gönder
	forkAndExecvWorks (args);
	}
}

void substringForExecFore(char *args[]) {
	char str[80];
	strcpy(str,args[ct-1]);

	char search[10]="/";
	int count1=0;
	int count2=0;
	int i;
	int j;
	int flag;

	while (str[count1]!='\0')
	count1++;

	while (search[count2]!='\0')
	count2++;

	for(i=0;i<=count1-count2;i++){
		for(j=i;j<i+count2;j++){
			flag=1;
			if (str[j]!=search[j-i]) {
				flag=0;
				break;
			}
		}
		if (flag==1)
		break;
	}
	if (flag==1) {
	//puts("SEARCH SUCCESSFUL!");
	//execl ye gönder
	forkAndExeclWorks (args);
	}
	else {
	//puts("SEARCH UNSUCCESSFUL!");
	//execv ye gönder
	forkAndExecvWorks(args);
	}
}

void substringForBackOrForeground(char *args[]) {
	char str[80];
	strcpy(str,args[ct-1]);

	char search[10]="&";
	int count1=0;
	int count2=0;
	int i;
	int j;
	int flag;

	while (str[count1]!='\0')
	count1++;

	while (search[count2]!='\0')
	count2++;

	for(i=0;i<=count1-count2;i++){
		for(j=i;j<i+count2;j++){
			flag=1;
			if (str[j]!=search[j-i]) {
				flag=0;
				break;
			}
		}
		if (flag==1)
		break;
	}
	if (flag==1) {
	puts("SEARCH SUCCESSFUL! background process");
	args[ct-1] = NULL;
	substringForExecBack(args);
	}
	else {
	puts("SEARCH UNSUCCESSFUL! foreground process");
	substringForExecFore(args);
	ground =1;
	}
}

void inputFromFile(char *sin){
	char in[70];
	char *inputs[70];
	int i =0;
	printf("\n1");
	char* word ;
	for (word = strtok(sin, " "); word != NULL; word = strtok(NULL, " ")){
	        inputs[i]=word;
        	printf("\n%s",inputs[i]);
        	i++;
    	}
	printf("\n2\n");
	FILE *p;
	printf("\n%s",inputs[0]);
        if((p=fopen(sin,"r"))==NULL){
    	    printf("burda Unable t open file text.txt");
    	    exit(1);
    	}
    	if(fgets(input,255,p)!=NULL)
        puts(input);
    	fclose(p);
    	printf("\n11input = %s",input);
}


void myprogio(char *args[]){
    printf("\n args[1]=%s ",args[1]);
    printf("args[2]=%s ad ",args[2]);
    inputFromFile(args[2]);
   
    char *inputs[70];
    char *inputs1[70]={NULL,NULL,NULL,NULL,NULL,NULL,NULL};

    strcpy(input_control,input);
    int i = 0;
    char* word ;
    for (word = strtok(input, " "); word != NULL; word = strtok(NULL, " ")){
        printf("%s",deblank(word));
        inputs1[i]=word;
        printf("inputs %s\n",inputs1[i]);
        i++;
    }

    printf(" \ninput = %s  input_control = %s\n",input,input_control);
    whereis(inputs1,1);

        in = open(args[2], O_RDONLY);
        if(args[4]!=NULL){
            if(strcmp(args[3],">>")==0)
                out=open(args[4],O_WRONLY|O_APPEND);
            else
                out = open(args[4], O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        }
        err = open("err.txt", O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

        exec_redirect(str, inputs1, in, out, err);
}

void myPipes(char *args[]){
	perror("pipeda");
	int i=0,j=0;
	char *c,a[100],b[100],temp[100];
	perror("pipeda1");
	int pipeNumber=0;
    
	while(strcmp(args[pipeNumber],"|")==1 || args[pipeNumber]==NULL)
	pipeNumber++;
        pid_t child=fork();
        
	if(child == 0){
	        while(strcmp(args[i],"|")!=0){
                           strcat(temp,args[i]);
           	           printf("commands %s",temp);
      
                if(args[i+1]==NULL){
                    strcpy(b,temp);
                    printf("\nb = %s",b);
                    break;
                }
                else if(strcmp(args[i+1],"|")==0 && j==0){
                    strcpy(a,temp);
                    strcpy(input_control,temp);
                    strcpy(temp,"");
                    printf("\na = %s",a);
                    j++;
                    i++;
                   
                }
		else if(strcmp(args[i+1],"|")==0  && j==1 ){
                   strcpy(b,temp);
                   printf("\nb = %s",b);
                   break;
                }
		
		else perror("333");
                strcat(temp," ");
        	i++;
        }
        c=strstr(a,args[0]);	
        printf("\na = %s b = %s\n",input_control,b);
	printf("Pipeda");
	pipes(c,b);
   }
}

void pipes(char *a,char b[]){
	pid_t child = fork();
        if(child == 0){
     	   FILE *pipein_fp, *pipeout_fp;
           char readbuf[80];

        /* Create one way pipe line with call to popen() */
        if (( pipein_fp = popen(a, "r")) == NULL)
        {
                perror("popen");
                exit(1);
        }

        /* Create one way pipe line with call to popen() */
        if (( pipeout_fp = popen(b, "w")) == NULL)
        {
                perror("popen");
                exit(1);
        }

        /* Processing loop */
        while(fgets(readbuf, 80, pipein_fp))
                fputs(readbuf, pipeout_fp);

        /* Close the pipes */
        pclose(pipein_fp);
        pclose(pipeout_fp);
  }
}

int startPage(char *args[]) {
    struct sigaction sigIntHandler; // ctrl c sinyali


	sigIntHandler.sa_handler = my_handler;
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	i++;
	background = 0;
        printf(" 333sh: ");
        /*setup() calls exit() when Control-D is entered */
        setup(inputBuffer, args, &background);
        if(strstr(args[0],"!")!=NULL)
        printf("Yanlış girdiniz");
        else{
    		addCommand(addLink);
	
		if(strcmp(args[0],"cd")==0)
		if(args[1] != NULL)
		if(args[2] == NULL)
		changeDirectory(args);

		if(strcmp(args[0],"clr")==0)
		if(args[1] == NULL)
		clearScreen(args);

		if(strcmp(args[0],"print")==0)
		printEnv(args);

		if(strcmp(args[0],"set")==0)
		if(args[1] != NULL)
		if(strcmp(args[2],"=")==0)
		if(args[3] != NULL)
		if(args[4] == NULL)
		setEnv(args);

		if(strcmp(args[0],"where")==0)
		if(args[1] != NULL)
		if(args[2] == NULL)
		whereis(args,0);

		if(strcmp(args[0],"exit")==0)
		if(args[1] == NULL)
		exitMethod(args);

		if(strcmp(args[0],"myprog")==0)
                myprogio(args);
		
		if(args[0] != NULL && strcmp(args[0],"where")!=0)
		substringForBackOrForeground(args);

        	if(strstr(pipeArgs,"|")!=NULL){
			myPipes(args);
        	}

		if (sigsetjmp(jmpbuf, 1))
                fprintf(stderr, "Returned to main loop due to ^c\n");
           	jumpok = 1;

		if(strcmp(args[0],"path")==0)
		pathMethod(args);
		else
		return 1;
        }

    return 1;
}

int main(void) {
        while (1){
	       startPage(args);

               /** the steps are:
               (1) fork a child process using fork()
               (2) the child process will invoke execvp()
   	       (3) if background == 0, the parent will wait,
               otherwise it will invoke the setup() function again. */
        }
    }

    void addCommand(char t[]){
        if(linkedListLength()==10){
            removeListOfCommand();
        }

        listOfCommandPtr newNode, temp, prev;

        newNode=malloc(sizeof(listOfCommand));
        strcpy(newNode->command,t);
        newNode->nextPtr=NULL;


        if (hdr == NULL)
            hdr=newNode;

        else {
            temp=hdr;
            while (temp != NULL) {
                prev=temp;
                temp=temp->nextPtr;
            }
            if (temp!=NULL)
                newNode->nextPtr=temp;


            if (temp==hdr)
                hdr=newNode;

            else
                prev->nextPtr=newNode;
        }

    }

    void removeListOfCommand(){

        hdr=hdr->nextPtr;
        printf("deleting was succesful\n");


    }

    void getCommand(int i){
        int j=0;
        listOfCommandPtr temp;
        temp=hdr;
        while(i>j){
            temp=temp->nextPtr;
            j++;
        }
        getList=temp->command;
    }

    void showLastTenCommands(listOfCommandPtr r){
    pid_t child;
        if(r==NULL){
            printf("there is no command\n");
        }
        int i=0;
        while(r!=NULL){
            printf("\n command %d: %s ",i,r->command);
            r=r->nextPtr;
            i++;
        }
        printf("linked Liste length is %d",linkedListLength());
        printf("\n");

    }

    int linkedListLength(){
        listOfCommandPtr temp, prev;
        temp=hdr;
        int i=0;
        while (temp != NULL) {
            prev=temp;
            temp=temp->nextPtr;
            i++;
        }
        return i;
    }

    void checkString(char *c1){ 
        char *c2;
        char w[50]="which";
        pid_t child;
       if(strstr(c1,"where")!=NULL){    
            c2=strstr(c1," ");
            strcat(w,c2);
            printf("c2 = %s",w);
            system(w);
        }else if(strstr(c1,"clr")!=NULL){
            system("clear");
        }
        else if(strstr(c1,"myprog")!=NULL){
            char *inputs[70]={NULL,NULL,NULL,NULL,NULL,NULL,NULL};
            int i = 0;
            strcpy(input_control,c1);
            char *inputs2[70];
            char* word;
            for (word = strtok(input_control," "); word != NULL; word = strtok(NULL, " "))// seperate sentece to word array
                {
                printf("%s",deblank(word));
                inputs[i]=word;
                printf("%s a",inputs[i]);
                i++;
                }
            printf(" \ninput = %s  input_control = %s\n",c1,input_control);
            myprogio(inputs);
        }else if(strstr(c1,"print")!=NULL){
            char *inputs[70]={NULL,NULL,NULL,NULL,NULL,NULL,NULL};
            int i = 0;
            strcpy(input_control,c1);
            char *inputs2[70];
            char* word;
            for (word = strtok(input_control," "); word != NULL; word = strtok(NULL, " "))// seperate sentece to word array
                {
                printf("%s",deblank(word));
                inputs[i]=word;
                printf("%s a",inputs[i]);
                i++;
                }
            printEnv(inputs);


	}else if(strstr(c1,"set")!=NULL){
            char *inputs[70]={NULL,NULL,NULL,NULL,NULL,NULL,NULL};
            int i = 0;
            strcpy(input_control,c1);
            char *inputs2[70];
            char* word;
            for (word = strtok(input_control," "); word != NULL; word = strtok(NULL, " "))// seperate sentece to word array
                {
                printf("%s",deblank(word));
                inputs[i]=word;
                printf("%s a",inputs[i]);
                i++;
                }
            setEnv(inputs);
	}else if(strstr(c1,"ls")!=NULL || strstr(c1,"cd")!=NULL){
            system(c1);
	}
	else{
		printf("linkedlist elsede");
 
        }
    }
	char* deblank(char* input)
	{
	    int i,j;
	    char *output=input;
	    for (i = 0, j = 0; i<strlen(input); i++,j++)
	    {
	        if (input[i]!='\n')
	            output[j]=input[i];
	        else
	            j--;
	    }
	    output[j]=0;
	    return output;
	}

	pid_t exec_redirect(
        	      const char *path, char *args[],
        	      int new_in, int new_out, int new_err
        	      )
	{

    	pid_t child;

   	 // Fork new process
  	  child = fork();

  	  if (child == 0) {

  	      // Child process
        	
     	   // Redirect standard input
        	if (new_in >= 0) {
        	    close(STDIN_FILENO);
        	    //perror("input içinde");
        	    printf("asasassaaaaaaaaa");

      
        	    dup2(new_in, STDIN_FILENO);
        	}

        	// Redirect standard output
        	if (new_out >= 0) {
        	    close(STDOUT_FILENO);
        	    printf("outtttttt");
        	    dup2(new_out, STDOUT_FILENO);
        	}

     
        	if (new_err >= 0) {
        	    perror("error içinde");
        	    close(STDERR_FILENO);
        	    dup2(new_err, STDERR_FILENO);
        	}
            printf("after error %s a",args[0]);
        	// Execute the command
                if(strcmp(args[0],"ls")==0 ||strcmp(args[0],"ps")==0 ){
        	    printf("\n\nlsde");
                char *inputs[70]={NULL,NULL,NULL,NULL,NULL,NULL,NULL};
                execv(path,args);
        	}
        	else if(strcmp(args[0],"where")==0){
                    printf("\n\nwherede");
        	    char a[70];
        	    whereis(args,0);
        	}
        	else if(strcmp(args[0],"print")==0){
            printf("\n\nprintde");
			printEnv(args);
			}
			else if(strcmp(args[0],"myprog")==0){
                printf("\n\nmyprogde");
			myprogio(args);
			}
			else if(strcmp(args[0],"set")==0){
                printf("\n\nsetde");
			if(args[1] != NULL)
            if(strcmp(args[2],"=")==0)
            if(args[3] != NULL)
            setEnv(args);
			}
            else{
        	printf("nereye girceni bulamıyo");
        	}
    	}

    	// Parent process
    	return child;

	}

