#include "Platform/Windows/WindowsFileSystem.h"
#include "CDialogEventHandler.h"
#include <algorithm>

wchar_t* CharToWChar(const char* text)
{
    size_t size = strlen(text) + 1;
    wchar_t* wa = new wchar_t[size];
    mbstowcs(wa, text, size);
    return wa;
}

std::string WSTRToStr(const std::wstring& wstr)
{
    std::string strTo;
    char* szTo = new char[wstr.length() + 1];
    szTo[wstr.size()] = '\0';
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
    strTo = szTo;
    delete[] szTo;
    return strTo;
}


std::string WindowsFileSystem::OpenFile(const char* filter)
{
    //HRESULT WindowsFileSystem::hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    //PWSTR pszFilePath = nullptr;
    //if (SUCCEEDED(hr))
    //{
    //    IFileDialog* pfd = nullptr;
    //    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
    //        NULL,
    //        CLSCTX_INPROC_SERVER,
    //        IID_PPV_ARGS(&pfd));
    //    if (SUCCEEDED(hr))
    //    {
    //        IFileDialogEvents* pfde = nullptr;
    //        hr = CDialogEventHandler::CreateInstance(IID_PPV_ARGS(&pfde));
    //        if (SUCCEEDED(hr))
    //        {
    //            // Hook up the event handler.
    //            DWORD dwCookie;
    //            hr = pfd->Advise(pfde, &dwCookie);
    //            if (SUCCEEDED(hr))
    //            {
    //                // Set the options on the dialog.
    //                DWORD dwFlags;

    //                // Before setting, always get the options first in order 
    //                // not to override existing options.
    //                hr = pfd->GetOptions(&dwFlags);
    //                if (SUCCEEDED(hr))
    //                {
    //                    // In this case, get shell items only for file system items.
    //                    hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
    //                    if (SUCCEEDED(hr))
    //                    {
    //                        // Set the file types to display only. 
    //                        // Notice that this is a 1-based array.
    //                        wchar_t* wc_filter = CharToWChar(filter);
    //                        COMDLG_FILTERSPEC c_saveTypes[] =
    //                        {
    //                            {L"Test", wc_filter}
    //                        };
    //                        hr = pfd->SetFileTypes(ARRAYSIZE(c_saveTypes), c_saveTypes);
    //                        if (SUCCEEDED(hr))
    //                        {
    //                            hr = pfd->SetFileTypeIndex(1);
    //                            if (SUCCEEDED(hr))
    //                            {
    //                                hr = pfd->SetDefaultExtension(wc_filter);
    //                                if (SUCCEEDED(hr))
    //                                {
    //                                    // Show the dialog
    //                                    hr = pfd->Show(NULL);
    //                                    if (SUCCEEDED(hr))
    //                                    {
    //                                        // Obtain the result once the user clicks 
    //                                        // the 'Open' button.
    //                                        // The result is an IShellItem object.
    //                                        IShellItem* psiResult;
    //                                        hr = pfd->GetResult(&psiResult);
    //                                        if (SUCCEEDED(hr))
    //                                        {
    //                                            // We are just going to print out the 
    //                                            // name of the file for sample sake.;
    //                                            psiResult->GetDisplayName(SIGDN_FILESYSPATH,
    //                                                &pszFilePath);
    //                                            //if (SUCCEEDED(hr))
    //                                            //{
    //                                            //    TaskDialog(NULL,
    //                                            //        NULL,
    //                                            //        CAMPFIRE,
    //                                            //        pszFilePath,
    //                                            //        NULL,
    //                                            //        TDCBF_OK_BUTTON,
    //                                            //        TD_INFORMATION_ICON,
    //                                            //        nullptr);
    //                                            //    CoTaskMemFree(pszFilePath);
    //                                            //}
    //                                            psiResult->Release();
    //                                        }
    //                                    }
    //                                }
    //                            }
    //                        }
    //                    }
    //                    // Unhook the event handler.
    //                    pfd->Unadvise(dwCookie);
    //                }
    //                pfde->Release();
    //            }
    //            pfd->Release();
    //            CoUninitialize();
    //            return WSTRToStr(pszFilePath);
    //        }
    //        return "";
    //    }
    //    return "";
    //}
    //return "";
    HWND owner = NULL;
    OPENFILENAME ofn;
    char fileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt = "";
    std::string fileNameStr;
    if (GetOpenFileName(&ofn))
        fileNameStr = fileName;

    // Replace '\' from windows filesystems with '/', which works on all platforms
    std::replace(fileNameStr.begin(), fileNameStr.end(), '\\', '/');

    return fileNameStr;
}

std::string WindowsFileSystem::SaveFile()
{
    //IFileSaveDialog* pfsd = nullptr;
    //HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog,
    //    NULL,
    //    CLSCTX_INPROC_SERVER,
    //    IID_PPV_ARGS(&pfsd));
    //if (SUCCEEDED(hr))
    //{

    //    DWORD dwOptions;
    //    hr = pfsd->GetOptions(&dwOptions);

    //    // Set the title of the dialog.
    //    if (SUCCEEDED(hr)) {
    //        hr = pfsd->SetTitle(L"Select Files");
    //        hr = pfsd->SetFileName(L"outputfile");
    //        hr = pfsd->SetDefaultExtension(L"txt");
    //        hr = pfsd->QueryInterface(IID_PPV_ARGS(&pfdc));
    //        if (SUCCEEDED(hr))
    //        {
    //            hr = pfdc->StartVisualGroup(CONTROL_GROUP, L"");
    //            if (SUCCEEDED(hr))
    //                hr = pfdc->AddText(CONTROL_LABEL, L"Min Distance:");
    //            if (SUCCEEDED(hr))
    //                hr = pfdc->AddEditBox(CONTROL_EDITBOX_MINDIST, L"2000.0");

    //            pfdc->EndVisualGroup();
    //        }
    //    }
    //}
    return "Failed";
}
