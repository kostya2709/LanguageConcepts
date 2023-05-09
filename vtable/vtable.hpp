#pragma once

#include <string>
#include <map>
#include <deque>
#include <memory>
#include <vector>
#include <optional>


using Vtable = std::vector<void(*)(void*)>;

template <typename U>
using Iter = typename std::deque<U>::iterator;

class type_info;

struct ClassInfo {

    ClassInfo( const std::string& name, Iter<Vtable> vptr) : name(name), vptr(vptr) {}
    ClassInfo( const ClassInfo&) = default;
    ClassInfo() = default;

    type_info getTypeInfo();

    std::string name;
    Iter<Vtable> vptr;
    std::map<std::string, size_t> method_offset;
    std::vector<ssize_t> parentIdx;
};

Iter<ClassInfo>& getVptr( void* ptr);

struct VptrPlaceholder {
    Iter<ClassInfo> vptr_;
};

#define VIRTUAL_CLASS( className ) \
    static Iter<ClassInfo> __ptr_##className = VtableInfo::GetInstance()->addClass( #className); \
    struct className : private VptrPlaceholder


#define VIRTUAL_CLASS_DERIVED( derivedName, baseName ) \
    static Iter<ClassInfo> __ptr_##derivedName = VtableInfo::GetInstance()->addDerivedClass( #derivedName, #baseName); \
    struct derivedName : baseName

#define VIRTUAL_CLASS_DERIVED_MULTIPLE( derivedName, base1Name, base2Name ) \
    static Iter<ClassInfo> __ptr_##derivedName = VtableInfo::GetInstance()->addDerivedClassMultiple( #derivedName, #base1Name, #base2Name); \
    struct derivedName : base1Name, base2Name


#define VIRTUAL_CALL( obj_ptr, methodName) \
    getVptr(obj_ptr)->vptr->at(getVptr(obj_ptr)->method_offset[#methodName])(obj_ptr)

#define CREATE_OBJ( className, objName, ...) \
    className objName = className(__VA_ARGS__); \
    getVptr(&objName) = __ptr_##className


class type_info {
    ClassInfo& classInfo;
    size_t classIdx{0};
public:
    type_info( ClassInfo& classInfo);
    std::string name();
    std::size_t hash_code() const noexcept;
    bool hasMethod( const std::string& methodName);
};

class VtableInfo {
public:
    bool addMethod( const std::string& className, const std::string& methodName, void(*foo_ptr)(void*));
    Iter<ClassInfo> addClass( const std::string& className);
    Iter<ClassInfo> addDerivedClass( const std::string& derivedName, const std::string& baseName);
    Iter<ClassInfo> addDerivedClassMultiple( const std::string& derivedName, const std::string& base1Name, const std::string& base2Name);
    bool isDynamicallyConvertable( const std::string& fromClass, const std::string& toClass);
    static std::unique_ptr<VtableInfo>& GetInstance();
    ssize_t classIdx( const std::string& className);
private:
    bool isBaseOf( ssize_t base, ssize_t derived);
    std::deque<ClassInfo> classVec;
    std::deque<Vtable> vtableVec;
    std::map<std::string, size_t> classMap;

    static std::unique_ptr<VtableInfo> instance_;
};

#define DECLARE_METHOD( className, methodName, methodPtr ) \
    static const bool __added##className##methodName = VtableInfo::GetInstance()->addMethod( #className, #methodName, (void(*)(void*))methodPtr);

#define DYNAMIC_CAST( toClass, obj_ptr) \
    (toClass*)(obj_ptr); \
    if ( !VtableInfo::GetInstance()->isDynamicallyConvertable( getVptr(obj_ptr)->name, #toClass) ) { \
        throw std::invalid_argument("Impossible to dynamically cast class \'" + getVptr(obj_ptr)->name + "\' to class \'" #toClass "\'"); \
    }

#define TYPEID( obj_ptr ) \
    getVptr(obj_ptr)->getTypeInfo()
