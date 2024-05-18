#include "xxx.hpp"

class Solution {
public:
  ListNode *reverseList(ListNode *head) {
    ListNode dummy(-1);
    while (head) {
      ListNode *next = head->next;
      head->next = dummy.next;
      dummy.next = head;
      head = next;
    }
    return dummy.next;
  }
};
