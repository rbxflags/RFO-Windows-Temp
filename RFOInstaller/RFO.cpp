#include <iostream>
#include <filesystem>
#include <thread>
#include <curl/curl.h>

int main(int argc, char* argv[]) {
    std::string userIn;
    std::cout << "Welcome to the RFO Installer/Updater!\nRFO is made, owned, and developed by Kaede and Expo\n\nType \"install\" and press enter to continue!\nType \"uninstall\" and press enter to uninstall :(\n\n";
    std::cin >> userIn;

    system("taskkill /f /im RFO.exe >nul 2>&1");

    std::string rootDir("C:\\Program Files (x86)\\RFO");

    if (std::filesystem::exists(rootDir) == false) {
        std::filesystem::create_directory(rootDir);
    }

    if (userIn == std::string("install")) { // Install
        FILE* file;
        if (fopen_s(&file, (rootDir + "\\RFO.exe").c_str(), "wb") != 0) {
            std::cout << "Could not open filepoint... | 0x1\n";
            std::cin.get();
            return 1;
        }

        CURL* req = curl_easy_init();
        CURLcode res;
        curl_easy_setopt(req, CURLOPT_URL, "https://raw.githubusercontent.com/rbxflags/RFO-Windows-Temp/main/RFO.exe");
        curl_easy_setopt(req, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
        curl_easy_setopt(req, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
        curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(req, CURLOPT_WRITEDATA, file);
        res = curl_easy_perform(req);
        if (res != CURLE_OK) {
            std::cout << "NETWORK ERROR | PLEASE CHECK YOUR INTERNET CONNECTION | 0x2\n";
            std::cin.get();
            return 2;
        }
        curl_easy_cleanup(req);

        fclose(file);

        HKEY hKey;
        if (RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
            RegSetValueExA(hKey, "RFO", 0, REG_SZ, (const BYTE*)(rootDir + "\\RFO.exe").c_str(), 36);
            RegCloseKey(hKey);
        }
        else {
            std::cout << "Registry error, RFO not added to startup...\n";
        }

        //CreateProcess code from https://stackoverflow.com/a/15440094
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        CreateProcessA((rootDir + "\\RFO.exe").c_str(), argv[1], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        std::cout << "\RFO has been successfully installed...\nClosing this installer in 5 seconds...\n\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
    else if (userIn == std::string("uninstall")) { // Uninstall
        if (argv[0] == (rootDir + "\\RFOInstaller.exe").c_str()) {
            std::cout << "To uninstall, please run the installer outside of this updater, you can redownload this installer on the github.\nClosing in 5 seconds.\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            return 0;
        }

        if (std::filesystem::exists("C:\\RClientOptimizer2") == true) {
            std::filesystem::remove_all("C:\\RClientOptimizer2");
        }

        if (std::filesystem::exists(rootDir) == true) {
            std::filesystem::remove_all(rootDir);
        }

        HKEY hKey;
        if (RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
            RegDeleteValueA(hKey, "RFO");
        }
        else {
            std::cout << "Registry error, RFO startup regkey not removed...\n";
        }
        std::cout << "\RFO has been successfully uninstalled...\nClosing this installer in 5 seconds...\n\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
    return 0;
}
