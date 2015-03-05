// 2014-08-12T16:25+08:00
// http://en.wikipedia.org/wiki/C%2B%2B11
// http://blog.csdn.net/gzlaiyonghao/article/details/6564162
//
// 2014-08-23T22:02+08:00
// http://stackoverflow.com/questions/12190051/member-initialization-while-using-delegated-constructor
#include <iostream>

class SomeType {
    int number;
public:
    SomeType(int new_number) : number(new_number) {};
    SomeType() : SomeType(42) {}

    // 2014-08-14T09:00+08:00
    // ISO C++ 11, 12.6.2/6
    // If a mem-initializer-id designates the constructor��s class,
    // it shall be the only mem-initializer; the constructor is a
    // delegating constructor, and the constructor selected by
    // the mem-initializer is the target constructor .
    //
    // error message produced by g++:
    // error: mem-initializer for 'SomeType::number' follows constructor delegation
    //
    // ����Ϊ����һ�ֺܺõĴ���������Ϊ��Ȼһ��delegating constructor
    // ����target constructor����ɹ��칤������ô������target constructor
    // �У����еĳ�Ա���ᱻ��ȷ�س�ʼ�������ԣ���delegating constructor
    // ����target constructor��ʱ�򣬺������������mem-initializer�ˡ�
    //
    //SomeType(double dbl) : SomeType((int)dbl), number(dbl) {}

    int get() const { return number; }
};

int main()
{
    SomeType obj;
    std::cout << obj.get() << std::endl;

    return 0;
}
