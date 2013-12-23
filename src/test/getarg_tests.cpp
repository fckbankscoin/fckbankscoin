#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-FCK");
    BOOST_CHECK(GetBoolArg("-FCK"));
    BOOST_CHECK(GetBoolArg("-FCK", false));
    BOOST_CHECK(GetBoolArg("-FCK", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-FCKo"));
    BOOST_CHECK(!GetBoolArg("-FCKo", false));
    BOOST_CHECK(GetBoolArg("-FCKo", true));

    ResetArgs("-FCK=0");
    BOOST_CHECK(!GetBoolArg("-FCK"));
    BOOST_CHECK(!GetBoolArg("-FCK", false));
    BOOST_CHECK(!GetBoolArg("-FCK", true));

    ResetArgs("-FCK=1");
    BOOST_CHECK(GetBoolArg("-FCK"));
    BOOST_CHECK(GetBoolArg("-FCK", false));
    BOOST_CHECK(GetBoolArg("-FCK", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noFCK");
    BOOST_CHECK(!GetBoolArg("-FCK"));
    BOOST_CHECK(!GetBoolArg("-FCK", false));
    BOOST_CHECK(!GetBoolArg("-FCK", true));

    ResetArgs("-noFCK=1");
    BOOST_CHECK(!GetBoolArg("-FCK"));
    BOOST_CHECK(!GetBoolArg("-FCK", false));
    BOOST_CHECK(!GetBoolArg("-FCK", true));

    ResetArgs("-FCK -noFCK");  // -FCK should win
    BOOST_CHECK(GetBoolArg("-FCK"));
    BOOST_CHECK(GetBoolArg("-FCK", false));
    BOOST_CHECK(GetBoolArg("-FCK", true));

    ResetArgs("-FCK=1 -noFCK=1");  // -FCK should win
    BOOST_CHECK(GetBoolArg("-FCK"));
    BOOST_CHECK(GetBoolArg("-FCK", false));
    BOOST_CHECK(GetBoolArg("-FCK", true));

    ResetArgs("-FCK=0 -noFCK=0");  // -FCK should win
    BOOST_CHECK(!GetBoolArg("-FCK"));
    BOOST_CHECK(!GetBoolArg("-FCK", false));
    BOOST_CHECK(!GetBoolArg("-FCK", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--FCK=1");
    BOOST_CHECK(GetBoolArg("-FCK"));
    BOOST_CHECK(GetBoolArg("-FCK", false));
    BOOST_CHECK(GetBoolArg("-FCK", true));

    ResetArgs("--noFCK=1");
    BOOST_CHECK(!GetBoolArg("-FCK"));
    BOOST_CHECK(!GetBoolArg("-FCK", false));
    BOOST_CHECK(!GetBoolArg("-FCK", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-FCK", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-FCK", "eleven"), "eleven");

    ResetArgs("-FCK -bar");
    BOOST_CHECK_EQUAL(GetArg("-FCK", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-FCK", "eleven"), "");

    ResetArgs("-FCK=");
    BOOST_CHECK_EQUAL(GetArg("-FCK", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-FCK", "eleven"), "");

    ResetArgs("-FCK=11");
    BOOST_CHECK_EQUAL(GetArg("-FCK", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-FCK", "eleven"), "11");

    ResetArgs("-FCK=eleven");
    BOOST_CHECK_EQUAL(GetArg("-FCK", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-FCK", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-FCK", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-FCK", 0), 0);

    ResetArgs("-FCK -bar");
    BOOST_CHECK_EQUAL(GetArg("-FCK", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-FCK=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-FCK", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-FCK=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-FCK", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--FCK");
    BOOST_CHECK_EQUAL(GetBoolArg("-FCK"), true);

    ResetArgs("--FCK=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-FCK", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noFCK");
    BOOST_CHECK(!GetBoolArg("-FCK"));
    BOOST_CHECK(!GetBoolArg("-FCK", true));
    BOOST_CHECK(!GetBoolArg("-FCK", false));

    ResetArgs("-noFCK=1");
    BOOST_CHECK(!GetBoolArg("-FCK"));
    BOOST_CHECK(!GetBoolArg("-FCK", true));
    BOOST_CHECK(!GetBoolArg("-FCK", false));

    ResetArgs("-noFCK=0");
    BOOST_CHECK(GetBoolArg("-FCK"));
    BOOST_CHECK(GetBoolArg("-FCK", true));
    BOOST_CHECK(GetBoolArg("-FCK", false));

    ResetArgs("-FCK --noFCK");
    BOOST_CHECK(GetBoolArg("-FCK"));

    ResetArgs("-noFCK -FCK"); // FCK always wins:
    BOOST_CHECK(GetBoolArg("-FCK"));
}

BOOST_AUTO_TEST_SUITE_END()
