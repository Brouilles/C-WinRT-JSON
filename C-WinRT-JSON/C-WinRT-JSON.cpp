// C-WinRT-JSON.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Data.Json.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Data::Json;

int main()
{
    init_apartment();

    printf("Parse JSON file with WinRT\n");
    
    StorageFile filePath = StorageFile::GetFileFromPathAsync(L"C:\\Users\\gaetan\\Documents\\Works\\C-WinRT-JSON\\C-WinRT-JSON\\Text.json").get();
    hstring fileContent = FileIO::ReadTextAsync(filePath).get();

    auto rootObject = JsonObject::Parse(fileContent);

    auto users = rootObject.GetNamedObject(L"users");
    auto listOfUsers = users.GetNamedArray(L"list");

    printf("====\n");
    for (auto user : listOfUsers)
    {
        auto userObject = user.GetObject();

        printf("Name: %ls\n", userObject.GetNamedString(L"name").c_str());
        printf("Job: %ls\n", userObject.GetNamedString(L"occupation").c_str());
        printf("Id: %d\n", userObject.GetNamedNumber(L"_id"));

        printf("====\n");
    }

    return 0;
}
