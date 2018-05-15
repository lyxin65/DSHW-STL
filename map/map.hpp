/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<class T>
inline T max(const T &x, const T &y) {
	return x > y ? x : y;
}

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;

	struct node // node for avl
	{
		typedef node* pointer;

		value_type *val;
		pointer lc, rc, pre, nxt;
		int h, sz;

		node(): val(0), lc(0), rc(0), pre(0), nxt(0), h(-1), sz(0) {}
		node(const value_type &_val): lc(0), rc(0), h(0), sz(1) {
			val = new value_type(_val);
		}
		node(const node &o): lc(o.lc), rc(o.rc), pre(o.pre), nxt(o.nxt), h(o.h), sz(o.sz) {
			val = new value_type(*o.val);
		}
		~node() {
			delete val;
		}
		int balance() const;
		void update();
	};

	int get_height(const node *o) {
		return o ? o->h : -1;
	}

	int get_size(const node *o) {
		return o ? o->sz : 0;
	}

	int node::balance() const {
		return get_height(lc) - get_height(rc);
	}

	void node::update() {
		sz = get_size(lc) + get_size(rc) + 1;
		h = max(get_height(lc) + get_height(rc)) + 1;
	}

	template<class, class> class base_iterator;
	typedef base_iterator<value_type&, value_type*> iterator;
	typedef base_iterator<const value_type&, const value_type*> const_iterator;
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	template<class reference, class pointer>
	class base_iterator {
		friend class map;
	private:
		map *self;
		node *data;

	public:
		base_iterator(): self(0), data(0) {}
		base_iterator(const map *self, node *data): self(self), data(data) {}
		base_iterator(const base_iterator &other): self(other.self), data(other.data) {}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/**
		 * iter++
		 */
		base_iterator operator++(int) {
			base_iterator tmp = *this;
			++(*this);
			return tmp;
		}
		/**
		 * ++iter
		 */
		base_iterator& operator++() {
			if (!p || !p->nxt) throw invalid_iterator(); 
			p = p->pre;
			return *this;
		}
		/**
		 * iter--
		 */
		base_iterator operator--(int) {
			base_iterator tmp = *this;
			--(*this);
			return tmp;
		}
		/**
		 * --iter
		 */
		base_iterator& operator--() {
			if (!p || !p->pre) throw invalid_iterator();
			 return *this;
		}
		/**
		 * some other operator for iterator.
		 */
		reference operator*() const {
			return *data->val;
		}
		pointer operator->() const noexcept {
			return data->val;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const base_iterator &rhs) const {
			return self == rhs.self && data == rhs.data;
		}
		bool operator!=(const base_iterator &rhs) const {
			return !(*this == rhs);
		}

	};
	/**
	 * two constructors
	 */
	map(): root(0), sz(0) {
		st = ed = new node();
	}
	map(const map &o) {
		if (!o.root) {
			st = ed = new node();
		} else {
			ed = new node();
			copy(root, o->root);
			st = left_most();
			link(right_most(), ed);
		}
	}
	/**
	 * assignment operator
	 */
	map& operator=(const map &o) {
		if (this == &o) {
			return *this;
		}
		clear();
		if (o.root) {
			copy(root, o->root);
			st = left_most();
			link(right_most(), ed);
		}
		return *this;
	}
	/**
	 * Destructors
	 */
	~map() {
		clear();
		delete ed;
	}
	/**
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T& at(const Key &key) {
		node *o = find(key);
		if (!o) throw index_out_of_bound();
		return o->val->second;
	}
	const T& at(const Key &key) const {
		node *o = find(key);
		if (!o) throw index_out_of_bound();
		return o->val->second;
	}
	/**
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
		node *o = insert(value_type(key, T())).first.data;
		return o->val->second;
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
		return at(key);
	}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {
		return iterator(this, st);
	}
	const_iterator cbegin() const {
		return const_iterator(const_cast<map*>(this), st);
	}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {
		return iterator(this, ed);
	}
	const_iterator cend() const {
		return const_iterator(const_cast<map*>(this), ed);
	}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {
		return !sz;
	}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {
		return sz;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		make_empty(root);
		root = 0;
		ed->pre = 0;
		st = ed;
	}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		auto res = insert(root, value);
		return pair<iterator, bool>(iterator(this, res.first), res.second);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
		if (pos.self != this) throw invalid_iterator();
		if (!pos.data || !pos.data->val) throw invalid_iterator(); // end()
		remove(root, pos.data->val->first);
	}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {
		return size_t(find(root, key) ? 1 : 0);
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
		node *o = find(root, key);
		if (!o) return end();
		return iterator(this, o);
	}
	const_iterator find(const Key &key) const {
		node *o = find(root, key);
		if (!o) return cend();
		return const_iterator(const_cast<map*>(this), o);
	}
	/*
	   Find the element position at k-th if sorted
	   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator rank(int k) {
		node *t = get_kth(root, k);
		if (!t) return end();
		return iterator(this, t);
	}
	const_iterator rank(int k) const {
		node *t = get_kth(root, k);
		if (!t) return cend();
		return const_iterator(const_cast<map*>(this), t);
	}

private:
	node *root;
	size_t sz;
	Compare cmp;

	void link(node *l, node *r) {
		if (l) l->nxt = r;
		if (r) r->pre = l;
	}
	/*
	   TODO
	   behave like stl_tree.hpp
	   (maybe faster)
	 */
	void copy(node *&o, const node *other) {
		if (!other) return;
		o = new node(*other->val);
		copy(o->lc, other->lc);
		copy(o->rc, other->rc);
	}
	/*
	   clear all data
	   behave like stl_tree.hpp
	 */
	void make_empty(node* o) {
		node *p;
		while (o) {
			if (o->lc)
				make_empty(o->lc);
			p = o;
			o = o->rc;
			delete p;
		}
	}
	node* left_most() {
		node *o = root;
		while (o->lc)
			o = o->lc;
		return o;
	}
	node* right_most() {
		node *o = root;
		while (o->rc)
			o = o->rc;
		return o;
	}
	/*
	   return pointer to key
	   if cannot find, return NULL
	 */
	node* find(node *o, const Key &key) const {
		while (o) {
			if (cmp(key, o->val->first))
				o = o->lc;
			else if (!cmp(key, o->val->first))
				o = o->rc;
			else 
				break;
		}
		return o;
	}
	void LL(node *&o) { // left rotate
		node *k = o->lc;
		o->lc = k->rc;
		k->rc = o;
		o->update();
		k->update();
		o = k;
	}
	void RR(node *&o) { // right rotate
		node *k = o->rc;
		o->rc = k->lc;
		k->lc = o;
		o->update();
		k->update();
		o = k;
	}
	inline void LR(node *&o) {
		RR(o->lc);
		LL(o);
	}
	inline void RL(node *&o) {
		LL(o->rc);
		RR(o);
	}

	node *get_kth(node *o, int k) const {
		if (get_size(o) < k) return 0;
		int s = get_size(o->lc);
		if (k == s + 1)
			return o;
		else if (k <= s)
			return get_kth(o->lc, k);
		else
			return get_kth(o->rc, k - s - 1);
	}

	pair<node*, bool> insert(node *&o, const value_type &value);
	bool remove(node *&o, const Key &key);
};

template<class Key, class T, class Compare>
pair<node*, bool> map<Key, T, Compare>::insert(node *&o, const value_type &value) {
	pair<node*, bool> res;
	if (!o) {
		o = new node(value);
		return pair<node*, bool>(o, true);
	} else if (cmp(value.first, o->val->first)) {
		res = insert(o->lc, value);
		if (res.second == false)
			return res;
		if (o->lc == res.first) {
			if (st == o)
				st = o->lc;
			link(o->pre, o->lc);
			link(o->lc, o);
		}
		if (o->balance() == 2) {
			if (cmp(value.first, o->lc->val->first)) {
				LL(o);
			} else {
				LR(o);
			}
		}
	} else if (!cmp(value.first, o->val->first)) {
		res = insert(o->rc, value);
		if (res.second == false)
			return res;
		if (o->rc == res.first) {
			link(o->rc, o->nxt);
			link(o, o->rc);
		}
		if (o->balance() == -2) {
			if (cmp(o->rc->val->first, value.first)) {
				RR(o);
			} else {
				RL(o);
			}
		}
	} else {
		res.first = o;
		res.second = false;
	}
	o->update();
	return res;
}

// rempve node by key
// return true if the tree don't need adjust
template<class Key, class T, class Compare>
bool map<Key, T, Compare>::remove(node *&o, const Key &key) {
	if (!o) throw runtime_error();
	if (cmp(key, o->val->first)) {
		if (remove(o->lc, key))
			return true;
		else
			return adjust(o, 0);
	} else if (!cmp(key, o->val->first)) {
		if (remove(o->rc, key))
			return true;
		else
			return adjust(o, 1);
	} else {
		if (o->lc && o->rc) {
			node *t = o->nxt;
			o->val = t->val;
			t->pre = o->pre;
			t->nxt = o->nxt;
			if (remove(o->rc, t->val->first))
				return true;
			return adjust(o, 1);
		} else {
			node *old = o;
			if (st == o)
				st = o->nxt;
			link(o->pre, o->nxt);
			if (o->lc)
				o = o->lc;
			else
				o = o->rc;
			delete old;
			return false;
		}
	}
}

template<class Key, class T, class Compare>
bool map<Key, T, Compare>::adjust(node *o, bool is_right) {
	if (is_right) {
		if (o->balance() == 1) return true; // type a
		if (o->balance() == 0) return o->h--, false; // type b
		if (o->lc->balance() < 0) return LR(o), false;
		LL(o);
		return o->balance() != 0;
	} else {
		if (o->balance() == -1) return true;
		if (o->balance() == 0) return o->h--, false;
		if (o->rc->balance() > 0) return RL(o), false;
		RR(o);
		return o->balance() != 0;
	}
}

#endif
