#ifndef LINKEDLIST_H
#define LINKEDLIST_H

template <typename T>
class LinkedListNode {
    public:
        T m_item;
        LinkedListNode* m_next = nullptr;
};

template <typename T>
class LinkedList {
    public:
        LinkedList();
        ~LinkedList();

        void insert(T item);
        T getFirst() const;
        T getLast() const;
        void removeFirst();
        int size() const;
        LinkedListNode<T>* getIterator() const;

    private:
        LinkedListNode<T> *m_firstNode = nullptr;  // "front node"
        LinkedListNode<T> *m_lastNode = nullptr;  // "back node"
        int m_size = 0;
};


template <typename T>
LinkedList<T>::LinkedList() {}

template <typename T>
LinkedList<T>::~LinkedList() {
    // go through nodes and delete them all
    
    LinkedListNode<T>* iter = this->m_firstNode;
    while (iter != nullptr) {
        LinkedListNode<T>* next = iter->m_next;
        delete iter;
        iter = next;
    }
}

template <typename T>
void LinkedList<T>::insert(T item) {
    if (this->m_firstNode == nullptr) {
        this->m_firstNode = new LinkedListNode<T>();
        this->m_firstNode->m_item = item;
        this->m_lastNode = this->m_firstNode;
        this->m_size = 1;
    } else {
        LinkedListNode<T>* node = new LinkedListNode<T>();
        node->m_item = item;
        this->m_lastNode->m_next = node;
        this->m_lastNode = node;
        this->m_size++;
    }
}

template <typename T>
T LinkedList<T>::getFirst() const {
    if (this->m_firstNode == nullptr) return 0;
    return this->m_firstNode->m_item;
}

template <typename T>
T LinkedList<T>::getLast() const {
    if (this->m_lastNode == nullptr) return 0;
    return this->m_lastNode->m_item;
}

template <typename T>
void LinkedList<T>::removeFirst() {
    LinkedListNode<T>* node = this->m_firstNode;
    this->m_firstNode = node->m_next;
    delete node;
    this->m_size--;
}

template <typename T>
int LinkedList<T>::size() const {
    return this->m_size;
}

template <typename T>
LinkedListNode<T>* LinkedList<T>::getIterator() const {
    return this->m_firstNode;
}


#endif
