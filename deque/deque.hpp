#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

#ifdef LOCAL
#include <cassert>
#include <iostream>
#define debug(x) std::cerr << #x << "=" << x << std::endl
#endif

namespace sjtu { 

template<class T>
class deque {
private:
	static const int inf = 150;
	static const int sup = inf * 4;

	struct cycle_array
	{
		int start;
		T *data[sup];

		cycle_array(): start(0) {}

		T*& operator[](const int &n) {
			if(start + n >= sup)
				return data[start + n - sup];
			return data[start + n];
		}
		const T* operator[](const int &n) const {
			if(start + n >= sup)
				return data[start + n - sup];
			return data[start + n];
		}
	};

	class block;
	typedef block* block_pointer;
	struct block {
		int sz;
		cycle_array data;
		block_pointer pre, nxt;

		block(): pre(0), nxt(0), sz(0), data() {}

		void operator=(const block &rhs) {
			sz = rhs.sz;
			for(int i = 0; i < sz; i++)
				data[i] = new T(*rhs.data[i]);
		}
	};

public:
	typedef T* pointer;

	template<class Ref, class Poi> class base_iterator;
//	template<class Ref, class Poi>
//	friend class<Ref, Poi> base_iterator;
	typedef base_iterator<T&, T*> iterator;
	typedef base_iterator<const T&, const T*> const_iterator;

	template<class Ref, class Poi>
	class base_iterator {
		friend class deque;
	private:
		const deque *self;
		int idx, cur;
		block_pointer node;
		pointer first, last;

	public:
		base_iterator(): self(0), node(0), first(0), last(0), cur(0), idx(0) {}
		base_iterator(const deque *_self, block_pointer _node, const int &_cur, const int &_idx): self(_self), node(_node), first(_node->data[0]), last(_node->data[_node->sz]), cur(_cur), idx(_idx) {}
		base_iterator(const base_iterator &other): self(other.self), node(other.node), first(other.first), last(other.last), cur(other.cur), idx(other.idx) {}

		void set_node(block_pointer o) {
			node = o;
			first = o->data[0];
			last = o->data[o->sz];
		}
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		base_iterator operator+(const int &n) const {
			base_iterator tmp = *this;
			return tmp += n;
		}
		base_iterator operator-(const int &n) const {
			base_iterator tmp = *this;
			return tmp -= n;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const base_iterator &rhs) const {
			if(self != rhs.self) throw invalid_iterator;
			return idx - rhs.idx;
		}
		base_iterator operator+=(const int &n) {
			idx += n;
			int total = 0;
			for(block_pointer i = self->head; i != self->tail; i = i->nxt) {
				if(idx < total + i->sz) {
					node = i;
					first = node->data[0];
					last = node->data[node->sz];
					cur = idx - total;
					return *this;
				}
				total += i->sz;
			}
			node = self->tail;
			first = last = node->data[0];
			cur = 0;
			return *this;
		}
		base_iterator operator-=(const int &n) {
			return *this += -n;
		}
		/**
		 * iter++
		 */
		base_iterator operator++(int) {
			base_iterator tmp = *this;
			++*this;
			return tmp;
		}
		/**
		 * ++iter
		 */
		base_iterator& operator++() {
			++cur;
			++idx;
			if(cur == node->sz && node->nxt) {
				set_node(node->nxt);
				cur = 0;
			}
			return *this;
		}
		/**
		 * iter--
		 */
		base_iterator operator--(int) {
			base_iterator tmp = *this;
			--*this;
			return tmp;
		}
		/**
		 * --iter
		 */
		base_iterator& operator--() {
			if(cur == 0 && node->pre) {
				set_node(node->pre);
				cur = node->sz;
			}
			--cur;
			--idx;
			return *this;
		}
		/**
		 * *it
		 */
		Ref operator*() const {
			if(idx < 0 || idx >= self->sz) throw runtime_error();
			return *node->data[cur];
		}
		/**
		 * it->field
		 */
		Poi operator->() const noexcept {
			if(idx < 0 || idx >= self->sz) throw runtime_error();
			return node->data[cur];
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const base_iterator &rhs) const {
			return self == rhs.self && idx == rhs.idx;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const base_iterator &rhs) const {
			return !(*this == rhs);
		}
	};
	
	/**
	 * Constructors
	 */
	deque(): sz(0) {
		head = new block();
		tail = new block();
		link(head, tail);
	}
	deque(const deque &other) {
		head = new block();
		*head = *other.head;
		block_pointer now = head;
		for(block_pointer i = other.head; i != other.tail; i = i->nxt) {
			block_pointer tmp = new block();
			*tmp = *i->nxt;
			link(now, tmp);
			now = tmp;
		}
		tail = now;
		sz = other.sz;
	}
	/**
	 * Deconstructor
	 */
	~deque() {
		block_pointer now = head, tmp;
		while(now != tail)
		{
			tmp = now->nxt;
			for(int i = 0; i < now->sz; i++)
				delete now->data[i];
			delete now;
			now = tmp;
		}
		delete now;
	}
	/**
	 * assignment operator
	 */
	deque& operator=(const deque &other) {
		if(this == &other) return *this;
		clear();
		delete tail;
		*head = *other.head;
		block_pointer now = head;
		for(block_pointer i = other.head; i != other.tail; i = i->nxt) {
			block_pointer tmp = new block();
			*tmp = *i->nxt;
			link(now, tmp);
			now = tmp;
		}
		tail = now;
		sz = other.sz;
		return *this;
	}

	deque& operator=(const deque &&other) {
#ifdef LOCAL
		std::cerr << "moving" << std::endl;
#endif
		if(this == &other) return *this;
		clear(); delete head; delete tail;
		block_pointer now = head;
		for(block_pointer i = other.head; i; i = i->nxt) {
			now = i;
#ifdef LOCAL
			assert(now->sz == i->sz);
#endif
			for(int j = 0; j < i->sz; j++) {
				now->data[j] = i->data[j];
				i->data[j] = 0;
			}
			now = now->nxt;
		}
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T& at(const size_t &pos) {
		if(pos < 0 || pos >= sz) throw index_out_of_bound();
		int total = 0;
		for(block_pointer p = head; p != tail; p = p->nxt) {
			if(pos < total + p->sz) {
				return *p->data[pos - total];
			}
			total += p->sz;
		}
		throw index_out_of_bound();
	}
	const T& at(const size_t &pos) const {
		if(pos < 0 || pos >= sz) throw index_out_of_bound();
		int total = 0;
		for(block_pointer p = head; p != tail; p = p->nxt) {
			if(pos < total + p->sz) {
				return *p->data[pos - total];
			}
			total += p->sz;
		}
		throw index_out_of_bound();
	}
	T& operator[](const size_t &pos) {
		return at(pos);
	}
	const T& operator[](const size_t &pos) const {
		return at(pos);
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T& front() const {
		if(!sz) throw container_is_empty();
		return *head->data[0];
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T& back() const {
		if(!sz) throw container_is_empty();
		return *tail->pre->data[tail->pre->sz - 1];
	}
	iterator begin() {
		return iterator(this, head, 0, 0);
	}
	const_iterator cbegin() const {
		return const_iterator(this, head, 0, 0);
	}
	iterator end() {
		return iterator(this, tail, 0, sz);
	}
	const_iterator cend() const {
		return const_iterator(this, tail, 0, sz);
	}
	bool empty() const { return sz == 0; }
	size_t size() const { return sz; }
	inline void clear() {
		block_pointer now = head, tmp;
		while(now != tail)
		{
			tmp = now->nxt;
			for(int i = 0; i < now->sz; i++)
				delete now->data[i];
			now->sz = 0;
			if(now != head) delete now;
			now = tmp;
		}
		link(head, tail);
		sz = 0;
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {
		if(pos.self != this) throw invalid_iterator();
		if(pos == begin()) {
			push_front(value);
			return begin();
		} else if(pos == end()) {
			push_back(value);
			iterator tmp = end();
			return --tmp;
		} else {
			return insert_aux(pos, value);
		}
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {
		if(sz == 0) throw container_is_empty();
		if(pos.self != this) throw invalid_iterator();
		int idx = pos - begin();
		if(idx < 0 || idx >= sz) throw invalid_iterator();
		if(pos == begin()) {
			pop_front();
			return begin();
		} else if(pos == --end()) {
			pop_back();
			return end();
		} else {
			return erase_aux(pos);
		}
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {
		++sz;
		block_pointer tmp = tail->pre;
		tmp->data[tmp->sz++] = new T(value);
		if(tmp->sz >= sup)
			link(split(tmp), tail);
	}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {
		if(sz == 0) throw container_is_empty();
		--sz;
		block_pointer tmp = tail->pre;
		delete tmp->data[--tmp->sz];
		if(tmp->pre && (tmp->sz == 0 || tmp->sz + tmp->pre->sz < inf))
			merge(tmp->pre, tmp);
	}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {
		++sz;
		if(--head->data.start < 0)
			head->data.start += sup;
		head->data[0] = new T(value);
		if(++head->sz >= sup) {
			split(head);
		}
	}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {
		if(sz == 0) throw container_is_empty();
		--sz;
		delete head->data[0];
		if(++head->data.start >= sup)
			head->data.start -= sup;
		head->sz--;
		if(head->nxt != tail && (head->sz == 0 || head->sz + head->nxt->sz < inf))
			merge(head, head->nxt);
	}

private:

	int sz;
	block_pointer head, tail;

	void link(block_pointer a, block_pointer b) {
		if(a) a->nxt = b;
		if(b) b->pre = a;
	}

	void merge(block_pointer a, block_pointer b) {
		if(a->sz > b->sz) {
			for(int i = 0; i < b->sz; i++)
				a->data[a->sz++] = b->data[i];
			link(a, b->nxt);
			delete b;
		} else {
			for(int i = 0; i < a->sz; i++)
				b->data[b->sz++] = a->data[i];
			link(a->pre, b);
			if(a == head) head = b;
			delete a;
		}
	}

	block_pointer split(block_pointer a) {
		int cc = a->sz / 2;
		block_pointer b = new block();
		for(int i = cc; i < a->sz; i++) {
			b->data[b->sz++] = a->data[i];
		}
		link(b, a->nxt);
		link(a, b);
		a->sz = cc;
		return b;
	}

	iterator insert_aux(iterator pos, const T &value) {
		++sz;
		int p = pos.idx;

#ifdef LOCAL
		assert(pos.last == pos.node->data[pos.node->sz]);
#endif
		pos.node->data[pos.node->sz] = new T(*pos.node->data[pos.node->sz - 1]);
		for(int t = pos.node->sz - 1; t != pos.cur; --t) {
			*pos.node->data[t] = *pos.node->data[t - 1];
		}
		*pos = value;
		++pos.node->sz;
		pos.last = pos.node->data[pos.node->sz];
		if(pos.node->sz >= sup) {
			split(pos.node);
		}
		return begin() + p;
	}

	iterator erase_aux(iterator pos) {
		--sz;
		int p = pos.idx;
		for(int t = pos.cur; t != pos.node->sz - 1; ++t) {
			*pos.node->data[t] = *pos.node->data[t + 1];
		}
		delete pos.node->data[--pos.node->sz];
		pos.last = pos.node->data[pos.node->sz];
		if(pos.node->pre && (pos.node->sz == 0 || pos.node->pre->sz + pos.node->sz < inf)) {
			merge(pos.node->pre, pos.node);
		} else if(pos.node->nxt != tail && (pos.node->sz == 0 || pos.node->sz + pos.node->nxt->sz < inf)) {
			merge(pos.node, pos.node->nxt);
		}
		return begin() + p;
	}
};

}

#endif
