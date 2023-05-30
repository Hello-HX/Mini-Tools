//
// Created by 陈焕新 on 2023-05-30.
//

#ifndef JSON_TEST_H
#define JSON_TEST_H

#include <iostream>
#include "JSON.h"
#include "Parser.h"
#include <fstream>

using namespace JSON;

#define  TEST(a,b) std::cout << "In Test_" << a << " testing the " << b<<'\n';
#define  TEST_FINISH(a) std::cout << "Test_ "<< a <<" completed: the code is working properly"<< std::endl;
#define  TEST_BEGIN(a) std::cout << "Now testing " << a <<"\n\n";

struct Base
{
    int pp;
    string qq;

    START_FROM_JSON
        pp = from("pp", int);
        qq = from("qq", string);
    END_FROM_JSON

    START_TO_JSON
        to("pp") = pp;
        to("qq") = qq;
    END_TO_JSON
};

struct Mytest
{
    int id;
    std::string name;
    Base q;

    START_TO_JSON
        to_struct("base", q);
        to("id") = id;
        to("name") = name;
    END_TO_JSON

    START_FROM_JSON
        id = from("id", int);
        name = from("name", string);
        from_struct("base", q);
    END_FROM_JSON
};

/**
 * 测试序列化的过程
 */

/*
void test_class_serialization()
{
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    std::cout << "Now test the test 3 :: JSON_OBJ -> JSON !" << std::endl;
    Mytest test{.id = 32, .name = "fda"};
    auto item = Parser::FromJson<Mytest>(R"({"base":{"pp":0,"qq":""},"id":32,"name":"fda"})"); // serialization
    std::cout << Parser::ToJSON(item) << std::endl;
    std::cout << "Tes3 finsh !" << std::endl;// deserialization
    std::cout <<"---------------------------" <<std::endl;
}

*/

/**
 * 测试string的解析，及其key键值对的回馈情况
 * @param filena 传入解析的文件路径
 */

void test_string_parser(const string & filename) {
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    TEST_BEGIN(1)
    std::cout << "testing the parsing of JSON text." << std::endl;
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cout << "read file error";
        return;
    }
    else std::cout << "JSON text opened successfully, now entering the parsing of key-value pairs " << std::endl <<
                      "(default formatting for JSON output, but you can also control the formatting yourself)." << std::endl;

    std::string text((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    {
        auto object = JSON::Parser::FromString(text);
        //获取键值对
        std::cout << "In the output of key-value pairs, five consecutive dashes-----" << std:: endl << std::endl;
        TEST(1,1)
        object.Get_Value("vim.insertModeKeyBindings");
        TEST(1,2)
        object.Get_Value("vim.handleKeys");
        TEST(1,3)
        object.Get_Value("workbench.iconTheme");
        TEST(1,4)
        object.Get_Value("vim.normalModeKeyBindingsNonRecursive");
        TEST(1,5)
        object.Get_Value("vim.insertModeKeyBindings");
    }

    TEST_FINISH(1)
    std::cout <<"---------------------------" <<std::endl;

}

/**
 * 测试string的解析，及其key键值对的回馈情况
 * @param filena 传入解析的文件路径
 */

void test_string_parser_noformat(const string & filename) {
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    TEST_BEGIN(2)
    std::cout << "testing the parsing of JSON text." << std::endl;
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cout << "read file error";
        return;
    }
    else std::cout << "JSON text opened successfully, now entering the parsing of key-value pairs " << std::endl <<
                   "(default formatting for JSON output, but you can also control the formatting yourself)." << std::endl;

    std::string text((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    {
        auto object = JSON::Parser::FromString(text);
        //获取键值对
        std::cout << "In the output of key-value pairs, five consecutive dashes-----" << std:: endl << std::endl;
        TEST(1,1)
        object.Get_Value("vim.insertModeKeyBindings",true);
        TEST(1,2)
        object.Get_Value("vim.normalModeKeyBindingsNonRecursive", true);
        TEST(1,3)
        object.Get_Value("workbench.iconTheme", true);
        TEST(1,4)
        object.Get_Value("vim.handleKeys", true);
        TEST(1,5)
        object.Get_Value("vim.insertModeKeyBindings", true);
    }

    TEST_FINISH(2)
    std::cout <<"---------------------------" <<std::endl;

}

/**
 * 测试合并所学键值对的内容为新的json文本
 * @param filename_in
 * @param filename_out
 * @return
 */
void test_string_combin(const string& filename_in,const string& filename_out) {
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    TEST_BEGIN(3)
    std::cout << "testing the combination of select key to output the JSON text." << std::endl;
    std::ifstream fin(filename_in);
    if (!fin.is_open()) {
        std::cout << "read file error";
        return;
    }
    std::string text((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    {
        auto object = JSON::Parser::FromString(text);
        object.select_output(filename_out);
    }
    TEST_FINISH(3)
}

#endif //JSON_TEST_H
