#include "Random.h"
using namespace eosblox;

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#define ASSERT_EQ_UINT64(lhs, rhs, file, line, func)                                               \
  {                                                                                                \
    const auto lhs_ = static_cast<uint64_t>(lhs);                                                  \
    const auto rhs_ = static_cast<uint64_t>(rhs);                                                  \
    if (lhs_ != rhs_) {                                                                            \
      printf(                                                                                      \
        "Assertion failed in file %s, line %d, func %s():\n\texpected %llu\n\tgot      %llu\n",    \
        file, line, func, lhs_, rhs_);                                                             \
      abort();                                                                                     \
    }                                                                                              \
  }

#define ASSERT_SEED_EQ(gen, value)                                                                 \
  ASSERT_EQ_UINT64(gen.seed(), value, __FILE__, __LINE__, __FUNCTION__)

#define ASSERT_NEXT_EQ(gen, value)                                                                 \
  ASSERT_EQ_UINT64(gen.next(), value, __FILE__, __LINE__, __FUNCTION__)

static inline bool doubleEqual(const double a, const double b)
{
  constexpr double epsilon = 0.0000001;
  return std::abs(a - b) <= ((std::abs(a) < std::abs(b) ? std::abs(b) : std::abs(a)) * epsilon);
}

#define ASSERT_EQ_DOUBLE(lhs, rhs, file, line, func)                                               \
  {                                                                                                \
    const auto lhs_ = static_cast<double>(lhs);                                                    \
    const auto rhs_ = static_cast<double>(rhs);                                                    \
    if (!doubleEqual(lhs_, rhs_)) {                                                                \
      printf("Assertion failed in file %s, line %d, func %s():\n\texpected %g\n\tgot      %g\n",   \
             file, line, func, lhs_, rhs_);                                                        \
      abort();                                                                                     \
    }                                                                                              \
  }

#define ASSERT_NEXT_DOUBLE_EQ(gen, value)                                                          \
  ASSERT_EQ_DOUBLE(gen.nextDouble(), value, __FILE__, __LINE__, __FUNCTION__)

#define ASSERT_NEXT_IN_RANGE_EQ(gen, min, max, value)                                              \
  ASSERT_EQ_UINT64(gen.nextInRange(min, max), value, __FILE__, __LINE__, __FUNCTION__)

#define ASSERT_EQ_CONTAINER(lhs, rhs, file, line, func)                                            \
  {                                                                                                \
    if (lhs != rhs) {                                                                              \
      printf("Assertion failed in file %s, line %d, func %s():\n\texpected: ", file, line, func);  \
      for (const auto &elm : lhs) {                                                                \
        std::cout << elm << " ";                                                                   \
      }                                                                                            \
      std::cout << std::endl;                                                                      \
      printf("  got:      ");                                                                      \
      for (const auto &elm : rhs) {                                                                \
        std::cout << elm << " ";                                                                   \
      }                                                                                            \
      std::cout << std::endl;                                                                      \
      abort();                                                                                     \
    }                                                                                              \
  }

#define ASSERT_SHUFFLE_EQ(gen, data, value)                                                        \
  {                                                                                                \
    gen.shuffle(data);                                                                             \
    ASSERT_EQ_CONTAINER(data, value, __FILE__, __LINE__, __FUNCTION__)                             \
  }

void testSeedAccumulation()
{
  Random gen(1);
  ASSERT_SEED_EQ(gen, 1)

  // Integer.
  gen.accumSeed(1);
  ASSERT_SEED_EQ(gen, 11295943761408656531LLU);

  // Initializer list.
  gen.accumSeedRange({1, 2, 3});
  ASSERT_SEED_EQ(gen, 18083823154816788002LLU);

  // General ranges.
  gen.accumSeedRange(std::string("Hello, World!"));
  ASSERT_SEED_EQ(gen, 7880214006551886600LLU);

  const std::string str("Hello, World!");
  gen.accumSeedRange(str);
  ASSERT_SEED_EQ(gen, 11498166657420254716LLU);

  gen.accumSeedRange(std::vector<int>{1, 2, 3});
  ASSERT_SEED_EQ(gen, 4175012130207295590LLU);

  // Arrays.
  const int arr[3]{1, 2, 3};
  gen.accumSeedArray(arr);
  ASSERT_SEED_EQ(gen, 4326221722869819695LLU);
}

void testNext()
{
  {
    Random gen(1);
    ASSERT_NEXT_EQ(gen, 17243114145050246623LLU);
    ASSERT_NEXT_EQ(gen, 5516405404208600209LLU);
    ASSERT_NEXT_EQ(gen, 4897608421861629041LLU);
    ASSERT_NEXT_EQ(gen, 8669024630748878125LLU);
    ASSERT_NEXT_EQ(gen, 13562114715725036114LLU);
  }
  {
    Random gen(111970);
    ASSERT_NEXT_EQ(gen, 8987405948927848382LLU);
    ASSERT_NEXT_EQ(gen, 4621043010224079033LLU);
    ASSERT_NEXT_EQ(gen, 15658672667220659145LLU);
    ASSERT_NEXT_EQ(gen, 14814678247671488241LLU);
    ASSERT_NEXT_EQ(gen, 849456367364724273LLU);
  }
  {
    Random gen(1);
    gen.accumSeedRange(std::string("Hello, World!"));
    ASSERT_NEXT_EQ(gen, 14607242223934345243LLU);
    ASSERT_NEXT_EQ(gen, 12308690566605765243LLU);
    ASSERT_NEXT_EQ(gen, 6585550730876598598LLU);
    ASSERT_NEXT_EQ(gen, 5507355692446769180LLU);
    ASSERT_NEXT_EQ(gen, 16383900445848122088LLU);
  }
}

void testNextDouble()
{
  Random gen(42);
  ASSERT_NEXT_DOUBLE_EQ(gen, 8.485680e-02);
  ASSERT_NEXT_DOUBLE_EQ(gen, 3.902738e-01);
  ASSERT_NEXT_DOUBLE_EQ(gen, 2.926740e-01);
  ASSERT_NEXT_DOUBLE_EQ(gen, 7.088247e-01);
  ASSERT_NEXT_DOUBLE_EQ(gen, 8.789325e-01);
}

void testNextInRange()
{
  Random gen(42);
  ASSERT_NEXT_IN_RANGE_EQ(gen, 10, 20, 10);
  ASSERT_NEXT_IN_RANGE_EQ(gen, 10, 20, 13);
  ASSERT_NEXT_IN_RANGE_EQ(gen, 10, 20, 12);
  ASSERT_NEXT_IN_RANGE_EQ(gen, 10, 20, 17);
  ASSERT_NEXT_IN_RANGE_EQ(gen, 10, 20, 18);
}

void testShuffle()
{
  {
    Random gen(42);
    std::string str("Hello, World!");
    ASSERT_SHUFFLE_EQ(gen, str, std::string("llo,lrH!do eW"));
    ASSERT_SHUFFLE_EQ(gen, str, std::string("Hlr ,ldoWel!o"));
    ASSERT_SHUFFLE_EQ(gen, str, std::string("l,Woordl Hle!"));
    ASSERT_SHUFFLE_EQ(gen, str, std::string("dllooe,lHr !W"));
    ASSERT_SHUFFLE_EQ(gen, str, std::string("oeH!d,rllWo l"));
  }
  {
    Random gen(42);
    using IVec = std::vector<int>;
    IVec vec{1, 2, 3, 4, 5};
    ASSERT_SHUFFLE_EQ(gen, vec, (IVec{1, 2, 5, 4, 3}));
    ASSERT_SHUFFLE_EQ(gen, vec, (IVec{4, 5, 1, 2, 3}));
    ASSERT_SHUFFLE_EQ(gen, vec, (IVec{1, 4, 2, 3, 5}));
    ASSERT_SHUFFLE_EQ(gen, vec, (IVec{1, 3, 5, 4, 2}));
    ASSERT_SHUFFLE_EQ(gen, vec, (IVec{3, 5, 4, 1, 2}));
  }
}

int main(int argc, char **argv)
{
  testSeedAccumulation();
  testNext();
  testNextDouble();
  testNextInRange();
  testShuffle();
  return 0;
}
