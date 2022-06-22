// C-WinRT-JSON.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Networking.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Web::Http::Headers;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Data::Json;

HttpClient httpClient{ nullptr };

struct DownloadFile : implements<DownloadFile, IInspectable>
{
    IAsyncAction Async(const hstring& fileName, HttpRequestMessage request)
    {
        auto lifetime = get_strong(); // <-- keep alive

       // Do not buffer the response.
        HttpRequestResult result = co_await httpClient.TrySendRequestAsync(request, HttpCompletionOption::ResponseHeadersRead);

        if (result.Succeeded())
        {
            //You will need to call QueryFullProcessImageName, then strip out the .exe from that using std::filesystem::path
            StorageFolder localFolder = co_await StorageFolder::GetFolderFromPathAsync(L"C:\\Users\\gaetan\\Desktop\\Download");
            StorageFile storageFile = co_await localFolder.CreateFileAsync(fileName, winrt::Windows::Storage::CreationCollisionOption::ReplaceExisting);
            IRandomAccessStream fileStream = co_await storageFile.OpenAsync(FileAccessMode::ReadWrite);
            IInputStream responseStream = co_await result.ResponseMessage().Content().ReadAsInputStreamAsync();

            co_await RandomAccessStream::CopyAsync(responseStream, fileStream);

            printf("Completed\n");
            httpClient.Close();
        }
        else
        {
            printf("Error in CancelableRequestAsync\n");
        }

        co_return;
    }
};

int main()
{
    init_apartment();

    // Download JSON file
    httpClient = HttpClient();

    Uri resourceUri{ L"http://download.test/launcher/version_manifest.json" };
    HttpRequestMessage request(HttpMethod::Get(), resourceUri);

    auto down = make_self<DownloadFile>();
    down->Async(L"version_manifest.json", request).get();

    // Parse JSON
    printf("Parse JSON file with WinRT\n");
    
    StorageFile filePath = StorageFile::GetFileFromPathAsync(L"C:\\Users\\gaetan\\Desktop\\Download\\version_manifest.json").get();
    hstring fileContent = FileIO::ReadTextAsync(filePath).get();

    auto rootObject = JsonObject::Parse(fileContent);

    auto files = rootObject.GetNamedArray(L"files");

    printf("====\n");
    for (auto file : files)
    {
        auto fileObject = file.GetObject();

        printf("Name: %ls\n", fileObject.GetNamedString(L"fileName").c_str());
        printf("Size: %d\n", fileObject.GetNamedNumber(L"size"));
        printf("md5: %ls\n", fileObject.GetNamedString(L"md5").c_str());

        printf("====\n");
    }

    // Download files

    return 0;
}
