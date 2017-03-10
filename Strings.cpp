#include <algorithm>
#include <iostream>
#include <cassert>
#include <vector>

#define MAX_STRING_SIZE 1000000
#define LOG2_STRING_SIZE 20

using namespace std;

// Suffix Array and LCP Array Construction
int suffixRank[LOG2_STRING_SIZE][MAX_STRING_SIZE] = {{ 0 }};

struct suffixTuple{
    int originalIndex;
    int firstHalf;
    int secondHalf;
};

int compare(suffixTuple x, suffixTuple y){
    if (x.firstHalf == y.firstHalf) return x.secondHalf < y.secondHalf;
    return x.firstHalf < y.firstHalf;
}

void getSuffixArray(string s, vector<int> &out){
    int n = s.size();

    for (int i = 0; i < n; i++) {
        suffixRank[0][i] = s[i] - 'a';
    }

    suffixTuple suffix[n];

    for (int cnt = 1, stp = 1; cnt < n; cnt *= 2, stp++){
        for (int i = 0; i < n; i++){
            suffix[i].firstHalf = suffixRank[stp - 1][i];
            suffix[i].secondHalf = i + cnt < n ? suffixRank[stp - 1][i + cnt] : -1;
            suffix[i].originalIndex = i;
        }

        sort(suffix, suffix + n, compare);

        suffixRank[stp][suffix[0].originalIndex] = 0;

        for (int i = 1, currRank = 0; i < n; i++){
            if (suffix[i - 1].firstHalf != suffix[i].firstHalf ||
                suffix[i - 1].secondHalf != suffix[i].secondHalf) currRank++;
            suffixRank[stp][suffix[i].originalIndex] = currRank;
        }
    }

    out.resize(n, 0);
    for (int i = 0; i < n; i++) out[i] = suffix[i].originalIndex;
}

void getLCP(string s, vector<int> &suffix, vector<int> &lcp){
    lcp.resize(s.size(), 0);
    int k = 0, n = s.size();
    vector<int> rank(n, 0);

    for (int i = 0; i < n; i++) rank[suffix[i]] = i;

    for (int i = 0; i < n; i++, k?k--:0){
        if (rank[i] == n - 1) {k = 0; continue;}
        int j = suffix[rank[i] + 1];
        while (i + k < n && j + k < n && s[i + k] == s[j + k]) k++;
        lcp[rank[i]] = k;
    }
}

// Runs Manacher's algorithm to find the longest palindromes
//   centered at each point in a string w/ buffers
//   e.g. abcbaddaca -> |a|b|c|b|a|d|d|a|c|a|
void manacher(string s, vector<int> &p) {
    int ps = s.size() * 2 + 1;
    p.resize(ps, 0);

    int c = 0, r = 0;
    int m = 0, n = 0;
    for (int i = 1; i < ps; i++) {
        if (i>r) {
            p[i] = 0;
            m = i - 1;
            n = i + 1;
        } else {
            int i2 = c * 2 - i;
            if (p[i2] < (r - i - 1)) {
                p[i] = p[i2];
                m = -1;
            } else {
                p[i] = r - i;
                n = r + 1;
                m = i * 2 - n;
            }
        }
        while (m >= 0 && n < ps && (((m % 2 == 0) && (n % 2 == 0)) || (s[m/2] == s[n/2]))) {
            p[i]++;
            m--;
            n++;
        }
        if ((i + p[i]) > r) {
            c = i;
            r = i + p[i];
        }
    }
}

// Returns an array with the length of the longest palindrome
//   starting at each character in the string
void getLongestPalindrome(string s, vector<int> &LP) {
    LP.resize(s.size(), 1);
    vector<int> p;
    manacher(s, p);
    for (int i = 1; i < p.size(); i++) {
        if (i % 2 == 0) {
            if (p[i] > 0) {
                for (int j = 1; j <= p[i] / 2; j++){
                    LP[i / 2 - j] = max(2 * j, LP[i / 2 - j]);
                }
            }
        } else {
            if (p[i] > 1) {
                for (int j = 1; j <= p[i] / 2; j++){
                    LP[i / 2 - j] = max(2 * j + 1, LP[i / 2 - j]);
                }
            }
        }
    }
}


// Tests
int expectedSuffix[10] = {9, 0, 7, 4, 3, 1, 8, 2, 6, 5};
int expectedLCP[10] = {1, 1, 1, 0, 1, 0, 1, 0, 1, 0};
int expectedManacher[21] = {0, 1, 0, 1, 0, 5, 0, 1, 0, 1, 0, 1, 4, 1, 0, 1, 0, 3, 0, 1, 0};
int expectedLP[10] = {5, 3, 1, 1, 4, 2, 1, 3, 1, 1};

int main() {
    string s = "abcbaddaca";

    vector<int> suffix, lcp, manach, lp;
    getSuffixArray(s, suffix);
    getLCP(s, suffix, lcp);
    manacher(s, manach);
    getLongestPalindrome(s, lp);

    for (int i = 0; i < 10; i++) {
        assert(expectedSuffix[i] == suffix[i]);
        assert(expectedLCP[i] == lcp[i]);
        assert(expectedLP[i] == lp[i]);
    }

    for (int i = 0; i < 21; i++) {
        assert(expectedManacher[i] == manach[i]);
    }

    return 0;
}
