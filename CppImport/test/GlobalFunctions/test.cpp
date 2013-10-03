static int test();


int main() {
    int var = test();

    return 0;
}


int test(){
    return 100;
}


class Test{
    static int myTest() {
        return 100;
    }
    int val;

    Test(int k) : val(k) {
        val = test();
    }
};
