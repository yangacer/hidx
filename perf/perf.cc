#include <chrono>
#include <iostream>
#include <random>
#include <unordered_set>

extern "C" {
#include "hidx/encap.h"
#include "hidx/hidx.h"
}

using TimePoint = std::chrono::steady_clock::time_point;
struct ElapsedTimer {
  auto elapsed() const {
    auto now = TimePoint::clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now - start);
  }
  void reset() { start = TimePoint::clock::now(); }
  TimePoint start = TimePoint::clock::now();
};

void gen_data(size_t count) {
  std::random_device rd;//Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> distrib('a', 'z');

  for (int n = 0; n < count; ++n) {
    std::string str;
    str.resize(distrib(gen));
    for (auto &c : str)
      c = (char)distrib(gen);
    std::cout << str << std::endl;
  }
}

std::vector<std::string> read_data() {
  std::string str;
  std::vector<std::string> data;
  while(std::getline(std::cin, str))
    data.emplace_back(str);
  return data;
}

int main(int argc, char**argv) {

  size_t data_count = 1 << 18;

  if (argc == 1) {
    gen_data(data_count);
    return 0;
  }
  char suit = argv[1][0];
  if (suit != 'u' && suit != 'h') {
    printf(
        "usage: perf [u|h] where\n"
        " u: Read data from stdin and run unordered set.\n"
        " h: Read data from stdin and run hidx.\n"
        " Generate data to stdout when no arguments.\n");
    return 0;
  }

  std::vector<std::string> data = read_data();

  if (suit == 'u') {
    std::cout << "[unordered set]\n";
    ElapsedTimer timer;

    struct Hash {
      std::hash<std::string> impl_;
      size_t operator()(const std::string* ptr) const { return impl_(*ptr); }
    };
    struct Equal {
      bool operator()(const std::string* lhs, const std::string* rhs) const {
        return *lhs == *rhs;
      }
    };

    std::unordered_set<std::string*, Hash, Equal> us;
    us.reserve(data_count * 0.7);
    for (auto& i : data)
      us.emplace(&i);
    std::cout << " insert=" << timer.elapsed().count() << std::endl;

    timer.reset();
    for (auto i : data) {
      if(!us.count(&i))
        abort();
    }
    std::cout << " count=" << timer.elapsed().count() << std::endl;
  }

  if (suit == 'h') {
    std::cout << "[hidx]\n";
    ElapsedTimer timer;

    auto hi = create_hidx(data_count * 0.7, [](const void* val) -> key_desc_t {
      auto *s = (std::string*)val;
      return { s->c_str(), s->size() };
    });

    for (auto& i : data)
      if(!call(hi, insert, &i))
        abort();

    std::cout << " insert=" << timer.elapsed().count() << std::endl;

    timer.reset();
    for (auto i : data)
      call(hi, count, {i.c_str(), i.size()});
    std::cout << " count=" << timer.elapsed().count() << std::endl;

    destroy_hidx(&hi);
  }

  return 0;
}
