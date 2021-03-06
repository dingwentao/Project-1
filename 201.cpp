#include <malloc.h>
#include <iostream>
#include <fstream>
#include <string>

#define IF "test3.mil"
#define OF "output"

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

void outputedge(int **edge, int n) //output edge given edge matrix[n][n], edge[i][j]=1 means there's an edge from BBi to BBj
{
	int i, j, num=0;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			if(edge[i][j]==1)
			{
				outputfile << "EDGE " << num <<": BB"<< i << " -> BB" << j << endl;
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
				outputfile << "DOM(" << i << ")=" << j << endl;
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
			outputfile << matrix[i][j] << "\t";
		}
		outputfile << endl;
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
				outputfile << "LOOP " << num << ":";
				while(p!=NULL)
				{
					outputfile << " BB" <<  p->number;
					p=p->next;
				}
				outputfile << endl;
				num++;
			}
		}
	}
}

int judgeflag(string s)
{
	int t = 0;
	if (s.compare(0, 3, ": L") == 0) t = 1;
	if (s.length() >= 3)
	  if (s.compare(1,2,":=") == 0) t = 1;
	if (s.length() >= 4)
	  if (s.compare(1,3,"?:=") == 0) t = 1;
	return t;
}
	 
int blocknumber ()
{
	int n = 0;
	string s1, s2;
	
	ifstream inputfile (IF);	
	getline (inputfile, s1);
	
	while (!inputfile.eof())
	{
		getline (inputfile,s2);
		if ((judgeflag (s1)) && !(judgeflag(s2))) n++;
		s1 = s2;
	}
	
	inputfile.close();

	return n+1;
}

int unconditionalbranch (string s)
{
	int i = 0;
	string s1, s2;
	ifstream inputfile (IF);
	getline (inputfile, s1);

	while (!inputfile.eof())
	{
		getline (inputfile,s2);
		if ((judgeflag (s1)) && !(judgeflag(s2))) i++;
		if (s1.compare(0,3,": L") == 0)
		  if (s1.compare(2, s1.length()-2, s, 4, s1.length()-2) == 0) return i;
		s1 = s2;
	}

	inputfile.close();
}

int conditionalbranch (string s)
{
	int i = 0;
	string s1, s2;
	ifstream inputfile (IF);
	getline (inputfile, s1);

	while (!inputfile.eof())
	{
		getline (inputfile,s2);
		if ((judgeflag (s1)) && !(judgeflag(s2))) i++;
		if (s1.compare(0,3,": L") == 0)
		  if (s1.compare(2, s1.length()-2, s, 5, s1.length()-2) == 0) return i;
		s1 = s2;
	}

	inputfile.close();
}

void initialization(int **edge, int n)
{
	int i, j;
	string s1, s2;

	for (i = 0; i < n; i++)
	  for (j = 0; j < n; j++)
		edge[i][j] = 0;

	ifstream inputfile (IF);
	getline (inputfile, s1);

	i = j = 0;
	while (!inputfile.eof())
	{
		getline (inputfile,s2);
		if ((judgeflag (s1)) && !(judgeflag(s2))) i++;
		if (!(judgeflag (s1)) && (judgeflag(s2)))
		  if (s2.compare(0,3,": L") == 0) edge[i][i+1] = 1;

		if (judgeflag (s1) && (s1.compare(0,3,": L") != 0))
		  if (s1.compare(1,2,":=") == 0)
		  {
			  j = unconditionalbranch (s1);
			  if (i!=j) edge[i][j] = 1;
		  }
		  else
		  {
			  j = conditionalbranch (s1);
			  if (i-1!=j) edge[i-1][j] = 1;
			  edge[i-1][i] = 1;
		  }

		s1 = s2;
	}

	inputfile.close();
}

void outputblock()
{

	int n = 0;
	string s1, s2;
	
	ifstream inputfile (IF);	
	getline (inputfile, s1);
	while (s1.compare(0, 7, ": START") != 0)
	  getline (inputfile,s1);

	outputfile << "BB0" << endl;
	while (!inputfile.eof())
	{
		getline (inputfile,s2);
		if ((judgeflag (s1)) && !(judgeflag(s2)))
		{
			n++;
			if (s1.compare(0,3,": L") == 0)
			{
				outputfile << "BB" << n << endl;
				outputfile << s1 << endl;
			}
			else
			{
				outputfile << s1 << endl;
				outputfile << "BB" << n << endl;
			}
		}
		else
		  outputfile << s1 << endl;
		s1 = s2;
	}
	
	inputfile.close();
}
int main()
{
	int **DOM, **EDGE, **BACKEDGE;
	int n;

	n = blocknumber();

	DOM=creatematrix(n);
	EDGE=creatematrix(n);
	BACKEDGE=creatematrix(n);
	
	initialization(EDGE, n);

	outputblock();
	
	outputfile << "===============\n";
	outputfile << "ALL EDGES:\n";
	outputedge(EDGE,n);

	DOM=dominator(EDGE,n);
//	outputfile << "===============\nALL DOM SETS:\n";
//	outputDOM(DOM,n);
	
	BACKEDGE=backedge(EDGE, DOM, n);
//	outputfile << "===============\nALL BACK EDGES:\n";
//	outputedge(BACKEDGE,n);
	
//	outputfile << "===============\nEDGE MATRIX:\n";
//	printmatrix(EDGE,n);
//	outputfile << "===============\nDOM MATRIX:\n";
//	printmatrix(DOM,n);
//	outputfile << "===============\nBACKEDGE MATRIX:\n";
//	printmatrix(BACKEDGE,n);
	
	outputfile << "===============\n";
	outputfile << "ALL LOOPS:\n";
	outputloop(EDGE, BACKEDGE, n);
	//system("pause");

	outputfile.close();
	return 0;
}
