#include "pch.h"
#include"mynginx.h"

template<typename T>
class Allactor
{
public:
	Allactor()
	{
		alloc.CreatPool_t(sizeof(T) * 10);
		cout << "Allactor()" << endl;
	}
	~Allactor() { alloc.destory(); }
	T* allocate(int size)
	{
		return (T*)alloc.ngxpalloc(sizeof(T)*size);
	}
	void deallocate(T* ptr)
	{
		alloc.ngxpfree(ptr);
	}
	void construct(T* ptr, const T &val)
	{
		new(ptr) T(val);
	}
	void destory(T *ptr)
	{
		ptr->~T();
	}
private:
	myngnix_pool alloc;
};

template<typename T, typename Alloc = Allactor<T>>
class vector
{
public:
	vector(int size = 5)
	{
		cout << "vector(int size = 5)" << endl;
		first = alloc.allocate(size);
		end = first + size;
		last = first;
	}
	~vector()
	{
		T* ptr = first;
		while (ptr != last)
		{
			alloc.destory(ptr);
			ptr++;
		}
		alloc.deallocate(first);
	}

	vector(const vector<T>& rhs)
	{
		int size = rhs.last - rhs.first;
		int len = rhs.end - rhs.first;
		first = alloc.allocate(len);
		for (int i = 0; i < size; i++)
		{
			alloc.construct(first + i, *(rhs.first + i));
		}
		last = rhs.last;
		end = rhs.end;
	}
	vector<T>& operator=(const vector<T> &rhs)
	{
		if (this == &rhs)
			return;
		T* ptr = first;
		while (ptr != last)
		{
			alloc.destory(ptr);
			ptr++;
		}
		alloc.deallcate(first);
		int size = rhs.last - rhs.first;
		int len = rhs.end - rhs.first;
		first = alloc.allocate(len);
		for (int i = 0; i < size; i++)
		{
			alloc.construct(first + i, *(rhs.first + i));
		}
		last = rhs.last;
		end = rhs.end;
	}
	void push_back(const T &val)//ÏòÄ©Î²Ìí¼ÓÔªËØ
	{
		if (full())
			expand();
		alloc.construct(last, val);
		last++;
	}
	void pop_back()//´ÓÄ©Î²É¾³ýÔªËØ
	{
		if (empty())
			return;
		alloc.destory(--last);
	}
	T back()const//·µ»ØÈÝÆ÷Ä©Î²µÄÔªËØÖµ
	{
		return*(last - 1);
	}
	bool empty()const
	{
		return first == last;
	}
	bool full()const
	{
		return last == end;
	}
private:
	T* first;
	T* last;
	T* end;
	Alloc alloc;
	bool expand()
	{
		int size = last - first;
		T* newdata = alloc.allocate(size * 2);
		for (int i = 0; i < size; i++)
		{
			alloc.construct(newdata + i, *(first + i));
			alloc.destory(first + i);
		}
		alloc.deallocate(first);
		first = newdata;
		last = first + size;
		end = first + size * 2;
		cout << "expand" << endl;
		return true;
	}
};
