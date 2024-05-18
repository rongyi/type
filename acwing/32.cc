#include "xxx.hpp"

class Solution {
public:
  ListNode *findKthToTail(ListNode *pListHead, int k) {
    len(pListHead, k);
    return target;
  }
  int len(ListNode *cur, int k) {
    if (!cur) {
      return 0;
    }
    auto rest = len(cur->next, k);
    if (rest + 1 == k) {
      target = cur;
    }
    return 1 + rest;
  }

private:
  ListNode *target = nullptr;
};
