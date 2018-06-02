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
	class iterator;
	class const_iterator;
	class Tree;
	class Node {
		friend class map<Key,T,Compare>;
		friend class Tree;
		friend class iterator;
		friend class const_iterator;
		value_type *data;
		Node *left;
		Node *right;
		Node *prev, *next;
		Node *point;
		int height;

		Node() { data = nullptr; }
		Node(const value_type &_data, Node *lt = nullptr, Node *rt = nullptr, Node *_pre = nullptr, Node *_next = nullptr, int h = 1)
			:left(lt), right(rt), prev(_pre), next(_next), height(h) {
			data = new value_type(_data);
		 };
		//const???!!!
		Node(const Node &other) {
			data = new value_type(*other.data);
			height = other.height;
			//other.point = this;    //不知道可不可行？？
			if (other.left != nullptr) left = new Node(*other.left); else left = nullptr;
			if (other.right != nullptr) right = new Node(*other.right); else right = nullptr;
		};
		~Node() { if (data != nullptr) delete data; }
	};

	class Tree {
		Node *root;
		Node *head, *tail;
		int size;
	public:
		friend class map<Key, T, Compare>;
		Tree() { 
			root = nullptr; size = 0; 
			head = new Node; tail = new Node; 
			head->next = tail; tail->prev = head;
		}
		//需要重新将树串成链
		Tree(const Tree &other) {
			head = new Node; tail = new Node;
			//other.head->point = head; 
			//other.tail->point = tail;
			if (other.size > 0)
			{
				root = new Node(*other.root);
				size = other.size;
			}
			else {
				root = nullptr;
				size = 0;
			}

			Node** p = new Node*[size + 5]{ nullptr };
			size_t count = 1;
			dfs(root, count, p);
			p[0] = head;  p[size + 1] = tail;
			size_t i = 0;
			for (; i <= size; i++) {
				p[i]->next = p[i + 1];
				p[i + 1]->prev = p[i];
			}
		    

			delete[]p;



			//node *p[];  p = new node[size];

			//head->next = other.head->next->point;
			//tail->prev = other.tail->prev->point;

			//Node *tmp = other.head;
			//while (tmp->next != other.tail)
			//{
			//	tmp = tmp->next;
			//	Node *now = tmp->point;
			//	now->prev = tmp->prev->point;
			//	now->next = tmp->next->point;
			//}
		}

		void dfs(Node* now, size_t &count, Node**p)//记目前对象；
		{
			if (now->left != nullptr)
				dfs(now->left, count, p);
			p[count] = now;
			count++;
			if (now->right != nullptr)
				dfs(now->right, count, p);
		}

		~Tree()
		{
			makeempty();
			delete head; 
			delete tail;	
		}

		void makeempty()
		{
			size = 0;
			Node *tmp = head->next;
			while (tmp != tail)
			{
				Node *now = tmp->next;
				delete tmp;
				tmp = now;
			}
			root = nullptr;
		}

		//比较是否key值相同            不知道是不是这么写？？？？
		bool Issamekey(const Key &x,const Key &y) const
		{
			if (!(Compare()(x, y) || Compare()(y, x))) return true;
			else return false;
		}

		int max(int a, int b) { return (a > b) ? a : b; }
		int height(Node *t) const { return t == nullptr ? 0 : t->height; }

		//寻找key值为x的那个，如果不存在，返回nullptr
		Node *find(const Key &x) const
		{
			Node *tmp = root;
			while (tmp != nullptr && !(Issamekey(x, tmp->data->first)))
			{
				if (Compare()(x, tmp->data->first)) tmp = tmp->left;
				else tmp = tmp->right;
			}
			if (tmp == nullptr) return nullptr;
			else return tmp;
		}

		//四种旋转方式
		void LL(Node *&t)
		{
			Node *t1 = t->left;
			t->left = t1->right;
			t1->right = t;
			t->height = max(height(t->left), height(t->right)) + 1;
			t1->height = max(height(t1->left), height(t)) + 1; 
			t = t1;
		}

		void RR(Node *&t)
		{
			Node *t1 = t->right;
			t->right = t1->left;
			t1->left = t;
			t->height = max(height(t->left), height(t->right)) + 1;
			t1-> height = max(height(t1->right), height(t)) + 1;
			t = t1;
		}

		void LR(Node *&t)
		{
			RR(t->left);
			LL(t);
		}

		void RL(Node *&t)
		{
			LL(t->right);
			RR(t);
		}
		//!!!
		void findpre(Node *&ans,Key val)
		{
			Node *tmp = root;
			ans = head;
			while (tmp != nullptr)
			{
				if (Compare()(tmp->data->first, val))
				{
					ans = tmp;
					tmp = tmp->right;
				}
				else
				{
					tmp = tmp->left;
				}
			}
		}


		//插入操作
		void insert(const value_type &x)
		{
			insert(x, root);
		}

		void insert(const value_type &x, Node *&t)
		{
			if (t == nullptr)
			{
				t = new Node(x);
				size = size + 1;
				
				Node *ans;
				findpre(ans,x.first);
				t->next = ans->next;
				ans->next->prev = t;
				t->prev = ans;
				ans->next = t;
			}
			else if (Compare()(x.first, t->data->first))
			{
				insert(x, t->left);
				if (height(t->left) - height(t->right) == 2)
					if (Compare()(x.first, t->left->data->first)) LL(t); else LR(t);
			}
			else if (Compare()(t->data->first, x.first))
			{
				insert(x, t->right);
				if (height(t->right) - height(t->left) == 2)
					if (Compare()(t->right->data->first, x.first)) RR(t); else RL(t);
			}
			t->height = max(height(t->left), height(t->right)) + 1;
		}

		//删除操作
		void remove(const Key &x)
		{
			remove(x, root);
		}

		bool remove(const Key &x,Node *&t)
		{
			if (t == nullptr) return true;
			if (Issamekey(x, t->data->first))
			{
				if (t->left == nullptr || t->right == nullptr)
				{
					Node *p = t;
					t = (t->left != nullptr) ? t->left : t->right;

					p->prev->next = p->next; 
					p->next->prev = p->prev;
					size--;
					delete p;
					return false;
				}
				else
				{
					Node *tmp = t->right; Node *father = t;
					//flag == 0 表示是右儿子 否则为左儿子
					int flag = 0;
					while (tmp->left != nullptr)
					{
						flag = 1;
						father = tmp;
						tmp = tmp->left;
					}
					//t->data->first = tmp->data->first;
					//t->data->second = tmp->data->second;
			
					if (flag == 0)
					{
						tmp->left = t->left; t->left = nullptr;
						t->right = tmp->right; tmp->right = t;
						int o = tmp->height; tmp->height = t->height; t->height = o;
						t = tmp;
						if (remove(t->right->data->first, t->right)) return true;
						return adjust(t, 1);
					}
					else
					{
						father->left = t;
						tmp->left = t->left; t->left = nullptr;
						Node *op = tmp->right; tmp->right = t->right; t->right = op; // swap(tmp->right, t->right)
						int o = tmp->height; tmp->height = t->height; t->height = o; // swap(tmp->height, t->height)
						op = t;  t = tmp; tmp = op; // swap(tmp, t)
						if (remove(tmp->data->first, t->right)) return true;
						return adjust(t, 1);
					}
					//交换两者的空间 data left right height
					
					
					
  				}
			}
			
			if (Compare()(x, t->data->first))
			{
				if (remove(x, t->left)) return true;
				return adjust(t, 0);
			}
			else
			{
				if (remove(x, t->right)) return true;
				return adjust(t, 1);
			}
		}

		bool adjust(Node *&t, int subTree)
		{
			if (subTree) {
				if (height(t->left) - height(t->right) == 1) return true;
				if (height(t->right) == height(t->left)) { --t->height; return false; }
				if (height(t->left->right) > height(t->left->left))
				{
					LR(t);
					return false;
				}
				LL(t);
				if (height(t->right) == height(t->left)) return false; else return true;
			}
			else
			{
				if (height(t->right) - height(t->left) == 1) return true;
				if (height(t->right) == height(t->left)) { --t->height;  return false; }
				if (height(t->right->left) > height(t->right->right))
				{
					RL(t);
					return false;
				}
				RR(t);
				if (height(t->right) == height(t->left)) return false; else return true;
			}
		}

		void dfs(Node *p)
		{
			
			if (p->left != nullptr) dfs(p->left);
			if (p->right != nullptr) dfs(p->right);
		}
		
	};
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	class const_iterator;
	class iterator {
		friend class const_iterator;;
		friend class map<Key, T, Compare>;
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		Tree * fa;
		Node *p;
		
	public:
		iterator() {
			p = nullptr; fa = nullptr;
		}
		iterator(Tree *_fa,Node *_p):fa(_fa),p(_p) {}
		iterator(const iterator &other) {
			p = other.p; fa = other.fa;
		}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator tmp = *this;
			if (p == fa->tail) throw index_out_of_bound();
			p = p->next;
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator & operator++() {
			if (p == fa->tail) throw index_out_of_bound();
			p = p->next;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator tmp = *this;
			if (p == fa->head->next) throw index_out_of_bound();
			p = p->prev;
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator & operator--() {
			if (p == fa->head->next) throw index_out_of_bound();
			p = p->prev;
			return *this;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const { return *p->data; }
		bool operator==(const iterator &rhs) const {
			return (p == rhs.p && fa == rhs.fa);
		}
		bool operator==(const const_iterator &rhs) const {
			return (p == rhs.p && fa == rhs.fa);
		}
		bool operator!=(const iterator &rhs) const {
			return (p != rhs.p || fa != rhs.fa);
		}
		bool operator!=(const const_iterator &rhs) const {
			return (p != rhs.p || fa != rhs.fa);
		}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {
			return p->data;
		}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			Tree * fa;
			Node * p;
			friend class iterator;;
			friend class map<Key, T, Compare>;
		public:
			const_iterator() {
				p = nullptr;
				fa = nullptr;
			}
			const_iterator(const const_iterator &other) {
				p = other.p;
				fa = other.fa;
			}
			const_iterator(const iterator &other) {
				p = other.p;
				fa = other.fa;
			}
			const_iterator(Tree *_fa, Node *_p) :fa(_fa), p(_p) {}
			const_iterator operator++(int) {
				const_iterator tmp = *this;
				if (p == fa->tail) throw index_out_of_bound();
				p = p->next;
				return tmp;
			}

			const_iterator & operator++() {
				if (p == fa->tail) throw index_out_of_bound();
				p = p->next;
				return *this;
			}
			const_iterator operator--(int) {
				const_iterator tmp = *this;
				if (p == fa->head->next) throw index_out_of_bound();
				p = p->prev;
				return tmp;
			}
			const_iterator & operator--() {
				if (p == fa->head->next) throw index_out_of_bound();
				p = p->prev;
				return *this;
			}

			value_type & operator*() const { return *p->data; }
			bool operator==(const iterator &rhs) const {
				return (p == rhs.p && fa == rhs.fa);
			}
			bool operator==(const const_iterator &rhs) const {
				return (p == rhs.p && fa == rhs.fa);
			}
			bool operator!=(const iterator &rhs) const {
				return (p != rhs.p || fa != rhs.fa);
			}
			bool operator!=(const const_iterator &rhs) const {
				return (p != rhs.p || fa != rhs.fa);
			}
			value_type* operator->() const noexcept {
				return p->data;
			}
	};
	Tree *splay;
	/**
	 * TODO two constructors
	 */
	
	void  show()
	{
		splay->dfs(splay->root);
	}

	map() {
		splay = new Tree();
	}
	map(const map &other) {
		if (this == &other) return;
		splay = new Tree(*other.splay);
	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {
		if (this == &other) return *this;
		if (splay != nullptr) delete splay;
		splay = new Tree(*other.splay);
		return *this;
	}
	~map() {
		if (splay != nullptr) delete splay;
	}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
		Node *tmp = splay->find(key);
		if (tmp == nullptr) throw index_out_of_bound();
		else return tmp->data->second;
	}
	const T & at(const Key &key) const {
		Node *tmp = splay->find(key);
		if (tmp == nullptr) throw index_out_of_bound();
		else return tmp->data->second;
	}
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	
	 //？？？！！！！
	T & operator[](const Key &key) {
		Node *tmp = splay->find(key);
		if (tmp == nullptr)
		{
			splay->insert(value_type(key, T()));
			tmp = splay->find(key);
		}
		return tmp->data->second;
	}

	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
		Node *tmp = splay->find(key);
		if (tmp == nullptr) throw index_out_of_bound();
		else return tmp->data->second;
	}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {
		return iterator(splay,splay->head->next);
	}
	const_iterator cbegin() const {
		return const_iterator(splay,splay->head->next);
	}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {
		return iterator(splay,splay->tail);
	}
	const_iterator cend() const {
		return const_iterator(splay, splay->tail);
	}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {
		if (splay->size == 0)  return true; else return false;
	}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {
		return splay->size;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		if(splay == nullptr) return;
		if(splay->size == 0) return;
		splay->makeempty();
		splay->head->next = splay->tail;
		splay->tail->prev = splay->head;
	}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		Node *tmp = splay->find(value.first);
		if (tmp == nullptr)
		{
			splay->insert(value);
			tmp = splay->find(value.first);
			return pair<iterator, bool>(iterator(splay,tmp), true);
		}
		else return pair<iterator, bool>(iterator(splay,tmp), false);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
		if (pos.fa != splay || pos.p == splay->tail) throw index_out_of_bound();
		splay->remove(pos.p->data->first);
	}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {
		if (splay->size == 0) return 0;
		Node *tmp = splay->find(key);
		if (tmp == nullptr) return 0;
		else return 1;
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
		Node *tmp = splay->find(key);
		if (tmp == nullptr) return iterator(splay, splay->tail);
		else return iterator(splay, tmp);
	}
	const_iterator find(const Key &key) const {
		Node *tmp = splay->find(key);
		if (tmp == nullptr) return const_iterator(splay, splay->tail);
		else return const_iterator(splay, tmp);
	}
};

}

#endif
