#include <bits/stdc++.h>
using namespace std;

struct block
{
	int sz;
	int *data[100];
	block(): sz(0) {}
};

int main()
{
	block* a = new block();
	a->data[0] = new int(100);
	block* b = new block();
	b->data[0] = a->data[0];
	delete a;
	cout<<*b->data[0]<<endl;
	return 0;
}
