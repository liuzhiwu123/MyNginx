#include"pch.h"
#include"nginxallocate.h"
/*
小块内存分配没有提供任何的内存释放函数，实际上，从小块内存的分配方式来看（直接通过last指针偏移来分配内存），也没有办法进行小块内存的回收
nginx本质：http服务器
是一个短链接的服务器，客户端发起一个请求，到达服务器以后，处里完成，ngnix会给客户端返回一个响应，http服务器
就会主动断开tcp链接，http服务器返回响应以后需要等待60s，60s之内客户端又一次发来请求，重置这个时间，否则60s之内没有客户端
发来响应，ngnix就主动断开链接，此时ngnix可以调用reset_pool重置内存池了，等待下一次该客户端的请求。
小块内存主要用在间歇性的场景，短链接
长连接就用sgistl，他又小块的分配和和释放。

二级alloca是给容器用
*/

void dele(void* ptr)
{
	int* p = (int*)ptr;
	delete(p);
	cout << "dele" << endl;
}
void closefile(void* fp)
{
	FILE* p = (FILE*)fp;
	fclose(p);
	cout << "fclose" << endl;
}
class Test
{
public:
	Test(int size) { ptr = new int[size]; top = 0; }
	~Test() { delete[]ptr; cout << "delete[]ptr" << endl; }
	void push(int val) { ptr[top++] = val; }
	void pri()
	{
		for (int i = 0; i < top; i++)
		{
			cout << ptr[i] << " ";
		}
		cout << endl;
	}
private:
	int* ptr;
	int top;
};
int main()
{
	vector<int> vec;
	vec.push_back(10);
	vec.push_back(20);
	vec.push_back(30);
	vec.push_back(40);
	vec.push_back(50);
	vec.push_back(60);
	vec.push_back(70);
	vec.push_back(80);
	vec.push_back(90);
	vec.push_back(100);
	vec.pop_back();
	int val = vec.back();
	cout << val << endl;
	while (!vec.empty())
	{
		val = vec.back();
		cout << val << endl;
		vec.pop_back();
	}
	cout << val << endl;
#if 0
	myngnix_pool ngx;
	ngx.CreatPool_t(43);
	ngx.ngxpalloc(10);
	ngx.ngxpalloc(30);
	ngx.prifailed();

	int* p = (int*)ngx.ngxpalloc(40);
	for (int i = 0; i < 10; i++)
		p[i] = i;
	/*
		Test* p = new Test(40);
		for (int i = 0; i < 10; i++)
			p->push(i);
		p->pri();
	*/
	clean_add* c1 = ngx.cleanadd(sizeof(Test*));
	c1->cleandata = p;
	c1->handler = dele;

	FILE* fp = fopen("tree.cpp", "r");
	clean_add* c2 = ngx.cleanadd(sizeof(FILE*));
	c2->cleandata = fp;
	c2->handler = closefile;

	ngx.destory();

	//p->pri();
	cout << endl;
#endif
	return 0;
}
