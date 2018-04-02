#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

template<typename T>
void swap(T &x, T &y) {
	T z = x;
	x = y;
	y = z;
}

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 * it should be based on the vector written by yourself.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	int sz;

	struct Node {
		T val;
		Node *lc, *rc;

		Node(T val, Node *lc, Node *rc): val(val), lc(lc), rc(rc) {}

		~Node() {
			if(lc) delete lc;
			if(rc) delete rc;
		}

		Node(const Node *other) {
			val = other->val;
			lc = rc = nullptr;
			if(other->lc) lc = new Node(other->lc);
			if(other->rc) rc = new Node(other->rc);
		}
	};

	Node *root;
public:
	/**
	 * constructors
	 */
	priority_queue(): sz(0), root(nullptr) {}
	priority_queue(const priority_queue &other) {
		sz = other.sz;
		if(other.root) {
			root = new Node(other.root);
		} else {
			root = nullptr;
		}
	}
	/**
	 * deconstructor
	 */
	~priority_queue() {
		if(root) {
			delete root;
		}
	}
	/**
	 * Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) {
		if(other.root == root) {
			return *this;
		}
		if(root) {
			delete root;
		}
		if(other.root) {
			root = new Node(other.root);
		} else {
			root = nullptr;
		}
		sz = other.sz;
		return *this;
	}
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const {
		if(sz == 0) {
			throw new container_is_empty;
		}
		return root->val;
	}
	/**
	 * TODO
	 * push new element to the priority queue.
	 */
	void push(const T &e) {
		if(sz == 0) {
			root = new Node(e, nullptr, nullptr);
		} else {
			Node *t = new Node(e, nullptr, nullptr);
			root = __merge(root, t);
		}
		sz++;
	}
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() {
		if(sz == 0) {
			throw new container_is_empty;
		}
		Node *t = root;
		root = __merge(root->lc, root->rc);
		t->lc = t->rc = nullptr;
		delete t;
		sz--;
	}
	/**
	 * return the number of the elements.
	 */
	size_t size() const {
		return sz;
	}
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const {
		return sz == 0;
	}
	/**
	 * return a merged priority_queue with at least O(logn) complexity.
	 */
	void merge(priority_queue &other) {
		sz += other.sz;
		root = __merge(root, other.root);
		other.root = nullptr;
		other.size = 0;
	}

private:
	Node *__merge(Node *a, Node *b) {
		if(!a) return b;
		if(!b) return a;
		static Compare c;
		if(!c(a->val, b->val)) {
			return solve(a, b);
		} else {
			return solve(b, a);
		}
	}
	Node *solve(Node *a, Node *b)
	{
		if(!a->lc) {
			a->lc = b;
		} else {
			a->rc = __merge(a->rc, b);
			swap(a->lc, a->rc);
		}
		return a;
	}
};

}

#endif
