#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
  vector<int> printMatrix(vector<vector<int>> matrix) {
    if (matrix.empty()) {
      return {};
    }
    auto m = matrix.size();
    auto n = matrix[0].size();
    vector<int> res;
    auto sz = m * n;

    auto i = 0;
    auto j = 0;
    auto cnt = 0;

    // inclusive
    auto left_end = 0;
    auto right_end = n - 1;
    auto down_end = m - 1;
    auto top_end = 0;

    while (cnt < sz) {
      for (int j = left_end; j <= right_end && cnt < sz; j++) {
        // cout << "ry: " << top_end << ":" << j << " -> " << matrix[top_end][j]
        // << endl;
        res.push_back(matrix[top_end][j]);
        cnt += 1;
      }
      top_end += 1;
      // todo: righ_end--
      for (int i = top_end; cnt < sz && i <= down_end; i++) {
        res.push_back(matrix[i][right_end]);
        cnt += 1;
      }
      right_end -= 1;

      for (int j = right_end; cnt < sz && j >= left_end; j--) {
        res.push_back(matrix[down_end][j]);
        cnt += 1;
      }
      down_end -= 1;

      for (int i = down_end; cnt < sz && i >= top_end; i--) {
        res.push_back(matrix[i][left_end]);
        cnt += 1;
      }

      left_end += 1;
    }

    return res;
  }
};

int main() {
  Solution so;
  // vector<vector<int>> input{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};
  vector<vector<int>> input{{}};
  so.printMatrix(input);
}
