#include <iostream>

#include "vtable.hpp"

// базовый класс
VIRTUAL_CLASS( Base ) {
    Base(int x) : a(x) {}
    Base() = default;
	int a;
};

void BaseBoth( Base* obj) {
    std::cout << "a = " << obj->a << '\n';
    std::cout << "Base::Both\n";
}

void OnlyBase( Base* obj) {
    std::cout << "a = " << obj->a << '\n';
    std::cout << "Base::OnlyBase\n";
}

// методы
DECLARE_METHOD( Base, Both, BaseBoth )
DECLARE_METHOD( Base, OnlyBase, OnlyBase )


// класс-наследник
VIRTUAL_CLASS_DERIVED( Derived, Base ) {
	int b;
};

void DerivedBoth( Derived* obj) {
    std::cout << "b = " << obj->b << '\n';
    std::cout << "Derived::Both\n";
}

void OnlyDerived( Derived* obj) {
    std::cout << "b = " << obj->b << '\n';
    std::cout << "Derived::OnlyDerived\n";
}

// методы
DECLARE_METHOD( Derived, Both, DerivedBoth )
DECLARE_METHOD( Derived, OnlyDerived, OnlyDerived )


// базовый класс
VIRTUAL_CLASS( Base1 ) {
    int field1 = 0;
};
void Base1Info( Base1& b) {
    std::cout << "Base1::Base1Info\n";
}
DECLARE_METHOD( Base1, Base1Info, Base1Info )

// базовый класс
VIRTUAL_CLASS( Base2 ) {
    int field2 = 0;
};
void Base2Info( Base2& b) {
    std::cout << "Base2::Base2Info\n";
}
DECLARE_METHOD( Base2, Base2Info, Base2Info )

// класс-наследник
VIRTUAL_CLASS_DERIVED_MULTIPLE( DerivedM, Base1, Base2 ) {
	int field3;
};
void DerivedMInfo( Base1& b) {
    std::cout << "DerivedM::DerivedMInfo\n";
}
DECLARE_METHOD( DerivedM, DerivedMInfo, DerivedMInfo )

int main()
{
    CREATE_OBJ( Base, base, 27 );   /* как-то создали базовый класс */
    base.a = 0;                     // работаем как со структурой
    CREATE_OBJ( Derived, derived ); /* ... как-то создали наследник */
    derived.a = 1;
    derived.b = 2;
	
    // 	полиморфизм
    Base* reallyDerived = reinterpret_cast<Base*>(&derived);

    VIRTUAL_CALL( &base, Both);             // печатает “Base::Both a = 0”
    VIRTUAL_CALL( reallyDerived, Both);     // печатает “Derived::Both b = 1”
    VIRTUAL_CALL( reallyDerived, OnlyBase); // печатает “Base::OnlyBase”
    VIRTUAL_CALL( reallyDerived, OnlyDerived);

    Derived* derived_again  = DYNAMIC_CAST(Derived, reallyDerived) // success
    auto*    derived_again1 = DYNAMIC_CAST(Base, reallyDerived)    // success
    try {
        Derived* derived_again1 = DYNAMIC_CAST(Derived, &base)
        assert(false && "Unreachable");
    } catch (...) {
        std::cout << "Caught bad dynamic cast\n";
    }

    type_info derived_info = TYPEID( reallyDerived);
    assert( derived_info.name() == "Derived" );
    assert( derived_info.hash_code() == 1);
    assert( derived_info.hasMethod( "OnlyDerived") );
    
    type_info base_info = TYPEID( &base);
    assert( base_info.name() == "Base" );
    assert( base_info.hash_code() == 0);
    assert( derived_info.hasMethod( "OnlyBase") );

    CREATE_OBJ( DerivedM, derivedM );
    derivedM.field1 = 1;
    derivedM.field2 = 2;
    derivedM.field3 = 3;
    VIRTUAL_CALL( &derivedM, Base1Info );
    VIRTUAL_CALL( &derivedM, Base2Info );
    VIRTUAL_CALL( &derivedM, DerivedMInfo );
    
    
    Base1* base1_p = &derivedM;
    Base2* base2_p = &derivedM;
    std::cout << sizeof(*base1_p) << " " << sizeof(*base2_p) << " " << sizeof(derivedM) << '\n';

    auto ptr1 = DYNAMIC_CAST(DerivedM, base1_p);
    auto ptr2 = DYNAMIC_CAST(DerivedM, base2_p);
    try {
        auto wrong_ptr = DYNAMIC_CAST(Derived, base1_p)
        assert(false && "Unreachable");
    } catch (...) {
        std::cout << "Caught bad dynamic cast\n";
    }
}
