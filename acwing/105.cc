#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;
const int N = 2e5 + 5;

int peoples[N];
int by_voice[N];
int by_title[N];
// put all lang to this vector, and using sort|uniq to get total langs
// and assign lang id with this uniq arrays index
vector<int> langs;
int normalized[3 * N];

// this is how we map user input lang to our normalized lang idx
int normal_idx(int val) {
  return lower_bound(langs.begin(), langs.end(), val) - langs.begin();
}

int main() {
  int n = 0;
  cin >> n;
  for (int i = 1; i <= n; i++) {
    cin >> peoples[i];
    langs.push_back(peoples[i]);
  }
  int m = 0;
  cin >> m;
  for (int i = 1; i <= m; i++) {
    cin >> by_voice[i];
    langs.push_back(by_voice[i]);
  }
  for (int i = 1; i <= m; i++) {
    cin >> by_title[i];
    langs.push_back(by_title[i]);
  }

  sort(langs.begin(), langs.end());
  langs.erase(unique(langs.begin(), langs.end()), langs.end());
  for (int i = 1; i <= n; i++) {
    normalized[normal_idx(peoples[i])] += 1;
  }
  int max_by_voice = 0;
  int max_by_title = 0;
  int ret = 1;
  for (int i = 1; i <= m; i++) {
    int cur_voice = normalized[normal_idx(by_voice[i])];
    int cur_title = normalized[normal_idx(by_title[i])];
    if (cur_voice > max_by_voice) {
      max_by_voice = cur_voice;
      ret = i;
      max_by_title = cur_title;
    } else if (cur_voice == max_by_voice && cur_title > max_by_title) {
      max_by_title = cur_title;
      ret = i;
    }
  }
  cout << ret << endl;
  return 0;
}
