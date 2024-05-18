#ifndef _DINGDONG_H_
#define _DINGDONG_H_

#include <EthernetENC.h>
#include <vector>

std::vector<String> splitString(String &str, char delimiter)
{
    std::vector<String> result;
    int start = 0;
    int end = str.indexOf(delimiter);

    while (end != -1)
    {
        result.push_back(str.substring(start, end));
        start = end + 1;
        end = str.indexOf(delimiter, start);
    }

    result.push_back(str.substring(start));
    return result;
}

namespace DINGDONG
{

    enum class HttpMethod
    {
        NONE,
        GET,
        POST
    };

    class RHandle
    {
    private:
        EthernetClient *holderClient;

    public:
        RHandle(EthernetClient *pHolderClient)
        {
            this->holderClient = pHolderClient;
        }
    };

    typedef void (*HANDLEResponseCallback)(const RHandle &pRHandle);

    class conn
    {
    private:
        EthernetClient holderClient;
        String rawData = "";

        void mapReq(HttpMethod method, const String &path, HANDLEResponseCallback callback, bool caseSensitive)
        {
            if (!this->connected || this->method != method)
            {
                return;
            }

            if (caseSensitive == true)
            {
                if (this->path != path)
                {
                    return;
                }
            }
            else
            {
                if (!this->path.equalsIgnoreCase(path))
                {
                    return;
                }
            }

            Serial.println("Connectionnn");
            Serial.println("Path: " + path);
            Serial.println("Version: " + this->httpVers);

            RHandle req(&holderClient);

            return callback(req);
        }

    public:
        bool connected = false;
        HttpMethod method = HttpMethod::NONE;
        String path = "";
        String httpVers = "Unknown";

        void get(const String &path, HANDLEResponseCallback callback, bool caseSensitive = false)
        {
            return mapReq(HttpMethod::GET, path, callback, caseSensitive);
        }

        void post(const String &path, HANDLEResponseCallback callback, bool caseSensitive = false)
        {
            return mapReq(HttpMethod::POST, path, callback, caseSensitive);
        }

        conn(EthernetServer &pMServer)
        {
            this->holderClient = pMServer.available();

            if (!holderClient)
            {
                this->connected = false;
                return;
            }

            this->rawData = this->holderClient.readString();

            if (this->rawData.length() < 16)
            {
                this->connected = false;
                return;
            }

            auto dataSplitted = splitString(this->rawData, '\n');
            for (int x = 0; x < dataSplitted.size(); x++)
            {
                if (method == HttpMethod::NONE)
                {
                    if (dataSplitted[x].startsWith("GET "))
                    {
                        method = HttpMethod::GET;
                    }
                    else if (dataSplitted[x].startsWith("POST "))
                    {
                        method = HttpMethod::POST;
                    }

                    if (method != HttpMethod::NONE)
                    {
                        String queryHeaderTrimmed = dataSplitted[x];
                        queryHeaderTrimmed.trim();

                        auto queryHeaderSplitted = splitString(queryHeaderTrimmed, ' ');

                        if (queryHeaderSplitted.size() < 2)
                        {
                            method != HttpMethod::NONE;
                            continue;
                        }

                        path = queryHeaderSplitted[1];

                        if (queryHeaderSplitted.size() >= 3)
                        {
                            for (int y = 2; y < queryHeaderSplitted.size(); y++)
                            {
                                if (queryHeaderSplitted[y].startsWith("HTTP"))
                                {
                                    httpVers = queryHeaderSplitted[2];
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if (method != HttpMethod::NONE)
            {
                this->connected = true;
            }
        }
    };
}

#endif