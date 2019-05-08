#include<stdio.h>
#include <cstdlib>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> 



#define MaxRow  200        //矩阵最多行列数
#define filenamesize   200    //文件名的字符串的大小 



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
	
}



// 初始化M(CrossList类型的变量必须初始化,否则创建、复制矩阵将出错) 
int InitSMatrix(CrossList *M)
{
    (*M).rhead=(*M).chead=NULL;
    (*M).mu=(*M).nu=(*M).tu=0;
    return 1;
}


int CreateSMatrix(CrossList *M, char filename[filenamesize])
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

