#include "xxx.hpp"

class MyQueue {
public:
  /** Initialize your data structure here. */
  MyQueue() {}


  /** Push element x to the back of queue. */
  void push(int x) { s1.push(x); }

  /** Removes the element from in front of queue and returns that element. */
  int pop() {
    if (!s2.empty()) {
      int val = s2.top();
      s2.pop();
      return val;
    }

    while (!s1.empty()) {
      int val = s1.top();
      s1.pop();
      s2.push(val);
    }
    int val = s2.top();
    s2.pop();
    return val;
  }

  /** Get the front element. */
  int peek() {
    if (!s2.empty()) {
      int val = s2.top();
      return val;
    }

    while (!s1.empty()) {
      int val = s1.top();
      s1.pop();
      s2.push(val);
    }
    int val = s2.top();
    return val;
  }

  /** Returns whether the queue is empty. */
  bool empty() { return s1.empty() && s2.empty(); }

private:
  stack<int> s1;
  stack<int> s2;
};
