#include "xxx.hpp"
#include <list>

class Solution {
public:
  // Insert one char from stringstream
  void insert(char ch) {
    cnt[ch] += 1;
    if (cnt[ch] == 1) {
      uniq.push_back(ch);
    } else {
      for (auto it = uniq.begin(); it != uniq.end(); it++) {
        if (*it == ch) {
          uniq.erase(it);
          break;
        }
      }
    }
  }
  // return the first appearence once char in current stringstream
  char firstAppearingOnce() {
    if (uniq.empty()) {
      return '#';
    }
    // dont goways
    return uniq.front();
  }

private:
  // thesa are all the candidate
  list<char> uniq;
  map<char, int> cnt;
};
