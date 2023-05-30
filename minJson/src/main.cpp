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
* ������ʦ����ã���������Ŀ�ļ���Ϊ�˷��������ģ�����main����д��һЩ����
 *
 * includeĿ¼
 * JSON.h �� Parser.h ΪһЩ������ģ�����������������ϸ����ÿ���ӿڵ�����
 * Test.h ����ʵ�ֵĲ��Խӿڣ���������JSON�ı����ϲ���ֵ��Ϊ�µ�JSON�����Լ���ע���˵����л����ⲿ�ַ�ʵ��Ҫ�󣬵�������ʵ��һ�£�
 *
 * ���Ľӿڽ���
 * Parser.h: parser( )����������� | FromString( )�ӿڸ��ã����ٶ������ɿ���
 * JSON.h: To_string( )����ת��ΪJSON�������,֧�ָ�ʽ����Ǹ�ʽ�� | Valueģ�壬��ÿɲ�����Valueʵ��
 *
 * srcĿ¼
 * JSON.cpp �� Parser.cpp ΪһЩ�ӿڵľ���ʵ��
 *
 * docĿ¼
 * test.json Ϊ�����JSON�ı�
 * test_out.json Ϊ�ϳ���JSON��������ĵط�
 *
 * CMakeLists.txt ��ʹ��cmake������Ŀ
 *
*/