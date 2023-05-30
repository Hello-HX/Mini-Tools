//
// Created by 陈焕新 on 2023-05-23.
//

#include "../include/JSON.h"
#include "../include/Parser.h"
#include<cctype>
#include<algorithm>
#include<iostream>

using namespace JSON;

//特殊的处理，注释
void Parser::skip_comment()
{
    if (my_json_text.compare(my_idx, 2, R"(//)") == 0) {
        while (true) {
            auto next_pos = my_json_text.find('\n', my_idx);
            if (next_pos == string::npos) {
                throw std::logic_error("invalid comment area!");
            }
            //查看下一行是否还是注释
            my_idx = next_pos + 1;
            while (isspace(my_json_text[my_idx])) {
                my_idx++;
            }
            if (my_json_text.compare(my_idx, 2, R"(//)") != 0) { //结束注释
                return;
            }
        }
    }
}



//处理转义
bool Parser::is_esc_consume(size_t pos) {
    size_t end_pos = pos;
    while (my_json_text[pos] == '\\') pos--;
    auto cnt = end_pos - pos;
    //如果 \ 的个数为偶数，则成功抵消，如果为奇数，则未抵消
    return cnt % 2 == 0;
}



//去除尾部空字符，方便最后的结束判断
void Parser::skip_space() {
    my_json_text.erase(
            std::find_if(my_json_text.rbegin(),my_json_text.rend(),
                         [](char ch){ return !std::isspace(ch);}
            ).base(), my_json_text.end()
    );
}



//获取token
char Parser::get_next_token() {
    while (std::isspace(my_json_text[my_idx])) my_idx++;
    if (my_idx >= my_json_text.size())
        throw std::logic_error("unexpected character in parse json");
    //如果是注释，记得跳过
    skip_comment();
    return my_json_text[my_idx];
}




//解析并返回对象
JSON_OBJ Parser::parser( ) {
    char token = get_next_token();
    if (token == 'n') {
        return parse_null();
    }
    else if (token == 't' || token == 'f') {
        return parse_bool();
    }
    else if (token == '-' || std::isdigit(token)) {
        return parse_number();
    }
    else if (token == '\"') {
        return parse_string();
    }
    else if (token == '[') {
        return parse_list();
    }
    else if (token == '{') {
        return parse_obj();
    }

    throw std::logic_error("unexpected character in parse json");

}

//错误输出
#define THROW_LOGIC(msg)                                                       \
   do {                                                                        \
      throw std::logic_error(std::string(msg) + std::string(" [") +            \
                             std::string(__FILE__ ":") +                       \
                             std::to_string(__LINE__) + std::string("] "));    \
   } while (0);


const int kNearbyLen = 20;


#define DEBUG_CONTENT                                                          \
   do {                                                                        \
      if ((int64_t)my_idx - kNearbyLen > 0) my_idx = my_idx - kNearbyLen;      \
      else my_idx = 0;                                                         \
      std::cout << "error nearby this string:\r\n\r\n\""                       \
                << my_json_text.substr(my_idx, kNearbyLen * 5) << "\"\r\n\r\n" \
                << std::endl;                                                  \
   } while (0);

//固定对象的生成
JSON_OBJ Parser::parse_null() {
    //比较是否是null，如果是，跳过返回空对象即可
    if (my_json_text.compare(my_idx, 4, "null") == 0) {
        my_idx += 4;
        return {};
    }
    DEBUG_CONTENT
    THROW_LOGIC("parse null error")
}

bool Parser::parse_bool() {
    //比较是否是true或者false
    if (my_json_text.compare(my_idx, 4, "true") == 0) {
        my_idx += 4;
        return true;
    }
    else if (my_json_text.compare(my_idx, 5, "false") == 0) {
        my_idx += 5;
        return false;
    }
    DEBUG_CONTENT
    THROW_LOGIC("parse bool error")
}

JSON_OBJ Parser::parse_number()
{
    auto pos_begin = my_idx;

    //整数部分的解析
    if (my_json_text[my_idx] == '-') {
        my_idx++;
    }

    if (isdigit(my_json_text[my_idx]))
        while (isdigit(my_json_text[my_idx]))
            my_idx++;
    else {
        DEBUG_CONTENT
        THROW_LOGIC("invalid character in number")
    }

    if (my_json_text[my_idx] != '.') {
        return (int) strtol(my_json_text.c_str() + pos_begin, nullptr, 10);    // to do it
    }

    //小数部分的解析
    if (my_json_text[my_idx] == '.')  {
        my_idx++;
        if (!std::isdigit(my_json_text[my_idx])) {
            DEBUG_CONTENT
            THROW_LOGIC("invalid character in number")
        }
        while (std::isdigit(my_json_text[my_idx]))
            my_idx++;
    }
    return strtod(my_json_text.c_str() + pos_begin, nullptr);         // to do it
}


string Parser::parse_string() {
    auto pre_pos = ++my_idx;
    auto pos = my_json_text.find('"',my_idx);
    if (pos != string::npos) {
        //解析还没有结束，看是否是转义
        while (true) {
            //如果不是转义则解析结束
            if (my_json_text[pos - 1] != '\\') {
                break;
            }
            //如果是转义字符，则判断是否已经被抵消，已经被消耗完则跳出，否则继续寻找下个字符串结束符
            //to do it
            if (is_esc_consume(pos - 1)) {
                break;
            }
            pos = my_json_text.find('"', pos + 1);
            if (pos == string::npos) {
                DEBUG_CONTENT
                THROW_LOGIC(R"(expected left '"' in parse string)")
            }
        }
        my_idx = pos + 1;
        return my_json_text.substr(pre_pos, pos - pre_pos);
    }
    DEBUG_CONTENT
    THROW_LOGIC("parse string error")
}

JSON_OBJ Parser::parse_list() {
    JSON_OBJ arry((list_json()));
    my_idx++;
    char ch = get_next_token();
    if (ch == ']') {
        my_idx++;
        return arry;
    }

    while (true) {
        // to do it
        arry.push_back(parser());
        ch = get_next_token();
        if (ch == ']') {
            my_idx++;
            break;
        }

        if (ch != ',') {
            DEBUG_CONTENT
            THROW_LOGIC("expected ',' in parse list")
        }

        my_idx++;
    }
    return arry;
}

JSON_OBJ Parser::parse_obj() {
    JSON_OBJ obj((obj_json ()));
    my_idx++;
    char ch = get_next_token();
    if (ch == '}') {
        my_idx++;
        return obj;
    }
    while (true) {
        //解析key
        string key = std::move(parser().Value<string>());
        ch = get_next_token();
        if (ch != ':') {
            DEBUG_CONTENT
            THROW_LOGIC("expected ':' in parse obj")
        }
        my_idx++;

        //解析value
        obj[key] = parser();
        ch = get_next_token();
        if (ch == '}') {
            my_idx++;
            break; //解析完毕
        }

        if (ch != ',') {
            DEBUG_CONTENT
            THROW_LOGIC("expected ',' in parse obj")
        }

        my_idx++;
    }
    return obj;
}



//提供对象复用的接口
void Parser::init(string_view src) {
    my_json_text = src;
    my_idx = 0;
    skip_space();
}

JSON_OBJ Parser::FromString(string_view content) {
    static Parser tmp_parser;
    tmp_parser.init(content);
    return tmp_parser.parser();
}





