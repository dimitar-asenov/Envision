namespace Core {

template <class Base>
class Reflect : public Base
{
    protected:
        using Super = Reflect<Base>;
};
}

template <class Base> using Super = Core::Reflect<Base>; 
