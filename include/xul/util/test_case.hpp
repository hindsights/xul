#pragma once

#include <xul/macro/token.hpp>
#include <xul/macro/foreach.hpp>
#include <xul/std/containers.hpp>
#include <xul/util/singleton.hpp>
#include <xul/config.hpp>
#include <list>
#include <map>
#include <string>
#include <assert.h>

#ifdef XUL_WINDOWS
#pragma warning(disable:4127)
#endif

/**
 * @file
 * @brief test framework
 */



#define XUL_LOG_TYPE_TESTCASE 110

#define TESTCASE_EVENT(message) XUL_LOG_IMPL(XUL_LOG_TYPE_TESTCASE, _XUL_EVENT, message)




#if defined(_DEBUG) && !defined(_WIN32_WCE)

#ifndef XUL_RUN_TEST
#define XUL_RUN_TEST
#endif

#pragma message("------ enable automatical unit tests")

#endif


namespace xul {


#ifdef XUL_RUN_TEST


class test_case
{
public:
    test_case() { }
    virtual ~test_case() { }

    bool init()
    {
        return do_init();
    }

    void run()
    {
        do_run();
    }

    void set_name(const std::string& name) { m_name = name; }
    const std::string& get_name() const { return m_name; }

protected:
    virtual bool do_init() { return true; }

    virtual void do_run() { }

private:
    std::string m_name;
};


typedef test_case TestCase;



class test_suite : public test_case, public singleton<test_suite>
{
public:
    test_suite() { }
    ~test_suite() { clear(); }

    void clear()
    {
        XUL_FOREACH(test_case_collection, m_tests, iter)
        {
            test_case* tc = *iter;
            delete tc;
        }
        m_tests.clear();
        m_test_index.clear();
    }

    void add_test(test_case* tc)
    {
        if (xul::containers::contains(m_test_index, tc->get_name()))
            return;
        m_tests.push_back(tc);
        m_test_index[tc->get_name()] = tc;
    }

protected:
    virtual void do_run()
    {
        //XUL_LIBXUL_DEBUG("Start running " << m_tests.size() << " test cases:");
        int i = 0;
        XUL_FOREACH_CONST(test_case_collection, m_tests, iter)
        {
            test_case* tc = *iter;
        //    XUL_LIBXUL_DEBUG("Start running test case " << i << ": " << tc->get_name());
            tc->run();
            i++;
        }
        //XUL_LIBXUL_DEBUG("All of " << m_tests.size() << " test cases passed.");
    }

private:
    typedef std::list<test_case*> test_case_collection;
    typedef std::map<std::string, test_case*> test_case_index;
    /// all test cases
    test_case_collection m_tests;
    test_case_index m_test_index;
};


/// auto registered test suite
class auto_register_test_suite
{
public:
    explicit auto_register_test_suite(test_case* tc, const char* name)
    {
        tc->set_name(name);
        test_suite::instance().add_test(tc);
    }
};


inline void run_tests()
{
    test_suite::instance().run();
    test_suite::instance().clear();
}

#else

inline void run_tests()
{
}

#endif

}


/// macro fro test suite registration
#define XUL_TEST_SUITE_REGISTRATION(testCase)        static xul::auto_register_test_suite XUL_MAKE_UNIQUE_NAME(testCase)(new testCase, #testCase)


#pragma warning(default:4127)

