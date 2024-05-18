#include "xxx.hpp"

class Solution {
public:
  ListNode *merge(ListNode *l1, ListNode *l2) {
    ListNode d(-1);
    ListNode *tail = &d;
    while (l1 && l2) {
      //
      if (l1->val <= l2->val) {
        tail->next = l1;
        l1 = l1->next;
        tail = tail->next;
        // cut everytime
        tail->next = nullptr;
      } else {
        tail->next = l2;
        l2 = l2->next;

        tail = tail->next;
        tail->next = nullptr;
      }
    }
    if (l1) {
      tail->next = l1;
    }
    if (l2) {
      tail->next = l2;
    }

    return d.next;
  }
};
