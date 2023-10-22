
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

struct Interval {
  int start_ = 0;
  int end_ = 0;
  Interval(int start, int end) {
    this->start_ = start;
    this->end_ = end;
  }
};

class MergeIntervals {
 public:
  // two sorted interval
  static vector<Interval> merge(vector<Interval> &arr1,
                                vector<Interval> &arr2) {
    int sz1 = arr1.size();
    int sz2 = arr2.size();
    int i = 0;
    int j = 0;
    vector<Interval> ret;
    while (i < sz1 && j < sz2) {
      if ((arr1[i].start_ >= arr2[j].start_ &&
           arr1[i].start_ <= arr2[j].end_) ||
          (arr2[j].start_ >= arr1[i].start_ &&
           arr2[j].start_ <= arr1[i].end_)) {
        ret.push_back({max(arr1[i].start_, arr2[j].start_),
                       min(arr1[i].end_, arr2[j].end_)});
      }

      // we keep the large end
      if (arr1[i].end_ < arr2[j].end_) {
        i++;
      } else {
        j++;
      }
    }

    return ret;
  }
  static vector<Interval> insert_sorted(vector<Interval> &intervals,
                                        Interval itv) {
    int sz = intervals.size();
    int i = 0;
    // ignore all intervals exactly before itv
    vector<Interval> ret;
    for (; i < sz && intervals[i].end_ < itv.start_; i++) {
      ret.push_back(intervals[i]);
    }
    // merge action, and it's chain action, so we use a loop
    for (; i < sz && intervals[i].start_ <= itv.end_; i++) {
      itv.start_ = min(itv.start_, intervals[i].start_);
      itv.end_ = max(itv.end_, intervals[i].end_);
    }

    ret.push_back(itv);
    for (; i < sz; i++) {
      ret.push_back(intervals[i]);
    }

    return ret;
  }
  static vector<Interval> merge(vector<Interval> &intervals) {
    sort(intervals.begin(), intervals.end(),
         [](auto &l, auto &r) { return l.start_ <= r.start_; });
    int sz = intervals.size();
    int prev_start = intervals[0].start_;
    int prev_end = intervals[0].end_;

    vector<Interval> ret;
    for (int i = 1; i < sz; i++) {
      // no intersection, safely intert the prev Interval
      if (intervals[i].start_ > prev_end) {
        ret.push_back(Interval(prev_start, prev_end));
        prev_start = intervals[i].start_;
        prev_end = intervals[i].end_;
      } else {
        // merge action
        prev_end = max(prev_end, intervals[i].end_);
      }
    }
    // the last section
    ret.push_back(Interval(prev_start, prev_end));

    return ret;
  }
};

int main() {
  // vector<Interval> input = {{1, 4}, {2, 5}, {7, 9}};
  // auto mi = MergeIntervals::merge(input);
  // for (auto &it : mi) {
  // cout << it.start_ << " " << it.end_ << endl;
  //}
  //
  vector<Interval> input1 = {{1, 3}, {5, 6}, {7, 9}};
  vector<Interval> input2 = {{2, 3}, {5, 7}};
  auto m = MergeIntervals::merge(input1, input2);
  for (auto &it : m) {
    cout << it.start_ << " " << it.end_ << endl;
  }
}
