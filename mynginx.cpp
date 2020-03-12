#include "pch.h"
#include"mynginx.h"
pool_t* myngnix_pool::CreatPool_t(int size)
{
	void* p = malloc(size);
	if (p == nullptr)
	{
		cout << "malloc pool failed" << endl;
		return nullptr;
	}
	cout << "malloc successfu" << endl;
	pool = (pool_t*)p;
	pool->ngxhead.end = (char*)p + size;
	pool->ngxhead.last = (char*)p + sizeof(pool_t);
	pool->ngxhead.next = nullptr;
	pool->ngxhead.failed = 0;
	pool->current = pool;
	pool->clean = nullptr;
	pool->large = nullptr;
	size = size - sizeof(pool_t);
	pool->max = (size < NGX_MAX_ALLOC_FROM_POOL) ? size : NGX_MAX_ALLOC_FROM_POOL;
}

void* myngnix_pool::ngxpalloc(int size)
{
	if (size < pool->max)
	{
		return smallalloc(size, 1);
	}
	return largepalloc(size);
}
void* myngnix_pool::ngx_pnalloc(int size)
{
	if (size < pool->max)
	{
		return smallalloc(size, 0);
	}
	return largepalloc(size);
}
void* myngnix_pool::ngx_pcalloc(int size)
{
	void* p = malloc(size);
	if (p)
	{
		memset(p, size, 0);
	}
	return p;
}
void myngnix_pool::ngxpfree(void* ptr)
{
	Large* lar = pool->large;
	for (; lar; lar = lar->next_large)
	{
		if (lar->alloc == ptr)
		{
			free(lar->alloc);
			lar->alloc = nullptr;
			return;
		}
	}
}
void myngnix_pool::reset()
{
	Large* lar = pool->large;
	for (; lar; lar = lar->next_large)
	{
		if (lar->alloc != nullptr)
			free(lar->alloc);
	}
	pool_t* fast = pool;
	fast->ngxhead.last = (char*)pool + sizeof(pool_t);
	fast->ngxhead.failed = 0;
	fast->current = pool;
	fast->large = nullptr;
	for (fast = fast->ngxhead.next; fast; fast = fast->ngxhead.next)
	{
		fast->ngxhead.failed = 0;
		fast->ngxhead.last = (char*)fast + sizeof(Ngx_Head);
	}
}
clean_add* myngnix_pool::cleanadd(int size)
{
	clean_add* m = (clean_add*)ngxpalloc(sizeof(clean_add));
	if (m == nullptr)
		return nullptr;
	if (size)
	{
		m->cleandata = ngxpalloc(size);
		if (m->cleandata == nullptr)
			return nullptr;
	}
	else
	{
		m->cleandata = nullptr;
	}
	m->handler = nullptr;
	m->clean_next = pool->clean;
	pool->clean = m;
	return m;
}
void myngnix_pool::destory()//先释放外部资源，然后大块内存，最后销毁小块内存池
{
	clean_add* cle = pool->clean;
	for (; cle; cle = cle->clean_next)
	{
		if (cle->cleandata)
		{
			cle->handler(cle->cleandata);//若释放的内存是大块内存上申请的，要把大块指针置空。
			Large* lar = pool->large;
			for (; lar; lar = lar->next_large)
			{
				if (lar->alloc == cle->cleandata)
				{
					lar->alloc = nullptr;
					break;
				}
			}
		}
	}
	Large* lar = pool->large;
	for (; lar; lar = lar->next_large)
	{
		if (lar->alloc != nullptr)
			free(lar->alloc);
	}
	pool_t* p = pool;
	while (pool)
	{
		pool = pool->ngxhead.next;
		free(p);
		p = pool;
	}
}
void myngnix_pool::prifailed()
{
	pool_t*p = pool;
	while (p)
	{
		cout << p->ngxhead.failed << endl;
		p = p->ngxhead.next;
	}
}
void* myngnix_pool::blockpalloc(int size)
{
	void* m;
	pool_t* newpool;
	pool_t* p;
	int psize = pool->ngxhead.end - (char*)pool;
	m = malloc(psize);
	newpool = (pool_t*)m;
	newpool->ngxhead.end = (char*)m + psize;
	m = (char*)m + sizeof(Ngx_Head);
	m = ngx_align_ptr(m, NGX_ALIGNMENT);
	newpool->ngxhead.last = (char*)m + size;
	newpool->ngxhead.failed = 0;
	newpool->ngxhead.next = nullptr;
	for (p = pool->current; p->ngxhead.next; p = p->ngxhead.next)
	{
		if (p->ngxhead.failed++ > 1)
			pool->current = p->ngxhead.next;
	}
	p->ngxhead.next = newpool;
	return m;
}
void* myngnix_pool::smallalloc(int size, int flag)
{
	pool_t* p = pool->current;
	char* m;
	do
	{
		m = p->ngxhead.last;
		if (flag)
		{
			m = ngx_align_ptr(m, NGX_ALIGNMENT);//需要内存对齐，就把m调整到平台所支持的整数倍
		}
		if (p->ngxhead.end - m > size)
		{
			p->ngxhead.last = m + size;
			return m;
		}
		p = p->ngxhead.next;
	} while (p);
	return blockpalloc(size);
}
void* myngnix_pool::largepalloc(int size)
{
	void* p = malloc(size);
	Large* lar = pool->large;
	int n = 0;
	for (; lar; lar = lar->next_large)
	{
		if (lar->alloc == nullptr)
		{
			lar->alloc = p;
			return p;
		}
		if (n++ > 1)
		{
			cout << "n++>1" << n << endl;
			break;
		}
	}
	void* m = smallalloc(sizeof(Large), 1);
	if (m == nullptr)
	{
		free(p);
		return nullptr;
	}
	lar = (Large*)m;
	lar->alloc = p;
	lar->next_large = pool->large;
	pool->large = lar;
	return p;
}
