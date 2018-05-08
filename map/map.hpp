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

inline int rnd() {
	static unsigned seed = 6791654;
	seed = seed * 998244353 + 21474;
	return seed & ((1 << 31) - 1);
}

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
private:
	struct Node {
		int r;
		Node *lc, *rc, *fa;
		value_type *val;
		Node(const value_type *&val = nullptr, const Node *&lc = nullptr, const Node *&rc = nullptr, const Node *&fa = nullptr): t(t), lc(lc), rc(rc), fa(fa) {
			r = rnd();
		}

	} *root;

	size_t sz;

	Node *vir_node() {
		Node *t = new Node();
		return t;
	}

	Node *find_pre(Node *o) {
		if(lc) {
			o = o->lc;
			while(o->rc) {
				o = o->rc;
			}
			return o;
		} else {
			while(o->fa && o->fa->rc != o) {
				o = o->fa;
			}
			if(o->fa == nullptr)
				throw invalid_iterator();
			return o->fa;
		}
	}

	Node *find_nxt(Node *o)
	{
		if(rc) {
			o = o->rc;
			while(o->lc) {
				o = o->lc;
			}
			return o;
		} else {
			while(o->fa && o->fa->lc != o) {
				o = o->fa;
			}
			if(o->fa == nullptr)
				throw invalid_iterator();
			return o->fa;
		}
	}

public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	class const_iterator;
	class iterator {
		friend class const_iterator;
	private:
		Node *o;

	public:
		iterator(const Node *&o): o(o) {}
		iterator(const iterator &other): o(other.o) {}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/**
		 * iter++
		 */
		iterator operator++(int) {
			iterator tmp = *this;
			++(*this);
			return tmp;
		}
		/**
		 * ++iter
		 */
		iterator & operator++() {
			o = find_nxt(o);
			return *this;
		}
		/**
		 * iter--
		 */
		iterator operator--(int) {
			iterator tmp = *this;
			--(*this);
			return tmp;
		}
		/**
		 * --iter
		 */
		iterator & operator--() {
			 o = find_nxt(o);
			 return *this;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {
			return *o->val;
		}
		bool operator==(const iterator &rhs) const {
			return o == rhs.o;
		}
		bool operator==(const const_iterator &rhs) const {
			return o == rhs.o;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(*this == rhs);
		}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {
			return o->val;
		}
	};
	class const_iterator {
		friend class iterator;
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
	private:
		Node *o;
	public:
		const_iterator(const Node *&o): o(o) {}
		const_iterator(const const_iterator &other): o(other.o) {}
		const_iterator(const iterator &other): o(other.o) {}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/**
		 * iter++
		 */
		const_iterator operator++(int) {
			const_iterator tmp = *this;
			++(*this);
			return tmp;
		}
		/**
		 * ++iter
		 */
		const_iterator & operator++() {
			o = find_nxt(o);
			return *this;
		}
		/**
		 * iter--
		 */
		const_iterator operator--(int) {
			const_iterator tmp = *this;
			--(*this);
			return tmp;
		}
		/**
		 * --iter
		 */
		const_iterator & operator--() {
			 o = find_nxt(o);
			 return *this;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		const value_type & operator*() const {
			return *o->val;
		}
		bool operator==(const iterator &rhs) const {
			return o == rhs.o;
		}
		bool operator==(const const_iterator &rhs) const {
			return o == rhs.o;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(*this == rhs);
		}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		const value_type* operator->() const noexcept {
			return o->val;
		}
	};
	/**
	 * TODO two constructors
	 */
	map(): root(vir_node()), sz(0) {}
	map(const map &other) {
		sz = other.sz;
		if(other.root) {
			root = new Node(other.root);
		} else {
			throw WTF();
		}
	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {
		if(root == other.root) {
			return;
		}
		if(root) {
			delete root;
		}
		sz = other.sz;
		if(other.root) {
			root = new Node(other.root);
		} else {
			throw WTF();
		}
	}
	/**
	 * TODO Destructors
	 */
	~map() {
		if(root) {
			delete root;
		}
	}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
		Node *o = find(key);
		if(o == nullptr)
			throw index_out_of_bound();
		return o->val->second;
	}
	const T & at(const Key &key) const {
		Node *o = find(key);
		if(o == nullptr)
			throw index_out_of_bound();
		return o->val->second;
	}
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {}
	const_iterator cbegin() const {}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {}
	const_iterator cend() const {}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {}
	/**
	 * clears the contents
	 */
	void clear() {}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {}
	const_iterator find(const Key &key) const {}
};

}

#endif
