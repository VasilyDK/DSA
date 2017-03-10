#include <cassert>
#include <iostream>

using namespace std;

typedef long long int ll;

// Greatest common divisor of x and y, runs in O(log(x + y))
ll GCD(ll x, ll y) {
    if (x == 0) return y;
    if (y == 0) return x;
    if (x > y) return GCD(x % y, y);
    return GCD(x, y % x);
}

// Modular power: x ^ y mod m, runs in O(log(y))
ll modpow(ll x, ll y, ll m) {
    ll ans = 1;
    while (y > 0) {
        if (y % 2 == 1) ans = (ans * x) % m;
        x = (x * x) % m;
        y /= 2;
    }
    return ans;
}


// Tests
int main() {
    assert(GCD(12, 1) == 1);
    assert(GCD(60, 90) == 30);
    assert(GCD(12345, 54321) == 3);
    assert(GCD(1000000007, 123456789) == 1);

    assert(modpow(12345, 0, 2385890) == 1);
    assert(modpow(123456, 1, 9283498) == 123456);
    assert(modpow(1234567, 1234567, 1000000007) == 496463181);

    return 0;
}
