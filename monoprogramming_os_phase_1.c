#include<stdio.h>
#include<string.h>
#include<stdlib.h>

// Function declaration
void IN_IT();
void START_EXECUTION();
void LOAD();

//variable declaration
char M[100][4]; // MEMORY
char IR[4]; // instruction register
char R[4]; // general purpose register
int IC; // instruction counter
int C;// toggle
char buf[41];  //buffer stores 1block and an empty space
int SI;// interrupt
int i=0,j;
int m = 0; //t
int bi=0; // buffer index ,a
int operand;  //stores the operand in integer format
FILE* inputfile;// file ptr for input file
FILE* outputfile;// file ptr for output file

void INIT(){

    int i,j;
    for(i=0; i<100;i++)
        for(j=0;j<4;j++)
            M[i][j]= '\0';// initialize with Null character

    for(i=0;i<4;i++){
        IR[i]='\0';//initialize with Null character
        R[i] = '\0';//initialize with Null character
    }
}


void READ(){
    int i,j,bi;

    IR[3] = '0'; //GD works blockwise so if in case unit digit of operand is not 0,
    //we will IN_ITialize it to 0 in this step to avoid errors

    if(fgets(buf,41,inputfile) != NULL){
        bi =0;
        for(i=operand;i<operand+10;i++){
            printf("M[%d] : ",i);
            for(j=0;j<4 && buf[bi] !='$';j++){
                M[i][j] = buf[bi];
                bi++;
                if(M[i][j] == '\n') continue;
                printf("%c",M[i][j]);
            }
            printf("\n");
        }
    }

    EXECUTE_USER_PROGRAM();
}

void WRITE(){
    int i,j;
    bi=0;
    IR[3] = '0'; //PD works blockwise so if unit digit of operand is not 0,
    //we will Initialize it to 0 in this step to avoid errors

    for(i =0;i<40;i++)
        buf[i]= '\0';

    for(i=operand;i<operand+10;i++){
        for(j=0; j<4 && M[i][j]!='$'; j++){
            if(M[i][j]=='\n')
            {
                buf[bi] = ' ';
            }
            else{
                buf[bi] = M[i][j];
                //printf("%c",buf[bi]);
            }

            //printf("%c",buf[bi]);
            bi++;
        }
    }
    buf[bi] ='\n';
    printf("\nOutput: ");
    int z;
    for(z=0;z<41;z++)
    {
        printf("%c",buf[z]);
    }
    fputs(buf,outputfile);
    fputs("\n" , outputfile);
    EXECUTE_USER_PROGRAM();
}


void TERMINATE(){
    for(int i =0; i<40;i++)
        buf[i] ='\0';
    buf[0]='\n';
    buf[1]='\n';
    fputs(buf,outputfile);
}


void M_O_S(){
    switch(SI){
        case 1:
            READ();
            break;
        case 2:
            WRITE();
            break;
        case 3:
            TERMINATE();
            break;
        default:
            SI= 0;
    }
}


void EXECUTE_USER_PROGRAM(){

    printf("\n");
    for(i=0;i<4;i++){
        IR[i]= M[IC][i];
        printf("%c",IR[i]);
    }
    printf("\n");
    IC++;

    //typecasting IR[2] and IR[3]
    operand = ((int)IR[2] -48)*10 + ((int)IR[3] -48);

    if(IR[0] == 'G' && IR[1] =='D')
    {
        SI = 1;
        //printf("\nGD: \n");
        M_O_S();
    }

    else if(IR[0] == 'P' && IR[1] =='D')
    {
        int i;
        SI = 2;
        //printf("\nPD: \n");
        for(i=operand;i<operand+10;i++)
        {
            printf("M[%d]: ",i);
            for(j=0;j<4;j++)
            {
                if(M[i][j] == '\n') continue;
                printf("%c",M[i][j]);
            }
            printf("\n");
        }
        M_O_S();
    }

    else if(IR[0] == 'H')
    {
        SI = 3;
        printf("Program Ends...");
        M_O_S();
    }

    else if(IR[0]=='L' && IR[1]=='R')
    {
        printf("M[%d] : " , operand);
        for(i=0;i<4;i++)
        {
            printf("%c",M[operand][i]);
        }

        printf("\nR     : ");
        for(i=0;i<4;i++)
        {
            R[i]=M[operand][i];   //R<-M[IC]
            printf("%c",R[i]);
        }
        printf("\n");
        EXECUTE_USER_PROGRAM();
    }

    else if(IR[0]=='S' && IR[1]=='R')
    {
        //printf("\nSR: \n" );
        printf("Stored word -->\n");
        printf("M[%d] : ",operand);
        for(i=0;i<4;i++)
        {
            M[operand][i]=R[i];   //M[IC]->R
            printf("%c",M[operand][i]);
        }
        printf("\n");
        EXECUTE_USER_PROGRAM();
    }

    else if(IR[0]=='C' && IR[1]=='R')
    {
        for(i=0;i<4;i++){
            if(R[i]==M[operand][i])
            {
               printf("Character are same!\n");
               C=1;
            }
            else
            {
                C=0;
                printf("Character are not same!\n");
                break;
            }
        }
        EXECUTE_USER_PROGRAM();
    }

    else if(IR[0] == 'A' && IR[1]=='D'){
        int op1 = ((int)R[0] -48)*1000 + ((int)R[1] -48)*100 + ((int)R[2] -48)*10 +((int)R[3] -48) ;
        int op2 = ((int)M[operand][0] -48)*1000 + ((int)M[operand][1] -48)*100 + ((int)M[operand][2] -48)*10 + ((int)M[operand][3] -48);
        int add = op1 + op2;

        printf("R : %d \nM[%d] : %d \nAddition : %d\n" , op1 , operand , op2 , add);
        if(add > 9999) {
             printf("Overflow...\n");
        }

        int div = 1000;
        for(int i = 0 ; i < 4 ; i++){
            M[operand][i] = add/div + '0';
            add = add%div;
            div = div/10;
        }

        //printf("%c %c %c %c\n" , M[operand][0] , M[operand][1] , M[operand][2] , M[operand][3]);

        EXECUTE_USER_PROGRAM();

    }

    else if(IR[0]=='B' && IR[1]=='T')
    {
        if(C==1)
        {
            IC=operand;
            printf("IC -->%d\n", IC); //prints pointer location between 0-99
        }

    EXECUTE_USER_PROGRAM();
    }
}


void START_EXECUTION(){
    IC =0;
    printf("\nInside start execution :- \n");
    EXECUTE_USER_PROGRAM();
}


void LOAD(){
    int t=0;
    int a=0;

    while(fgetc(inputfile)!=EOF)//get all the characters in the input file
	{
        fseek(inputfile,-1,SEEK_CUR);     // finds the current line pointed by IC

        for(int i=0; i<41;i++)
            buf[i] ='\0';

        fgets(buf,42,inputfile);
        printf("\n%s" , buf);

    	if(buf[0] == '$' && buf[1] == 'A' && buf[2] =='M' && buf[3] == 'J') // AMJ - Starts the program
    	{
        	printf("Process starts...\n");
        	t=0;
        	INIT();
    	}
    	else if(buf[0] == '$' && buf[1] == 'D' && buf[2] =='T' && buf[3] == 'A')
    	{
        	START_EXECUTION();
    	}
    	else if(buf[0] == '$' && buf[1] == 'E' && buf[2] =='N' && buf[3] == 'D'){
            printf("\n---------------------------------------------------------------------------");
        	continue;
    	}
		else
    	{
	    	int k;
	        for(k=t;k<t+10;k++)
	        {
	            printf("M[%d]:",k);
	            int i;
	            for(i=0;i<4;i++)
	            {
	                M[k][i]=buf[a];
	                a++;
	                if(M[k][i] == '\n') continue;
	                printf("%c",M[k][i]);
	            }
	            fflush(stdin);
	            printf("\n");
	        }
	        t=t+10;
	        a=0;
	    }
    }
    fclose(inputfile);
    fclose(outputfile);
}


int main(){

    inputfile = fopen("inputfile.txt","r");//open input file in read mode
    outputfile = fopen("outputfile.txt","w");//open output file in write mode
    if(inputfile == NULL){
        printf("Can't find Input file");
        exit(1);
    }
    LOAD();
    return 0;
}






