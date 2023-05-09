#pragma once

#include <setjmp.h>
#include <stack>
#include <vector>
#include <iostream>
#include <memory>

class DestructableObject {
public:
    DestructableObject();
    virtual ~DestructableObject() {}
};

enum error_t {
    io_error = 1,
    math_error,
    bad_alloc,
    bad_weak_ptr,
    invalid_argument,
    any_error,

    invalid_error_type
};

class ExceptionSingleton {

    error_t err_{invalid_error_type};
    static std::unique_ptr<ExceptionSingleton> instance_;
    std::stack<std::vector<int>> env_;
    std::stack<std::stack<DestructableObject*>> obj_;
public:
    ExceptionSingleton();
    ExceptionSingleton( const ExceptionSingleton&) = delete;
    ExceptionSingleton& operator=( const ExceptionSingleton&) = delete;
    static std::unique_ptr<ExceptionSingleton>& GetInstance();
    void pushEnv(jmp_buf env);
    std::vector<int> getEnv();
    void pushObj( DestructableObject* obj);
    void destroyScope();
    void addScope();
    void setError( error_t err);
    error_t getError();
};

#define TRY         \
    jmp_buf env;    \
    int inner_val = setjmp(env); \
    if (inner_val == 0 ) { \
        ExceptionSingleton::GetInstance()->pushEnv(env);\
        ExceptionSingleton::GetInstance()->addScope(); \
    } \
    if (inner_val == 0) 

#define THROW_FURTHER \
    auto& exception_manager = ExceptionSingleton::GetInstance();\
    exception_manager->destroyScope();          \
    auto env_vec = exception_manager->getEnv(); \
    longjmp(env_vec.data(), exception_manager->getError())

#define THROW(error) \
    ExceptionSingleton::GetInstance()->setError( error);  \
    THROW_FURTHER;

#define CATCH_ALL(smth) \
        else

#define CATCH(error) \
        else if (inner_val == error)
