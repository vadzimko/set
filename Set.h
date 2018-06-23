#ifndef SET_SET_H
#define SET_SET_H

#include <iterator>
#include <queue>

template <typename T>
struct Set {
private:
    struct Node {
        Node *left;
        Node *right;
        Node *parent;

        Node() : left(nullptr), right(nullptr), parent(nullptr) {}
        Node(Node* other) : left(other->left), right(other->right), parent(other->parent) {}
        virtual ~Node() = default;

        T& value() { return static_cast<ValueNode&>(*this).value; }
        Node* prev();
        Node* next();
    };
    struct ValueNode : Node {
        T value;

        ValueNode(T const& value) : Node(), value(value) {}
        ValueNode(ValueNode* other) : Node(other), value(other->value) {}
        ~ValueNode() = default;
    };

    template <typename V>
    struct Iterator;
public:

    Set() noexcept;
    Set(Set const& other);
    Set &operator=(Set const& other);
    ~Set();

    using iterator = Iterator<T const>;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    std::pair<iterator, bool> insert(T const& element);
    iterator erase(const_iterator it);

    const_iterator find(T const& element);
    const_iterator lower_bound(T const& element) const;
    const_iterator upper_bound(T const& element) const;

    bool empty() const;
    void clear();

    void swap(Set& other);

    iterator begin() { return iterator(begin_); }
    const_iterator begin() const { return const_iterator(begin_); }
    iterator end(){ return iterator(&top_node_); }
    const_iterator end() const { return const_iterator(&top_node_); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

private:
    Node top_node_;
    Node* begin_ = &top_node_;
    Node* get_root() const { return top_node_.left; }

    void build_impl(Node* root, Node* root_other);
    void clear_impl(Node *node);
    void erase_impl(Node* node);
    Node* upper_lower_impl (T const& element) const;
};

template <typename T>
Set<T>::Set() noexcept {}

template <typename T>
Set<T>::Set(Set const& other) {
    try {
        if (other.top_node_.left) {
            top_node_.left = new ValueNode(other.top_node_.left->value());
            top_node_.left->parent = &top_node_;
            build_impl(top_node_.left, other.top_node_.left);
        }
        while (begin_->left)
            begin_ = begin_->left;
    } catch (...) {
        clear();
        throw;
    }
}

template <typename T>
void Set<T>::build_impl(Node *root, Node *root_other) {
    std::queue<std::pair<Node *, Node* >> pairs;
    pairs.push({root, root_other});

    while (!pairs.empty()) {
        Node* node = pairs.front().first;
        Node* other = pairs.front().second;
        pairs.pop();

        if (other->left) {
            node->left = new ValueNode(other->left->value());
            node->left->parent = node;
            pairs.push({node->left, other->left});
        }

        if (other->right) {
            node->right = new ValueNode(other->right->value());
            node->right->parent = node;
            pairs.push({node->right, other->right});
        }
    }

}

template <typename T>
void Set<T>::clear_impl(Node *node) {
    if (node == nullptr)
        return;

    clear_impl(node->left);
    clear_impl(node->right);
    delete node;
}

template <typename T>
Set<T>& Set<T>::operator=(Set const &other) {
    Set temp(other);
    swap(temp);
    return *this;
}

template <typename T>
Set<T>::~Set() {
    clear();
}

template <typename T>
std::pair<typename Set<T>::iterator, bool> Set<T>::insert(T const &element) {
    if (get_root() == nullptr) {
        top_node_.left = new ValueNode(element);
        top_node_.left->parent = &top_node_;
        begin_ = top_node_.left;
        return {begin(), true};
    }

    if (element < *begin()) {
        Node* node = begin_;
        node->left = new ValueNode(element);
        node->left->parent = node;
        begin_ = node->left;
        return {begin(), true};
    }

    Node* node = get_root();
    while (true) {
        if (node->value() == element)
            return {iterator(node), false};

        if (element < node->value()) {
            if (node->left == nullptr) {
                node->left = new ValueNode(element);
                node->left->parent = node;
                return {iterator(node->left), true};
            } else node = node->left;
        } else {
            if (node->right == nullptr) {
                node->right = new ValueNode(element);
                node->right->parent = node;
                return {iterator(node->right), true};
            } else node = node->right;
        }
    }
}

template <typename T>
typename Set<T>::iterator Set<T>::erase(const_iterator it) {
    if (it == end())
        return end();

    Node* node = it.node;
    iterator ret(node->next());

    erase_impl(node);
    delete node;

    return ret;
}

template <typename T>
void Set<T>::erase_impl(Node *node) {
    if (node->left == nullptr && node->right == nullptr) {
        if (node == begin_) {
            if (node->parent == &top_node_) {
                top_node_.left = nullptr;
                begin_ = &top_node_;
            } else {
                begin_ = node->parent;
            }
        }
        if (node == node->parent->left)
            node->parent->left = nullptr;
        else node->parent->right = nullptr;

        return;
    }

    if (node->left == nullptr || node->right == nullptr) {
        if (node == node->parent->left) {
            if (node->right != nullptr) {
                if (node == begin_) {
                    begin_ = node->right;
                    while (begin_->left != nullptr)
                        begin_ = begin_->left;
                }
                node->parent->left = node->right;
                node->right->parent = node->parent;
            } else {
                node->parent->left = node->left;
                node->left->parent = node->parent;
            }
        } else {
            if (node->right != nullptr) {
                node->parent->right= node->right;
                node->right->parent = node->parent;
            } else {
                node->parent->right = node->left;
                node->left->parent = node->parent;
            }
        }

        return;
    }

    Node* next = node->next();
    erase_impl(next);

    next->parent = node->parent;
    if (node == node->parent->right)
        node->parent->right = next;
    else node->parent->left = next;

    if (node->left) {
        next->left = node->left;
        node->left->parent = next;
    }

    if (node->right) {
        next->right = node->right;
        node->right->parent = next;
    }
}

template <typename T>
typename Set<T>::const_iterator Set<T>::find(T const &element) {
    Node* node = upper_lower_impl(element);
    if (node != nullptr && node->value() == element) {
        return const_iterator(node);
    }
    else return end();
}

template <typename T>
typename Set<T>::const_iterator Set<T>::lower_bound(T const &element) const {
    Node* node = upper_lower_impl(element);
    if (node == nullptr || node == &top_node_)
        return end();
    else if (node->value() < element)
        return const_iterator(node->next());
    else return const_iterator(node);

}
template <typename T>
typename Set<T>::const_iterator Set<T>::upper_bound(T const &element) const {
    if (element < begin_->value())
        return begin();

    Node* node = upper_lower_impl(element);


    if (node == nullptr || node == &top_node_)
        return end();

    if (node->value() <= element)
        return const_iterator(node->next());
    else return const_iterator(node);
}

template <typename T>
typename Set<T>::Node* Set<T>::upper_lower_impl(T const &element) const {
    Node* node = get_root();
    if (node == nullptr)
        return nullptr;

    while (true) {
        if (node->value() == element)
            return node;

        if (element < node->value()) {
            if (node->left == nullptr)
                break;
            else node = node->left;
        } else {
            if (node->right == nullptr)
                break;
            else node = node->right;
        }
    }

    while (node != &top_node_ && node->value() < element) {
        Node *next = node->next();
        if (next == &top_node_ || element < next->value())
            break;
        node = node->next();
    }
    return node;
}

template <typename T>
void Set<T>::clear() {
    clear_impl(top_node_.left);
    top_node_.left = nullptr;
    begin_ = &top_node_;
}

template <typename T>
bool Set<T>::empty() const {
    return get_root() == nullptr;
}

template <typename T>
typename Set<T>::Node* Set<T>::Node::next() {
    Node* node = this;
    if (node->right != nullptr) {
        node = node->right;
        while (node->left != nullptr)
            node = node->left;
        return node;
    }

    while (node->parent != nullptr && node == node->parent->right)
        node = node->parent;
    return node->parent == nullptr ? node : node->parent;
}

template <typename T>
typename Set<T>::Node* Set<T>::Node::prev() {
    Node* node = this;
    if (node->left != nullptr) {
        node = node->left;
        while (node->right != nullptr)
            node = node->right;
        return node;
    }

    while (node->parent != nullptr && node == node->parent->left)
        node = node->parent;
    return node->parent == nullptr ? node : node->parent;
}


template <typename T>
void Set<T>::swap(Set &other) {
    if (top_node_.left == nullptr && other.top_node_.left == nullptr)
        return;

    if (top_node_.left != nullptr && other.top_node_.left != nullptr) {
        std::swap(top_node_.left, other.top_node_.left);
        std::swap(top_node_.left->parent, other.top_node_.left->parent);
        std::swap(begin_, other.begin_);
    } else if (top_node_.left == nullptr) {
        top_node_.left = other.top_node_.left;
        top_node_.left->parent = &top_node_;
        begin_ = other.begin_;
        other.top_node_.left = nullptr;
        other.begin_ = &other.top_node_;
    } else {
        other.top_node_.left = top_node_.left;
        other.top_node_.left->parent = &other.top_node_;
        other.begin_ = begin_;
        top_node_.left = nullptr;
        begin_ = &top_node_;
    }
}

template <typename T>
void swap(Set<T>& first, Set<T>& second) {
    first.swap(second);
}

template <typename T>
template <typename V>
struct Set<T>::Iterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = V;
    using pointer = V*;
    using reference = V&;

    friend struct Set;
    Iterator() {}
    Iterator(Iterator const&) = default;
    Iterator& operator=(Iterator const&) = default;

    Iterator& operator++() {
        node = node->next();
        return *this;
    }
    const Iterator operator++(int) {
        Iterator temp(node);
        ++*this;
        return temp;
    }
    Iterator& operator--() {
        node = node->prev();
        return *this;
    }
    const Iterator operator--(int) {
        Iterator temp(node);
        --*this;
        return temp;
    }

    V& operator*() const { return node->value(); }
    V* operator->() const { return &node->value(); }
    friend bool operator==(Iterator const& first, Iterator const& second) { return first.node == second.node; }
    friend bool operator!=(Iterator const& first, Iterator const& second) { return first.node != second.node; }

private:
    Iterator(const Node* node) {
        this->node = const_cast<Node* >(node);
    }
    Node* node;

};

#endif //SET_SET_H
