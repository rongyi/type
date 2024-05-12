#include <vector>

using namespace std;

class MinStack {
public:
  /** initialize your data structure here. */
  MinStack() {}

  void push(int x) {
    vals_.push_back(x);
    if (mins_.empty()) {
      mins_.push_back(x);
    } else {
      auto val = min(x, mins_.back());
      mins_.push_back(val);
    }
  }

  void pop() {
    vals_.pop_back();
    mins_.pop_back();
  }

  int top() { return vals_.back(); }

  int getMin() { return mins_.back(); }

private:
  vector<int> vals_;
  vector<int> mins_;
};
