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

class Diameter {
 public:
  int findDiameter(TreeNode *root) {
    dfs(root);
    return diameter_;
  }

 private:
  int dfs(TreeNode *cur) {
    if (!cur) {
      return 0;
    }
    int l = dfs(cur->left_);
    int r = dfs(cur->right_);
    int cur_meter = l + r + 1;
    diameter_ = max(diameter_, cur_meter);

    return max(l, r) + 1;
  }

 private:
  int diameter_ = 0;
};

class FindAllTreePaths {
 public:
  static vector<vector<int>> findPathes(TreeNode *root, int sum) {
    vector<vector<int>> all_paths;
    vector<int> cur_path;
    dfs(root, cur_path, sum, all_paths);
    return all_paths;
  }
  static int findAnyPathTopDown(TreeNode *root, int t) {
    vector<int> cur_path;
    return countPath(root, t, cur_path);
  }

 private:
  static int countPath(TreeNode *cur, int t, vector<int> &cur_path) {
    if (!cur) {
      return 0;
    }
    cur_path.push_back(cur->val_);
    int ret = 0;
    int psum = 0;
    for (auto it = cur_path.rbegin(); it != cur_path.rend(); ++it) {
      psum += *it;
      if (psum == t) {
        ret++;
      }
    }

    ret += countPath(cur->left_, t, cur_path);
    ret += countPath(cur->right_, t, cur_path);

    cur_path.pop_back();

    return ret;
  }
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

