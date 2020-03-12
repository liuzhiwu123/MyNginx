#include"pch.h"
#include"nginxallocate.h"
/*
С���ڴ����û���ṩ�κε��ڴ��ͷź�����ʵ���ϣ���С���ڴ�ķ��䷽ʽ������ֱ��ͨ��lastָ��ƫ���������ڴ棩��Ҳû�а취����С���ڴ�Ļ���
nginx���ʣ�http������
��һ�������ӵķ��������ͻ��˷���һ�����󣬵���������Ժ󣬴�����ɣ�ngnix����ͻ��˷���һ����Ӧ��http������
�ͻ������Ͽ�tcp���ӣ�http������������Ӧ�Ժ���Ҫ�ȴ�60s��60s֮�ڿͻ�����һ�η��������������ʱ�䣬����60s֮��û�пͻ���
������Ӧ��ngnix�������Ͽ����ӣ���ʱngnix���Ե���reset_pool�����ڴ���ˣ��ȴ���һ�θÿͻ��˵�����
С���ڴ���Ҫ���ڼ�Ъ�Եĳ�����������
�����Ӿ���sgistl������С��ķ���ͺ��ͷš�

����alloca�Ǹ�������
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
