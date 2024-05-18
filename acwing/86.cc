#include "xxx.hpp"

class Solution {
public:
  ListNode *entryNodeOfLoop(ListNode *head) {
    if (!head || !head->next) {
      return nullptr;
    }
    ListNode *slow = head;
    ListNode *fast = head;
    bool has_loop = false;

    while (fast && fast->next) {
      slow = slow->next;
      fast = fast->next->next;
      if (slow == fast) {
        has_loop = true;
        break;
      }
    }
    if (!has_loop) {
      return nullptr;
    }
    // now let slow to start, walk as same speed, when
    // they meet
    slow = head;
    while (slow != fast) {
      slow = slow->next;
      fast = fast->next;
    }
    return slow;
  }
};
