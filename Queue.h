#ifndef QUEUE_H
#define QUEUE_H

#include "Vehicle.h"

class Queue {
private:
    struct Node {
        Vehicle data;
        Node* next;
    };

    Node* front;

public:
    Queue() {
        front = nullptr;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    void enqueue(Vehicle v) {
        Node* newNode = new Node{v, nullptr};

        if (isEmpty() || v.priority < front->data.priority) {
            newNode->next = front;
            front = newNode;
            return;
        }

        Node* current = front;
        while (current->next != nullptr &&
               current->next->data.priority <= v.priority) {
            current = current->next;
        }

        newNode->next = current->next;
        current->next = newNode;
    }

    Vehicle dequeue() {
        Vehicle v;
        if (isEmpty()) return v;

        Node* temp = front;
        v = front->data;
        front = front->next;
        delete temp;

        return v;
    }
};

#endif