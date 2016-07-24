#include "New_Dbg.h"

#include <iostream>
#include <cstdio>
#include <cstring>

#include <vector>

#include <memory>
using namespace std;

//https://oopscenities.net/2013/10/06/smart-pointers-part-4-shared_ptr/

class Integer
{
	int n;
public:
	Integer(int n) : n(n) { }
	~Integer() { printf("class Integer Deleting %d\n", n); }
	int get() const { return n; }
};

shared_ptr<Integer> get_instance(int n)
{
	return make_shared<Integer>(n);
}

class A
{
	int n;
	int* buf;
public:
	A(int n) : n(n) { buf = new int[n]; }
	~A() {
		printf("class A Deleting %d\n", n); 
		if (buf) 
			delete[] buf;
	}
	int get() const { return n; }
};

shared_ptr<A> get_instance_A(int n)
{
	return make_shared<A>(n);
}

void shared_ptr_test_1()
{

	{
		vector<shared_ptr<Integer>> vec;

		for (int i = 0; i < 100; i++)
			vec.push_back(get_instance(i));

		//We do something with the elements of the vector       
		int sum = 0;
		for (auto& i : vec)
			sum += i->get();

		printf("Sum: %d\n", sum);

	}
	
}
void shared_ptr_test_2()
{

	{
		vector<shared_ptr<A>> vec;

		for (int i = 0; i < 100; i++)
			vec.push_back(get_instance_A(i));

		//We do something with the elements of the vector       
		int sum = 0;
		for (auto& i : vec)
			sum += i->get();

		printf("Sum: %d\n", sum);

	}

}

void shared_ptr_test_3(shared_ptr<Integer> &p)
{
	shared_ptr<Integer> a(new Integer{ 10 });
	shared_ptr<Integer> b(new Integer{ 20 });
	shared_ptr<Integer> c = a;
	shared_ptr<Integer> d(new Integer{ 30 });
	shared_ptr<Integer> e = b;
	a = d;
	b = shared_ptr<Integer>(new Integer(40));
	shared_ptr<Integer> f = c;
	b = f;

	printf("%d\n", a->get());
	printf("%d\n", b->get());
	printf("%d\n", c->get());
	printf("%d\n", d->get());
	printf("%d\n", e->get());
	printf("%d\n", f->get());

	p = c;
}// when it goes out of scope it releases all objects except object c ( with Integer value 10)

void shared_ptr_test_4()
{
	vector<shared_ptr<A>> vec;
	shared_ptr<A> el( new A(10));
	vec.push_back(el);
	printf("vec goes out of scope..\n");
}

void shared_ptr_test()
{
	shared_ptr_test_1();
	shared_ptr_test_2();

	shared_ptr<Integer> ZZ;
	shared_ptr_test_3(ZZ);

	printf("ZZ goes out of scope..\n");

	shared_ptr_test_4();
}
