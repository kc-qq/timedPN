#include<stdio.h>
#include <cstdlib>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> 


#define Len_node         sizeof(struct Node)
#define  place_num 	SMatrix_C.mu  //�������� 
#define  transition_num    SMatrix_C.nu  //��Ǩ����
#define MaxRow  200        //�������������

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
	int cover_mark; //���ǵı�ʶ 
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
