#include <iostream>
#include <algorithm>
#include <utility>
#include <string>
//#include "map.hpp"

template<class T, class Compare = std::less<T>>
class vector {
private:
	T data[100];

public:
	typedef T* pointer;

	vector() { data[0] = 100; }

	struct node;
	int get(node *o) const {
		return o ? o->data : 0;
	}
	struct node
	{
		int data;
		node *lc, *rc;
		int calc() {
			return get(lc) + get(rc);
		}
	};

	template<class reference, class pointer>
	class base_iterator {
		friend class vector;
		typedef base_iterator<T&, T*> iterator;
		typedef base_iterator<const T&, const T*> const_iterator;
	private:
		vector *self;
		T *cur;
	public:

		base_iterator() {}
		base_iterator(vector *self, T* cur): self(self), cur(cur) {}
//		base_iterator(const base_iterator &other): self(other.self), cur(other.cur) {}
		base_iterator(const iterator &other): self(other.self), cur(other.cur) {}
		base_iterator(const const_iterator &other): self(other.self), cur(other.cur) {}

		reference operator*() const {
			return *cur;
		}
		pointer operator->() const {
			return cur;
		}
	};

	typedef base_iterator<T&, T*> iterator;
	typedef base_iterator<const T&, const T*> const_iterator;

	iterator begin() {
		return iterator(this, data);
	}
	const_iterator cbegin() const {
		return const_iterator(const_cast<vector*>(this), const_cast<T*>(data));
	}
};

/*
template<class T, class Compare = std::less<T>>
class vector {
private:
	T data[100];

public:
	typedef T* pointer;

	vector() { data[0] = 100; }

	class node
	{
		int data;
		node *lc, *rc;
		int calc();
	};

	int get(node *o) {
		return o ? o->data : 0;
	}

	class iterator {
	private:
		vector *self;
		pointer cur;
	public:

		iterator() {}
		iterator(vector *self, pointer cur): self(self), cur(cur) {}
		iterator(const iterator &other): self(other.self), cur(other.cur) {}

		T& operator*() const {
			return *cur;
		}
		T* operator->() const {
			return cur;
		}
	};

	class const_iterator {
	private:
		vector *self;
		pointer cur;
	public:

		const_iterator() {}
		const_iterator(vector *_self, pointer _cur): self(_self), cur(_cur) {}
		const_iterator(const const_iterator &other): self(other.self), cur(other.cur) {}

		const T& operator*() const {
			return *cur;
		}
		const T* operator->() const {
			return cur;
		}
	};

	iterator begin() {
		return iterator(this, data);
	}
	const_iterator cbegin() const {
		return const_iterator(const_cast<vector *>(this), const_cast<int *>(data));
	//	return const_iterator(this, data);
	}
};
*/

int main()
{
	vector<int> a;
	
	vector<int>::iterator it = a.begin();
	vector<int>::const_iterator cit(it);

	std::string string = "";
	vector<std::string> b;
	vector<std::string>::iterator j = b.begin();

//	sjtu::map<int, std::string> mp;

	std::cout << *j << std::endl;

	std::cout << "*it=" << *it << " " << std::endl;
	std::cout << "*cit=" << *cit << " " << std::endl;

	return 0;
}
