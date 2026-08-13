/*

MIT License

Copyright (c) 2025-2026 JustStudio. <https://juststudio.is-a.dev/>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <memory>
#include "../../parser.h"

#ifdef _WIN32
    #define NOMINMAX
    #undef INFINITE
    #undef NAN
    #undef ERROR
    #include "IWindow.h"
#elif defined(__linux__)
    #include <X11/Xlib.h>
    #include <X11/Xatom.h>
#elif defined(__APPLE__) && defined(__MACH__)
    #include <CoreFoundation/CoreFoundation.h>
    #include <ApplicationServices/ApplicationServices.h>
#endif

namespace JUSTCWindow {

struct WindowInfo {
    std::string title;
    uint64_t handle;
    int x;
    int y;
    int width;
    int height;
    bool isVisible;
    
    WindowInfo() : handle(0), x(0), y(0), width(0), height(0), isVisible(false) {}
};

struct WindowCreateOptions {
    std::string title = "JUSTC Window";
    int x = 100;
    int y = 100;
    int width = 800;
    int height = 600;
    bool resizable = true;
    bool visible = true;
};

Value Create(const std::vector<Value>& args, Parser* parser);
Value Close(const std::vector<Value>& args);
Value SetTitle(const std::vector<Value>& args);
Value GetTitle(const std::vector<Value>& args);
Value SetPosition(const std::vector<Value>& args);
Value GetPosition(const std::vector<Value>& args);
Value SetSize(const std::vector<Value>& args);
Value GetSize(const std::vector<Value>& args);
Value Show(const std::vector<Value>& args);
Value Hide(const std::vector<Value>& args);
Value IsVisible(const std::vector<Value>& args);
Value List(const std::vector<Value>& args);
Value RunMessageLoop(const std::vector<Value>& args);

WindowInfo getWindowInfo(uint64_t handle);
std::vector<WindowInfo> getAllWindows();
bool setWindowTitle(uint64_t handle, const std::string& title);
bool setWindowPosition(uint64_t handle, int x, int y);
bool setWindowSize(uint64_t handle, int width, int height);
bool showWindow(uint64_t handle);
bool hideWindow(uint64_t handle);
bool closeWindow(uint64_t handle);

Value windowInfoToValue(const WindowInfo& info);
Value windowInfoListToValue(const std::vector<WindowInfo>& windows);

#ifdef __linux__
    void initX11();
#endif

}

#endif
