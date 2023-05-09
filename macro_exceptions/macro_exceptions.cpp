#include "macro_exceptions.hpp"

std::unique_ptr<ExceptionSingleton> ExceptionSingleton::instance_;

ExceptionSingleton::ExceptionSingleton() {}

std::unique_ptr<ExceptionSingleton>& ExceptionSingleton::GetInstance() {
    if ( instance_ == nullptr ) {
        instance_ = std::make_unique<ExceptionSingleton>();
    }
    return instance_;
}

void ExceptionSingleton::pushEnv( jmp_buf env) {
    constexpr size_t len = sizeof(jmp_buf) / sizeof(int);
    std::vector<int> vec( len);
    for ( int i = 0; i < len; ++i) {
        vec[i] = env[i];
    }
    env_.push(std::move(vec));
}

std::vector<int> ExceptionSingleton::getEnv() {
    if ( env_.empty() ) {
        throw std::invalid_argument( "Empty env stack!");
    }
    std::vector<int> last = env_.top();
    env_.pop();
    return last;
}

void ExceptionSingleton::addScope() {
    obj_.emplace();
}

void ExceptionSingleton::destroyScope() {
    if ( obj_.empty() ) {
        throw std::invalid_argument( "Stack is already empty, can't destroy scope");
    }

    auto& scope = obj_.top();
    while ( !scope.empty() ) {
        auto obj_ptr = scope.top();
        obj_ptr->~DestructableObject();
        scope.pop();
    }
    obj_.pop();
}

void ExceptionSingleton::pushObj( DestructableObject* obj) {
    obj_.top().push( obj);
}

DestructableObject::DestructableObject() {
    ExceptionSingleton::GetInstance()->pushObj( this);
}

void ExceptionSingleton::setError( error_t err) {
    err_ = err;
}

error_t ExceptionSingleton::getError() {
    if ( err_ == error_t::invalid_error_type ) {
        throw std::invalid_argument( "Trying to use THROW_FURTHER without THROW before");
    }
    return err_;
}
