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

class PalindromLinkList {
 public:
  static bool isPalindrom(ListNode *head) {
    if (!head || !head->next_) {
      return true;
    }
    ListNode *slow = head;
    ListNode *fast = slow;
    while (fast && fast->next_) {
      slow = slow->next_;
      fast = fast->next_->next_;
    }
    // now slow at the middle and we reverse it
    // note: we don't change the pointer before slow
    // so the second half is till chained with the first half
    ListNode *rhead = reverse(slow);
    ListNode *rhead_cp = rhead;
    while (rhead && head) {
      if (rhead->val_ != head->val_) {
        break;
      }
      rhead = rhead->next_;
      head = head->next_;
    }
    // reverse it back
    reverse(rhead_cp);
    if (!head || !rhead) {
      return true;
    }
    return false;
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
  ListNode a = ListNode(2);
  ListNode b = ListNode(4);
  ListNode c = ListNode(6);
  ListNode d = ListNode(4);
  ListNode e = ListNode(2);
  a.next_ = &b;
  b.next_ = &c;
  c.next_ = &d;
  d.next_ = &e;
  cout << "is PalindromLinkList " << PalindromLinkList::isPalindrom(&a) << endl;
  ListNode f = ListNode(2);
  e.next_ = &f;
  cout << "is PalindromLinkList " << PalindromLinkList::isPalindrom(&a) << endl;
  ListNode *head = &a;
  // still chained
  while (head) {
    cout << "val: " << head->val_ << endl;
    head = head->next_;
  }
}
