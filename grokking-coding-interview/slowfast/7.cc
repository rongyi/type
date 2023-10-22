#include <iostream>
using namespace std;

struct ListNode {
  int val_;
  ListNode *next_;
  ListNode(int val) {
    this->val_ = val;
    next_ = nullptr;
  }
};

class RearrangeList {
 public:
  static void rearrange(ListNode *head) {
    if (!head || !head->next_) {
      return;
    }
    ListNode *slow = head;
    ListNode *fast = slow;
    ListNode *slow_prev = nullptr;
    while (fast && fast->next_) {
      slow_prev = slow;
      slow = slow->next_;
      fast = fast->next_->next_;
    }
    slow_prev->next_ = nullptr;
    // now slow at the middle and we reverse it
    // note: we don't change the pointer before slow
    // so the second half is till chained with the first half
    ListNode *rhead = reverse(slow);
    ListNode dummy(-1);
    // this is easy to understand
    // the original solve has loop
    ListNode *cur = &dummy;
    while (head || rhead) {
      if (head) {
        cur->next_ = head;
        cur = cur->next_;
        head = head->next_;
      }
      if (rhead) {
        cur->next_ = rhead;
        cur = cur->next_;
        rhead = rhead->next_;
      }
    }
  }

 private:
  static ListNode *reverse(ListNode *head) {
    ListNode ret(-1);
    while (head) {
      ListNode *next = head->next_;
      head->next_ = ret.next_;
      ret.next_ = head;

      head = next;
    }

    return ret.next_;
  }
};

int main() {
  ListNode a = ListNode(1);
  ListNode b = ListNode(2);
  ListNode c = ListNode(3);
  ListNode d = ListNode(4);
  ListNode e = ListNode(5);
  // ListNode f = ListNode(6);
  a.next_ = &b;
  b.next_ = &c;
  c.next_ = &d;
  d.next_ = &e;
  // e.next_ = &f;

  RearrangeList::rearrange(&a);

  // still chained
  ListNode *head = &a;
  while (head) {
    cout << "val: " << head->val_ << endl;
    head = head->next_;
  }
}
