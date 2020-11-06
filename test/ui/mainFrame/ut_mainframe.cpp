#include <iostream>
#include "gtest/gtest.h"
#include "mainframe.h"

class ut_MainFreme : public ::testing::Test
    , public QObject
{
protected:
    ut_MainFreme()
    {
    }

    virtual ~ut_MainFreme()
    {
    }
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};
