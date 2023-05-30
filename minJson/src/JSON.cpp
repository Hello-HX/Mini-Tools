//
// Created by 陈焕新 on 2023-05-23.
//
#include "../include/JSON.h"
#include<iostream>
#include<fstream>

using namespace JSON;

//折构函数的定义
JSON_OBJ::JSON_OBJ() {
    Null();
}

JSON_OBJ::JSON_OBJ(int_json value) {
    Int(value);
}

JSON_OBJ::JSON_OBJ(bool_json value) {
    Bool(value);
}

JSON_OBJ::JSON_OBJ(double_json value) {
    Double(value);
}

JSON_OBJ::JSON_OBJ(str_json const &value) {
    Str(value);
}

JSON_OBJ::JSON_OBJ(list_json value) {
    List(std::move(value));
}

JSON_OBJ::JSON_OBJ(obj_json value) {
    Obj(std::move(value));
}



//类型初始化
void JSON_OBJ::Null( ) {
    My_Type = Json_NULL ;
    My_Value = "null";
}

void JSON_OBJ::Int(int_json value) {
    My_Type = Json_INT;
    My_Value = value;
}

void JSON_OBJ::Bool(bool_json value) {
    My_Type = Json_BOOL;
    My_Value = value;
}

void JSON_OBJ::Double(double_json value) {
    My_Type = Json_DOUBLE;
    My_Value = value;
}

void JSON_OBJ::Str(string_view value) {
    My_Type = Json_STR;
    My_Value = string(value);
}

void JSON_OBJ::List(list_json value) {
    My_Type = Json_LIST;
    My_Value = std::move(value);
}

void JSON_OBJ::Obj(obj_json value) {
    My_Type = Json_OBJ ;
    My_Value = std::move(value);
}



//针对数据结构的增善
void JSON_OBJ::push_back(JSON_OBJ item) {
    if (My_Type == Json_LIST) {
        auto &list = Value<list_json>();
        list.push_back(std::move(item));
        return;
    }
    EJSON_THROW_ERROR_POS("not a list type! JSON_OBJ::push_back()");
}

void JSON_OBJ::pop_back() {
    if (My_Type == Json_LIST) {
        auto &list = Value<list_json>();
        list.pop_back();
        return;
    }
    EJSON_THROW_ERROR_POS("not a list type! JSON_OBJ::push_back()");
}



//重载运算符
JSON_OBJ& JSON_OBJ::operator[](string const &key) {
    if (My_Type == Json_OBJ) {
        auto &obj = Value<obj_json>();
        return obj[key];
    }
    throw std::logic_error("not obj type! JSON_OBJ::opertor[]()");
}



//获得类型
Json_Type JSON_OBJ::Type() {
    return My_Type;
}




//获取类型空指针，后续Value堆其进行类型转换
void* JSON_OBJ::value() {
    switch (My_Type) {
        case Json_NULL:
            return get_if<null_json>(&My_Value);
        case Json_BOOL:
            return get_if<bool_json>(&My_Value);
        case Json_INT:
            return get_if<int_json>(&My_Value);
        case Json_DOUBLE:
            return get_if<double_json>(&My_Value);
        case Json_LIST:
            return get_if<list_json>(&My_Value);
        case Json_OBJ:
            return get_if<obj_json>(&My_Value);
        case Json_STR:
            return std::get_if<str_json>(&My_Value);
        default:
            return nullptr;
    }
}

//将JSON对象转换为JSON字符串，默认格式化
string JSON_OBJ::To_string(bool choice) {
    if(choice == 0)
        return To_string_Format();
    else return To_string_NoFormat();
}

//将json对象转换为无格式化的JSON序列
string JSON_OBJ::To_string_NoFormat() {
    void *value = this->value();
    std::ostringstream os;
    switch (My_Type) {
        case Json_NULL:
            os << "null";
            break;
        case Json_BOOL:
            if (GET_VALUE(bool))
                 os << "true";
            else os << "false";
            break;
        case Json_INT:
            os << GET_VALUE(int);
            break;
        case Json_DOUBLE:
            os << GET_VALUE(double);
        case Json_STR:
            os << '\"' << GET_VALUE(string) << '\"';
            break;
        case Json_LIST:
        {
            list_json &list = GET_VALUE(list_json);
            os <<'[';
            for (auto i = 0 ; i < list.size(); i++) {
                if(i != list.size() - 1) {
                    os << list[i].To_string_NoFormat();
                    os << ',';
                }
                else os << list[i].To_string_NoFormat();
            }
            os << ']';
            break;
        }
        case Json_OBJ:
        {
            obj_json &obj = GET_VALUE(obj_json);
            os << '{';
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                if(it != obj.begin())
                    os << ',';
                os << '\"' << it->first << "\":" << it ->second.To_string_NoFormat();
            }
            os << '}';
            break;
        }
        default:
            return "";
    }
    return os.str();
}

//将json对象转换有格式化的字符串
string JSON_OBJ::To_string_Format() {
    void *value = this->value();
    std::ostringstream os;
    switch (My_Type) {
        case Json_NULL:
            os << "null";
            break;
        case Json_BOOL:
            if (GET_VALUE(bool))
                os << "true";
            else os << "false";
            break;
        case Json_INT:
            os << GET_VALUE(int);
            break;
        case Json_DOUBLE:
            os << GET_VALUE(double);
        case Json_STR:
            os << '\"' << GET_VALUE(string) << '\"';
            break;
        case Json_LIST:
        {
            list_json &list = GET_VALUE(list_json);
            os <<'[';
            for (auto i = 0 ; i < list.size(); i++) {
                if(i != list.size() - 1) {
                    os << list[i].To_string_Format();
                    os << ',' ;
                }
                else os << list[i].To_string_Format();
            }
            if ((os.str())[os.str().size() -1] == '}')
                os << '\n';
            os << ']';
            break;
        }
        case Json_OBJ:
        {
            obj_json &obj = GET_VALUE(obj_json);
            os << "\n  {" << '\n';
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                if(it != obj.begin())
                    os << ',' << '\n';
                os <<"    "<<'\"' << it->first << "\" : " << it ->second.To_string_Format();
            }
            os << '\n';
            os << "  }";
            break;
        }
        default:
            return "\n";
    }
    return os.str();
}



void JSON_OBJ::Get_Value(const string& key,bool choice) {
    auto& object = *this;
    auto tmp = object[key].To_string(choice);
    std::cout << "\"" << key << "\" : " << tmp<< "\n\n";
}

//这里有两种实现方法
//一种是创建JSON对象
//另外一种没有创建，直接怼着文件输出
//因为作业要求，所以我这里都实现了一下，个人觉得JSON对象的创建有点浪费内存，但是细想一下，创建JSON对象能避免IO阻塞问题
/*
void JSON_OBJ::select_output(const string& filename,bool choice) {
    auto& object = *this;
    std::cout << "if you want to braak, cin >> -1 " << std::endl;
    std::ofstream file(filename , std::ios::out | std::ios::trunc);
    file << "{\n";
    int num = 0;
    while (1) {
        std::cout << "cin the key you want to find " << std::endl;
        string  tmp;
        std::cin >> tmp;
        if(tmp == "-1") break;
        if(num) file << "," << '\n';
        num++;
        if(object[tmp].To_string() == "null" ) continue;
        file << "  \""<< tmp  << "\""<< " : " << object[tmp].To_string(choice);
    }
    file<<"\n}";
    file.close();
}
 */

void JSON_OBJ::select_output(const string& filename,bool choice) {
    auto& object = *this;
    {
        JSON_OBJ new_json;
        new_json.Obj({});
        std::cout << "if you want to braak, cin >> -1 " << std::endl;
        std::ofstream file(filename, std::ios::out | std::ios::trunc);
        while (1) {
            std::cout << "cin the key you want to find " << std::endl;
            string tmp;
            std::cin >> tmp;
            if (tmp == "-1") break;
            new_json[tmp] = object[tmp];
        }
        file << new_json.To_string();
        file.close();
    }
}


