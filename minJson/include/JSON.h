//
// Created by 陈焕新 on 2023-05-23.
//

#ifndef JSON_JSON_H
#define JSON_JSON_H

#include<stdexcept>
#include <utility>
#include<variant>
#include <map>
#include<vector>
#include<string>
#include<string_view>
#include<sstream>

namespace JSON {

//简化指针获取的过程
#define GET_VALUE(type) *((type*) value)

//用#erron转换为固定字符串，预处理宏
#define THROW_GET_ERROR(erron) throw std::logic_error("type error in get "#erron" value!")

//判断对象的类型是否相等，调用底层的Type函数
#define IS_TYPE(typea, typeb)  std::is_same<typea,typeb>::value

#define EJSON_ERROR_POS_STRING(erron)                                          \
   (std::string("\nerror:") + std::string(erron) + std::string(" at \"") +     \
    std::string(__FILE__) + std::string(":") + std::to_string(__LINE__) +      \
    std::string("\",func:") + std::string(__FUNCTION__))

#define EJSON_THROW_ERROR_POS(erron)                                           \
   throw std::runtime_error(EJSON_ERROR_POS_STRING(erron))



    using std::variant;
    using std::map;
    using std::string;
    using std::string_view;
    using std::stringstream;
    using std::vector;
    using std::get_if;



    //使用enum给JSON_OBJ一个识别类型的tag
    enum Json_Type {
        Json_NULL=0,
        Json_BOOL,
        Json_INT,
        Json_DOUBLE,
        Json_STR,
        Json_LIST,
        Json_OBJ
    };


    class JSON_OBJ;

    //重新命名类型
    using null_json = string;
    using int_json = int32_t;
    using bool_json = bool;
    using double_json = double;
    using str_json = string;
    using list_json = vector<JSON_OBJ>;
    using obj_json = map<string, JSON_OBJ>;


    //判断是否是笔记类型
    template<class T>
    constexpr bool is_basic_type()
    {
        if constexpr(IS_TYPE(T, str_json) ||
                     IS_TYPE(T, bool_json) ||
                     IS_TYPE(T, double_json) ||
                     IS_TYPE(T, int_json))
            return true;

        return false;
    }


    //一个基本的JSON_OBJ对象，包括类型识别和自己的数据
    class JSON_OBJ {
    public:
        //变体数据类型
        using Value_json = variant<int_json,double_json, bool_json,
                                   str_json, list_json, obj_json>;
        //默认为null类型
        /**
         * 构造对象
         * @param value 数据复制
         */
        JSON_OBJ();
        JSON_OBJ(int_json value);
        JSON_OBJ(bool_json value);
        JSON_OBJ(double_json value);
        JSON_OBJ(str_json const &value);
        JSON_OBJ(list_json value);
        JSON_OBJ(obj_json value);

        //对应折构下面的初始化函数
        /**
         * param value
         * 将类型与数据绑定到对象本身
         */
        void Null( );
        void Int(int_json value);
        void Bool(bool_json value);
        void Double(double_json value);
        void Str(string_view value);
        void List(list_json value);
        void Obj(obj_json value);

        //对应的list操作
        /**
         * list类型的操作
         * @param item
         */
        void push_back(JSON_OBJ item);
        void pop_back();

        //重载运算符。方便对象类型的操作
        /**
         * 重载返回key对应的value
         * @param key
         * @return
         */
        JSON_OBJ& operator[](string const &key);
        //处理obj对象的输出
        /**
         * 格式化输出键值对
         * @param key
         */
        void  Get_Value(const string& key,bool choice = false);

        //获得类型
        /**
         * 获得对象的类型tag
         * @return
         */
        Json_Type Type();

        //获得对象
        /**
         * 获得对象V的实体
         * @tparam V
         * @return
         */
        template<class V> V& Value( ) {

            //添加安全检查
            //保获取数据时所需的类型与实际存储的类型相匹配
            //使用if constexpr语句中的条件表达式，必须在编译期计算出结果，提升运行时执行效率
            if constexpr(IS_TYPE(V, str_json)) {
                if (My_Type != Json_STR)
                    THROW_GET_ERROR(string);
            }
            else if constexpr(IS_TYPE(V, bool_json)) {
                if (My_Type != Json_BOOL)
                    THROW_GET_ERROR(BOOL);
            }
            else if constexpr(IS_TYPE(V, int_json)) {
                if (My_Type != Json_INT)
                    THROW_GET_ERROR(INT);
            }
            else if constexpr(IS_TYPE(V, double_json)) {
                if (My_Type != Json_DOUBLE)
                    THROW_GET_ERROR(DOUBLE);
            }
            else if constexpr(IS_TYPE(V, list_json)) {
                if (My_Type != Json_LIST)
                    THROW_GET_ERROR(LIST);
            }
            else if constexpr(IS_TYPE(V, obj_json)) {
                if (My_Type != Json_OBJ)
                    THROW_GET_ERROR(OBJ);
            }

            void* v = value( );
            //处理异常
            if(v == nullptr)
                EJSON_THROW_ERROR_POS("unknown type in JObject::Value()");

            return *((V *) v);

        }


        //将json对象转换为Json序列，默认格式化
        /**
         * 将对象序列化输出，默认格式化输出
         * @param choice
         * @return
         */
        string To_string(bool choice = false);
        /**
         * 对应的接口，返回string对象
         * @return
         */
        //转换为没有格式化的Json序列
        string To_string_NoFormat();
        //转换为格式好看的Json序列
        string To_string_Format();

        //导出json文件
        /**
         * 选择键值对，然后导出，其中choice为是否格式化，默认格式化
         * @param filename
         * @param choice
         */
        void select_output(const string& filename, bool choice = false);


    private:
        //获得类型空指针
        /**
         * 获得对象的空指针
         * @return
         */
        void* value();

        //存储数据
        Json_Type My_Type;
        Value_json My_Value;
    };

}

#endif //JSON_JSON_H
