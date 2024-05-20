#include "xxx.hpp"
#include <string>

class Solution {
public:
  // Encodes a tree to a single string.
  string serialize(TreeNode *root) {
    if (!root) {
      return "";
    }
    cache.clear();
    queue<TreeNode *> q;
    q.push(root);
    while (!q.empty()) {
      auto cur = q.front();
      q.pop();
      if (cur) {
        cache += to_string(cur->val) + ",";
        q.push(cur->left);
        q.push(cur->right);
      } else {
        cache += "nil,";
      }
    }

    return cache;
  }

  // Decodes your encoded data to tree.
  TreeNode *deserialize(string data) {
    if (data.empty()) {
      return nullptr;
    }
    int i = 0;
    int j = data.find(',', i);
    queue<TreeNode *> q;
    TreeNode *root = new TreeNode(stoi(data.substr(0, j - i)));
    q.push(root);
    while (!q.empty()) {
      auto cur = q.front();
      i = j + 1;
      j = data.find(',', i);
      string tmp = data.substr(i, j - i);
      if (tmp != "nil") {
        cur->left = new TreeNode(stoi(tmp));
        q.push(cur->left);
      }
      i = j + 1;
      j = data.find(',', i);
      tmp = data.substr(i, j - i);
      if (tmp != "nil") {
        cur->right = new TreeNode(stoi(tmp));
        q.push(cur->right);
      }

      q.pop();
    }
    return root;
  }

private:
  string cache;
};
