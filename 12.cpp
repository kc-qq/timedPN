#include<stdio.h>
#include <cstdlib>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> 


#define MaxRow  200        //�������������
#define Len_node         sizeof(struct Node)
#define filenamesize   200    //�ļ������ַ����Ĵ�С 
#define  place_num 	SMatrix_C.mu  //�������� 
#define  transition_num    SMatrix_C.nu  //��Ǩ���� 


//ϡ��������� 
typedef int ElemType; 

typedef struct OLNode
{
    int i,j;    // �÷���Ԫ���к����±� 
    ElemType e;    // ����Ԫ��ֵ 
    struct OLNode *right,*down; // �÷���Ԫ�����б���б�ĺ������ 
}OLNode, *OLink;


typedef struct// �к�������ͷָ��������ַ,��CreatSMatrix_OL()����
{
    OLink *rhead, *chead; 
    int mu, nu, tu;        // ϡ�����������������ͷ���Ԫ���� 
}CrossList;

int InitSMatrix(CrossList *M);    // ��ʼ��M(CrossList���͵ı��������ʼ��,���򴴽������ƾ��󽫳���) 
int DestroySMatrix(CrossList *M);    // ����ϡ�����M
int CreateSMatrix(CrossList *M,char filename[filenamesize]);     // ����ʮ������洢��ʾ
int PrintSMatrix(CrossList M);  // ���л������ϡ�����M
int SubtSMatrix(CrossList M,CrossList N,CrossList *Q); //  ��ϡ�����Ĳ�Q=M-N 
CrossList SMatrix_C_pre,SMatrix_C_post;
CrossList SMatrix_C;




//�ɴ������� 
struct Node
{
	int new_m_num;//�±�ʶ���±�
	int new_m[MaxRow];//�±�ʶ�ľ����ʶ
	int new_m_flag[MaxRow];//�±�ʶ�ı�־λ 
	int new_m_come;//�±�ʶ�ı�ʶ��Դ
	int new_m_transition;//�±�ʶ�ı�Ǩ��Դ
	struct Node *next_open;//��һ��Ҫ��չ�ı�ʶ
	struct Node *next;//��һ������ַ
	int cover;  //�Ƿ񸲸� 
	int cover_mark; //���Ǳ�ʶ���±� 
	int old;//�ɵı�ʶ
	int old_mark;//�ɵı�ʶ���±�
};

struct  Node *head_Node,*former,*latter,*open_end;//�����Ķ�̬��չ 
int C[MaxRow][MaxRow],M0[MaxRow],C_pre[MaxRow][MaxRow],C_post[MaxRow][MaxRow];
int Transition,num_Node;
int come=0;//��ǰ��չ�ı�ʶ�±� 

void Initialize(); //��ȡ��ʼ��ʶ ,������̬����ĵ�һ����� 
int TransFireCondition(int new_m[]);  //�ж��ڱ�ʶnew_m�£���Ǩt�Ƿ񼤷�
int Createnew_m(int m[],CrossList SMatrix_C,int t,int new_m[]  );  //�ڱ�ʶm�£������µı�ʶnew_m
int new_m_new_or_old(void);//�ж����µĻ��Ǿɵ� 
void add_to_only_new(int is_new);//�µļ���һЩ��� 
void product_new_m(int new_m[],int new_m_flag[]);//�����µı�ʶ 
void add_to_node(int new_m[],int new_m_flag[]);//��ʶ�������� 
void out_put(void);//������ 


 


int main()
{
	

	InitSMatrix(&SMatrix_C_pre);   // CrossList���͵ı����ڳ���ʹ��֮ǰ�����ʼ�� 
    printf("����ǰ�ù�������SMatrix_C_pre: ");
    CreateSMatrix(&SMatrix_C_pre,"C_pref.txt");
    PrintSMatrix(SMatrix_C_pre);  // C_pre��ʮ������
    
    printf("\n");
    
    
	InitSMatrix(&SMatrix_C_post);
	printf("�������ù�������SMatrix_C_post: ");
    CreateSMatrix(&SMatrix_C_post,"C_postf.txt");
    PrintSMatrix(SMatrix_C_post);    // C_post��ʮ������

	printf("\n");
	

	SubtSMatrix(SMatrix_C_post,SMatrix_C_pre,&SMatrix_C);
	printf("��������SMatrix_C: ");
	PrintSMatrix(SMatrix_C);   // C��ʮ������
    
	printf("\n");
	
	
	
	
	Initialize();
	int j;
	int new_mark[place_num]; //��ǰ��ʶ  
	int new_mark_flag[place_num];//��ǰ��ʶ�ı�־λ 
	struct Node *q;
	q=head_Node;
	do
	{
		for(j=0;j<place_num;j++)
		{
			new_mark[j]=q->new_m[j];//���±��е�i�еı�ʶ��ֵ����ǰ�ı�ʶ 
			new_mark_flag[j] =q->new_m_flag[j];
	    }
			come=q->new_m_num;
			product_new_m(new_mark,new_mark_flag);
			q=q->next_open;
	}while(q!=NULL);
	former->next=NULL;
	out_put();
	printf("����");
	
	do
	{
		
	}
	while(1);
}




int CreateSMatrix(CrossList *M,char filename[filenamesize])
{
    int k;
    int NotZeroNum=0;
    ElemType e;
    OLNode *p,*q;
    if((*M).rhead)
    DestroySMatrix(M);
    //��ʼ��������ͷ
    (*M).rhead=(OLink*)malloc((MaxRow)*sizeof(OLink));
    if(!(*M).rhead)
        exit(0);
    //��ʼ��������ͷ
    (*M).chead=(OLink*)malloc((MaxRow)*sizeof(OLink));
    if(!(*M).chead)
        exit(0);
    for(k=0;k<MaxRow;k++) // ��ʼ����ͷָ������;��������Ϊ������ 
        (*M).rhead[k]=NULL;
    for(k=0;k<MaxRow;k++) // ��ʼ����ͷָ������;��������Ϊ������ 
        (*M).chead[k]=NULL;
    
    
    int buf[MaxRow][MaxRow];
    char line[100];
    FILE * pFile;
    int irow = 0;
    int icol = 0;
    int colNum = 0;
    pFile = fopen (filename,"r");
    if (pFile!=NULL)
    {
        while(1)
        {
            if (fgets (line, 100, pFile)==NULL)
                break;
         
            // �ָ�ÿ��
         
            char *pch = strtok (line, " ");
 
            while (pch != NULL)
            {
                int val = atoi(pch);
                 
                // �洢����
                buf[irow][icol] = val;
                if(val!=0)
                {
                	p=(OLNode*)malloc(sizeof(OLNode));
        			if(!p)
            		   exit(0);
        			p->i=irow; // ���ɽ�� 
			        p->j=icol;
			        p->e=val;
			        if((*M).rhead[irow]==NULL||(*M).rhead[irow]->j>icol)    
			        {
			            // p���ڸ��еĵ�һ����㴦
			            p->right=(*M).rhead[irow];
			            (*M).rhead[irow]=p;
			        }
			        else // Ѱ�����б��еĲ���λ�� 
			        {
			            //�Ӹ��е�������ͷ��ʼ��ֱ���ҵ�
			            for(q=(*M).rhead[irow]; q->right && q->right->j < icol;q = q->right)
			                ;
			            p->right=q->right; // ����в��� 
			            q->right=p;
			        }
			        if((*M).chead[icol] == NULL || (*M).chead[icol]->i > irow) 
			        {
			            // p���ڸ��еĵ�һ����㴦
			            p->down = (*M).chead[icol];
			            (*M).chead[icol] = p;
			        }
			        else // Ѱ�����б��еĲ���λ�� 
			        {
			            for(q = (*M).chead[icol];q->down && q->down->i < irow;q = q->down)
			                ;
			            p->down=q->down; // ����в��� 
			            q->down=p;
			        }
					NotZeroNum++;		                                        
				}
 				
 				icol++; 
                // �����ָ�
                pch = strtok (NULL, " ");
            }
 
            irow++;
            colNum = icol;
            icol = 0;
        }
        fclose (pFile);
    }
 	
	 
	(*M).mu=irow;
    (*M).nu=colNum;
    (*M).tu=NotZeroNum;
    //printf("%3d ",irow );
    //printf("%3d ",colNum );
    printf("\n");
    // ������� irow��¼ʵ��������colNum��¼ʵ������
    for (int i=0; i<irow; i++)
    {
        for (int j=0; j<colNum; j++)
        {
            printf("%3d ", buf[i][j]);
        }
 
        printf("\n");
    }
     
}


// ��ʼ��M(CrossList���͵ı��������ʼ��,���򴴽������ƾ��󽫳���) 
int InitSMatrix(CrossList *M)
{
    (*M).rhead=(*M).chead=NULL;
    (*M).mu=(*M).nu=(*M).tu=0;
    return 1;
}



// ����ϡ�����M
int DestroySMatrix(CrossList *M)
{
    int i;
    OLNode *p,*q;
    
    for(i=1;i<=(*M).mu;i++) // �����ͷŽ�� 
    {
        p=*((*M).rhead+i);
        while(p)
        {
            q=p;
            p=p->right;
            free(q);
        }
    }
    free((*M).rhead);
    free((*M).chead);
    (*M).rhead=(*M).chead=NULL;
    (*M).mu=(*M).nu=(*M).tu=0;
    return 1;
}



// ���л������ϡ�����M
int PrintSMatrix(CrossList M)
{
    int i,j;
    OLink p;
    printf("%d��%d��%d������Ԫ��\n",M.mu,M.nu,M.tu);
    printf("������ѡ��(1.������� 2.�������): ");
    scanf("%d",&i);
    switch(i)
    {
    case 1: 
        for(j=0;j<M.mu;j++)
        {
            p=M.rhead[j];
            while(p)
            {
                printf("%d��%d��ֵΪ%d\n",(p->i)+1,(p->j)+1,p->e);
                p=p->right;
            }
        }
        break;
    case 2: 
        for(j=0;j<M.nu;j++)
        {
            p=M.chead[j];
            while(p)
            {
                printf("%d��%d��ֵΪ%d\n",(p->i)+1,(p->j)+1,p->e);
                p=p->down;
            }
        }
    }
    return 1;
}




//  ��ϡ�����Ĳ�Q=M-N 
int SubtSMatrix(CrossList M,CrossList N,CrossList *Q)
{
    int i,k;
    OLink p,pq,pm,pn;
    OLink *col;
    
    if(M.mu!=N.mu||M.nu!=N.nu)
    {
        printf("����������ͬ���͵�,�������\n");
        exit(0);
    }
    (*Q).mu=M.mu; // ��ʼ��Q���� 
    (*Q).nu=M.nu;
    (*Q).tu=0; // Ԫ�ظ����ĳ�ֵ 
    (*Q).rhead=(OLink*)malloc(((*Q).mu)*sizeof(OLink));
    if(!(*Q).rhead)
        exit(0);
    (*Q).chead=(OLink*)malloc(((*Q).nu)*sizeof(OLink));
    if(!(*Q).chead)
        exit(0);
    for(k=0;k<(*Q).mu;k++) // ��ʼ��Q����ͷָ������;��������Ϊ������ 
        (*Q).rhead[k]=NULL;
    for(k=0;k<(*Q).nu;k++) // ��ʼ��Q����ͷָ������;��������Ϊ������ 
        (*Q).chead[k]=NULL;
    // ����ָ���е�����������
    col=(OLink*)malloc(((*Q).nu+1)*sizeof(OLink)); 
    if(!col)
        exit(0);
    for(k=0;k<(*Q).nu;k++) // ����ֵ 
        col[k]=NULL;
    for(i=0;i<M.mu;i++) // ���е�˳����� 
    {
        pm=M.rhead[i]; // pmָ�����M�ĵ�i�еĵ�1����� 
        pn=N.rhead[i]; // pnָ�����N�ĵ�i�еĵ�1����� 
        while(pm&&pn) // pm��pn������ 
        {
            if(pm->j<pn->j) // ����M��ǰ������С�ھ���N��ǰ������ 
            {
                p=(OLink)malloc(sizeof(OLNode)); // ���ɾ���Q�Ľ�� 
                if(!p)
                    exit(0);
                (*Q).tu++; // ����Ԫ������1 
                p->i=i; // ����㸳ֵ 
                p->j=pm->j;
                p->e=pm->e;
                p->right=NULL;
                pm=pm->right; // pmָ�������� 
            }
            // ����M��ǰ�����д��ھ���N��ǰ������ 
            else if(pm->j>pn->j) 
            {
                p=(OLink)malloc(sizeof(OLNode)); // ���ɾ���Q�Ľ�� 
                if(!p)
                    exit(0);
                (*Q).tu++; // ����Ԫ������1 
                p->i=i; // ����㸳ֵ 
                p->j=pn->j;
                p->e=-pn->e;
                p->right=NULL;
                pn=pn->right; // pnָ�������� 
            }
            else if(pm->e-pn->e) 
            {
                // ����M��N��ǰ�������������Ԫ��֮�Ϊ0 
                p=(OLink)malloc(sizeof(OLNode)); // ���ɾ���Q�Ľ�� 
                if(!p)
                    exit(0);
                (*Q).tu++; // ����Ԫ������1 
                p->i=i; // ����㸳ֵ 
                p->j=pn->j;
                p->e=pm->e-pn->e;
                p->right=NULL;
                pm=pm->right; // pmָ�������� 
                pn=pn->right; // pnָ�������� 
            }
            else // ����M��N��ǰ�������������Ԫ��֮��Ϊ0 
            {
                pm=pm->right; // pmָ�������� 
                pn=pn->right; // pnָ�������� 
                continue;
            }
            if((*Q).rhead[i]==NULL) // pΪ���еĵ�1����� 
                // p���ڸ��еı�ͷ��pqָ��p(���е����һ�����)
                (*Q).rhead[i]=pq=p;  
            else // ����pq��ָ���֮�� 
            {
                pq->right=p; // ����в��� 
                pq=pq->right; // pqָ����е����һ����� 
            }
            if((*Q).chead[p->j]==NULL) // pΪ���еĵ�1����� 
                // p���ڸ��еı�ͷ��col[p->j]ָ��p
                (*Q).chead[p->j]=col[p->j]=p;  
            else // ����col[p->]��ָ���֮�� 
            {
                col[p->j]->down=p; // ����в���
                // col[p->j]ָ����е����һ����� 
                col[p->j]=col[p->j]->down;  
            }
        }
        while(pm) // ������M���е�ʣ��Ԫ�ز������Q 
        {
            p=(OLink)malloc(sizeof(OLNode)); // ���ɾ���Q�Ľ�� 
            if(!p)
                exit(0);
            (*Q).tu++; // ����Ԫ������1 
            p->i=i; // ����㸳ֵ 
            p->j=pm->j;
            p->e=pm->e;
            p->right=NULL;
            pm=pm->right; // pmָ�������� 
            if((*Q).rhead[i]==NULL) // pΪ���еĵ�1����� 
                // p���ڸ��еı�ͷ��pqָ��p(���е����һ�����)
                (*Q).rhead[i]=pq=p;  
            else // ����pq��ָ���֮�� 
            {
                pq->right=p; // ����в��� 
                pq=pq->right; // pqָ����е����һ����� 
            }
            if((*Q).chead[p->j]==NULL) // pΪ���еĵ�1�����
                // p���ڸ��еı�ͷ��col[p->j]ָ��p  
                (*Q).chead[p->j]=col[p->j]=p; 
            else // ����col[p->j]��ָ���֮�� 
            {
                col[p->j]->down=p; // ����в���
                // col[p->j]ָ����е����һ����� 
                col[p->j]=col[p->j]->down;  
            }
        }
        while(pn) // ������N���е�ʣ��Ԫ�ز������Q 
        {
            p=(OLink)malloc(sizeof(OLNode)); // ���ɾ���Q�Ľ�� 
            if(!p)
                exit(0);
            (*Q).tu++; // ����Ԫ������1 
            p->i=i; // ����㸳ֵ 
            p->j=pn->j;
            p->e=-pn->e;
            p->right=NULL;
            pn=pn->right; // pmָ�������� 
            if((*Q).rhead[i]==NULL) // pΪ���еĵ�1����� 
                // p���ڸ��еı�ͷ��pqָ��p(���е����һ�����)
                (*Q).rhead[i]=pq=p;  
            else // ����pq��ָ���֮�� 
            {
                pq->right=p; // ����в��� 
                pq=pq->right; // pqָ����е����һ����� 
            }
            if((*Q).chead[p->j]==NULL) // pΪ���еĵ�1�����
                // p���ڸ��еı�ͷ��col[p->j]ָ��p 
                (*Q).chead[p->j]=col[p->j]=p;  
            else // ����col[p->j]��ָ���֮�� 
            {
                col[p->j]->down=p; // ����в���
                // col[p->j]ָ����е����һ�����  
                col[p->j]=col[p->j]->down; 
            }
        }
    }
       for(k=0;k<(*Q).nu;k++)
           if(col[k]) // k���н�� 
               col[k]->down=NULL; // ��������һ������downָ��Ϊ�� 
       free(col);
       return 1;
} 




//��ȡ��ʼ��ʶ ,������̬����ĵ�һ����� 
void Initialize()
{
	int i,j;
	FILE *fp;
	fp=fopen("m0.txt","r"); //��ȡ��ʼ��ʶ 
	if(fp==NULL)
	{
		printf("cannot open file\n");
	}
	for(i=0;i<place_num;i++)
	{
		fscanf(fp,"%d",&M0[i]);
	}
	fclose(fp);

	printf("��ʼ��ʶ��\n"); //�����ʼ 
	for(i=0;i<place_num;i++)
	{
		printf("%3d",M0[i]);
	}
	printf("\n");
	
	//������̬����ĵ�һ����� 
	head_Node=former=latter=(struct Node *)malloc(Len_node);
	num_Node=0;
    //��ʼ����һ���������� 
	for(i=0;i<place_num;i++)//��ʼ����һ�����ľ����ʶ 
	{
		former->new_m[i]=M0[i];
	}
	for(i=0;i<place_num;i++)//��ʼ����һ�����ľ����ʶ�ı�־λ 
	{
		former->new_m_flag[i]=0;
	}
	former->new_m_num=0;
	former->new_m_come=0;
	former->new_m_transition=0;
	former->next=NULL;
	former->next_open=NULL; 
	former->cover=0;
	former->old=0;
	former->cover_mark =0; 
	former->old_mark=0;
	open_end=former;    
    
}




//�����µı�ʶ 
void product_new_m(int new_m[],int new_m_flag[])
{
	
	for(Transition=0;Transition<transition_num;Transition++)//ѭ���жϿɷ����ı�Ǩ 
	{
		int can_transition=0;
		can_transition=TransFireCondition(new_m);
		if(can_transition==1)
		{
			add_to_node(new_m,new_m_flag);//���빫ʾ�����������־λ��ֵ 
		}
	}
}




//�ж��ڱ�ʶm�£���Ǩt�Ƿ񼤷�
int TransFireCondition(int new_m[])
{	
	int flag=1;
	int a;
	OLink p;
//	for(j=0;j<SMatrix_C_pre.nu;j++)
	p=SMatrix_C_pre.chead[Transition];
	while(p)
	{	
		a=p->e;
		if (new_m[p->i]> a || new_m[p->i]== a)
		{
		    p=p->down;
		}
		else
		{
			flag=0;
			break;
		}
	}
	if(flag==0)
	{
		
		return 0;
    }
	else
	{
		return 1;
	}	
}




//�����µı�ʶnew_m
void add_to_node(int new_m[],int new_m_flag[])
{
	int T_num,i,is_new;
	int m[place_num];
	OLink P;
	P=SMatrix_C.chead[Transition];
	for(i=0;i<place_num;i++)
	{
		m[i]=new_m[i];
	}
		
	while(P!=NULL)
	{
		m[P->i]=m[P->i]+(P->e);
		P=P->down;
	}
	T_num=Transition+1;
	num_Node++;//����Ľڵ�����һ 
	former=(struct Node *)malloc(Len_node);//�����µĽ�㣬���ַ����former 
	latter->next=former;//����һ������nextָ���½��Ľ�� 
	latter=former;//ʹlatterҲָ���½��Ľ�� 
	for(i=0;i<place_num;i++)//���½��Ľ�㸳ֵ 
	{
		former->new_m[i]=m[i]; //�������±�ʶ�ŵ�������� 
	}
	for(i=0;i<place_num;i++)
	{
		former->new_m_flag[i]=new_m_flag[i]; 
	}
	former->new_m_come=come;//��ʶ����Դ�±�ŵ�������� 
	former->new_m_transition=T_num;//��ʶ�����ı�Ǩ���±�ŵ�������� 
	former->new_m_num=num_Node;
	former->next_open=NULL;
	is_new=new_m_new_or_old();//�ж��Ƿ����µı�ʶ 
	add_to_only_new(is_new);//�µı�ʶ����Ҫ��� 
}



int new_m_new_or_old(void)//�ж����µĻ��Ǿɵ� 
{
	int i,j,Flag_1,Flag_2,same,cover;
	cover=1;
	same=1;
	int pnum,num;
	num=0;
	struct Node *P, *q;
	
	
//	q=head_Node;
//	while(q!=NULL)
//	{
//		
//		for(i=0;i<place_num;i++)
//		{
//			if(former->new_m[i]>=q->new_m[i])
//			{
//				cover=1;
//			}
//			else
//			{
//				cover=0;
//				break;
//			}
//		}
//		if(cover==1)
//		{
//			former->cover =1;
//			former->cover_mark=q->new_m_num;
//			for(i=0;i<place_num;i++)
//			{
//				if(former->new_m[i]>q->new_m[i])
//				{
//					former->new_m_flag[i]=1;
//				}	
//			}
//			break;
//					
//		}
//		else
//		{
//			former->cover =0;
//		}
//		q=q->next_open;
//	}
//
//
//
//	q=head_Node;
//	while(q!=NULL)
//	{
//		for(i=0;i<place_num;i++)
//		{
//			if(former->new_m[i]>=q->new_m[i])
//			{
//				cover=1;
//			}
//			else
//			{
//				cover=0;
//				break;
//			}
//		}
//		if(cover==1)
//		{	
//			num++;	
//		}
//		if(num>2)
//		{
//			Flag_2=1;
//			pnum=q->new_m_num;
//			break;
//		}
//		else
//		{
//			Flag_2=0;
//		}
//		q=q->next_open;
//	}
//	
//	
	
	
	
	P=head_Node;
	do
	{
		for(i=0;i<place_num;i++)
		{
			if(former->new_m[i]==P->new_m[i])//���ɵ��±�ʶ������ĵ�һ�����ı�ʶÿһλ�ж��Ƿ���ͬ����ͬΪ�棩 
			{
				same=1;
			}
			else
			{
				same=0;
				break;//��ʶ���i�в�ͬ���˳�ѭ���ж���һ�� 
			}
		}
		if(same==1)
		{
			Flag_1=0;
			former->old=1;
			former->old_mark=P->new_m_num;
			former->cover =0;
			former->cover_mark=0;
			break;//��ͬ�����˳��жϣ��������ӱ�־ 
		}
		else
		{
			q=head_Node;
			while(q!=NULL)
			{
				
				for(i=0;i<place_num;i++)
				{
					if(former->new_m[i]>=q->new_m[i])
					{
						cover=1;
					}
					else
					{
						cover=0;
						break;
					}
				}
				if(cover==1)
				{
					former->cover =1;
					former->cover_mark=q->new_m_num;
					for(i=0;i<place_num;i++)
					{
						if(former->new_m[i]>q->new_m[i])
						{
							former->new_m_flag[i]=1;
						}	
					}
					break;
							
				}
				else
				{
					former->cover =0;
				}
				q=q->next_open;
			}

			q=head_Node;
			num=0;
			while(q!=NULL)
			{
				for(i=0;i<place_num;i++)
				{
					if(former->new_m[i]>=q->new_m[i])
					{
						cover=1;
					}
					else
					{
						cover=0;
						break;
					}
				}
				if(cover==1)
				{	
					num++;	
				}
				if(num>2)
				{
					Flag_2=1;
					pnum=q->new_m_num;
					break;
				}
				else
				{
					Flag_2=0;
				}
				q=q->next_open;
			}
			
			if(Flag_2==1)
			{
				Flag_1=0;
				former->old=1;
				former->old_mark=pnum;
				break;
			}
			if(Flag_2==0)
			{
				Flag_1=1;
			}
		}
		P=P->next_open;
	}while(P!=NULL);
	if(Flag_1==1)
	{
		return 1;
	}
	else
	{
		former->next_open=NULL;
		return 0;
	}
}



void add_to_only_new(int is_new)
{
	if(is_new==1)//���±�ʶ������Ҫ����չ��һ���½ڵ� 
	{
		former->old=0;
		former->old_mark=0;
		open_end->next_open=former;
		open_end=former;
	}
}




void out_put(void)
{
	int i;
	int flag;
	FILE *fp;
	struct Node *P;
	P=head_Node;
	fp=fopen("out.txt","w+");
	printf("%s","������");
	printf("\n");
	do
	{
		fprintf(fp,"m%2d�ı�ʶ��",P->new_m_num);
		printf("m%2d�ı�ʶ��",P->new_m_num);
		for(i=0;i<place_num;i++)
		{
			if(P->new_m_flag[i]==0)
			{
				printf("%2d",P->new_m[i]);
				fprintf(fp,"%2d\t",P->new_m[i]);
			}
			else
			{
				printf("%s"," w");
				fprintf(fp,"%s"," w");
			}
		}
		//for(i=0;i<place_num;i++)
		//{
		//	printf("%2d",P->new_m_flag[i]);
		//	fprintf(fp,"%2d\t",P->new_m_flag[i]);
		//}
		printf("%s"," ��ʶ��Դ��M");
		fprintf(fp,"%s"," ��ʶ��Դ��M");
		printf("%2d",P->new_m_come);
		fprintf(fp,"%2d\t",P->new_m_come);
		printf("%s"," �����ı�Ǩ");
		fprintf(fp,"%s\t"," �����ı�Ǩ");
		printf("%2d",P->new_m_transition);
		fprintf(fp,"%2d\t",P->new_m_transition);
		
		
		if(P->old==0)
		{
			printf("%s"," �������ظ���ʶ");
			fprintf(fp,"%s"," �������ظ���ʶ");
		}
		else
		{
			printf("%s"," �����ظ���ʶ��");
			fprintf(fp,"%s"," �����ظ���ʶ��");
			printf("%2d",P->old_mark);
			fprintf(fp,"%2d\t",P->old_mark);
		}
		
		
		if(P->cover==0)
		{
			printf("%s"," �����Ǹ��Ǳ�ʶ");
			fprintf(fp,"%s"," �����Ǹ��Ǳ�ʶ");
		}
		else
		{
			printf("%s"," ���ĸ��Ǳ�ʶ��");
			fprintf(fp,"%s"," ���ĸ��Ǳ�ʶ��");
			printf("%2d",P->cover_mark);
			fprintf(fp,"%2d\t",P->cover_mark);
		}
		fprintf(fp,"\n");
		printf("\n");
		P=P->next;
	}while(P!=NULL);
	fclose(fp);
}



