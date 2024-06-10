#include <cstring>
#include <ios>
#include <iostream>
using namespace std;

struct node {
  node *child[26];
  int cnt;
  bool is_leaf;

  node() {
    memset(child, 0, sizeof(child));
    cnt = 0;
    is_leaf = false;
  }
};

struct trie {
  node *root;

  void insert(string &s) {
    node *cur = root;
    for (auto c : s) {
      auto idx = c - 'a';
      if (!cur->child[idx]) {
        cur->child[idx] = new node();
      }
      cur = cur->child[idx];
    }

    cur->is_leaf = true;
    cur->cnt += 1;
  }
  int search(string &s) {
    node *cur = root;
    int sum = 0;
    for (auto c : s) {
      auto idx = c - 'a';
      if (!cur->child[idx]) {
        return sum;
      }
      cur = cur->child[idx];
      if (cur->is_leaf) {
        sum += cur->cnt;
      }
    }
    return sum;
  }
  trie() { root = new node(); }
};

int main() {
  ios::sync_with_stdio(false);
  int n, m;
  cin >> n >> m;
  trie t;
  for (int i = 0; i < n; i++) {
    string s;
    cin >> s;
    t.insert(s);
  }
  for (int i = 0; i < m; i++) {
    string s;
    cin >> s;
    cout << t.search(s) << endl;
  }

  return 0;
}
