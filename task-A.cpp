#include <iostream>
#include <list>

template<class ValueType> class Set{
 private:
    struct node {
        node* left;
        node* right;
        ValueType val;
        int height;
        typename std::list<ValueType>::iterator c_it;

        node(const ValueType &x) {
            left = right = nullptr;
            val = x;
            height = 1;
        }
    };
    size_t size_;
    node* root;
    std::list<ValueType> right_order;

 public:
    typedef typename std::list<ValueType>::const_iterator iterator;

    int height(node* cur) {
        if (cur == nullptr)
            return 0;
        return cur->height;
    }
    int diff(node* cur) {
        return height(cur->right) - height(cur->left);
    }
    void new_height(node* cur) {
        if (height(cur->left) > height(cur->right)) {
            cur->height = height(cur->left) + 1;
            return;
        }
        cur->height = height(cur->right) + 1;
        return;
    }
    node* rotateright(node* cur) {
        node* other = cur->left;
        cur->left = other->right;
        other->right = cur;
        new_height(cur);
        new_height(other);
        return other;
    }
    node* rotateleft (node* cur) {
        node* other = cur->right;
        cur->right = other->left;
        other->left = cur;
        new_height(cur);
        new_height(other);
        return other;
    }
    node* balance(node* cur) {
        new_height(cur);
        if (diff(cur) == 2) {
            if (diff(cur->right) < 0)
                cur->right = rotateright(cur->right);
            return rotateleft(cur);
        }
        if (diff(cur) == -2) {
            if (diff(cur->left) > 0)
                cur->left = rotateleft(cur->left);
            return rotateright(cur);
        }
        return cur;
    }
    node* way_balance(node* cur, const ValueType &x) {
        if (cur == nullptr)
            return nullptr;
        if (!(cur->val < x || x < cur->val))
            return cur;
        if (cur->val < x)
            cur->right = way_balance(cur->right, x);
        if (x < cur->val)
            cur->left = way_balance(cur->left, x);
        return cur = balance(cur);
    }
    node* myfind (node* cur, const ValueType &x) const {
        if (cur == nullptr)
            return nullptr;
        if (!(x < cur->val || cur->val < x))
            return cur;
        if (cur->val < x && cur->right == nullptr)
            return cur;
        if (x < cur->val && cur->left == nullptr)
            return cur;
        if (cur->val < x)
            return myfind(cur->right, x);
        if (x < cur->val)
            return myfind(cur->left, x);
        return nullptr;
    }

    void insert(const ValueType &x) {
        node* ver = myfind(root, x);
        if (ver == nullptr) {
            root = new node(x);
            size_++;
            right_order.push_back(x);
            root->c_it = right_order.begin();
            return;
        }
        if (!(ver->val < x || x < ver->val))
            return;
        ++size_;
        if (ver->val < x) {
            ver->right = new node(x);
            auto it = ver->c_it; ++it;
            right_order.insert(it, x);
            it = ver->c_it; ++it;
            ver->right->c_it = it;
            root = way_balance(root, x);
            return;
        }
        if (x < ver->val) {
            ver->left = new node(x);
            auto it = ver->c_it;
            if (it == right_order.begin()) {
                right_order.push_front(x);
            } else {
                right_order.insert(it, x);
            }
            it = ver->c_it; --it;
            ver->left->c_it = it;
            root = way_balance(root, x);
            return;
        }
        return;
    }

    Set(): size_(0), root(nullptr) {};

    template <typename Iter> Set(Iter begin, Iter end) : Set() {
        while (begin != end) {
            insert(*begin);
            ++begin;
        }
    }

    Set (const std::initializer_list<ValueType> &List) : Set() {
        auto Begin = List.begin();
        auto End = List.end();
        while (Begin != End) {
            insert(*Begin);
            ++Begin;
        }
    }

    node* deep_copy(node* my_node, node* other_node) {
        if (other_node == nullptr) {
            my_node = nullptr;
            return nullptr;
        }
        my_node = new node(other_node->val); my_node->height = other_node->height;
        my_node->left = deep_copy(my_node->left, other_node->left);

        right_order.push_back(my_node->val);
        auto it = right_order.end(); --it;
        my_node->c_it = it;

        my_node->right = deep_copy(my_node->right, other_node->right);
        return my_node;
    }
    Set (Set &other): size_(other.size_) {
        root = nullptr;
        root = deep_copy(root, other.root);
    }

    void clear(node* cur) {
        if (cur == nullptr)
            return;
        clear(cur->left);
        clear(cur->right);
        delete cur;
    }
    bool empty() const {
        return size_ == 0;
    }
    Set& operator = (Set &other) {
        if (this != &other) {
            clear(root);
            root = nullptr;
            right_order.clear();
            size_ = 0;
            if (other.empty())
                return *this;
            root = deep_copy(root, other.root);
            size_ = other.size_;
        }
        return *this;
    }

    ~Set() {
        right_order.clear();
        clear(root);
        size_ = 0;
    }

    size_t size() const {
        return size_;
    }

    node* minimum(node* cur) {
        if (cur->left == nullptr)
            return cur;
        return minimum(cur->left);
    }
    node* parent(node* cur, const ValueType &x) const {
        if (cur->val < x) {
            if (!(cur->right->val < x || x < cur->right->val)) {
                return cur;
            }
            return parent(cur->right, x);
        }
        if (x < cur->val) {
            if (!(cur->left->val < x || x < cur->left->val)) {
                return cur;
            }
            return parent(cur->left, x);
        }
        return nullptr;
    }
    void erase(const ValueType& x) {
        node* cur = myfind(root, x);
        if (cur == nullptr)
            return;
        if (cur->val < x || x < cur->val)
            return;
        --size_;
        node *tmp = nullptr;
        if (cur->left == nullptr && cur->right != nullptr) {
            tmp = cur->right;
        }
        if (cur->right == nullptr && cur->left != nullptr) {
            tmp = cur->left;
        }
        if (cur->right != nullptr && cur->left != nullptr) {
            tmp = minimum(cur->right);
            node* new_ver = new node(tmp->val);
            new_ver->c_it = tmp->c_it;
            auto it = new_ver->c_it; ++it;
            size_++;
            erase(tmp->val);
            cur->right = way_balance(cur->right, new_ver->val); /// loook at it
            right_order.insert(it, new_ver->val);
            new_ver->c_it = --it;
            if (cur->left != nullptr && cur->left->val != new_ver->val)
                new_ver->left = cur->left;
            if (cur->right != nullptr && cur->right->val != new_ver->val)
                new_ver->right = cur->right;
            if (cur != root) {
                node* p = parent(root, x);
                if (p->val < x)
                    p->right = new_ver;
                if (x < p->val)
                    p->left = new_ver;
            }
            right_order.erase(cur->c_it);
            if (cur == root) {
                delete root;
                root = new_ver;
            } else {
                delete cur;
            }
            root = way_balance(root, x);
            return;
        }
        if (cur != root) {
            node* p = parent(root, x);
            if (p->val < x)
                p->right = tmp;
            if (x < p->val)
                p->left = tmp;
        }
        right_order.erase(cur->c_it);
        if (cur == root) {
            delete root;
            root = tmp;
        } else {
            delete cur;
        }
        root = way_balance(root, x);
    }

    typename std::list<ValueType>::const_iterator begin() const {
        return right_order.cbegin();
    }
    typename std::list<ValueType>::const_iterator end() const {
        return right_order.cend();
    }
    typename std::list<ValueType>::const_iterator find(const ValueType &x) const {
        node* tmp = myfind(root, x);
        if (tmp == nullptr)
            return right_order.cend();
        if (tmp->val < x || x < tmp->val)
            return right_order.cend();
        return tmp->c_it;
    }
    typename std::list<ValueType>::const_iterator lower_bound(const ValueType &x) const {
        node* tmp = myfind(root, x);
        if (tmp == nullptr)
            return right_order.end();
        auto it = tmp->c_it;
        if (!(tmp->val < x || x < tmp->val))
            return it;
        if (x < tmp->val)
            return it;
        ++it;
        return it;
    }
};
