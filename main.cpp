#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>

// Function to load API key from file
std::string loadApiKey()
{
    std::ifstream file(".config.txt");
    if (!file)
    {
        return "";
    }
    std::string apiKey;
    std::getline(file, apiKey);
    return apiKey;
}

// Function to save API key to file
void saveApiKey(const std::string &apiKey)
{
    std::ofstream file(".config.txt");
    if (file)
    {
        file << apiKey;
    }
}

// Function to make a request to the API
void makeRequest(const std::string &apiKey, const std::string &text)
{
    CURL *curl = curl_easy_init();
    if (curl)
    {
        std::string url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash-latest:generateContent?key=" + apiKey;
        std::string jsonPayload = "{\"contents\":[{\"parts\":[{\"text\":\"" + text + "\"}]}]}";

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Content-Type: application/json"));

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "Curl error: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        std::string command = argv[1];

        if (command == "--set-api-key")
        {
            if (argc != 3)
            {
                std::cerr << "Error: Missing API key.\nUsage: --set-api-key <key>\n";
                return 1;
            }
            std::string apiKey = argv[2];
            saveApiKey(apiKey);
            std::cout << "API key saved successfully.\n";
        }
        else if (command == "--request")
        {
            if (argc != 3)
            {
                std::cerr << "Error: Missing request text.\nUsage: --request <text>\n";
                return 1;
            }

            std::string apiKey = loadApiKey();
            if (apiKey.empty())
            {
                std::cerr << "Error: No API key found. Set an API key using --set-api-key <key>.\n";
                return 1;
            }

            std::string text = argv[2];
            makeRequest(apiKey, text);
        }
        else
        {
            std::cerr << "Unknown command: " << command << "\n";
            std::cerr << "Usage:\n"
                      << "  --request <text>      Make a request with the specified text\n"
                      << "  --set-api-key <key>   Set or update the API key\n";
            return 1;
        }
    }
    else
    {
        // No arguments provided
        std::string apiKey = loadApiKey();
        if (apiKey.empty())
        {
            std::cerr << "Error: No API key found. Set an API key using --set-api-key <key>.\n";
            return 1;
        }

        std::cout << "API key found. Enter text to send requests (type 'exit' to quit):\n";
        while (true)
        {
            std::cout << "> ";
            std::string text;
            std::getline(std::cin, text);

            if (text == "exit")
            {
                std::cout << "Exiting...\n";
                break;
            }

            makeRequest(apiKey, text);
        }
    }

    return 0;
}
