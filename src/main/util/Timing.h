#include <chrono>
#include <unordered_map>
#include <string>

class TimeKeeper;

class Timer {
  public:
    explicit Timer(TimeKeeper &tk, std::string n) : timeKeeper(tk), name(std::move(n)) {
        start = std::chrono::high_resolution_clock::now();
    }
    ~Timer();

  private:
    TimeKeeper &timeKeeper;
    std::string name;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

class TimeKeeper {
  public:
    void add(const std::string &name, std::chrono::nanoseconds duration);
    [[nodiscard]] std::chrono::nanoseconds get(const std::string &name) const;

  private:
    std::unordered_map<std::string, std::chrono::nanoseconds> times;
};
