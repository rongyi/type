
#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

struct Job {
  int start_ = 0;
  int end_ = 0;
  int weight_ = 0;
  Job(int start, int end, int weight) {
    this->start_ = start;
    this->end_ = end;
    this->weight_ = weight;
  }
};

class MaxLoad {
 public:
  static int findMaxCPULoad(vector<Job> &jobs) {
    if (jobs.empty()) {
      return 0;
    }
    sort(jobs.begin(), jobs.end(),
         [](auto &l, auto &r) { return l.start_ < r.start_; });
    // push the end time
    priority_queue<pair<int, int>, vector<pair<int, int>>,
                   greater<pair<int, int>>>
        pq;
    int max_load = 0;
    int cur_load = 0;
    for (auto &m : jobs) {
      // job no overlap should go out and shrink load accordingly
      while (!pq.empty() && pq.top().first <= m.start_) {
        cur_load -= pq.top().second;
        pq.pop();
      }
      // add one job
      pq.push({m.end_, m.weight_});
      cur_load += m.weight_;
      max_load = max(max_load, cur_load);
    }

    return max_load;
  }
};

int main() {
  vector<Job> input = {{1, 4, 3}, {7, 9, 6}, {2, 5, 4}};
  cout << MaxLoad::findMaxCPULoad(input) << endl;
  input = {{6, 7, 10}, {8, 12, 15}, {2, 4, 11}};
  cout << MaxLoad::findMaxCPULoad(input) << endl;
  input = {{1, 4, 2}, {3, 6, 5}, {2, 4, 1}};
  cout << MaxLoad::findMaxCPULoad(input) << endl;
}
