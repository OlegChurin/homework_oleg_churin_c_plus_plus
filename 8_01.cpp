#include <iostream>
#include <cstddef> 

class Entity_v1
{
private:
    int secret; 

public:
    Entity_v1(int value = 0) : secret(value) {}

    void print() const
    {
        std::cout << "Entity_v1::secret = " << secret << std::endl;
    }
};

class Entity_v2
{
public:
    int exposed; 
};

void hack_via_pointer(Entity_v1& obj, int new_value)
{
  
    int* ptr = reinterpret_cast<int*>(&obj);
    *ptr = new_value;
}

int main()
{
    Entity_v1 original(123);
    std::cout << "До взлома: ";
    original.print();

    
    {
        Entity_v2& hack_ref = reinterpret_cast<Entity_v2&>(original);
        hack_ref.exposed = 456;
        std::cout << "После первого взлома (через Entity_v2): ";
        original.print();
    }

    
    {
        hack_via_pointer(original, 789);
        std::cout << "После второго взлома (через указатель на int): ";
        original.print();
    }

    return 0;
}
