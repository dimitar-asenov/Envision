#include <memory>


struct A {
    int returnZero() {
        return 0;
    }
};

std::shared_ptr<A> createA() {
    return std::make_shared<A>();
}


int main() {
    createA()->returnZero();
    return 0;
}
