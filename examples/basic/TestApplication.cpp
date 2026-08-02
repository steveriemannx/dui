#include "TestApplication.h"
#include "MainThread.h"

TestApplication::TestApplication()
{
}

TestApplication::~TestApplication()
{
}

void TestApplication::Run()
{
    // Create the main thread
    MainThread thread;

    // Run the main thread loop
    thread.RunMessageLoop();
}
