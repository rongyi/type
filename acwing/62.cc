#include "xxx.hpp"

class Solution {
public:
  ListNode *findFirstCommonNode(ListNode *heada, ListNode *headb) {
    ListNode *p = heada;
    ListNode *q = headb;
    while (p != q) {
      if (p) {
        p = p->next;
      } else {
        p = headb;
      }

      if (q) {
        q = q->next;
      } else {
        q = heada;
      }
    }
    return p;
  }
};

class SolutionSet {
public:
  ListNode *findFirstCommonNode(ListNode *p, ListNode *q) {
    set<ListNode *> ps;
    while (p) {
      ps.insert(p);
      p = p->next;
    }
    while (q) {
      if (ps.count(q)) {
        return q;
      }
      q = q->next;
    }
    return nullptr;
  }
};
