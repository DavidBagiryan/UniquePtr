#include <cassert>   // нужно для assert
#include <cstddef>   // нужно для nullptr_t
#include <utility>   // нужно для move

using namespace std;

template <typename T>
class UniquePtr {
public:
    UniquePtr() = default;
    
    explicit UniquePtr(T* ptr) 
        : uniq_ptr_(ptr)
    {
    }
    
    UniquePtr(const UniquePtr&) = delete;
    
    UniquePtr(UniquePtr&& other) 
        : uniq_ptr_(other.Release()) 
    {
    }
    
    UniquePtr& operator=(const UniquePtr&) = delete;
    
    UniquePtr& operator=(nullptr_t) {
        if (Get() != nullptr) {
            delete uniq_ptr_;
            uniq_ptr_ = nullptr;
        }
        return *this;
    }
    
    UniquePtr& operator=(UniquePtr&& other) {
        if (this != &other) {
            delete uniq_ptr_;
            uniq_ptr_ = other.uniq_ptr_;
            other.uniq_ptr_ = nullptr;
        }
        return *this;
    }
    
    ~UniquePtr() {
        delete uniq_ptr_;
    }

    T& operator*() const {
        if (!uniq_ptr_) {
            throw "Cannot dereference nullptr";
        }
        return *uniq_ptr_;
    }
    
    T* operator->() const {
        return Get();
    }
    
    T* Release() {
        return exchange(uniq_ptr_, nullptr);
    }
    
    void Reset(T* ptr) {
        delete uniq_ptr_;
        uniq_ptr_ = ptr;
    }
    
    void Swap(UniquePtr& other) {
        UniquePtr temp(move(other));
        other = move(*this);
        *this = move(temp);
    }
    
    T* Get() const {
        return uniq_ptr_;
    }
    
private:
    T* uniq_ptr_ = nullptr;
};

struct Item {
    static int counter;
    int value;
    Item(int v = 0)
        : value(v) 
    {
        ++counter;
    }
    Item(const Item& other)
        : value(other.value) 
    {
        ++counter;
    }
    ~Item() {
        --counter;
    }
};

int Item::counter = 0;

void TestLifetime() {
    Item::counter = 0;
    {
        UniquePtr<Item> ptr(new Item);
        assert(Item::counter == 1);

        ptr.Reset(new Item);
        assert(Item::counter == 1);
    }
    assert(Item::counter == 0);

    {
        UniquePtr<Item> ptr(new Item);
        assert(Item::counter == 1);

        auto rawPtr = ptr.Release();
        assert(Item::counter == 1);

        delete rawPtr;
        assert(Item::counter == 0);
    }

}

void TestGetters() {
    UniquePtr<Item> ptr(new Item(42));
    assert(ptr.Get()->value == 42);
    assert((*ptr).value == 42);
    assert(ptr->value == 42);
}

int main() {
    TestLifetime();
    TestGetters();
}
