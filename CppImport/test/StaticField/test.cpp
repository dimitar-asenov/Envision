
namespace test{
class Test {
  static int field;
};

int Test::field = 10;

template<class T>
class TestManager
{
  	private:
		static int* testConstructors;
};

template<class T> int* TestManager<T>::testConstructors = nullptr;

}