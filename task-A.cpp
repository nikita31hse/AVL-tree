#include <iostream>
#include <list>

template<class ValueType> class Set{
 private:
    struct Node {
        Node* left;
        Node* right;
        ValueType val;
        int height;
        typename std::list<ValueType>::iterator iter;

        Node(const ValueType &x) {
            left = right = nullptr;
            val = x;
            height = 1;
        }
    };
    size_t set_size;
    Node* root;
    std::list<ValueType> right_order;

 public:
    typedef typename std::list<ValueType>::const_iterator iterator;

    int height(Node* cur) {
        if (cur == nullptr)
            return 0;
        return cur->height;
    }
    int diff(Node* cur) {
        return height(cur->right) - height(cur->left);
    }
    void new_height(Node* cur) {
        if (height(cur->left) > height(cur->right)) {
            cur->height = height(cur->left) + 1;
            return;
        }
        cur->height = height(cur->right) + 1;
        return;
    }
    Node* rotateright(Node* cur) {
        Node* other = cur->left;
        cur->left = other->right;
        other->right = cur;
        new_height(cur);
        new_height(other);
        return other;
    }
    Node* rotateleft (Node* cur) {
        Node* other = cur->right;
        cur->right = other->left;
        other->left = cur;
        new_height(cur);
        new_height(other);
        return other;
    }
    Node* balance(Node* cur) {
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
    Node* way_balance(Node* cur, const ValueType &x) {
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
    Node* myfind (Node* cur, const ValueType &x) const {
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
        Node* ver = myfind(root, x);
        if (ver == nullptr) {
            root = new Node(x);
            set_size++;
            right_order.push_back(x);
            root->iter = right_order.begin();
            return;
        }
        if (!(ver->val < x || x < ver->val))
            return;
        ++set_size;
        if (ver->val < x) {
            ver->right = new Node(x);
            auto it = ver->iter; ++it;
            right_order.insert(it, x);
            it = ver->iter; ++it;
            ver->right->iter = it;
            root = way_balance(root, x);
            return;
        }
        if (x < ver->val) {
            ver->left = new Node(x);
            auto it = ver->iter;
            if (it == right_order.begin())
                right_order.push_front(x);
            else 
                right_order.insert(it, x);
            it = ver->iter; --it;
            ver->left->iter = it;
            root = way_balance(root, x);
            return;
        }
        return;
    }

    Set(): set_size(0), root(nullptr) {};

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

    Node* deep_copy(Node* my_Node, Node* other_Node) {
        if (other_Node == nullptr) {
            my_Node = nullptr;
            return nullptr;
        }
        my_Node = new Node(other_Node->val); my_Node->height = other_Node->height;
        my_Node->left = deep_copy(my_Node->left, other_Node->left);

        right_order.push_back(my_Node->val);
        auto it = right_order.end(); --it;
        my_Node->iter = it;

        my_Node->right = deep_copy(my_Node->right, other_Node->right);
        return my_Node;
    }
    Set (Set &other): set_size(other.set_size) {
        root = nullptr;
        root = deep_copy(root, other.root);
    }

    void clear(Node* cur) {
        if (cur == nullptr)
            return;
        clear(cur->left);
        clear(cur->right);
        delete cur;
    }
    bool empty() const {
        return set_size == 0;
    }
    Set& operator = (Set &other) {
        if (this != &other) {
            clear(root);
            root = nullptr;
            right_order.clear();
            set_size = 0;
            if (other.empty())
                return *this;
            root = deep_copy(root, other.root);
            set_size = other.set_size;
        }
        return *this;
    }

    ~Set() {
        right_order.clear();
        clear(root);
        set_size = 0;
    }

    size_t size() const {
        return set_size;
    }

    Node* minimum(Node* cur) {
        if (cur->left == nullptr)
            return cur;
        return minimum(cur->left);
    }
    Node* parent(Node* cur, const ValueType &x) const {
        if (cur->val < x) {
            if (!(cur->right->val < x || x < cur->right->val))
                return cur;
            return parent(cur->right, x);
        }
        if (x < cur->val) {
            if (!(cur->left->val < x || x < cur->left->val))
                return cur;
            return parent(cur->left, x);
        }
        return nullptr;
    }
    
    void erase(const ValueType& x) {
        Node* cur = myfind(root, x);
        if (cur == nullptr)
            return;
        if (cur->val < x || x < cur->val)
            return;
        --set_size;
        Node *tmp = nullptr;
        if (cur->left == nullptr && cur->right != nullptr)
            tmp = cur->right;
        if (cur->right == nullptr && cur->left != nullptr)
            tmp = cur->left;
            
        if (cur->right != nullptr && cur->left != nullptr) {
            tmp = minimum(cur->right);
            Node* new_ver = new Node(tmp->val);
            new_ver->iter = tmp->iter;
            auto it = new_ver->iter; ++it;
            set_size++;
            erase(tmp->val);
            cur->right = way_balance(cur->right, new_ver->val);
            right_order.insert(it, new_ver->val);
            new_ver->iter = --it;
            
            if (cur->left != nullptr && cur->left->val != new_ver->val)
                new_ver->left = cur->left;
            if (cur->right != nullptr && cur->right->val != new_ver->val)
                new_ver->right = cur->right;
            if (cur != root) {
                Node* p = parent(root, x);
                if (p->val < x)
                    p->right = new_ver;
                if (x < p->val)
                    p->left = new_ver;
            }
            
            right_order.erase(cur->iter);
            if (cur == root) {
                delete root;
                root = new_ver;
            } else
                delete cur;
            root = way_balance(root, x);
            return;
        }
        
        if (cur != root) {
            Node* p = parent(root, x);
            if (p->val < x)
                p->right = tmp;
            if (x < p->val)
                p->left = tmp;
        }
        
        right_order.erase(cur->iter);
        if (cur == root) {
            delete root;
            root = tmp;
        } else
            delete cur;
        root = way_balance(root, x);
    }

    typename std::list<ValueType>::const_iterator begin() const {
        return right_order.cbegin();
    }
    typename std::list<ValueType>::const_iterator end() const {
        return right_order.cend();
    }
    typename std::list<ValueType>::const_iterator find(const ValueType &x) const {
        Node* tmp = myfind(root, x);
        if (tmp == nullptr)
            return right_order.cend();
        if (tmp->val < x || x < tmp->val)
            return right_order.cend();
        return tmp->iter;
    }
    typename std::list<ValueType>::const_iterator lower_bound(const ValueType &x) const {
        Node* tmp = myfind(root, x);
        if (tmp == nullptr)
            return right_order.end();
        auto it = tmp->iter;
        if (!(tmp->val < x || x < tmp->val))
            return it;
        if (x < tmp->val)
            return it;
        ++it;
        return it;
    }
};
