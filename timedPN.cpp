#include<stdio.h>
#include <cstdlib>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> 


#define MaxRow  200        //矩阵最多行列数
#define Len_node         sizeof(struct Node)
#define filenamesize   200    //文件名的字符串的大小 
#define  place_num 	SMatrix_C.mu  //库所数量 
#define  transition_num    SMatrix_C.nu  //变迁数量 


//稀疏矩阵问题 
typedef int ElemType; 

typedef struct OLNode
{
    int i,j;    // 该非零元的行和列下标 
    ElemType e;    // 非零元素值 
    struct OLNode *right,*down; // 该非零元所在行表和列表的后继链域 
}OLNode, *OLink;


typedef struct// 行和列链表头指针向量基址,由CreatSMatrix_OL()分配
{
    OLink *rhead, *chead; 
    int mu, nu, tu;        // 稀疏矩阵的行数、列数和非零元个数 
}CrossList;


int InitSMatrix(CrossList *M);    // 初始化M(CrossList类型的变量必须初始化,否则创建、复制矩阵将出错) 
int DestroySMatrix(CrossList *M);    // 销毁稀疏矩阵M
int CreateSMatrix(CrossList *M,char filename[filenamesize]);     // 采用十字链表存储表示
int PrintSMatrix(CrossList M);  // 按行或按列输出稀疏矩阵M
int SubtSMatrix(CrossList M,CrossList N,CrossList *Q); //  求稀疏矩阵的差Q=M-N 
CrossList SMatrix_C_pre,SMatrix_C_post;
CrossList SMatrix_C;


//可达树问题 
struct Node
{
	int new_m_num;//新标识的下标
	int new_m[MaxRow];//新标识的具体标识
	int new_m_x[MaxRow]; // 此时刻库所中托肯已经等待时间
	int new_m_g; //已耗费的时间代价
	int new_m_come;//新标识的标识来源
	int new_m_transition;//新标识的变迁来源
	struct Node *next_open;//下一个要扩展的标识
	struct Node *next;//下一个结点地址
	int old; //旧的标识
	int old_mark;//旧的标识的下标
};


struct  Node *head_Node,*former,*latter,*open_end;//输出表的动态扩展 
int C[MaxRow][MaxRow],M0[MaxRow],Delay[MaxRow],C_pre[MaxRow][MaxRow],C_post[MaxRow][MaxRow];
int Transition,num_Node;
int come=0;//当前扩展的标识下标 

void Initialize(); //读取初始标识 ,建立动态链表的第一个结点 
int TransFireCondition(int new_m[]);  //判断在标识new_m下，变迁t是否激发
int Createnew_m(int m[],CrossList SMatrix_C,int t,int new_m[]  );  //在标识m下，生成新的标识new_m
int new_m_new_or_old(void);//判断是新的还是旧的 
void add_to_only_new(int is_new);//新的加入一些标记 
void product_new_m(int new_m[],int new_m_x[], int new_m_g);//产生新的标识 
void add_to_node(int new_m[],int new_m_x[], int new_m_g);//标识加入链表 
void out_put(void);//输出结果 
void out_save(void);//保存结果 



int main()
{
	

	InitSMatrix(&SMatrix_C_pre);   // CrossList类型的变量在初次使用之前必须初始化 
    printf("创建前置关联矩阵SMatrix_C_pre: ");
    CreateSMatrix(&SMatrix_C_pre,"C_pref.txt");
    PrintSMatrix(SMatrix_C_pre);  // C_pre的十字链表
    
    printf("\n");
    
    
	InitSMatrix(&SMatrix_C_post);
	printf("创建后置关联矩阵SMatrix_C_post: ");
    CreateSMatrix(&SMatrix_C_post,"C_postf.txt");
    PrintSMatrix(SMatrix_C_post);    // C_post的十字链表

	printf("\n");
	

	SubtSMatrix(SMatrix_C_post,SMatrix_C_pre,&SMatrix_C);
	printf("关联矩阵SMatrix_C: ");
	PrintSMatrix(SMatrix_C);   // C的十字链表
    
	printf("\n");
	
	
	
	
	Initialize();
	int j;
	int new_mark[place_num]; //当前标识
	int new_mark_x[place_num];//当前标识,库所中托肯已经等待时间
	int new_mark_g; // 当前标识下已耗费的时间 
	struct Node *q;
	q=head_Node;
	do
	{
		for(j=0;j<place_num;j++)
		{
			new_mark[j]=q->new_m[j];//将新表中第i行的标识赋值给当前的标识
			new_mark_x[j]=q->new_m_x[j];
	    }
			new_mark_g=q->new_m_g;
			come=q->new_m_num;
			product_new_m(new_mark,new_mark_x,new_mark_g);
			q=q->next_open;
	}while(q!=NULL);
	former->next=NULL;
	out_put();
	out_save(); 
	printf("结束");
	
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
    //初始化行链表头
    (*M).rhead=(OLink*)malloc((MaxRow)*sizeof(OLink));
    if(!(*M).rhead)
        exit(0);
    //初始化列链表头
    (*M).chead=(OLink*)malloc((MaxRow)*sizeof(OLink));
    if(!(*M).chead)
        exit(0);
    for(k=0;k<MaxRow;k++) // 初始化行头指针向量;各行链表为空链表 
        (*M).rhead[k]=NULL;
    for(k=0;k<MaxRow;k++) // 初始化列头指针向量;各列链表为空链表 
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
         
            // 分割每行
         
            char *pch = strtok (line, " ");
 
            while (pch != NULL)
            {
                int val = atoi(pch);
                 
                // 存储数据
                buf[irow][icol] = val;
                if(val!=0)
                {
                	p=(OLNode*)malloc(sizeof(OLNode));
        			if(!p)
            		   exit(0);
        			p->i=irow; // 生成结点 
			        p->j=icol;
			        p->e=val;
			        if((*M).rhead[irow]==NULL||(*M).rhead[irow]->j>icol)    
			        {
			            // p插在该行的第一个结点处
			            p->right=(*M).rhead[irow];
			            (*M).rhead[irow]=p;
			        }
			        else // 寻查在行表中的插入位置 
			        {
			            //从该行的行链表头开始，直到找到
			            for(q=(*M).rhead[irow]; q->right && q->right->j < icol;q = q->right)
			                ;
			            p->right=q->right; // 完成行插入 
			            q->right=p;
			        }
			        if((*M).chead[icol] == NULL || (*M).chead[icol]->i > irow) 
			        {
			            // p插在该列的第一个结点处
			            p->down = (*M).chead[icol];
			            (*M).chead[icol] = p;
			        }
			        else // 寻查在列表中的插入位置 
			        {
			            for(q = (*M).chead[icol];q->down && q->down->i < irow;q = q->down)
			                ;
			            p->down=q->down; // 完成列插入 
			            q->down=p;
			        }
					NotZeroNum++;		                                        
				}
 				
 				icol++; 
                // 继续分割
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
    // 输出数据 irow记录实际行数，colNum记录实际列数
    for (int i=0; i<irow; i++)
    {
        for (int j=0; j<colNum; j++)
        {
            printf("%3d ", buf[i][j]);
        }
 
        printf("\n");
    }
     
}


// 初始化M(CrossList类型的变量必须初始化,否则创建、复制矩阵将出错) 
int InitSMatrix(CrossList *M)
{
    (*M).rhead=(*M).chead=NULL;
    (*M).mu=(*M).nu=(*M).tu=0;
    return 1;
}



// 销毁稀疏矩阵M
int DestroySMatrix(CrossList *M)
{
    int i;
    OLNode *p,*q;
    
    for(i=1;i<=(*M).mu;i++) // 按行释放结点 
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



// 按行或按列输出稀疏矩阵M
int PrintSMatrix(CrossList M)
{
    int i,j;
    OLink p;
    printf("%d行%d列%d个非零元素\n",M.mu,M.nu,M.tu);
    printf("请输入选择(1.按行输出 2.按列输出): ");
    scanf("%d",&i);
    switch(i)
    {
    case 1: 
        for(j=0;j<M.mu;j++)
        {
            p=M.rhead[j];
            while(p)
            {
                printf("%d行%d列值为%d\n",(p->i)+1,(p->j)+1,p->e);
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
                printf("%d行%d列值为%d\n",(p->i)+1,(p->j)+1,p->e);
                p=p->down;
            }
        }
    }
    return 1;
}




//  求稀疏矩阵的差Q=M-N 
int SubtSMatrix(CrossList M,CrossList N,CrossList *Q)
{
    int i,k;
    OLink p,pq,pm,pn;
    OLink *col;
    
    if(M.mu!=N.mu||M.nu!=N.nu)
    {
        printf("两个矩阵不是同类型的,不能相加\n");
        exit(0);
    }
    (*Q).mu=M.mu; // 初始化Q矩阵 
    (*Q).nu=M.nu;
    (*Q).tu=0; // 元素个数的初值 
    (*Q).rhead=(OLink*)malloc(((*Q).mu)*sizeof(OLink));
    if(!(*Q).rhead)
        exit(0);
    (*Q).chead=(OLink*)malloc(((*Q).nu)*sizeof(OLink));
    if(!(*Q).chead)
        exit(0);
    for(k=0;k<(*Q).mu;k++) // 初始化Q的行头指针向量;各行链表为空链表 
        (*Q).rhead[k]=NULL;
    for(k=0;k<(*Q).nu;k++) // 初始化Q的列头指针向量;各列链表为空链表 
        (*Q).chead[k]=NULL;
    // 生成指向列的最后结点的数组
    col=(OLink*)malloc(((*Q).nu+1)*sizeof(OLink)); 
    if(!col)
        exit(0);
    for(k=0;k<(*Q).nu;k++) // 赋初值 
        col[k]=NULL;
    for(i=0;i<M.mu;i++) // 按行的顺序相加 
    {
        pm=M.rhead[i]; // pm指向矩阵M的第i行的第1个结点 
        pn=N.rhead[i]; // pn指向矩阵N的第i行的第1个结点 
        while(pm&&pn) // pm和pn均不空 
        {
            if(pm->j<pn->j) // 矩阵M当前结点的列小于矩阵N当前结点的列 
            {
                p=(OLink)malloc(sizeof(OLNode)); // 生成矩阵Q的结点 
                if(!p)
                    exit(0);
                (*Q).tu++; // 非零元素数加1 
                p->i=i; // 给结点赋值 
                p->j=pm->j;
                p->e=pm->e;
                p->right=NULL;
                pm=pm->right; // pm指针向右移 
            }
            // 矩阵M当前结点的列大于矩阵N当前结点的列 
            else if(pm->j>pn->j) 
            {
                p=(OLink)malloc(sizeof(OLNode)); // 生成矩阵Q的结点 
                if(!p)
                    exit(0);
                (*Q).tu++; // 非零元素数加1 
                p->i=i; // 给结点赋值 
                p->j=pn->j;
                p->e=-pn->e;
                p->right=NULL;
                pn=pn->right; // pn指针向右移 
            }
            else if(pm->e-pn->e) 
            {
                // 矩阵M、N当前结点的列相等且两元素之差不为0 
                p=(OLink)malloc(sizeof(OLNode)); // 生成矩阵Q的结点 
                if(!p)
                    exit(0);
                (*Q).tu++; // 非零元素数加1 
                p->i=i; // 给结点赋值 
                p->j=pn->j;
                p->e=pm->e-pn->e;
                p->right=NULL;
                pm=pm->right; // pm指针向右移 
                pn=pn->right; // pn指针向右移 
            }
            else // 矩阵M、N当前结点的列相等且两元素之差为0 
            {
                pm=pm->right; // pm指针向右移 
                pn=pn->right; // pn指针向右移 
                continue;
            }
            if((*Q).rhead[i]==NULL) // p为该行的第1个结点 
                // p插在该行的表头且pq指向p(该行的最后一个结点)
                (*Q).rhead[i]=pq=p;  
            else // 插在pq所指结点之后 
            {
                pq->right=p; // 完成行插入 
                pq=pq->right; // pq指向该行的最后一个结点 
            }
            if((*Q).chead[p->j]==NULL) // p为该列的第1个结点 
                // p插在该列的表头且col[p->j]指向p
                (*Q).chead[p->j]=col[p->j]=p;  
            else // 插在col[p->]所指结点之后 
            {
                col[p->j]->down=p; // 完成列插入
                // col[p->j]指向该列的最后一个结点 
                col[p->j]=col[p->j]->down;  
            }
        }
        while(pm) // 将矩阵M该行的剩余元素插入矩阵Q 
        {
            p=(OLink)malloc(sizeof(OLNode)); // 生成矩阵Q的结点 
            if(!p)
                exit(0);
            (*Q).tu++; // 非零元素数加1 
            p->i=i; // 给结点赋值 
            p->j=pm->j;
            p->e=pm->e;
            p->right=NULL;
            pm=pm->right; // pm指针向右移 
            if((*Q).rhead[i]==NULL) // p为该行的第1个结点 
                // p插在该行的表头且pq指向p(该行的最后一个结点)
                (*Q).rhead[i]=pq=p;  
            else // 插在pq所指结点之后 
            {
                pq->right=p; // 完成行插入 
                pq=pq->right; // pq指向该行的最后一个结点 
            }
            if((*Q).chead[p->j]==NULL) // p为该列的第1个结点
                // p插在该列的表头且col[p->j]指向p  
                (*Q).chead[p->j]=col[p->j]=p; 
            else // 插在col[p->j]所指结点之后 
            {
                col[p->j]->down=p; // 完成列插入
                // col[p->j]指向该列的最后一个结点 
                col[p->j]=col[p->j]->down;  
            }
        }
        while(pn) // 将矩阵N该行的剩余元素插入矩阵Q 
        {
            p=(OLink)malloc(sizeof(OLNode)); // 生成矩阵Q的结点 
            if(!p)
                exit(0);
            (*Q).tu++; // 非零元素数加1 
            p->i=i; // 给结点赋值 
            p->j=pn->j;
            p->e=-pn->e;
            p->right=NULL;
            pn=pn->right; // pm指针向右移 
            if((*Q).rhead[i]==NULL) // p为该行的第1个结点 
                // p插在该行的表头且pq指向p(该行的最后一个结点)
                (*Q).rhead[i]=pq=p;  
            else // 插在pq所指结点之后 
            {
                pq->right=p; // 完成行插入 
                pq=pq->right; // pq指向该行的最后一个结点 
            }
            if((*Q).chead[p->j]==NULL) // p为该列的第1个结点
                // p插在该列的表头且col[p->j]指向p 
                (*Q).chead[p->j]=col[p->j]=p;  
            else // 插在col[p->j]所指结点之后 
            {
                col[p->j]->down=p; // 完成列插入
                // col[p->j]指向该列的最后一个结点  
                col[p->j]=col[p->j]->down; 
            }
        }
    }
       for(k=0;k<(*Q).nu;k++)
           if(col[k]) // k列有结点 
               col[k]->down=NULL; // 令该列最后一个结点的down指针为空 
       free(col);
       return 1;
} 




//读取初始标识 ,建立动态链表的第一个结点 
void Initialize()
{
	int i,j;
	FILE *fp;
	fp=fopen("m0.txt","r"); //读取初始标识 
	if(fp==NULL)
	{
		printf("cannot open file\n");
	}
	for(i=0;i<place_num;i++)
	{
		fscanf(fp,"%d",&M0[i]);
	}
	fclose(fp);

	printf("初始标识：\n"); //输出初始 
	for(i=0;i<place_num;i++)
	{
		printf("%3d",M0[i]);
	}
	printf("\n");
	
	fp=fopen("delay.txt","r"); //读取库所上赋时时间 
	if(fp==NULL)
	{
		printf("cannot open file\n");
	}
	for(i=0;i<place_num;i++)
	{
		fscanf(fp,"%d",&Delay[i]);
	}
	fclose(fp);

	printf("库所赋时时间：\n"); //输出赋时时间 
	for(i=0;i<place_num;i++)
	{
		printf("%3d",Delay[i]);
	}
	printf("\n");
	
	//建立动态链表的第一个结点 
	head_Node=former=latter=(struct Node *)malloc(Len_node);
	num_Node=0;
    //初始化第一个结点的数据 
	for(i=0;i<place_num;i++)//初始化第一个结点的具体标识 
	{
		former->new_m[i]=M0[i];
	}
	for(i=0;i<place_num;i++)//初始化第一个结点的具体标识的等待时间 
	{
		former->new_m_x[i]=0;
	}
	former->new_m_num=0;
	former->new_m_g=0;
	former->new_m_come=0;
	former->new_m_transition=0;
	former->next=NULL;
	former->next_open=NULL; 
	former->old=0;
	former->old_mark=0;
	open_end=former;   
    
}




//产生新的标识 
void product_new_m(int new_m[],int new_m_x[], int new_m_g)
{
	
	for(Transition=0;Transition<transition_num;Transition++)//循环判断可发生的变迁 
	{
		int can_transition=0;
		can_transition=TransFireCondition(new_m);
		if(can_transition==1)
		{
			add_to_node(new_m, new_m_x, new_m_g);//进入公示计算和其他标志位赋值 
		}
	}
}




//判断在标识m下，变迁t是否激发
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




//生成新的标识new_m
void add_to_node(int new_m[],int new_m_x[], int new_m_g)
{
	int T_num,i,is_new;
	int m[place_num];
	int x[place_num];
	int ans,step,g;  //step表示当前标识到下一标识的损耗时间 
	step=0;
	OLink P,Q;
	P=SMatrix_C.chead[Transition];
	Q=SMatrix_C_pre.chead[Transition];
	for(i=0;i<place_num;i++)
	{
		m[i]=new_m[i];
		x[i]=new_m_x[i];
	}
		
	while(P!=NULL)//更新m 
	{
		m[P->i]=m[P->i]+(P->e);
		P=P->down;
	}	
	
	while(Q!=NULL) //更新step 
	{
		ans=Delay[Q->i]*(Q->e)-x[Q->i];
		if(ans > step)
			step=ans;
		Q=Q->down;
	}
	printf("%2d",step);
	g = new_m_g+step;
	for(i=0;i<place_num;i++) //更新结点处的x 
	{
		if(Delay[i]==0)
		{
			x[i] = 0;
		}
		else
		{
			if(m[i]==0)
			{
				x[i]=0;
			}
			else
			{
				if(new_m[i]==0)
				{
					x[i]=0;
				}
				else
				{
					x[i] = x[i] + step;
				}
			}
		}
	}
	T_num=Transition+1;
	num_Node++;//链表的节点数加一 
	former=(struct Node *)malloc(Len_node);//建立新的结点，其地址赋给former 
	latter->next=former;//将上一个结点的next指向新建的结点 
	latter=former;//使latter也指向新建的结点 
	for(i=0;i<place_num;i++)//给新建的结点赋值 
	{
		former->new_m[i]=m[i]; //产生的新标识放到输出表中 
		former->new_m_x[i]=x[i];
	}
//	for(i=0;i<place_num;i++)
//	{
//		former->new_m_flag[i]=new_m_flag[i]; 
//	}
	former->new_m_g=g;
	former->new_m_come=come;//标识的来源下标放到输出表中 
	former->new_m_transition=T_num;//标识激发的变迁的下标放到输出表中 
	former->new_m_num=num_Node;
	former->next_open=NULL;
	is_new=new_m_new_or_old();//判断是否是新的标识 
	add_to_only_new(is_new);//新的标识则需要添加 
}


int new_m_new_or_old(void)//判断是新的还是旧的 
{
	int i,j,Flag,same;
	same=1;
	struct Node *P;
	P=head_Node;
	do
	{
		for(i=0;i<place_num;i++)
		{
			if(former->new_m[i]==P->new_m[i]&&former->new_m_x[i]==P->new_m_x[i])//生成的新标识与链表的第一个结点的标识每一位判断是否相同（相同为真） 
			{
				same=1;
			}
			else
			{
				same=0;
				break;//标识与第i行不同则退出循环判断下一行 
			}
		}
		
		
		if(same==1&&former->new_m_g==P->new_m_g)
		{
			Flag=0;
			former->old=1;
			former->old_mark=P->new_m_num;
			break;//相同，则退出判断，并标记添加标志 
		}
		else
		{
			Flag=1;
		}
		P=P->next_open;
	}while(P!=NULL);
	if(Flag==1)
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
	if(is_new==1)//是新标识，则需要将扩展下一个新节点 
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
	fp=fopen("out8.txt","w+");
	printf("%s","输出结果");
	printf("\n");
	do
	{
		fprintf(fp,"m%2d的标识是",P->new_m_num);
		printf("m%2d的标识是",P->new_m_num);
		for(i=0;i<place_num;i++)
		{
			printf("%2d",P->new_m[i]);
			fprintf(fp,"%2d\t",P->new_m[i]);
		}
		
		printf("%s"," 已等待时间x是");
		fprintf(fp,"%s"," 已等待时间x是");
		for(i=0;i<place_num;i++)
		{
			printf("%2d",P->new_m_x[i]);
			fprintf(fp,"%2d\t",P->new_m_x[i]);
		}
		//for(i=0;i<place_num;i++)
		//{
		//	printf("%2d",P->new_m_flag[i]);
		//	fprintf(fp,"%2d\t",P->new_m_flag[i]);
		//}
		printf("%s"," 已耗费时间g为");
		fprintf(fp,"%s"," 已耗费时间g为");
		printf("%2d",P->new_m_g);
		fprintf(fp,"%2d\t",P->new_m_g);
		printf("%s"," 标识来源于M");
		fprintf(fp,"%s"," 标识来源于M");
		printf("%2d",P->new_m_come);
		fprintf(fp,"%2d\t",P->new_m_come);
		printf("%s"," 激发的变迁");
		fprintf(fp,"%s\t"," 激发的变迁");
		printf("%2d",P->new_m_transition);
		fprintf(fp,"%2d\t",P->new_m_transition);
		
		
		if(P->old==0)
		{
			printf("%s"," 它不是重复标识");
			fprintf(fp,"%s"," 它不是重复标识");
		}
		else
		{
			printf("%s"," 它的重复标识是");
			fprintf(fp,"%s"," 它的重复标识是");
			printf("%2d",P->old_mark);
			fprintf(fp,"%2d\t",P->old_mark);
		}
		fprintf(fp,"\n");
		printf("\n");
		P=P->next;
	}while(P!=NULL);
	fclose(fp);
}


void out_save(void)
{
	int i;
	int flag;
	FILE *fp;
	struct Node *P;
	P=head_Node;
	fp=fopen("save8.txt","w+");
	printf("%s","输出结果");
	printf("\n");
	do
	{
		for(i=0;i<place_num;i++)
		{
			fprintf(fp,"%2d\t",P->new_m[i]);
		}
		
		for(i=0;i<place_num;i++)
		{
			fprintf(fp,"%2d\t",P->new_m_x[i]);
		}
		//for(i=0;i<place_num;i++)
		//{
		//	printf("%2d",P->new_m_flag[i]);
		//	fprintf(fp,"%2d\t",P->new_m_flag[i]);
		//}
		fprintf(fp,"%2d\t",P->new_m_g);
		fprintf(fp,"%2d\t",P->new_m_come);
		fprintf(fp,"%2d\t",P->new_m_transition);
		
		
		if(P->old==0)
		{
			fprintf(fp,"%2d",-1);
		}
		else
		{
			fprintf(fp,"%2d",P->old_mark);
		}
		fprintf(fp,"\n");
		P=P->next;
	}while(P!=NULL);
	fclose(fp);
}



