
struct ListNode {
  int val_;
  ListNode *next_;
  ListNode(int val) {
    this->val_ = val;
    this->next_ = nullptr;
  }
};

class MiddleLinkList {
 public:
  static ListNode *middle(ListNode *head) {
    ListNode *slow = head;
    ListNode *fast = head;
    while (fast && fast->next_) {
      slow = slow->next_;
      fast = fast->next_->next_;
    }

    return slow;
  }
};

int main() {}
