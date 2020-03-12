#include "pch.h"
#include<memory>
#include<iostream>
using namespace std;

typedef struct Ngx_Pool_t pool_t;

#define  ngx_pagesize 4096//一个物理页面的大小，4k
const int NGX_MAX_ALLOC_FROM_POOL = ngx_pagesize - 1;//可申请的最大小块内存，也是大小块内存的界限。超过一个物理页面可能空间不连续，造成指针偏移效率低下

const int NGX_DEFAULT_POOL_SIZE = 16 * 1024;//默认内存池开辟大小16k，4个物理页面

const int NGX_POOL_ALIGNMENT = 16;//开辟的内存按16字节进行内存对齐
#define NGX_ALIGNMENT   sizeof(unsigned long)//内存池里申请的空间按平台字节对齐  32位是4

//小块内存池的头部控制信息
struct Ngx_Head
{
	char* last;
	char* end;
	int failed;
	pool_t* next;
};
//大块内存的入口，也是大块内存链表
typedef struct Large_Pool
{
	void* alloc;
	struct Large_Pool* next_large;
}Large;
//设置外部资源清理函数，防止内存泄漏
typedef void(*clean_fun)(void* p);
//内存池的清理操作链表
typedef struct Clean_add
{
	clean_fun handler;
	void* cleandata;
	struct Clean_add*clean_next;
}clean_add;
//整个内存池的控制信息
struct Ngx_Pool_t
{
	Ngx_Head ngxhead;
	int max;
	pool_t* current;
	Large* large;
	clean_add* clean;
};

#define ngx_align(d, a)   (((d) + (a - 1)) & ~(a - 1))//按a的整数倍进行字节对齐
const int NGX_MIN_POOL_SIZE = ngx_align((sizeof(pool_t) + 2 * sizeof(Large)), NGX_POOL_ALIGNMENT);//申请的最小空间，来保证内存池的头部信息完整

#define ngx_align_ptr(p, a)  (char *) (((int) (p) + ((int) a - 1)) & ~((int) a - 1))//把p调整到a的整数倍返回p

class myngnix_pool
{
public:
	//创建内存池，创建size-sizeof(pool_t)大小的分界限，且不超过一个页面，大小按NGX_POOL_ALIGNMENT字节对齐
	pool_t* CreatPool_t(int size);
	//内存申请size大小，且是内存对齐方式申请。即返回的首地址是平台所支持的整数倍
	void* ngxpalloc(int size);
	//内存申请，但内存是不对齐方式申请，即就是跟着上次的位置开辟
	void* ngx_pnalloc(int size);
	//内存申请，且空间初始化为0，直接malloc。
	void* ngx_pcalloc(int size);
	//释放大块内存
	void ngxpfree(void* ptr);
	//内存重置函数,针对小块内存的清理释放
	void reset();
	//添加回调操作函数，释放外部资源。小块内存也是字节对齐方式申请。size是用来开辟外部资源的大小，然后释放时可以把外部资源释放完整。根据类型释放
	clean_add* cleanadd(int size);//注意：若此处的操作的内存是大块内存，则ngxpfree,和destory函数会由于相同的内存多次释放造成崩溃
	//内存池销毁函数
	void destory();
	//打印信息，自己测试用的
	void prifailed();
private:
	pool_t* pool;
	//申请小块内存池，也是按NGX_POOL_ALIGNMENT的整数倍开辟，返回的空间首地址调整到平台所支持的整数倍返回
	void* blockpalloc(int size);
	//申请小块内存，分对齐与不对齐开辟
	void* smallalloc(int size, int flag);
	//申请大块内存，如果要开辟小块内存存放large，则也是内存对齐方式开辟小块内存
	void* largepalloc(int size);
};
