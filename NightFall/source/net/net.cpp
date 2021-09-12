/*Copyright (c) 2020 D3fau4

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
SOFTWARE.*/

#include <switch.h>
#include "net/net.hpp"
#include <cstring>
#include "json.hpp"

namespace Network
{
    struct Net::MemoryStruct buffer;

    Net::Net(int UseMemory)
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        Net::UseMemory = UseMemory;
        if (Net::UseMemory == 1)
        {
            buffer.memory = (char *)malloc(1);
            buffer.size = 0;
        }
    }

    static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        size_t realsize = size * nmemb;
        struct Net::MemoryStruct *mem = (struct Net::MemoryStruct *)userp;

        char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
        if (!ptr)
        {
            /* out of memory! */
            brls::Logger::error("not enough memory (realloc returned NULL)\n");
            return 0;
        }

        mem->memory = ptr;
        memcpy(&(mem->memory[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;

        return realsize;
    }

    size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string *data)
    {
        try
        {
            data->append((char *)ptr, size * nmemb);
        }
        catch(std::bad_alloc &e)
        {
            brls::Logger::error("mori");
        }
        
        return size * nmemb;
    }

    string Net::Request(string method, string url)
    {
        CURLcode res = CURLE_OK;
        CURL *curl = curl_easy_init();
        string response;
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "NightFall/1.0");
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        return !res ? response : "{}";
    }

    Result Net::CheckURL(string url)
    {
        CURLcode res = CURLE_OK;
        CURL *curl = curl_easy_init();

        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "NightFall/1.0");
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        if (res != CURLE_OK)
            return 1;
        return 0;
    }

    Result Net::DownloadLastUpdate(string Owner, string repo, string file)
    {
        using json = nlohmann::json;
        json j;
        string url = "https://api.github.com/repos/" + Owner + "/" + repo + "/releases";

        string jsontext = Request("application/json", url);

        j = json::parse(jsontext);
        string tag_name = j[0]["tag_name"].get<std::string>();
        
        string file_url = "https://github.com/" + Owner +"/" + repo + "/releases/download/" + tag_name + "/" + file;

        return 0;
    }

    bool Net::Download(string url, string filepath)
    {
        FILE *fp;
        CURLcode res = CURLE_OK;
        CURL *curl = curl_easy_init();

        if (curl)
        {
            fp = fopen(filepath.c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "NightFall/1.0");
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            if (Net::UseMemory != 1)
            {
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            }
            else
            {
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buffer);
            }
            curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
            res = curl_easy_perform(curl);
            if (Net::UseMemory == 1)
            {
                fwrite(buffer.memory, 1, buffer.size, fp);
                free(buffer.memory);
            }
            curl_easy_cleanup(curl);
            fclose(fp);
        }
        else
        {
            res = CURLE_HTTP_RETURNED_ERROR;
        }
        if (res != CURLE_OK)
            remove(filepath.c_str());
        return res == CURLE_OK ? false : true;
    }

    bool Net::HasInternet()
    {
        u32 strg = 0;
        nifmGetInternetConnectionStatus(NULL, &strg, NULL);
        return (strg > 0);
    }

} // namespace Network
