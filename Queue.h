#ifndef QUEUE_H
#define QUEUE_H

#include "Vehicle.h"

struct QNode
{
    Vehicle data;
    QNode *next;
};

class Queue
{
private:
    QNode *front;
    QNode *rear;

public:
    Queue()
    {
        front = rear = nullptr;
    }

    bool isEmpty()
    {
        return front == nullptr;
    }

    Vehicle top()
    {
        if (isEmpty())
        {
            return Vehicle("", -1);
        }
        return front->data;
    }

    void enqueue(Vehicle v)
    {
        QNode *newNode = new QNode{v, nullptr};

        if (isEmpty() || v.priority < front->data.priority)
        {
            newNode->next = front;
            front = newNode;
            if (rear == nullptr)
                rear = newNode;
            return;
        }

        QNode *current = front;
        while (current->next != nullptr && current->next->data.priority <= v.priority)
        {
            current = current->next;
        }

        newNode->next = current->next;
        current->next = newNode;

        if (newNode->next == nullptr)
        {
            rear = newNode;
        }
    }

    Vehicle dequeue()
    {
        if (isEmpty())
        {
            return Vehicle("", -1);
        }

        QNode *temp = front;
        Vehicle v = temp->data;
        front = front->next;

        if (front == nullptr)
            rear = nullptr;

        delete temp;
        return v;
    }

    ~Queue()
    {
        while (!isEmpty())
        {
            dequeue();
        }
    }
};

#endif