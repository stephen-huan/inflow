#include <cassert>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

typedef long long ll;
typedef std::vector<ll> li;
typedef std::vector<std::string> ls;
typedef std::pair<ll, ll> pi;

// infinity, but not really
const ll INF = std::numeric_limits<ll>::max();
// characters allowed to be in a prefix
const std::unordered_set<char> PREFIX = {
    ' ', '>', ':', '-', '*', '|', '#', '$', '%', '"', '\'',
};

// fraction methods
inline ll det(pi const &x, pi const &y) {
  /* ad - bc */
  return x.first * y.second - x.second * y.first;
}
inline pi mul(pi const &x, pi const &y) {
  /* x * y */
  return std::make_pair(x.first * y.first, x.second * y.second);
}
inline pi sub(pi const &x, pi const &y) {
  /* x - y */
  return std::make_pair(det(x, y), x.second * y.second);
}
inline bool cmp(pi const &x, pi const &y) {
  /* x < y */
  return det(x, y) < 0;
}

std::pair<li, ll> get_lines(ls const &para, ll width) {
  /* Compute optimal line lengths with forward greedy. */
  std::size_t i;
  ll num, x, v, chars;
  i = 0;
  num = 0;
  chars = 0;
  li lines = {0};
  while (i < para.size()) {
    x = 0;
    v = para[i].size();
    num++;
    while (v <= width and i + 1 < para.size()) {
      i++;
      x = v;
      v += 1 + para[i].size();
      lines.push_back(num);
    }
    chars += x;
    if (v <= width) {
      i++;
      chars += v - x;
      lines.push_back(num);
    }
  }
  return std::make_pair(lines, chars);
}

std::vector<pi> vardp(ls const &para, li const &lines, ll width) {
  /* Computes the minimum variance, constrained to use optimal lines.
   * Minimizing variance is equivalent to minimizing the sum of squares,
   * if the number of lines is constrained. */
  std::vector<pi> dp = {std::make_pair(0, 0)};
  ll j, x, v, k, sum_x2, sum_x2j;
  for (size_t i = 1; i < para.size() + 1; i++) {
    k = 0;
    x = 0;
    sum_x2 = INF;
    for (j = i - 1; j >= 0; j--) {
      v = x + (x != 0) + para[j].size();
      if (v <= width) {
        x = v;
        sum_x2j = dp[j].second;
        sum_x2j += x * x;
        if (sum_x2j < sum_x2 and lines[j] + 1 == lines[i]) {
          k = j;
          sum_x2 = sum_x2j;
        }
      } else {
        break;
      }
    }
    dp.push_back(std::make_pair(k, sum_x2));
  }
  return dp;
}

ll process_dp(ls const &para, li const &lines, ll chars,
              std::vector<pi> const &dp, ll width) {
  /* Finds an ending index to minimize variance, ignoring the last line. */
  // if the paragraph is less than three lines long, don't ignore the last line
  if (lines.back() <= 3) {
    return para.size();
  }
  ll x, kl, kg;
  pi bestl, bestg, mean, var;
  // find best last line starting point, initialize best to infinity (1/0)
  // maintain two solutions: last line shorter than average, last line greater
  bestl = std::make_pair(1, 0);
  bestg = std::make_pair(1, 0);
  kl = 0;
  kg = 0;
  x = 0;
  for (size_t i = para.size() - 1; i >= 0; i--) {
    x += (x != 0) + para[i].size();
    if (x > width) {
      break;
    }
    // Var[X] = E[X^2] - E[X]^2
    mean = std::make_pair(chars - x, lines[i]);
    var = sub(std::make_pair(dp[i].second, lines[i]), mul(mean, mean));
    if (lines[i] + 1 == lines.back()) {
      if (det(std::make_pair(x, 1), mean) <= 0) {
        if (cmp(var, bestl)) {
          bestl = var;
          kl = i;
        }
      } else {
        if (cmp(var, bestg)) {
          bestg = var;
          kg = i;
        }
      }
    }
  }
  // use shorter last line if it exists, otherwise default to greater
  return (bestl.second != 0) ? kl : kg;
}

std::pair<ls, std::string> parse_prefix(ls const &lines, ll width) {
  /* Parses lines into a vector of tokens, taking into account prefixes. */
  // find prefix, where a prefix is defined as a series
  // of the same character, if the character is in PREFIX
  ls para;
  std::string prefix;
  bool end = false;
  for (size_t ch = 0; ch < lines[0].size(); ch++) {
    if (PREFIX.find(lines[0][ch]) == PREFIX.end()) {
      break;
    }
    for (auto &line : lines) {
      if (ch >= line.size() or line[ch] != lines[0][ch]) {
        end = true;
        break;
      }
    }
    if (end) {
      break;
    } else {
      prefix += lines[0][ch];
    }
  }
  // remove prefix from each line and load into tokens
  width -= prefix.size();
  for (auto &line : lines) {
    std::istringstream ss(line.substr(prefix.size()));
    if (line.size() != 0) {
      std::string token;
      while (ss >> token) {
        if ((ll)token.size() > width) {
          std::cout << "AssertionError: word too long: " + token << std::endl;
        }
        assert((ll)token.size() <= width);
        para.push_back(token);
      }
    }
  }
  return std::make_pair(para, prefix);
}

void process(ls const &lines, ll width) {
  /* Processes lines into a final paragraph and prints it out. */
  ls para;
  std::string prefix;
  std::vector<pi> dp;
  ll chars;
  size_t k;
  li line_lengths;
  std::vector<size_t> out;
  tie(para, prefix) = parse_prefix(lines, width);
  // don't include prefix
  width -= prefix.size();
  tie(line_lengths, chars) = get_lines(para, width);
  dp = vardp(para, line_lengths, width);
  k = process_dp(para, line_lengths, chars, dp, width);

  // generate indexes in reverse order
  out = {k};
  while (out.back() > 0) {
    out.push_back(dp[out.back()].first);
  }
  // print out each line, given by contiguous segments of the paragraph
  for (size_t i = out.size() - 1; i > 0; i--) {
    std::cout << prefix;
    for (size_t j = out[i]; j < out[i - 1] - 1; j++) {
      std::cout << para[j] << ' ';
    }
    std::cout << para[out[i - 1] - 1] << '\n';
  }
  // print last line, if it exists
  if (k < para.size()) {
    std::cout << prefix;
    for (size_t i = k; i < para.size() - 1; i++) {
      std::cout << para[i] << ' ';
    }
    std::cout << para.back() << '\n';
  }
}

int main(int argc, char *argv[]) {
  // fast cin
  std::ios::sync_with_stdio(false);
  std::cin.tie(NULL);

  // read command line arguments - one parameter, width
  ll width = (argc > 1) ? (ll)std::stoi(argv[1]) : 79;

  // read input into paragraph blocks, maintaing empty lines
  ls lines;
  for (std::string line; getline(std::cin, line);) {
    if (line.size() > 0) {
      lines.push_back(line);
    } else {
      if (lines.size() > 0) {
        process(lines, width);
      }
      lines.clear();
      std::cout << '\n';
    }
  }
  if (lines.size() > 0) {
    process(lines, width);
  }

  return 0;
}
