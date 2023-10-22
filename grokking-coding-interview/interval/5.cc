#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

struct Meeting {
  int start_ = 0;
  int end_ = 0;
  Meeting(int start, int end) {
    this->start_ = start;
    this->end_ = end;
  }
};

class MinimumMeetingRooms {
 public:
  static int findMinimumMeetingRooms(vector<Meeting> &meetings) {
    if (meetings.empty()) {
      return 0;
    }
    sort(meetings.begin(), meetings.end(),
         [](auto &l, auto &r) { return l.start_ < r.start_; });
    // push the end time
    priority_queue<int, vector<int>, greater<int>> pq;
    int min_rooms = 0;
    for (auto &m : meetings) {
      // shrink all unessary rooms
      while (!pq.empty() && pq.top() <= m.start_) {
        pq.pop();
      }
      // use one room
      pq.push(m.end_);
      min_rooms = max(min_rooms, (int)pq.size());
    }

    return min_rooms;
  }
};

int main() {
  vector<Meeting> input = {{4, 5}, {2, 3}, {2, 4}, {3, 5}};
  int ret = MinimumMeetingRooms::findMinimumMeetingRooms(input);
  cout << ret << endl;
  input = {{1, 4}, {2, 5}, {7, 9}};
  ret = MinimumMeetingRooms::findMinimumMeetingRooms(input);
  cout << ret << endl;
  input = {{6, 7}, {2, 4}, {8, 12}};
  ret = MinimumMeetingRooms::findMinimumMeetingRooms(input);
  cout << ret << endl;
  input = {{1, 4}, {2, 3}, {3, 6}};
  ret = MinimumMeetingRooms::findMinimumMeetingRooms(input);
  cout << ret << endl;
  input = {{4, 5}, {2, 3}, {2, 4}, {3, 5}};
  ret = MinimumMeetingRooms::findMinimumMeetingRooms(input);
  cout << ret << endl;
}
