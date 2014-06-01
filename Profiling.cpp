#include <iostream>
#include <fstream>
#include <malloc.h>
#include <string>

#define IF1 inputfilename
#define IF2 "output2"
#define OF "trace_output"

char *inputfilename;

using namespace std;

ofstream outputfile (OF);

typedef struct list
{
	int number;
	struct list *next;
}linklist, Node;

int *createarray(int n) //create n array, array[n]=1
{
	int i, *p;
	p=(int *)malloc(n*sizeof(int));	
	for(i=0;i<n;i++)
	{
		p[i]=1;
	}
	return p;
}

int **creatematrix(int n) //create n*n matrix, matrix[n][n]=1
{
	int i, j, **p;
	p=(int **)malloc(n*sizeof(int *));
	for(i=0;i<n;i++)
	{
		p[i]=(int *)malloc(n*sizeof(int));
		for(j=0;j<n;j++)
		{
			p[i][j]=1;
		}
	}
	return p;
}

void freearray(int **p, int n) //free matrix pointer
{
	int i;
	for(i=0;i<n;i++)
	{
		free(p[i]);
	}
	free(p);
	p=NULL;
}
/*
void outputedge(int **edge, int n) //output edge given edge matrix[n][n], edge[i][j]=1 means there's an edge from BBi to BBj
{
	int i, j, num=0;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			if(edge[i][j]==1)
			{
				printf("EDGE %d: BB%d -> BB%d\n",num,i,j);
				num++;
			}
		}
	}
}

void outputDOM(int **DOM, int n) //output DOM(i) given edge matrix[n][n], DOM[i][j]=1 means BBj DOM BBi
{
	int i, j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			if(DOM[i][j]==1)
			{
				printf("DOM(%d)=%d\n",i,j);
			}
		}
	}
}

int **dominator(int **edge, int n)
{
	int **DOM, *NEWD;
	DOM=creatematrix(n); //Initial values for all matrix elements are 1
	NEWD=createarray(n); //temporary dominator set for current node
	int i, j, k;

	for(j=1;j<n;j++)
		DOM[0][j]=0; //starting node n0, DOM(n0)=n0
	//Initialization, DOM[i][j]=DOM(i), DOM[i][j]=1 means j DOM i, 0 otherwise

	int change=1;
	while(change)
	{
		change=0;
		for(i=1;i<n;i++) //for each node i in N-n0, find DOM(i)
		{
			for(j=0;j<n;j++) //find the predecessor of node i in all nodes N
			{
				if(edge[j][i]==1) //edge ends at node i from node j, if edge[j][i]=1, j is predecessor of i
				{
					for(k=0;k<n;k++)
					{
						NEWD[k]=NEWD[k]*DOM[j][k]; //intersection of DOM(j)
					}
				}
			}
			NEWD[i]=1; //node i is the dominator of itself
			//constructed the new dominator set for current node i
			
			for(k=0;k<n;k++)
			{
				if(DOM[i][k]!=NEWD[k])
				{
					change=1;
					DOM[i][k]=NEWD[k];
				}
			}
			//Once DOM(i) changes, update DOM(i) and label change=1
		}
	}
	return DOM;
}

int **backedge(int **edge, int **DOM, int n) //find all back edges, backedge[i][j]=1 means i->j is a back edge
{
	int **BACKEDGE;
	BACKEDGE=creatematrix(n);
	int i, j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			BACKEDGE[i][j]=edge[i][j]*DOM[i][j]; //when edge[i][j]=1 (there's an edge i->j) AND DOM[i][j]=1 (j belongs to DOM(i)), i->j is a back edge
		}
	}
	return BACKEDGE;
}

void printmatrix(int **matrix, int n) //output DOM(i) given edge matrix[n][n], DOM[i][j]=1 means BBj DOM BBi
{
	int i, j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			printf("%d\t",matrix[i][j]);
		}
		printf("\n");
	}
} 

linklist *push(linklist *head, int ins) //Insert an element at the end of the linklist
{
	Node *p,*q;
    q=head;
	while(q->next!=NULL)
	{
		q=q->next;
	}
	p=(Node*)malloc(sizeof(Node));
	p->number=ins;
    p->next=NULL;
	q->next=p;
    return head;
}

int pop(linklist *head) //Pop the last element of the linklist
{
	Node *p,*q;
    p=head->next;
    q=head;
    if(p==NULL) return -1; //Empty linklist
    while(p->next!=NULL)
    {
		q=p;
        p=p->next;
    }
	q->next=p->next;
    return p->number;
}

void insert(linklist *loophead, linklist *stackhead, int m)
{
	Node *p;
    p=loophead->next;
	while(p!=NULL&&p->number!=m)
	{
        p=p->next;
	}
	if(p==NULL) //if m is not in loop
	{
		push(loophead, m); //add m into loop set
		push(stackhead, m); //push m onto stack
	}
}

linklist *loop(int N, int D, int **edge, int n) //Construct a loop given a back edge N->D, n=number of nodes (dimension of edge[][])
{
	int m;
	linklist *stackhead, *loophead;
	stackhead=(linklist*)malloc(sizeof(Node));
	stackhead->next=NULL;
	loophead=(linklist*)malloc(sizeof(Node));
	loophead->next=NULL;
	push(loophead, D);
	insert(loophead, stackhead, N);
	while(stackhead->next!=NULL)
	{
		m=pop(stackhead);
		for(int i=0;i<n;i++)
		{
			if(edge[i][m]==1)
			{
				insert(loophead, stackhead, i); //for each predecessor p of m, insert(p)
			}
		}
	}
	return loophead;
}

void outputloop(int **edge, int **backedge, int n)
{
	int i, j, num=0;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			if(backedge[i][j]==1)
			{
				linklist *loophead, *p;
				loophead=loop(i, j, edge, n);
				p=loophead->next;
				printf("LOOP %d:", num);
				while(p!=NULL)
				{
					printf(" BB%d", p->number);
					p=p->next;
				}
				printf("\n");
				num++;
			}
		}
	}
}
*/
///////////////////////START///////////////////////////
int *blockcount(int **edgecount, int n) //Count frequencies of each basic block
{
	int *BLOCKCOUNT;
	BLOCKCOUNT=createarray(n);
	int i, j;
	if (n == 0) BLOCKCOUNT[0] = 1;
	else
	  for(i=0;i<n;i++)
	  {
		  BLOCKCOUNT[i]=0; //Array is created as array[n]=1
		  for(j=0;j<n;j++)
		  {
			  BLOCKCOUNT[i]+=edgecount[j][i]; //Add up iterations of edge ending with i (entry of block i)
		  }
	  }
	return BLOCKCOUNT;
}

void loopcount(int **edgecount, int **backedge, int n)
{
	int i, j, num = 0;
	for(i = 0; i < n; i++)
	  for(j = 0; j < n; j++)
		{
			if(backedge[i][j] && edgecount[i][j])
			{
				outputfile << "9999" << endl << edgecount[i][j] << endl;
				num++;
			}
		}
}
////////////////////////END/////////////////////////////////

int main(int argc, char *argv[])
{
	
	int n;

////////////////////////START/////////////////////////////	
	int **EDGE, **EDGECOUNT, *BLOCKCOUNT, **BACKEDGE;
	string s;

	inputfilename = argv[1];

	ifstream inputfile1 (IF1);
	ifstream inputfile2 (IF2);

	if (!inputfile1.is_open())
	{
		cout << "Cannot open the file" << IF1 << "!" << endl;
		return 0;
	}

	if (!inputfile2.is_open())
	{
		cout << "Cannot open the file" << IF2 << "!" << endl;
		return 0;
	}

	getline (inputfile2, s);
	while (s.compare("BASIC BLOCK NUMBER:") != 0) getline (inputfile2, s);
	inputfile2 >> n;

	EDGE = creatematrix(n);
	getline (inputfile2, s);
	while (s.compare("EDGE MATRIX:") != 0) getline (inputfile2, s);
	for (int i = 0; i < n; i++)
	  for (int j = 0; j < n; j++)
		inputfile2 >> EDGE[i][j];

	EDGECOUNT = creatematrix(n);
	getline (inputfile1, s);
	while (s.compare("1111") != 0)
	{
		outputfile << s << endl;
		getline (inputfile1, s);
	}
	
	for (int i = 0; i < n; i++)
	  for (int j = 0; j < n; j++)
		if (EDGE[i][j]) inputfile1 >> EDGECOUNT[i][j];
	
	BLOCKCOUNT=blockcount(EDGECOUNT,n);
	
	BACKEDGE=creatematrix(n);
	getline (inputfile2, s);
	while (s.compare("BACKEDGE MATRIX:")) getline (inputfile2, s);
	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			inputfile2 >> BACKEDGE[i][j];
	
	
	outputfile << "1111" << endl;//Start counting basic block frequency
	for(int i = 0; i < n; i++)
		outputfile << BLOCKCOUNT[i] << endl;
	
	outputfile << "5555" << endl; //Start counting edge frequency
	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
		{
		  if(EDGE[i][j])
			outputfile << EDGECOUNT[i][j] << endl;
		}
	
	loopcount(EDGECOUNT,BACKEDGE,n);  //Start counting loop frequency
	
	inputfile1.close();
	inputfile2.close();
	outputfile.close();
}
