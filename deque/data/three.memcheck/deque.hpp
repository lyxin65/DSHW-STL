#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP
 
#include "exceptions.hpp"
 
#include <cstddef>
 
//to be continued: Split Merge const_iterator
 
const int lX = 150;
const int rX = 4 * lX;
 
namespace sjtu { 
 
template<class T>
class deque {
private:
	struct node{
		T *a[rX];
		size_t size, nowsize;
		node *next, *last;
		
		node(): size(0), nowsize(lX), next(NULL), last(NULL){}
		
		~node()
		{
			for(int i = 0; i < size; i++)
				if(a[i])delete a[i];
			if(next)delete next;
		}
	};
 
	size_t Size;
	T* tmp[rX + lX];
	node *head, *tail;
	
public:	
	class const_iterator;
	class iterator {
	friend class deque;
	private:
		int x, nx;
		const deque *v;
		node *now;
		T *p;
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		iterator(): x(0), nx(0), v(NULL), p(NULL), now(NULL){}
		
		void Get()
		{
			if(x <= 0)
			{
				nx = 0;
				now = NULL;
				p = NULL;
				return;
			}
			now = v -> head;
			size_t nsize = 0;
			while(now != v -> tail && nsize + now -> size < x)
			{
				nsize += now -> size;
				now = now -> next;
			}
			if(now == v -> tail)
			{
				nx = 0;
				now = NULL;
				p = NULL;
				return;
			}
			nx = x - nsize;
			p = now -> a[x - nsize - 1];
		}
		
		iterator(int x_, const deque *v_): x(x_), v(v_)
		{
			Get();
		}
		 
		iterator operator+(const int &n) const {
			return iterator(x + n, v);
			//TODO
		}
		iterator operator-(const int &n) const {
			return iterator(x - n, v);
			//TODO
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			if(v != rhs.v)throw invalid_iterator();
			return x - rhs.x;
			//TODO
		}
		iterator operator+=(const int &n) {
			x += n;
			Get();
			return *this;
			//TODO
		}
		iterator operator-=(const int &n) {
			x -= n;
			Get();
			return *this;
			//TODO
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int)
		{
			iterator lit = *this;
			x++;
			if(x > v -> Size)
			{
				nx = 0;
				now = NULL;
				p = NULL;
				return lit;
			}
			nx++;
			if(nx > now -> size)
			{
				now = now -> next;
				if(now == v -> tail)
				{
					nx = 0;
					now = NULL;
					p = NULL;
					return lit;
				}
				nx = 1;
				p = now -> a[0];
			}
			else
			{
				p = now -> a[nx - 1];
			}
			return lit;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++()
		{
			++x;
			if(x > v -> Size)
			{ 
				nx = 0;
				now = NULL;
				p = NULL;
				return *this;
			}
			++nx;
			if(nx > now -> size)
			{
				now = now -> next;
				if(now == v -> tail)
				{
					nx = 0;
					now = NULL;
					p = NULL;
					return *this;
				}
				nx = 1;
				p = now -> a[0];
			}
			else
			{
				p = now -> a[nx - 1];
			}
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int)
		{
			iterator lit = *this;
			x--;
			if(x == v -> Size)
			{
				now = v -> tail -> last;
				nx = now -> size;
				p = now -> a[nx - 1];
				return lit;
			}
			if(x <= 0)
			{
				nx = 0;
				now = NULL;
				p = NULL;
				return lit;
			}
			nx--;
			if(!nx)
			{
				now = now -> last;
				nx = now -> size;
				p = now -> a[nx - 1];
			}
			else
			{
				p = now -> a[nx - 1];
			}
			return lit;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--()
		{
			--x;
			if (x == v -> Size)
			{
				now = v -> tail -> last;
				nx = now -> size;
				p = now -> a[nx - 1];
				return *this;
			}
			if(x <= 0)
			{
				nx = 0;
				now = NULL;
				p = NULL;
				return *this;
			}
			--nx;
			if(!nx)
			{
				now = now -> last;
				nx = now -> size;
				p = now -> a[nx - 1];
			}
			else
			{
				p = now -> a[nx - 1];
			}
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const
		{
			if(x <= 0 || x > v -> Size)throw runtime_error();
			return *p;
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept
		{
			if(x <= 0 || x > v -> Size)throw runtime_error();
			return p;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const
		{
			return (x == rhs.x && v == rhs.v);
		}
		bool operator==(const const_iterator &rhs) const
		{
			return (x == rhs.x && v == rhs.v);
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const
		{
			return !(x == rhs.x && v == rhs.v);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return !(x == rhs.x && v == rhs.v);
		}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		friend class deque;
		private:
			int x, nx;
			const deque *v;
			node *now;
			T *p;
			// data members.
		public:
			const_iterator(): x(0), nx(0), v(NULL), p(NULL), now(NULL){}
			
			const_iterator(const const_iterator &other)
			{
				x = other.x;
				nx = other.nx;
				v = other.v;
				now = other.now;
				p = other.p;
			}
			
			const_iterator(const iterator &other)
			{
				x = other.x;
				nx = other.nx;
				v = other.v;
				now = other.now;
				p = other.p;
			}
			
			void Get()
			{
				if(x <= 0)
				{
					nx = 0;
					now = NULL;
					p = NULL;
					return;
				}
				now = v -> head;
				size_t nsize = 0;
				while(now != v -> tail && nsize + now -> size < x)
				{
					nsize += now -> size;
					now = now -> next;
				}
				if(now == v -> tail)
				{
					nx = 0;
					now = NULL;
					p = NULL;
					return;
				}
				nx = x - nsize;
				p = now -> a[x - nsize - 1];
			}
			
			const_iterator(int x_, const deque *v_): x(x_), v(v_)
			{
				Get();
			}
			 
			const_iterator operator+(const int &n) const
			{
				return const_iterator(x + n, v);
			}
			const_iterator operator-(const int &n) const
			{
				return const_iterator(x - n, v);
			}
			
			int operator-(const const_iterator &rhs) const
			{
				if(v != rhs.v)throw invalid_iterator();
				return x - rhs.x;
			}
			
			const_iterator operator+=(const int &n)
			{
				x += n;
				Get();
				return *this;
			}
			
			const_iterator operator-=(const int &n)
			{
				x -= n;
				Get();
				return *this;
			}
			
			const_iterator operator++(int)
			{
				const_iterator lit = *this;
				x++;
				if(x > v -> Size)
				{
					nx = 0;
					now = NULL;
					p = NULL;
					return lit;
				}
				nx++;
				if(nx > now -> size)
				{
					now = now -> next;
					if(now == v -> tail)
					{
						nx = 0;
						now = NULL;
						p = NULL;
						return lit;
					}
					nx = 1;
					p = now -> a[0];
				}
				else
				{
					p = now -> a[nx - 1];
				}
				return lit;
			}
 
			const_iterator& operator++()
			{
				++x;
				if(x > v -> Size)
				{
					nx = 0;
					now = NULL;
					p = NULL;
					return *this;
				}
				++nx;
				if(nx > now -> size)
				{
					now = now -> next;
					if(now == v -> tail)
					{
						nx = 0;
						now = NULL;
						p = NULL;
						return *this;
					}
					nx = 1;
					p = now -> a[0];
				}
				else
				{
					p = now -> a[nx - 1];
				}
				return *this;
			}
 
			const_iterator operator--(int)
			{
				const_iterator lit = *this;
				x--;
				if (x == v -> Size)
				{
					now = v -> tail -> last;
					nx = now -> size;
					p = now -> a[nx - 1];
					return lit;
				}
				if(x <= 0)
				{
					nx = 0;
					now = NULL;
					p = NULL;
					return lit;
				}
				nx--;
				if(!nx)
				{
					now = now -> last;
					nx = now -> size;
					p = now -> a[nx - 1];
				}
				else
				{
					p = now -> a[nx - 1];
				}
				return lit;
			}
 
			const_iterator& operator--()
			{
				--x;
				if (x == v -> Size)
				{
					now = v -> tail -> last;
					nx = now -> size;
					p = now -> a[nx - 1];
					return *this;
				}
				if(x <= 0)
				{
					nx = 0;
					now = NULL;
					p = NULL;
					return *this;
				}
				--nx;
				if(!nx)
				{
					now = now -> last;
					nx = now -> size;
					p = now -> a[nx - 1];
				}
				else
				{
					p = now -> a[nx - 1];
				}
				return *this;
			}
 
			T& operator*() const
			{
				if(x <= 0 || x > v -> Size)throw runtime_error();
				return *p;
			}
 
			T* operator->() const noexcept
			{
				if(x <= 0 || x > v -> Size)throw runtime_error();
				return p;
			}
 
			bool operator==(const iterator &rhs) const
			{
				return (x == rhs.x && v == rhs.v);
			}
			
			bool operator==(const const_iterator &rhs) const
			{
				return (x == rhs.x && v == rhs.v);
			}
 
			bool operator!=(const iterator &rhs) const
			{
				return !(x == rhs.x && v == rhs.v);
			}
			
			bool operator!=(const const_iterator &rhs) const
			{
				return !(x == rhs.x && v == rhs.v);
			}
	};
public:
	/**
	 * TODO Constructors
	 */
		/**
	 * returns an iterator to the beginning.
	 */
	iterator begin()
	{
		iterator now;
		now.x = 1;
		now.nx = 1;
		now.v = this;
		now.now = head;
		now.p = head -> a[0];
		return now;
	}
	const_iterator cbegin() const
	{
		const_iterator now;
		now.x = 1;
		now.nx = 1;
		now.v = this;
		now.now = head;
		now.p = head -> a[0];
		return now;
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end()
	{
		iterator now;
		now.x = Size + 1;
		now.nx = 0;
		now.v = this;
		now.now = NULL;
		now.p = NULL;
		return now;
	}
	const_iterator cend() const
	{
		const_iterator now;
		now.x = Size + 1;
		now.nx = 0;
		now.v = this;
		now.now = NULL;
		now.p = NULL;
		return now;
	}
	 
	deque(): Size(0)
	{
		head = new node();
		tail = new node();
		head -> next = tail;
		tail -> last = head;
	}
	
	void copy(node * &A, node *const &B)
	{
		A -> size = B -> size;
		for(int i = 0; i < B -> size; i++)
			A -> a[i] = new T(*(B -> a[i]));
	}
	
	void add(node *A, node *B)
	{
		A -> next -> last = B;
		B -> next = A -> next;
		B -> last = A;
		A -> next = B;
	}
	
	deque(const deque &other)
	{
		Size = other.Size;
		head = new node();
		tail = new node();
		head -> next = tail;
		tail -> last = head;
		if(other.head != other.tail)
		{
			copy(head, other.head);
			head -> next = tail;
			node *p = other.head -> next, *last = head;
			while(p != other.tail)
			{
				node *now = new node;
				copy(now, p);
				add(last, now);
				last = now;
				p = p -> next;
			}
		}
	}
	/**
	 * TODO Deconstructor
	 */
	~deque()
	{
		if(head)delete head;
	}
	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other)
	{
		if(this == &other)return *this;
		Size = other.Size;
		delete head;
		head = new node();
		tail = new node();
		head -> next = tail;
		tail -> last = head;
		if(other.head != other.tail)
		{
			copy(head, other.head);
			head -> next = tail;
			node *p = other.head -> next, *last = head;
			while(p != other.tail)
			{
				node *now = new node();
				copy(now, p);
				add(last, now);
				last = now;
				p = p -> next;
			}
		}
		return *this;
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos_)
	{
		size_t pos = pos_ + 1;
		if(pos <= 0 || pos > Size)throw index_out_of_bound();
		node *now = head;
		size_t nsize = 0;
		while(now != tail)
		{
			if(nsize + now -> size >= pos)
			{
				return *(now -> a[pos - nsize - 1]);
			}
			nsize += now -> size;
			now = now -> next;
		}
		throw index_out_of_bound();
	}
	
	const T & at(const size_t &pos_) const
	{
		size_t pos = pos_ + 1;
		if(pos <= 0 || pos > Size)throw index_out_of_bound();
		node *now = head;
		size_t nsize = 0;
		while(now != tail)
		{
			if(nsize + now -> size >= pos)
			{
				return *(now -> a[pos - nsize - 1]);
			}
			nsize += now -> size;
			now = now -> next;
		}
	}
	
	T & operator[](const size_t &pos_)
	{
		size_t pos = pos_ + 1;
		if(pos <= 0 || pos > Size)throw index_out_of_bound();
		node *now = head;
		size_t nsize = 0;
		while(now != tail)
		{
			if(nsize + now -> size >= pos)
			{
				return *(now -> a[pos - nsize - 1]);
			}
			nsize += now -> size;
			now = now -> next;
		}
	}
	
	const T & operator[](const size_t &pos_) const
	{
		size_t pos = pos_ + 1;
		if(pos <= 0 || pos > Size)throw index_out_of_bound();
		node *now = head;
		size_t nsize = 0;
		while(now != tail)
		{
			if(nsize + now -> size >= pos)
			{
				return *(now -> a[pos - nsize - 1]);
			}
			nsize += now -> size;
			now = now -> next;
		}
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const
	{
		if(head -> size == 0)throw container_is_empty();
		return *(head -> a[0]);
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const
	{
		if(head -> size == 0)throw container_is_empty();
		node *now = tail -> last;
		return *(now -> a[now -> size - 1]);
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const
	{
		return head -> size == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const
	{
		return Size;
	}
	/**
	 * clears the contents
	 */
	void clear()
	{
		if(head -> size == 0)return;
		delete head;
		head = new node();
		tail = new node();
		head -> next = tail;
		tail -> last = head;
		Size = 0;
	}
	
	 void Split(node *now)
	 {
	 	node *p = new node();
	 	size_t nsize = now -> size;
	 	now -> size = nsize / 2;
	 	for(int i = nsize / 2; i < nsize; i++)
	 		p -> a[i - nsize / 2] = now -> a[i];
	 	p -> size = nsize - now -> size;
	 	add(now, p);
	 }
	 
	 void Work(node *now, node *r)
	 {
	 	if(now -> size + r -> size < rX)
 		{
 			for(int i = 0; i < r -> size; i++)
 				now -> a[now -> size + i] = new T(*(r -> a[i]));
 			now -> size += r -> size;
 			now -> next = r -> next;
 			r -> next -> last = now;
 			r -> last = r -> next = NULL;
 			delete r;
 		}
 		else
 		{
	 		for(int i = 0; i < now -> size; i++)
	 			tmp[i] = now -> a[i];
	 		for(int i = 0; i < r -> size; i++)
	 			tmp[i + now -> size] = r -> a[i];
	 		size_t nsize = now -> size + r -> size;
	 		now -> size = nsize / 2;
	 		for(int i = 0; i < now -> size; i++)
	 			now -> a[i] = tmp[i];
	 		r -> size = nsize - now -> size;
	 		for(int i = now -> size; i < nsize; i++)
	 			r -> a[i - now -> size] = tmp[i];
 		}
	 }
	 
	void Merge(node *now)
	{
	 	node *l = now -> last, *r = now -> next;
	 	size_t lsize = (l ? l -> size : 1e9), rsize = (r != tail ? r -> size : 1e9);
	 	if(lsize == 1e9 && rsize == 1e9)return;
	 	if(lsize > rsize)
	 	{
	 		Work(now, r);
	 	}
	 	else
	 	{
	 		Work(l, now);
	 	}
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 * throw if the iterator is invalid or it point to a wrong place.
	 */
	
	iterator insert(iterator pos, const T &value)
	{
		if(pos.v != this)throw invalid_iterator();
		if(pos.x <= 0 || pos.x > Size + 1)throw invalid_iterator();
		Size++;
		if(!pos.nx)
		{
			node *now = tail -> last;
			now -> a[now -> size++] = new T(value);
			if(now -> size == rX)Split(now);
			return iterator(pos.x, this);
		}
		node *now = pos.now;
		int nx = pos.nx;
		for(int i = now -> size - 1; i >= nx - 1; i--)
			now -> a[i + 1] = now -> a[i];
		now -> a[nx - 1] = new T(value);
		now -> size++;
		if(now -> size == rX)Split(now);
		return iterator(pos.x, this);
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos)
	{
		if(empty())throw invalid_iterator();
		if(pos.v != this)throw invalid_iterator();
		if(pos.x <= 0 || pos.x > Size)throw invalid_iterator();
		Size--;
		node *now = pos.now;
		int nx = pos.nx;
		delete now -> a[nx - 1];
		for(int i = nx - 1; i < now -> size; i++)
			now -> a[i] = now -> a[i + 1];
		now -> size--;
		if(now -> size == lX)Merge(now);
		return iterator(pos.x, this);
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value)
	{
		Size++;
		node *now = tail -> last;
		now -> a[now -> size++] = new T(value);
		if(now -> size == rX)Split(now);
	}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back()
	{
		if(head -> size == 0)throw container_is_empty();
		Size--;
		node *now = tail -> last;
		delete now -> a[--now -> size];
		if(now -> size == lX)Merge(now);
	}
	/**
	 * inserts an element to the beginning.
	 */
	 
	void push_front(const T &value)
	{
		Size++;
		for(int i = head -> size; i; i--)
			head -> a[i] = head -> a[i - 1];
		head -> a[0] = new T(value);
		head -> size++;
		if(head -> size == rX)Split(head);
	}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front()
	{
		if(head -> size == 0)throw container_is_empty();
		Size--;
		delete head -> a[0];
		for(int i = 0; i < head -> size - 1; i++)
			head -> a[i] = head -> a[i + 1];
		head -> size--;
		if(head -> size == lX)Merge(head);
	}
};
 
}
 
#endif
