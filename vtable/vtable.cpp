#include "vtable.hpp"
#include <iostream>

std::unique_ptr<VtableInfo> VtableInfo::instance_;

std::unique_ptr<VtableInfo>& VtableInfo::GetInstance() {
    if ( instance_ == nullptr ) {
        instance_ = std::make_unique<VtableInfo>();
    }
    return instance_;
}

bool VtableInfo::addMethod( const std::string& className, const std::string& methodName, void(*foo_ptr)(void*)) {
    
    size_t classIdx = 0;
    if ( classMap.contains(className) ) {
        classIdx = classMap[className];
    } else {
        throw std::invalid_argument( "Can't declare method \'" + methodName + "\' as class \'" + className + "\' was not declared\n");
    }

    ClassInfo& class_info = classVec[classIdx];
    if ( class_info.method_offset.contains( methodName) ) {
        size_t method_idx = class_info.method_offset[methodName];
        class_info.vptr->at(method_idx) = foo_ptr;
    } else {
        class_info.method_offset[methodName] = class_info.vptr->size();
        class_info.vptr->push_back( foo_ptr);
    }

    return true;
}

Iter<ClassInfo> VtableInfo::addClass( const std::string& className) {
    if ( classMap.contains(className) ) {
        throw std::invalid_argument("Class \'" + className + "\' was already declared in this scope\n");
    }

    classMap[className] = classVec.size();
    vtableVec.emplace_back();

    classVec.emplace_back( className, std::prev(vtableVec.end()));
    return std::prev(classVec.end());
}

Iter<ClassInfo> VtableInfo::addDerivedClass( const std::string& derivedName, const std::string& baseName) {
    
    if ( !classMap.contains( baseName) ) {
        throw std::invalid_argument("Base class \'" + baseName + "\' was not declared in this scope\n");
    }
    
    if ( classMap.contains( derivedName) ) {
        throw std::invalid_argument("Class \'" + derivedName + "\' was already declared in this scope\n");
    }

    ClassInfo info;
    ssize_t baseIdx = classMap[baseName];
    auto& baseInfo = classVec[baseIdx];
    info.method_offset = baseInfo.method_offset;
    vtableVec.push_back(*baseInfo.vptr);
    info.vptr = std::prev(vtableVec.end());
    info.name = derivedName;
    info.parentIdx = {baseIdx};

    classMap[derivedName] = classVec.size();
    classVec.push_back( std::move(info));

    return std::prev(classVec.end());
}

Iter<ClassInfo> VtableInfo::addDerivedClassMultiple( const std::string& derivedName, const std::string& base1Name, const std::string& base2Name) {
        
    if ( !classMap.contains( base1Name) ) {
        throw std::invalid_argument("Base class \'" + base1Name + "\' was not declared in this scope\n");
    }

    if ( !classMap.contains( base2Name) ) {
        throw std::invalid_argument("Base class \'" + base2Name + "\' was not declared in this scope\n");
    }
    
    if ( classMap.contains( derivedName) ) {
        throw std::invalid_argument("Class \'" + derivedName + "\' was already declared in this scope\n");
    }

    ClassInfo info;
    ssize_t base1Idx = classMap[base1Name];
    auto& base1Info = classVec[base1Idx];

    ssize_t base2Idx = classMap[base2Name];
    auto& base2Info = classVec[base2Idx];
    Vtable vtable = *base1Info.vptr;
    Vtable& vtable2 = *base2Info.vptr;
    size_t method_n = vtable.size();
    info.method_offset = base1Info.method_offset;

    for ( const auto [key, value] : base2Info.method_offset ) {
        if ( base1Info.method_offset.contains( key) ) {
            throw std::runtime_error("member \'" + key + "\' found in multiple base classes of different types");
        }
        info.method_offset[key] = method_n++;
        vtable.push_back( vtable2.at(value));
    }

    vtableVec.push_back(vtable);
    info.vptr = std::prev(vtableVec.end());
    info.name = derivedName;
    info.parentIdx = {base1Idx, base2Idx};

    classMap[derivedName] = classVec.size();
    classVec.push_back( std::move(info));

    return std::prev(classVec.end());
}

bool VtableInfo::isBaseOf( ssize_t base, ssize_t derived) {
    if ( derived == -1 || base == -1 ) {
        return false;
    }

    for ( auto idx : classVec[derived].parentIdx ) {
        if ( idx == base ) {
            return true;
        }

        if ( isBaseOf( base, idx) ) {
            return true;
        }
    }
    return false;
}

bool VtableInfo::isDynamicallyConvertable( const std::string& fromClass, const std::string& toClass) {
    if ( !classMap.contains( fromClass) ) {
        throw std::invalid_argument("Base class \'" + fromClass + "\' was not declared in this scope\n");
    }
    if ( !classMap.contains( toClass) ) {
        throw std::invalid_argument("Base class \'" + toClass + "\' was not declared in this scope\n");
    }

    auto fromIdx = classMap[fromClass];
    auto toIdx = classMap[toClass];
    return (fromIdx == toIdx) || isBaseOf(toIdx, fromIdx);
}

ssize_t VtableInfo::classIdx( const std::string& className) {
    auto found = classMap.find( className);
    if ( found == classMap.end() ) {
        return -1;
    }
    return found->second;
}

type_info::type_info( ClassInfo& classInfo) : classInfo( classInfo), 
        classIdx(VtableInfo::GetInstance()->classIdx( classInfo.name)){
        
}

type_info ClassInfo::getTypeInfo() {
    return type_info( *this);
}

std::string type_info::name() {
    return classInfo.name;
}

std::size_t type_info::hash_code() const noexcept {
    return classIdx;
}

bool type_info::hasMethod( const std::string& methodName) {
    return classInfo.method_offset.contains( methodName);
}

Iter<ClassInfo>& getVptr( void* ptr) {
    return *(Iter<ClassInfo>*)(ptr);
}