#include "xxx.hpp"

class Solution {
public:
  ListNode *deleteDuplication(ListNode *head) {
    ListNode dummy(-1);
    ListNode *tail = &dummy;
    ListNode *cur = head;
    while (cur) {
      // not keep one, delete them all
      bool skip = (cur->next != nullptr) && cur->next->val == cur->val;
      if (skip) {
        auto val = cur->val;
        while (cur && cur->val == val) {
          cur = cur->next;
        }
      } else {
        tail->next = cur;

        cur = cur->next;
        tail = tail->next;
        tail->next = nullptr;
      }
    }

    return dummy.next;
  }
};
