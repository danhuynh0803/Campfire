#include "Platform/Windows/WindowsFileSystem.h"
#include "CDialogEventHandler.h"

std::string WindowsFileSystem::OpenFile(const char* filter)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileDialog* pfd = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&pfd));
        if (SUCCEEDED(hr))
        {
            IFileDialogEvents* pfde = nullptr;
            hr = CDialogEventHandler::CreateInstance(IID_PPV_ARGS(&pfde));
            if (SUCCEEDED(hr))
            {
                // Hook up the event handler.
                DWORD dwCookie;
                hr = pfd->Advise(pfde, &dwCookie);
                if (SUCCEEDED(hr))
                {
                    // Set the options on the dialog.
                    DWORD dwFlags;

                    // Before setting, always get the options first in order 
                    // not to override existing options.
                    hr = pfd->GetOptions(&dwFlags);
                    if (SUCCEEDED(hr))
                    {
                        // In this case, get shell items only for file system items.
                        hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
                        if (SUCCEEDED(hr))
                        {
                            // Set the file types to display only. 
                            // Notice that this is a 1-based array.
                            hr = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
                            if (SUCCEEDED(hr))
                            {
                                // Set the selected file type index to Word Docs for this example.
                                hr = pfd->SetFileTypeIndex(INDEX_WORDDOC);
                                if (SUCCEEDED(hr))
                                {
                                    // Set the default extension to be ".doc" file.
                                    hr = pfd->SetDefaultExtension(L"doc;docx");
                                    if (SUCCEEDED(hr))
                                    {
                                        // Show the dialog
                                        hr = pfd->Show(NULL);
                                        if (SUCCEEDED(hr))
                                        {
                                            // Obtain the result once the user clicks 
                                            // the 'Open' button.
                                            // The result is an IShellItem object.
                                            IShellItem* psiResult;
                                            hr = pfd->GetResult(&psiResult);
                                            if (SUCCEEDED(hr))
                                            {
                                                // We are just going to print out the 
                                                // name of the file for sample sake.
                                                PWSTR pszFilePath = NULL;
                                                hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH,
                                                    &pszFilePath);
                                                if (SUCCEEDED(hr))
                                                {
                                                    TaskDialog(NULL,
                                                        NULL,
                                                        L"CampFire",
                                                        pszFilePath,
                                                        NULL,
                                                        TDCBF_OK_BUTTON,
                                                        TD_INFORMATION_ICON,
                                                        nullptr);
                                                    CoTaskMemFree(pszFilePath);
                                                }
                                                psiResult->Release();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        // Unhook the event handler.
                        pfd->Unadvise(dwCookie);
                    }
                    pfde->Release();
                }
                pfd->Release();
                CoUninitialize();
                return "Success";
            }
            CoUninitialize();
            return "Failed";
        }
        CoUninitialize();
        return "Failed";
    }
    CoUninitialize();
    return "Failed";
}

std::string WindowsFileSystem::SaveFile()
{
    IFileSaveDialog* pfsd = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pfsd));
    return "Failed";
}