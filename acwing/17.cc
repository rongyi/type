#include <iostream>
#include <vector>

using namespace std;

/**
 * Definition for singly-linked list.
 */

struct ListNode {
  int val;
  ListNode *next;
  ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
public:
  vector<int> printListReversingly(ListNode *head) {
    vector<int> collect;

    dfs(head, collect);

    return collect;
  }
  void dfs(ListNode *cur, vector<int> &res) {
    if (!cur) {
      return;
    }
    dfs(cur->next, res);
    res.push_back(cur->val);
  }
};
