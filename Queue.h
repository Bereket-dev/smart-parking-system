#ifndef QUEUE_H
#define QUEUE_H

#include "Vehicle.h"

class Queue {
private:
    Vehicle arr[100];
    int front, rear;

public:
    Queue() {
        front = rear = -1;
    }

    bool isEmpty() {
        return front == -1;
    }

    bool isFull() {
        return rear == 99;
    }

    void enqueue(Vehicle v) {
        if (isFull()) return;
        if (front == -1) front = 0;
        arr[++rear] = v;
    }

    Vehicle dequeue() {
        Vehicle v;
        if (isEmpty()) return v;

        v = arr[front];
        if (front == rear)
            front = rear = -1;
        else
            front++;
        return v;
    }
};

#endif
