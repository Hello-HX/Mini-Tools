#include "../include/Test.h"

using namespace std;

int main() {
    const string filename_in = "D:\\Json\\doc\\test.json";
    const string filename_out = "D:\\Json\\doc\\test_out.json";
    test_string_parser(filename_in);
    test_string_parser_noformat(filename_in);
    test_string_combin(filename_in,filename_out);
    return 0;
}

/**
* 助教老师，你好，这是我项目文件，为了方便你审阅，我在main函数写入一些介绍
 *
 * include目录
 * JSON.h 和 Parser.h 为一些函数和模板等声明，里面有详细介绍每个接口的作用
 * Test.h 是我实现的测试接口，包括解析JSON文本，合并键值对为新的JSON对象，以及我注释了的序列化（这部分非实验要求，但是我想实现一下）
 *
 * 核心接口介绍
 * Parser.h: parser( )负责解析数据 | FromString( )接口复用，减少对象生成开销
 * JSON.h: To_string( )对象转换为JSON序列输出,支持格式化或非格式化 | Value模板，获得可操作的Value实体
 *
 * src目录
 * JSON.cpp 和 Parser.cpp 为一些接口的具体实现
 *
 * doc目录
 * test.json 为处理的JSON文本
 * test_out.json 为合成新JSON对象输出的地方
 *
 * CMakeLists.txt 我使用cmake构建项目
 *
*/