#include "http_server.h"

#include <thread>

#ifndef PLATFORM_UWP
#include <mutex>
#include <condition_variable>
#include <cmath>

#if defined(__MINGW32__) || defined(__MINGW64__)
namespace std
{
    template<typename Ret, typename ...Args>
    inline Ret _hypot(Args... args) {
        return std::hypot(args...);
    }
}
#endif // MINGW
#include <Python.h>

#include <libgearbox_logger_p.h>

namespace
{
    struct Semaphore
    {
    public:
        Semaphore() : count_(0) {}

        inline void notify()
        {
            std::unique_lock<std::mutex> lock(mtx_);
            ++count_;
            cv_.notify_one();
        }

        inline void wait()
        {
            std::unique_lock<std::mutex> lock(mtx_);

            while (count_ == 0)
            {
                cv_.wait(lock);
            }
            --count_;
        }

    private:
        std::mutex mtx_;
        std::condition_variable cv_;
        int count_;
    };

    Semaphore startSemaphore;
    Semaphore stopSemaphore;
}

void gearbox::test::startHttpServer()
{
    std::thread pyThread([] {
        static constexpr const char initScript[] =
            "entry = open('"
            GEARBOX_PYTHON_MODULE_PATH
            "/main.py')\n"
            "content = entry.read()\n"
            "entry.close()\n"
            "exec(content)\n";

        static const auto serverReady = [](PyObject *, PyObject *) -> PyObject * {
            startSemaphore.notify();
            return PyLong_FromLong(0);
        };

        static struct PyMethodDef methods[] = {
            { "server_ready", serverReady, METH_VARARGS, "Signals that the server is up and running" },
            { NULL, NULL, 0, NULL }
        };

        static struct PyModuleDef modDef = {
            PyModuleDef_HEAD_INIT, "gearbox_test", NULL, -1, methods,
            NULL, NULL, NULL, NULL
        };

        static const auto initModule = []() -> PyObject * {
            return PyModule_Create(&modDef);
        };

        wchar_t pyArgv[] = L"__main__";
        wchar_t *pyArgv_p = &pyArgv[0];

        PyImport_AppendInittab("gearbox_test", initModule);
        Py_SetProgramName(pyArgv_p);
        Py_Initialize();
        PySys_SetArgv(1, &pyArgv_p);

        auto sysPath = PySys_GetObject("path");
        if (sysPath == nullptr)
        {
            LOG_FATAL("Failed to get Python system import path");
        }
        PyList_Append(sysPath, PyUnicode_FromString(GEARBOX_PYTHON_MODULE_PATH));

        PyRun_SimpleString(initScript);

        Py_Finalize();
    });
    startSemaphore.wait();
    pyThread.detach();
}

void gearbox::test::stopHttpServer()
{
    static const auto quit = [](void *) -> int {
        PyErr_SetInterrupt();
        stopSemaphore.notify();
        return 0;
    };
    Py_AddPendingCall(quit, nullptr);
    stopSemaphore.wait();
}
#else
void gearbox::test::startHttpServer() { }
void gearbox::test::stopHttpServer() { }
#endif // !PLATFORM_UWP


