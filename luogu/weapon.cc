#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <vector>
using namespace std;

int x = 0;
int y = 0;
int m = 0;
int n = 0;
int pack_sz = 0;
int cur_sz = 0;
int timestamp = 0;
int op_sz = 0;
int ko = 0;
using GridValue = array<int, 3>;
vector<vector<GridValue>> grid =
    vector<vector<GridValue>>(101, vector<GridValue>(101));
vector<vector<int>> visited = vector<vector<int>>(101, vector<int>(101, 0));
// 霰弹枪>突击步枪>射手步枪>栓动步枪>空手
array<int, 10> gun_priority = {0, 3, 3, 2, 1, 3, 2, 2, 4, 4};

// gun matchec bullet type
array<int, 10> gun_bullet{0, 14, 14, 14, 14, 15, 15, 15, 16, 16};
// space_unit

array<int, 17> space_unit{0, 0,  0,  0,  0,  0, 0, 0, 0,
                          0, 50, 40, 30, 20, 2, 1, 5};
// first: ID
// second: num
using PackageValue = pair<int, int>;
array<PackageValue, 17> package = {};

int main_weapon = 0;
int sec_weapon = 0;

void switch_weapon() {
  int l1 = gun_priority[main_weapon];
  int l2 = gun_priority[sec_weapon];
  int newl = gun_priority[grid[x][y][0]];
  if (newl > l1) {
    main_weapon = grid[x][y][0];
  } else if (newl != l1 && newl > l2) {
    sec_weapon = grid[x][y][0];
  }
}

// id count
void update_inventory() {
  int id = grid[x][y][0];
  int num = grid[x][y][1];
  int need_space = space_unit[id] * num;
  if (cur_sz + need_space <= pack_sz) {
    cur_sz += need_space;
    package[id].first += num;
    package[id].second = timestamp;
    return;
  }

  int cur_ts = numeric_limits<int>::max();
  int id1 = gun_bullet[main_weapon];
  int id2 = gun_bullet[sec_weapon];
  int delete_id = -1;
  while (cur_sz + need_space > pack_sz) {
    for (int i = 14; i <= 16; i++) {
      if (package[i].first == 0 || i == id1 || id == id2) {
        continue;
      }
      if (package[i].second < cur_ts) {
        cur_ts = package[i].second;
        delete_id = i;
      }
    }
    if (cur_ts == numeric_limits<int>::max()) {
      break;
    }
    cur_ts = numeric_limits<int>::max();
    int drop_num = (cur_sz + need_space - pack_sz + space_unit[delete_id] - 1) /
                   space_unit[delete_id];
    drop_num = min(drop_num, package[delete_id].first);
    cur_sz -= drop_num * space_unit[delete_id];
    package[delete_id].first -= drop_num;
  }
  vector<int> ids(17, 0);
  iota(ids.begin(), ids.end(), 0);
  sort(ids.begin(), ids.end(),
       [&](auto &l, auto &r) { return package[l].second < package[r].second; });
  for (auto &id : ids) {
    if (cur_sz + need_space <= pack_sz) {
      break;
    }
    if (package[id].first == 0) {
      continue;
    }
    int drop_num =
        (cur_sz + need_space - pack_sz + space_unit[id] - 1) / space_unit[id];
    drop_num = min(drop_num, package[id].first);
    cur_sz -= drop_num * space_unit[id];
    package[id].first -= drop_num;
  }
  cur_sz += need_space;
  package[id].first += num;
  package[id].second = timestamp;
}

void fight() {
  int havea = package[main_weapon].first;
  int haveb = package[sec_weapon].first;

  if (havea >= grid[x][y][1]) {
    package[main_weapon].first -= grid[x][y][1];
    cur_sz -= space_unit[gun_bullet[main_weapon]] * grid[x][y][1];
  } else if (haveb >= grid[x][y][2]) {
    package[sec_weapon].first -= grid[x][y][2];
    cur_sz -= space_unit[gun_bullet[sec_weapon]] * grid[x][y][2];
  } else {
    ko = 1;
  }
}

// pick up or fight!
void entry() {
  if (visited[x][y]) {
    return;
  }
  visited[x][y] = 1;

  int v = grid[x][y][0];
  // a weapon, update main/sec weapon
  if (v >= 1 && v <= 9) {
    switch_weapon();
  } else if (v >= 10 && v <= 16) {
    update_inventory();
  } else if (v == 17) {
    fight();
  }
}
vector<vector<int>> dirs = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

int main() {
  cin >> m >> n >> pack_sz >> op_sz;
  pack_sz *= 10;

  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      int first = 0;
      int second = 0;
      cin >> grid[i][j][0] >> grid[i][j][1];
      if (grid[i][j][0] == 17) {
        int third = 0;
        cin >> grid[i][j][2];
      }
    }
  }

  vector<int> ops(op_sz, 0);
  for (int i = 0; i < op_sz; i++) {
    cin >> ops[i];
    ops[i] -= 1;
  }

  entry();

  for (auto &d : ops) {
    timestamp++;
    x += dirs[d][0];
    y += dirs[d][1];
    if (ko) {
      break;
    }
    entry();
  }
  if (ko) {
    printf("%d %d\n", x, y);
  } else {
    printf("%d\n", main_weapon);
    printf("%d\n", sec_weapon);

    vector<int> ids(17, 0);
    iota(ids.begin(), ids.end(), 0);
    sort(ids.begin(), ids.end(), [&](auto &l, auto &r) {
      return package[l].second < package[r].second;
    });
    for (auto id : ids) {
      if (package[id].first > 0) {
        printf("%d %d\n", id, package[id].first);
      }
    }
  }

  // action
}
