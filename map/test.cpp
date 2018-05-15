#include <iostream>

template<class> class vector;
typedef vector<int> vi;

template<class T>
class vector {
private:
	T data[100];

public:
	typedef T* pointer;

	vector() { data[0] = 100; }

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

int main()
{
	vector<int> a;
	
	vector<int>::iterator it = a.begin();
	vector<int>::const_iterator cit = a.cbegin();

	std::cout << "*it=" << *it << " " << std::endl;
	std::cout << "*cit=" << *cit << " " << std::endl;

	return 0;
}
