#include<stdio.h>
#include <cstdlib>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> 


#define Len_node         sizeof(struct Node)
#define  place_num 	SMatrix_C.mu  //库所数量 
#define  transition_num    SMatrix_C.nu  //变迁数量
#define MaxRow  200        //矩阵最多行列数

//可达树问题 
struct Node
{
	int new_m_num;//新标识的下标
	int new_m[MaxRow];//新标识的具体标识
	int new_m_flag[MaxRow];//新标识的标志位 
	int new_m_come;//新标识的标识来源
	int new_m_transition;//新标识的变迁来源
	struct Node *next_open;//下一个要扩展的标识
	struct Node *next;//下一个结点地址
	int cover;  //是否覆盖 
	int cover_mark; //覆盖的标识 
	int old;//旧的标识
	int old_mark;//旧的标识的下标
};

struct  Node *head_Node,*former,*latter,*open_end;//输出表的动态扩展 
int C[MaxRow][MaxRow],M0[MaxRow],C_pre[MaxRow][MaxRow],C_post[MaxRow][MaxRow];
int Transition,num_Node;
int come=0;//当前扩展的标识下标 

void Initialize(); //读取初始标识 ,建立动态链表的第一个结点 
int TransFireCondition(int new_m[]);  //判断在标识new_m下，变迁t是否激发
int Createnew_m(int m[],CrossList SMatrix_C,int t,int new_m[]  );  //在标识m下，生成新的标识new_m
int new_m_new_or_old(void);//判断是新的还是旧的 
void add_to_only_new(int is_new);//新的加入一些标记 
void product_new_m(int new_m[],int new_m_flag[]);//产生新的标识 
void add_to_node(int new_m[],int new_m_flag[]);//标识加入链表 
void out_put(void);//输出结果 
