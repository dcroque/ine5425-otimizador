// Copyright [2020] <Matheus Dhanyel Cândido Roque>
#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace structures {

template <typename T>
//!...
class BinaryTree {
public:
    ~BinaryTree();
    //!...
    void insert(const T& data);
    //!...
    void remove(const T& data);
    //!...
    bool contains(const T& data) const;
    //!...
    bool empty() const;
    //!...
    std::size_t size() const;
    //!...
    std::vector<T> pre_order() const;
    //!...
    std::vector<T> in_order() const;
    //!...
    std::vector<T> post_order() const;

private:
    struct Node {
        explicit Node(const T& data_) {
            data = data_;
        }

        T data;
        Node* left = nullptr;
        Node* right = nullptr;

        void insert(const T& data_) {
            if (data_ < data) {
                if (left == nullptr) {
                    Node* new_node = new Node(data_);
                    left = new_node;
                } else {
                    left->insert(data_);
                }
            } else {
                if (right == nullptr) {
                    Node* new_node = new Node(data_);
                    right = new_node;
                } else {
                    right->insert(data_);
                }
            }
        }

        bool remove(const T& data_) {
            Node* to_remove = nullptr;
            bool is_left;
            if (data == data_) {
                return true;
            } else if (left != nullptr && data_ < data) {
                if (left->remove(data_)) {
                    to_remove = left;
                    is_left = true;
                }
            } else if (right != nullptr && data_ > data) {
                if (right->remove(data_)) {
                    to_remove = right;
                    is_left = false;
                }
            }
            if (to_remove != nullptr) {
                if (to_remove->left == nullptr &&
                    to_remove->right != nullptr) {
                    if (is_left) {
                        left = to_remove->right;
                    } else {
                        right = to_remove->right;
                    }
                    delete to_remove;
                } else if (to_remove->left != nullptr &&
                            to_remove->right == nullptr) {
                    if (is_left) {
                        left = to_remove->left;
                    } else {
                        right = to_remove->left;
                    }
                    delete to_remove;
                } else if (to_remove->left == nullptr &&
                            to_remove->right == nullptr) {
                    if (is_left) {
                        left = nullptr;
                    } else {
                        right = nullptr;
                    }
                    delete to_remove;
                } else {
                    if (to_remove->right->left != nullptr) {
                        to_remove->data = to_remove->right->left->data;
                        to_remove->right->remove(to_remove->data);
                    } else {
                        to_remove->data = to_remove->right->data;
                        to_remove->right->remove(to_remove->data);
                    }
                }
            }
            return false;
        }

        bool contains(const T& data_) const {
            if (data == data_) {
                return true;
            } else if (left != nullptr && data_ < data) {
                return left->contains(data_);
            } else if (right != nullptr && data_ > data) {
                return right->contains(data_);
            }
            return false;
        }

        void pre_order(std::vector<T>& v) const {
            v.push_back(data);
            if (left != nullptr) {
                left->pre_order(v);
            }
            if (right != nullptr) {
                right->pre_order(v);
            }
            return;
        }

        void in_order(std::vector<T>& v) const {
            if (left != nullptr) {
                left->in_order(v);
            }
            v.push_back(data);
            if (right != nullptr) {
                right->in_order(v);
            }
            return;
        }

        void post_order(std::vector<T>& v) const{
            if (left != nullptr) {
                left->post_order(v);
            }
            if (right != nullptr) {
                right->post_order(v);
            }
            v.push_back(data);
            return;
        }
    };

    Node* root;
    std::size_t size_ = 0;
};

}  // namespace structures

#endif

template<typename T>
structures::BinaryTree<T>::~BinaryTree() {
    while (!empty()) {
        remove(root->data);
    }
    return;
}

template<typename T>
void structures::BinaryTree<T>::insert(const T& data_) {
    if (empty()) {
        Node* new_node = new Node(data_);
        root = new_node;
    } else {
        root->insert(data_);
    }
    size_++;
    return;
}

template<typename T>
void structures::BinaryTree<T>::remove(const T& data) {
    if (contains(data)) {
        if (root->remove(data)) {
            if (root->left == nullptr &&
                root->right != nullptr) {
                Node* temp = root;
                root = root->right;
                delete temp;
            } else if (root->left != nullptr &&
                        root->right == nullptr) {
                Node* temp = root;
                root = root->left;
                delete temp;
            } else if (root->left == nullptr &&
                        root->right == nullptr) {
                delete root;
            } else {
                if (root->right->left != nullptr) {
                    root->data = root->right->left->data;
                    root->right->remove(root->data);
                } else {
                    Node* temp = root;
                    root = root->right;
                    root->left = temp->left;
                    delete temp;
                }
            }
        }
        size_--;
    }
    return;
}

template<typename T>
bool structures::BinaryTree<T>::contains(const T& data) const {
    if (empty()) {
        return false;
    } else {
        return root->contains(data);
    }
}

template<typename T>
bool structures::BinaryTree<T>::empty() const {
    return (size_ == 0);
}

template<typename T>
std::size_t structures::BinaryTree<T>::size() const {
    return size_;
}

template<typename T>
std::vector<T> structures::BinaryTree<T>::pre_order() const {
    std::vector<T> temp{size_};
    if (!empty()) {
        root->pre_order(temp);
    }
    return temp;
}

template<typename T>
std::vector<T> structures::BinaryTree<T>::in_order() const {
    std::vector<T> temp{size_};
    if (!empty()) {
        root->in_order(temp);
    }
    return temp;
}

template<typename T>
std::vector<T> structures::BinaryTree<T>::post_order() const {
    std::vector<T> temp{size_};
    if (!empty()) {
        root->post_order(temp);
    }
    return temp;
}
