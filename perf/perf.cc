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

int main(int argc, char**argv) {

  size_t data_count = 1 << 20;
  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> distrib('a', 'z');

  std::vector<std::string> data;
  for (int n = 0; n < data_count; ++n) {
    std::string str;
    str.resize(distrib(gen));
    for (auto &c : str)
      c = (char)distrib(gen);

    data.emplace_back(std::move(str));
  }

  {
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

  {
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
  }

  return 0;
}
