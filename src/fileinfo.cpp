#pragma comment(lib, "Version")

#include <node.h>
#include <Windows.h>
#include <string>
#include <memory>

using v8::Array;
using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Integer;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

void GetFileVersion(const FunctionCallbackInfo<Value> &args)
{
  Isolate *isolate = args.GetIsolate();

  if (args.Length() != 1)
  {
    // Throw an Error that is passed back to JavaScript
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString())
  {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong argument type. File path expected")));
    return;
  }

  String::Utf8Value arg0(args[0]->ToString());
  std::string temp = std::string(*arg0);
  std::wstring filePath = std::wstring(temp.begin(), temp.end());

  DWORD versionHandle = NULL;
  DWORD versionDataSize = ::GetFileVersionInfoSizeW(filePath.c_str(), &versionHandle);
  if (NULL == versionDataSize)
  {
    std::string errMsg = "::GetFileVersionInfoSize failed. Error: " + std::to_string(GetLastError());
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, errMsg.c_str())));
    return;
  }

  std::unique_ptr<BYTE[]> versionData(new BYTE[versionDataSize]);
  BOOL ret = ::GetFileVersionInfoW(filePath.c_str(), versionHandle, versionDataSize, versionData.get());
  if (!ret)
  {
    std::string errMsg = "::GetFileVersionInfo failed. Error: " + std::to_string(GetLastError());
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, errMsg.c_str())));
    return;
  }

  UINT size = 0;
  VS_FIXEDFILEINFO *pVerInfo = nullptr;
  ret = ::VerQueryValueW(versionData.get(), L"\\", (LPVOID *)&pVerInfo, &size);
  if (!ret)
  {
    std::string errMsg = "::VerQueryValue failed. Error: " + std::to_string(GetLastError());
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, errMsg.c_str())));
    return;
  }

  int major = HIWORD(pVerInfo->dwFileVersionMS);
  int minor = LOWORD(pVerInfo->dwFileVersionMS);
  int buildMajor = HIWORD(pVerInfo->dwFileVersionLS);
  int buildMinor = LOWORD(pVerInfo->dwFileVersionLS);

  // we'll populate this with the results
  Local<Array> result_list = Array::New(isolate, 4);
  result_list->Set(0, Integer::New(isolate, major));
  result_list->Set(1, Integer::New(isolate, minor));
  result_list->Set(2, Integer::New(isolate, buildMajor));
  result_list->Set(3, Integer::New(isolate, buildMinor));

  args.GetReturnValue().Set(result_list);
}

void Init(Local<Object> exports)
{
  NODE_SET_METHOD(exports, "GetFileVersion", GetFileVersion);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init);