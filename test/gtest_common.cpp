#include "common.h"
#include "graph_exception.h"
#include "standard_graph.h"
#include <cassert>
#include "gtest/gtest.h"
#include <random>
#include <algorithm>
#include <iterator>
#include <functional>

WT_CONNECTION *conn;
WT_CURSOR *cursor;
WT_SESSION *session;
const char *home;

void __setup()
{
  // Create new directory for WT DB
  std::filesystem::path dirname = "./db/common_test";
  if (std::filesystem::exists(dirname))
  {
    filesystem::remove_all(dirname); // remove if exists;
  }
  std::filesystem::create_directories(dirname);

  wiredtiger_open(dirname.c_str(), NULL, "create", &conn);
  conn->open_session(conn, NULL, NULL, &session);
}

void __close()
{
  conn->close(conn, NULL);
  filesystem::remove_all("./db/common_test");
}

TEST(CommonTest, TestPackStringVector)
{
  __setup();
  string testname = "String Vector Packing";
  int ret = 0;

  vector<string> test = {"hello", "world", "this", "is", "test", "string", "packing"
                                                                           "0.00",
                         "0.00", "last"};

  size_t size;
  string format;
  char *buffer = CommonUtil::pack_string_vector_wt(test, session, &size, &format);
  vector<string> result = CommonUtil::unpack_string_vector_wt(buffer, session);
  ASSERT_EQ(result, test) << " Packed string vector matches unpacked string vector";
  __close();
}

TEST(CommonTest, TestPackIntVector)
{
  __setup();
  string testname = "Integer Vector Packing";
  int ret = 0;

  vector<int> test = {1, 2, 3, 4, 55, 66, 77, 888, 999};
  size_t size;
  string format;
  char *buffer = CommonUtil::pack_int_vector_wt(test, session, &size, &format);
  vector<int> result = CommonUtil::unpack_int_vector_wt(buffer, session);

  ASSERT_EQ(result, test) << " Packed Int Vector matches unpacked Int Vector";
  __close();
}

TEST(CommonTest, TestPackString)
{
  __setup();
  string test_string = "This is the string being packed.";
  size_t size;
  string format;

  char *buffer = CommonUtil::pack_string_wt(test_string, session, &format);
  string result_string = CommonUtil::unpack_string_wt(buffer, session);

  ASSERT_EQ(result_string, test_string) << "Packed string and unpacked strings match";
  __close();
}
TEST(CommonTest, TestPackStringVectorStd)
{
  __setup();
  string testname = "String Vector Packing";
  int ret = 0;

  vector<string> test = {"hello", "world", "this", "is", "test", "string", "packing", "0.00", "0.00", "last"};

  size_t size;
  string format;
  string buffer = CommonUtil::pack_string_vector_std(test, &size);
  vector<string> result = CommonUtil::unpack_string_vector_std(buffer);
  ASSERT_EQ(result, test) << " Packed string vector matches unpacked string vector";
  __close();
}

TEST(CommonTest, TestPackIntVectorStd)
{
  __setup();
  string testname = "Integer Vector Packing";
  int ret = 0;

  // vector<int> test = {1, 2, 3, 4, 55, 66, 77, 888, 999};
  size_t size;

  // First create an instance of an engine.
  random_device rnd_device;
  // Specify the engine and distribution.
  mt19937 mersenne_engine{rnd_device()}; // Generates random integers
  uniform_int_distribution<int> dist{1, 52};

  auto gen = [&dist, &mersenne_engine]()
  {
    return dist(mersenne_engine);
  };

  vector<int> test(100);
  generate(begin(test), end(test), gen);

  string buffer = CommonUtil::pack_int_vector_std(test, &size);
  vector<int> result = CommonUtil::unpack_int_vector_std(buffer);

  ASSERT_EQ(result, test) << " Packed Int Vector matches unpacked Int Vector";
  __close();
}