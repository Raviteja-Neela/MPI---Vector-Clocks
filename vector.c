//********************************************************************
//
// RAVI TEJA NEELA 
// Programming Project: VECTOR

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <stdio.h>
#include <math.h>
int main(int argc, char *argv[])
{
	int size,rank,i,j,l,num,ss,sp;
	
	char buff[100],arr[100];
	char buff1[100],str[100][100],msg[100][100],m;
	FILE *fp;
	char *ptr,*token;
	
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Status st;
	int clock[size],clockval[100]; 
	
	fp = fopen(argv[1],"r");
	
	//initially all the clocks are assigned to 0
	for(i=1;i<size;i++)
	{
		clock[i]=0;
	}
	if(rank==0)
	{
		
		fgets(buff,100,fp);
		num= atoi(buff);
		printf("There are %d processes executing in the System\n",num);
		if(num!= (size-1))
		{ //reads the first line of the file and if the processes are less than the or greater than the processes needed
			m='o';
			printf("you need to enter %d processes",num+1);
			for(i=1;i<size;i++)
				{
					MPI_Send(&m,1,MPI_CHAR,i,1,MPI_COMM_WORLD);

				}
			MPI_Finalize();
			return 0;
			
		}
		while(!feof(fp))
		{
			
			fgets(buff,100,fp);	
			strcpy(buff1,buff);			
			ptr=strtok(buff, " ");//strtok is used to tokenize the strings untill a specified delimiter is found.
			int r=0;	
			while(ptr!=NULL)
			{
				strcpy(str[r],&ptr[0]);
				//printf("Tokenized String : %s \n", &ptr[0]);
				ptr=strtok(NULL," ");
				r++;
			}
						
			//this is for exection command
			if(!strncmp(str[0],"exec",4))
			{
				
				ss=atoi(str[1]);
				m='x';
				MPI_Send(&m,1,MPI_CHAR,ss,1,MPI_COMM_WORLD);
			}
			//this is for sending command
			else if(!strncmp(str[0],"send",4)) 
			{
				token=strtok(buff1, "\"");
				int y=0;
				while(token!=NULL)
				{
					strcpy(msg[y],&token[0]);
					//printf("Tokenized message of j: %d : %s \n", j,msg[1]);
					token=strtok(NULL,"\"");
					y++;
				}
				
				sp=atoi(str[1]);
				
				i=atoi(str[2]);
				msg[1];
				m='e';
				MPI_Send(&m,1,MPI_CHAR,sp,1,MPI_COMM_WORLD);
				MPI_Send(&i,1,MPI_INT,sp,2,MPI_COMM_WORLD);
				MPI_Send(msg[1],100,MPI_CHAR,sp,3,MPI_COMM_WORLD);
				//MPI_Recv(&l,1,MPI_CHAR,MPI_ANY_SOURCE,100,MPI_COMM_WORLD,&st);
				//printf("Process %d, received acknowledgement from Process %d\n", rank, st.MPI_SOURCE);
			}
			//this is for end command
			else if(!strncmp(str[0],"end",3))
			{
				m='n';
				for(i=1;i<size;i++)
				{
					MPI_Send(&m,1,MPI_CHAR,i,1,MPI_COMM_WORLD);
					MPI_Recv(clock,100,MPI_INT,i,101,MPI_COMM_WORLD,&st);
					printf( "Logical time at process %d is ( ", i);
					for(j=1;j<size;j++)
					{
						printf("%d ",clock[j]);
					}
					printf(")\n");	
				}

			}
		}
		
	}
	if(rank!=0)
	{
		while(1)
		{	
			
			MPI_Recv(&m,1,MPI_CHAR,MPI_ANY_SOURCE,1,MPI_COMM_WORLD,&st);
			//if you receive message 'x' to start execute command 
			if(m=='x')
			{
				printf( "Execution event in process %d\n", rank);
				clock[rank]++;
				printf( "Logical time at process %d is ( ", rank);
				for(i=1;i<size;i++)
				{
					printf("%d ",clock[i]);
				}
				printf(")\n");
			}
			//if you receive message 'e' to start send command
			//sending from co-ordinator to a process
			else if(m=='e')
			{
				printf( "send event in process %d\n", rank);
				clock[rank]++;
				printf( "Logical time at process %d is ( ", rank);
				for(i=1;i<size;i++)
				{
					printf("%d ",clock[i]);
				}
				printf(")\n");
				MPI_Recv(&i,1,MPI_INT,MPI_ANY_SOURCE,2,MPI_COMM_WORLD,&st);
				
				MPI_Recv(arr,100,MPI_CHAR,MPI_ANY_SOURCE,3,MPI_COMM_WORLD,&st);
				printf("Message sent from process %d to process %d : %s \n",rank,i,arr);
				m='z';
				MPI_Send(&m,1,MPI_CHAR,i,1,MPI_COMM_WORLD);
				MPI_Send(clock,100,MPI_INT,i,25,MPI_COMM_WORLD);
				MPI_Send(arr,100,MPI_CHAR,i,35,MPI_COMM_WORLD);

			}
			//sending from one processe to other process
			else if(m=='z')
			{
				MPI_Recv(clockval,100,MPI_INT,MPI_ANY_SOURCE,25,MPI_COMM_WORLD,&st);
				
				printf( "process %d received clock value from process %d and it is ( ", rank, st.MPI_SOURCE);
				for(i=1;i<size;i++)
				{
					printf("%d ",clockval[i]);
				}
				printf(")\n");
				MPI_Recv(arr,100,MPI_CHAR,MPI_ANY_SOURCE,35,MPI_COMM_WORLD,&st);
				printf("Message received to process %d from process %d : %s \n",rank,st.MPI_SOURCE,arr);
				clock[rank]++;
				for(i=1;i<size;i++)
				{
					if(clock[i]<clockval[i])
					{
						clock[i]=clockval[i];
					}
				}
				printf( "Logical time at process %d is ( ", rank);
				for(i=1;i<size;i++)
				{
					printf("%d ",clock[i]);
				}
				printf(")\n");
				//sending acknowledgement that sending is done
				MPI_Send(&l,1,MPI_CHAR,0,100,MPI_COMM_WORLD);
			}
			else if(m=='n')
			{
				MPI_Send(clock,100,MPI_INT,0,101,MPI_COMM_WORLD);
				MPI_Finalize();
				return 0;
			}
			//this is for ending processes if the processes give are inappropriate
			else if (m=='o')
			{
				MPI_Finalize();
				return 0; 
			}
		}
	}
	fclose(fp);
	MPI_Finalize();
	return 0;
}