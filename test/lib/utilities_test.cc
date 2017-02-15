// utilities_test.cc
// Oct 13, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

#include "types.h"
#include "utilities.h"
#include "gtest/gtest.h"
#include <iostream>
#include <sstream>

namespace corct {
void calls_here()  // DO NOT MOVE THIS EVER!! (or fix the line # in the test.)
{
  HERE("hola, testy amigos!");
  return;
}

TEST(utilities, here) {
  std::stringstream s;
  std::streambuf* orig_buf = std::cout.rdbuf(s.rdbuf());
  calls_here();
  // restore cout
  std::cout.rdbuf(orig_buf);
  string_t exp_str = "calls_here:15: hola, testy amigos!\n";
  EXPECT_EQ(exp_str, s.str());
}  // TEST(utilities, here)

TEST(utilities, add_tab) {
  string_t const s1("ooga");
  string_t const s2 = add_tab(s1);
  EXPECT_TRUE(s2 == (s1 + '\t'));
}

TEST(utilities, remove_tab) {
  string_t const s1("ooga\t");
  string_t const s2 = remove_tab(s1);
  EXPECT_TRUE(s2 == "ooga");
}

TEST(utilities, in_vec) {
  using corct::in_vec;
  {
    using vec_t = std::vector<int>;
    vec_t v1 = {1, 2, 3};
    EXPECT_TRUE(in_vec(v1, 1));
    EXPECT_FALSE(in_vec(v1, 4));
  }
  {
    using vec_t = std::vector<double>;
    vec_t v1 = {1, 2, 3};
    EXPECT_TRUE(in_vec(v1, 1));
    EXPECT_FALSE(in_vec(v1, 4));
  }
}  // TEST(utilities,in_vec)

TEST(utilities, split) {
  string_t s = "foo,bar,bumz,bibz";
  vec_str ss = split(s, ',');
  vec_str ss_exp = {"foo", "bar", "bumz", "bibz"};
  bool const ok = std::equal(ss.begin(), ss.end(), ss_exp.begin());
  EXPECT_TRUE(ok);
}

TEST(utilities, check_ptr) {
  std::stringstream s;
  std::streambuf* orig_buf = std::cout.rdbuf(s.rdbuf());
  check_ptr<double>(nullptr,"bob","  ");
  // restore cout
  std::cout.rdbuf(orig_buf);
  string_t exp_str = "  Invalid pointer bob\n";
  EXPECT_EQ(exp_str, s.str());
}  // TEST(utilities, check_ptr)


} // corct::

// End of file
