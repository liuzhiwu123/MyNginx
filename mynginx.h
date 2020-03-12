#include "pch.h"
#include<memory>
#include<iostream>
using namespace std;

typedef struct Ngx_Pool_t pool_t;

#define  ngx_pagesize 4096//һ������ҳ��Ĵ�С��4k
const int NGX_MAX_ALLOC_FROM_POOL = ngx_pagesize - 1;//����������С���ڴ棬Ҳ�Ǵ�С���ڴ�Ľ��ޡ�����һ������ҳ����ܿռ䲻���������ָ��ƫ��Ч�ʵ���

const int NGX_DEFAULT_POOL_SIZE = 16 * 1024;//Ĭ���ڴ�ؿ��ٴ�С16k��4������ҳ��

const int NGX_POOL_ALIGNMENT = 16;//���ٵ��ڴ水16�ֽڽ����ڴ����
#define NGX_ALIGNMENT   sizeof(unsigned long)//�ڴ��������Ŀռ䰴ƽ̨�ֽڶ���  32λ��4

//С���ڴ�ص�ͷ��������Ϣ
struct Ngx_Head
{
	char* last;
	char* end;
	int failed;
	pool_t* next;
};
//����ڴ����ڣ�Ҳ�Ǵ���ڴ�����
typedef struct Large_Pool
{
	void* alloc;
	struct Large_Pool* next_large;
}Large;
//�����ⲿ��Դ����������ֹ�ڴ�й©
typedef void(*clean_fun)(void* p);
//�ڴ�ص������������
typedef struct Clean_add
{
	clean_fun handler;
	void* cleandata;
	struct Clean_add*clean_next;
}clean_add;
//�����ڴ�صĿ�����Ϣ
struct Ngx_Pool_t
{
	Ngx_Head ngxhead;
	int max;
	pool_t* current;
	Large* large;
	clean_add* clean;
};

#define ngx_align(d, a)   (((d) + (a - 1)) & ~(a - 1))//��a�������������ֽڶ���
const int NGX_MIN_POOL_SIZE = ngx_align((sizeof(pool_t) + 2 * sizeof(Large)), NGX_POOL_ALIGNMENT);//�������С�ռ䣬����֤�ڴ�ص�ͷ����Ϣ����

#define ngx_align_ptr(p, a)  (char *) (((int) (p) + ((int) a - 1)) & ~((int) a - 1))//��p������a������������p

class myngnix_pool
{
public:
	//�����ڴ�أ�����size-sizeof(pool_t)��С�ķֽ��ޣ��Ҳ�����һ��ҳ�棬��С��NGX_POOL_ALIGNMENT�ֽڶ���
	pool_t* CreatPool_t(int size);
	//�ڴ�����size��С�������ڴ���뷽ʽ���롣�����ص��׵�ַ��ƽ̨��֧�ֵ�������
	void* ngxpalloc(int size);
	//�ڴ����룬���ڴ��ǲ����뷽ʽ���룬�����Ǹ����ϴε�λ�ÿ���
	void* ngx_pnalloc(int size);
	//�ڴ����룬�ҿռ��ʼ��Ϊ0��ֱ��malloc��
	void* ngx_pcalloc(int size);
	//�ͷŴ���ڴ�
	void ngxpfree(void* ptr);
	//�ڴ����ú���,���С���ڴ�������ͷ�
	void reset();
	//��ӻص������������ͷ��ⲿ��Դ��С���ڴ�Ҳ���ֽڶ��뷽ʽ���롣size�����������ⲿ��Դ�Ĵ�С��Ȼ���ͷ�ʱ���԰��ⲿ��Դ�ͷ����������������ͷ�
	clean_add* cleanadd(int size);//ע�⣺���˴��Ĳ������ڴ��Ǵ���ڴ棬��ngxpfree,��destory������������ͬ���ڴ����ͷ���ɱ���
	//�ڴ�����ٺ���
	void destory();
	//��ӡ��Ϣ���Լ������õ�
	void prifailed();
private:
	pool_t* pool;
	//����С���ڴ�أ�Ҳ�ǰ�NGX_POOL_ALIGNMENT�����������٣����صĿռ��׵�ַ������ƽ̨��֧�ֵ�����������
	void* blockpalloc(int size);
	//����С���ڴ棬�ֶ����벻���뿪��
	void* smallalloc(int size, int flag);
	//�������ڴ棬���Ҫ����С���ڴ���large����Ҳ���ڴ���뷽ʽ����С���ڴ�
	void* largepalloc(int size);
};
