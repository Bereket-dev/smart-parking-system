#ifndef QUEUE_H
#define QUEUE_H

#include "Vehicle.h"

struct QNode {
    Vehicle data;
    QNode* next;
};

class Queue {
private:
    QNode* front;
    QNode* rear;

public:
    Queue() {
        front = rear = nullptr;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    void enqueue(Vehicle v) {
        QNode* temp = new QNode{v, nullptr};

        if (rear == nullptr) {
            front = rear = temp;
            return;
        }
        rear->next = temp;
        rear = temp;
    }

    Vehicle dequeue() {
        if (isEmpty()) {
            return Vehicle("", -1);
        }

        QNode* temp = front;
        Vehicle v = temp->data;
        front = front->next;

        if (front == nullptr)
            rear = nullptr;

        delete temp;
        return v;
    }
};

#endif
