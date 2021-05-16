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
namespace Network
{
    static struct curl_slist *hosts = NULL;

    Net::Net()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        hosts = curl_slist_append(NULL, "reinx.guide:167.99.228.103");
    }

    size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string *data)
    {
        data->append((char *)ptr, size * nmemb);
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
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_RESOLVE, hosts);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        return !res ? response : "{}";
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
            curl_easy_setopt(curl, CURLOPT_RESOLVE, hosts);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "NightFall");
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            fclose(fp);
        }
        else
        {
            res = CURLE_HTTP_RETURNED_ERROR;
        }
        if (res != CURLE_OK) {
            brls::Logger::error("Fallo al descargar: " + url);
            remove(filepath.c_str());
        }
        return res == CURLE_OK ? false : true;
    }

    bool Net::HasInternet(){
        u32 strg = 0;
        nifmGetInternetConnectionStatus(NULL, &strg, NULL);
        return (strg > 0);
    }
} // namespace Network
