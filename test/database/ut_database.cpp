#include <iostream>
#include "gtest/gtest.h"
#include "database.h"

class databaseTest : public ::testing::Test
{
protected:
    databaseTest()
    {
    }

    virtual ~databaseTest()
    {
    }
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(databaseTest, getDB)
{
    auto q = DataBase::Instance().getDB();
    bool ret = q.open();
    EXPECT_TRUE(ret);
}
