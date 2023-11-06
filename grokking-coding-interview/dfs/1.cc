#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

struct TreeNode {
  TreeNode *left_;
  TreeNode *right_;
  int val_ = 0;
  TreeNode(int val) {
    val_ = val;
    left_ = nullptr;
    right_ = nullptr;
  }
};

class FindAllTreePaths {
 public:
  static vector<vector<int>> findPathes(TreeNode *root, int sum) {
    vector<vector<int>> all_paths;
    vector<int> cur_path;
    dfs(root, cur_path, sum, all_paths);
    return all_paths;
  }

 private:
  static void dfs(TreeNode *cur, vector<int> &cur_path, int sum,
                  vector<vector<int>> &all_paths) {
    if (!cur) {
      return;
    }
    if (cur->val_ == sum && !cur->left_ && !cur->right_) {
      all_paths.push_back(vector<int>(cur_path));
    }
    dfs(cur->left_, cur_path, sum - cur->val_, all_paths);
    dfs(cur->right_, cur_path, sum - cur->val_, all_paths);

    cur_path.pop_back();
  }
};

