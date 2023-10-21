#include <iostream>
using namespace std;

class HappyNumber {
 public:
  static bool find(int num) {
    int slow = next(num);
    int fast = next(next(num));

    while (slow != fast) {
      slow = next(slow);
      fast = next(next(fast));
    }

    return slow == 1;
  }

 private:
  static int next(int num) {
    int val = 0;
    while (num) {
      int d = num % 10;
      val += d * d;
      num /= 10;
    }

    return val;
  }
};

int main() {
  cout << HappyNumber::find(23) << endl;
  cout << HappyNumber::find(12) << endl;
}
