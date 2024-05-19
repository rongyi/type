#include <map>
using namespace std;

struct ListNode {
  int val;
  ListNode *next, *random;
  ListNode(int x) : val(x), next(nullptr), random(nullptr) {}
};

class Solution {
public:
  ListNode *copyRandomList(ListNode *head) {
    ListNode cp(-1);
    ListNode *tail = &cp;
    ListNode *p = head;
    map<ListNode *, ListNode *> clone;
    while (p) {

      tail->next = new ListNode(p->val);
      tail = tail->next;
      clone[p] = tail;

      p = p->next;
    }
    tail = cp.next;
    p = head;
    while (p) {
      tail->random = clone[p->random];
      tail = tail->next;
      p = p->next;
    }

    return cp.next;
  }
};
