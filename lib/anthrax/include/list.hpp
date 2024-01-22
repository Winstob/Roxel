/* ---------------------------------------------------------------- *\
 * list.hpp
 * Author: Gavin Ralston
 * Date Created: 2024-01-04
\* ---------------------------------------------------------------- */

#ifndef VOXELDISPLAYLIST_HPP
#define VOXELDISPLAYLIST_HPP

#include <memory>

namespace Anthrax
{

template <class T>
class List 
{
public:
  List() : front_(nullptr), back_(nullptr), size_(0) {}
  ~List() { destroy_list(); }
  size_t size() { return size_; }
  void push_back(std::weak_ptr<T> new_node);
  void push_front(std::weak_ptr<T> new_node);
  void destroy_list();

  template <class T1>
  class Node
  {
  public:
    Node(std::weak_ptr<T1> ptr, Node<T1> *previous_node, Node *next_node)
    {
      ptr_ = ptr;

      if (previous_node != nullptr)
      {
        previous_node->setNext(this);
      }
      previous_node_ = previous_node;

      if (next_node != nullptr)
      {
        next_node->setPrevious(this);
      }
      next_node_ = next_node;
    }

    Node(std::weak_ptr<T> ptr) : Node(ptr, nullptr, nullptr) {}
    Node() : Node(std::weak_ptr<T>(), nullptr, nullptr) {}
    ~Node() {}

    void setPrevious(Node *new_previous_node)
    {
      previous_node_ = new_previous_node;
    }
    void setNext(Node *new_next_node)
    {
      next_node_ = new_next_node;
    }
    Node* next() const { return next_node_; }
    Node* previous() const { return previous_node_; }
    std::weak_ptr<T> ptr_;
    Node *previous_node_;
    Node *next_node_;
  };

  class iterator
  {
  public:
    iterator(Node<T> *node = nullptr) : node_(node) {}
    std::weak_ptr<T> operator*() { return node_->ptr_; }
    iterator& operator++()
    {
      node_ = node_->next();
      return *this;
    }
    iterator operator++(int)
    {
      iterator tmp(*this);
      node_ = node_->next();
      return tmp;
    }
    iterator& operator--()
    {
      node_ = node_->previous();
      return *this;
    }
    iterator operator--(int)
    {
      iterator tmp(*this);
      node_ = node_->previous();
      return tmp;
    }
    bool operator==(const iterator& r) const { return node_ == r.node_; }
    bool operator!=(const iterator& r) const { return node_ != r.node_; }

    Node<T> *node_;
  };

  iterator begin() { return iterator(front_); }

  iterator erase(iterator itr);

private:
  Node<T> *front_;
  Node<T> *back_;
  size_t size_;
};

template <class T>
typename List<T>::iterator List<T>::erase(iterator itr)
{
  if (size_ <= 0) return nullptr;

  iterator result = itr.node_->next_node_;
  size_--;
  // List containing a single node
  if (itr.node_ == front_ && front_ == back_)
  {
    front_ = back_ = nullptr;
  }
  // Removing the front
  else if (itr.node_ == front_)
  {
    front_ = front_->next_node_;
    front_->previous_node_ = nullptr;
  }
  // Removing the back
  else if (itr.node_ == back_)
  {
    back_ = back_->previous_node_;
    back_->next_node_ = nullptr;
  }
  // Normal removal
  else
  {
    itr.node_->previous_node_->next_node_ = itr.node_->next_node_;
    itr.node_->next_node_->previous_node_ = itr.node_->previous_node_;
  }
  delete(itr.node_);
  return result;
}


template <class T>
void List<T>::push_back(std::weak_ptr<T> new_cube)
{
  if (front_ == nullptr)
  {
    front_ = new Node<T>(new_cube, nullptr, nullptr);
    back_ = front_;
  }
  else
  {
    back_ = new Node<T>(new_cube, back_, nullptr);
  }
  size_++;
}


template <class T>
void List<T>::push_front(std::weak_ptr<T> new_cube)
{
  if (back_ == nullptr)
  {
    front_ = new Node<T>(new_cube);
    back_ = front_;
    return;
  }
  else
  {
    front_ = new Node<T>(new_cube, nullptr, front_);
  }
  size_++;
}


template <class T>
void List<T>::destroy_list() {
  while (front_ != nullptr)
  {
    Node<T> *tmp = front_;
    front_ = front_->next();
    delete tmp;
  }

}

} // namespace Anthrax

#endif // VOXELDISPLAYLIST_HPP


