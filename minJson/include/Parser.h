//
// Created by 陈焕新 on 2023-05-23.
//

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<string>
#include<string_view>
#include<sstream>

namespace JSON {
/**
 * 下面是一系列反序列的操作，为扩展实现
 */
//调用生成函数
#define FUNC_TO_NAME _to_json

//调用反函数
#define FUNC_FROM_NAME _from_json

//生成该函数
#define START_TO_JSON void FUNC_TO_NAME(JSON::JSON_OBJ & obj) const{

//key解析
#define to(key) obj[key]

//构造结构体
#define to_struct(key, struct_member) JSON::JSON_OBJ tmp((JSON::obj_json())); struct_member.FUNC_TO_NAME(tmp); obj[key] = tmp

//结束生成
#define END_TO_JSON  }

//反序列
#define START_FROM_JSON void FUNC_FROM_NAME(JSON::JSON_OBJ& obj) {

//来源
#define from(key, type) obj[key].Value<type>()

//反结构
#define from_struct(key, struct_member) struct_member.FUNC_FROM_NAME(obj[key])

//结束
#define END_FROM_JSON }


    class Parser {
    public:

        //使用默认构造
        Parser() = default;

        //提供复用对象的接口
        /**
         * 对象初始化
         * @param src
         */
        void init(string_view src);
        /**
         * 用于反复的构造对象，减少内存开销
         * @param content
         * @return
         */
        static JSON_OBJ FromString(string_view content);

        //跳过空格
        /**
         * 跳过空格
         */
        void skip_space();

        //跳过注释
        /**
         * 跳过注释，虽然json数据不支持注释，但是我也实现了这功能
         */
        void skip_comment();

        //转义处理
        /**
         * 对转义字符的处理
         * @param pos
         * @return
         */
        bool is_esc_consume(size_t pos);

        //获取token
        /**
         * 获得解析文本的下一个token标记
         * @return
         */
        char get_next_token();

        //解析对象
        /**
         * 解析对象，递归下降解析
         * @return
         */
        JSON_OBJ parser();

        //每个解析对象的具体操作
        /**
         * 不同类型的具体解析过程
         * @return
         */
        JSON_OBJ parse_null();
        JSON_OBJ parse_number();
        bool parse_bool();
        string parse_string();
        JSON_OBJ parse_list();
        JSON_OBJ parse_obj();

        //序列化
        /**
         * 序列化
         * @tparam T
         * @param src
         * @return
         */
        template<class T>
        static string ToJSON(T const &src) {
            //如果是基本类型
            if constexpr(IS_TYPE(T, int_json))
            {
                JSON_OBJ object(src);
                return object.To_string();
            } else if constexpr(IS_TYPE(T, bool_json))
            {
                JSON_OBJ object(src);
                return object.To_string();
            } else if constexpr(IS_TYPE(T, double_json))
            {
                JSON_OBJ object(src);
                return object.To_string();
            } else if constexpr(IS_TYPE(T, str_json))
            {
                JSON_OBJ object(src);
                return object.To_string();
            }
            //如果是自定义类型调用方法完成obj的赋值，然后to_string即可
            JSON_OBJ obj((obj_json()));
            src.FUNC_TO_NAME(obj);
            return obj.To_string();
        }

        //反序列化
        /**
         * 反序列化
         * @tparam T
         * @param src
         * @return
         */
        template<class T>
        static T FromJson(string_view src)
        {
            JSON_OBJ object = FromString(src);
            //如果是基本类型
            if constexpr(is_basic_type<T>())
            {
                return object.template Value<T>();
            }

            //调用T类型对应的成岩函数
            if (object.Type() != Json_OBJ)throw std::logic_error("not obj type fromjson");
            T ret;
            ret.FUNC_FROM_NAME(object);
            return ret;
        }
    private:
        //数据
        string my_json_text;
        size_t my_idx;
    };

}

#endif //JSON_PARSER_H
