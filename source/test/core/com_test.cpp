// ######################################################################################### - FILE
/*!

\author Daniel Craig \par Copyright 2016, All Rights reserved.
*/
// ######################################################################################### - FILE

// ######################################################################################### - FILE
/* Includes */
// ######################################################################################### - FILE
#ifndef XR_DEFINES_H
#include "xr/defines.h"
#endif
#ifndef XR_CORE_UNKNOWN_H
#include "xr/core/unknown.h"
#endif
#ifndef XR_CORE_ASSERT_H
#include "xr/core/assert.h"
#endif
#ifndef XR_CORE_TEST_H
#include "xr/core/test.h"
#endif
#ifndef XR_CORE_ALLOCATOR_H
#include "xr/core/allocator.h"
#endif
#ifndef XR_CORE_INTRUSIVE_PTR_H
#include "xr/core/intrusive_ptr.h"
#endif
// ######################################################################################### - FILE
/* Unit Tests                                                                */
// ######################################################################################### - FILE
#if defined(XR_TEST_FEATURES_ENABLED)

// ######################################################################################### - FILE
// ######################################################################################### - FILE
XR_UNITTEST_GROUP_BEGIN( COM )



class IInterface1 : public xr::COM::IUnknown
{
public:
    XR_COM_CLASS_ID(0x11111);
    virtual void Run() = 0;
};
class IInterface2 : public xr::COM::IUnknown
{
public:
    XR_COM_CLASS_ID(0x11112);
    virtual void Other() = 0;
};
class IInterface3 : public xr::COM::IUnknown
{
public:
    XR_COM_CLASS_ID(0x11113);
};
class IInterface4 : public xr::COM::IUnknown
{
public:
    XR_COM_CLASS_ID(0x11114);
};
class IInterface5 : public xr::COM::IUnknown
{
public:
    XR_COM_CLASS_ID(0x11115);
};

class MyTest0 : public xr::Core::IUnknownHelper0<MyTest0>
{
public:
    XR_COM_CLASS_ID(0x222220);

    void Run()
    {
    }
};
class MyTest1 : public xr::Core::IUnknownHelper1<MyTest1, IInterface1>
{
public:
    XR_COM_CLASS_ID(0x222221);

    void Run()
    {
    }
};
class MyTest2 : public xr::Core::IUnknownHelper2<MyTest2, IInterface1, IInterface2>
{
public:
    XR_COM_CLASS_ID(0x222222);

    void Run()
    {
    }
    void Other()
    {
    }
};
class MyTest3 : public xr::Core::IUnknownHelper3<MyTest3, IInterface1, IInterface2, IInterface3>
{
public:
    XR_COM_CLASS_ID(0x222223);

    void Run()
    {
    }
    void Other()
    {
    }
};
class MyTest4 : public xr::Core::IUnknownHelper4<MyTest4, IInterface1, IInterface2, IInterface3, IInterface4>
{
public:
    XR_COM_CLASS_ID(0x222224);

    void Run()
    {
    }
    void Other()
    {
    }
};

template <typename T, typename U>
void ShouldImplement(U *u)
{
    T * t = u->U::template interface_cast<T>();
    XR_ASSERT_ALWAYS_NE(t, nullptr);
    t->Release();
}
template <typename T, typename U>
void ShouldNotImplement(U *u)
{
    T * t = u->U::template interface_cast<T>();
    XR_ASSERT_ALWAYS_EQ(t, nullptr);
}

template <typename T, typename U>
void ShouldBe(U *u)
{
    T * t = T::QueryImplementation(u);
    XR_ASSERT_ALWAYS_NE(t, nullptr);
    t->Release();
}
template <typename T, typename U>
void ShouldNotBe(U *u)
{
    T * t = T::QueryImplementation(u);
    XR_ASSERT_ALWAYS_EQ(t, nullptr);
}

XR_UNITTEST_TEST_FUNC( Com2 )
{
    {
        MyTest0 * test = XR_NEW("com") MyTest0();
        ShouldNotImplement<IInterface1>(test);
        ShouldNotImplement<IInterface2>(test);
        ShouldNotImplement<IInterface3>(test);
        ShouldNotImplement<IInterface4>(test);
        ShouldNotImplement<IInterface5>(test);

        xr::COM::IUnknown *u =  test->QueryInterface(xr::COM::IUnknown::kIID);
        XR_ASSERT_ALWAYS_NE(u, nullptr);
        ShouldNotImplement<IInterface1>(u);
        ShouldNotImplement<IInterface2>(u);
        ShouldNotImplement<IInterface3>(u);
        ShouldNotImplement<IInterface4>(u);
        ShouldNotImplement<IInterface5>(u);
        ShouldBe<MyTest0>(u);
        ShouldNotBe<MyTest1>(u);
        ShouldNotBe<MyTest2>(u);
        ShouldNotBe<MyTest3>(u);
        ShouldNotBe<MyTest4>(u);
        u->Release();
        u = nullptr;
        test->Release();
        test = nullptr;
    }

    {
        MyTest1 * test = XR_NEW("com") MyTest1();
        ShouldImplement<IInterface1>(test);
        ShouldNotImplement<IInterface2>(test);
        ShouldNotImplement<IInterface3>(test);
        ShouldNotImplement<IInterface4>(test);
        ShouldNotImplement<IInterface5>(test);

        xr::COM::IUnknown *u =  test->QueryInterface(xr::COM::IUnknown::kIID);
        XR_ASSERT_ALWAYS_NE(u, nullptr);
        ShouldImplement<IInterface1>(u);
        ShouldNotImplement<IInterface2>(u);
        ShouldNotImplement<IInterface3>(u);
        ShouldNotImplement<IInterface4>(u);
        ShouldNotImplement<IInterface5>(u);
        ShouldNotBe<MyTest0>(u);
        ShouldBe<MyTest1>(u);
        ShouldNotBe<MyTest2>(u);
        ShouldNotBe<MyTest3>(u);
        ShouldNotBe<MyTest4>(u);
        u->Release();
        u = nullptr;
        test->Release();
        test = nullptr;
    }

    {
        MyTest2 * test = XR_NEW("com") MyTest2();
        ShouldImplement<IInterface1>(test);
        ShouldImplement<IInterface2>(test);
        ShouldNotImplement<IInterface3>(test);
        ShouldNotImplement<IInterface4>(test);
        ShouldNotImplement<IInterface5>(test);

        xr::COM::IUnknown *u =  test->QueryInterface(xr::COM::IUnknown::kIID);
        XR_ASSERT_ALWAYS_NE(u, nullptr);
        ShouldImplement<IInterface1>(u);
        ShouldImplement<IInterface2>(u);
        ShouldNotImplement<IInterface3>(u);
        ShouldNotImplement<IInterface4>(u);
        ShouldNotImplement<IInterface5>(u);
        ShouldNotBe<MyTest0>(u);
        ShouldNotBe<MyTest1>(u);
        ShouldBe<MyTest2>(u);
        ShouldNotBe<MyTest3>(u);
        ShouldNotBe<MyTest4>(u);
        u->Release();
        u = nullptr;
        test->Release();
        test = nullptr;
    }

    {
        MyTest3 * test = XR_NEW("com") MyTest3();
        ShouldImplement<IInterface1>(test);
        ShouldImplement<IInterface2>(test);
        ShouldImplement<IInterface3>(test);
        ShouldNotImplement<IInterface4>(test);
        ShouldNotImplement<IInterface5>(test);

        xr::COM::IUnknown *u =  test->QueryInterface(xr::COM::IUnknown::kIID);
        XR_ASSERT_ALWAYS_NE(u, nullptr);
        ShouldImplement<IInterface1>(u);
        ShouldImplement<IInterface2>(u);
        ShouldImplement<IInterface3>(u);
        ShouldNotImplement<IInterface4>(u);
        ShouldNotImplement<IInterface5>(u);
        ShouldNotBe<MyTest0>(u);
        ShouldNotBe<MyTest1>(u);
        ShouldNotBe<MyTest2>(u);
        ShouldBe<MyTest3>(u);
        ShouldNotBe<MyTest4>(u);
        u->Release();
        u = nullptr;
        test->Release();
        test = nullptr;
    }

    {
        MyTest4 * test = XR_NEW("com") MyTest4();
        ShouldImplement<IInterface1>(test);
        ShouldImplement<IInterface2>(test);
        ShouldImplement<IInterface3>(test);
        ShouldImplement<IInterface4>(test);
        ShouldNotImplement<IInterface5>(test);

        xr::COM::IUnknown *u =  test->QueryInterface(xr::COM::IUnknown::kIID);
        XR_ASSERT_ALWAYS_NE(u, nullptr);
        ShouldImplement<IInterface1>(u);
        ShouldImplement<IInterface2>(u);
        ShouldImplement<IInterface3>(u);
        ShouldImplement<IInterface4>(u);
        ShouldNotImplement<IInterface5>(u);
        ShouldNotBe<MyTest0>(u);
        ShouldNotBe<MyTest1>(u);
        ShouldNotBe<MyTest2>(u);
        ShouldNotBe<MyTest3>(u);
        ShouldBe<MyTest4>(u);
        u->Release();
        u = nullptr;
        test->Release();
        test = nullptr;
    }


    {
        MyTest1 * test = XR_NEW("com") MyTest1();

        xr::Core::intrusive_ptr<xr::COM::IUnknown> u;
        test->interface_cast(u);
        XR_ASSERT_ALWAYS_NE(u, nullptr);

        test->Release();
        test = nullptr;
    }

    {
        MyTest2 * test = XR_NEW("com") MyTest2();

        xr::Core::intrusive_ptr<IInterface2> u;
        test->interface_cast(u);
        XR_ASSERT_ALWAYS_NE(u, nullptr);

        test->Release();
        test = nullptr;
    }

    {
        MyTest3 * test = XR_NEW("com") MyTest3();

        xr::Core::intrusive_ptr<IInterface2> u;
        test->interface_cast(u);
        XR_ASSERT_ALWAYS_NE(u, nullptr);

        test->Release();
        test = nullptr;
    }

    {
        MyTest4 * test = XR_NEW("com") MyTest4();

        xr::Core::intrusive_ptr<IInterface2> u;
        test->interface_cast(u);
        XR_ASSERT_ALWAYS_NE(u, nullptr);

        test->Release();
        test = nullptr;
    }

}



XR_UNITTEST_GROUP_END()

#endif // #if defined(XR_TEST_FEATURES_ENABLED)
