#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include <string>
#include <map>
#include <memory>
#include <vector>

namespace rathena {

/**
 * @brief HTTP response structure
 */
struct HttpResponse {
    int statusCode;
    std::string body;
    std::map<std::string, std::string> headers;
};

/**
 * @brief HTTP client interface
 * 
 * This class provides an interface for making HTTP requests.
 */
class HttpClient {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~HttpClient() = default;
    
    /**
     * @brief Initialize the HTTP client
     * 
     * @return true if initialization was successful, false otherwise
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Make a GET request
     * 
     * @param url The URL to request
     * @param headers The request headers
     * @return HttpResponse The response
     */
    virtual HttpResponse Get(
        const std::string& url,
        const std::map<std::string, std::string>& headers = {}) = 0;
    
    /**
     * @brief Make a POST request
     * 
     * @param url The URL to request
     * @param body The request body
     * @param headers The request headers
     * @return HttpResponse The response
     */
    virtual HttpResponse Post(
        const std::string& url,
        const std::string& body,
        const std::map<std::string, std::string>& headers = {}) = 0;
    
    /**
     * @brief Make a PUT request
     * 
     * @param url The URL to request
     * @param body The request body
     * @param headers The request headers
     * @return HttpResponse The response
     */
    virtual HttpResponse Put(
        const std::string& url,
        const std::string& body,
        const std::map<std::string, std::string>& headers = {}) = 0;
    
    /**
     * @brief Make a DELETE request
     * 
     * @param url The URL to request
     * @param headers The request headers
     * @return HttpResponse The response
     */
    virtual HttpResponse Delete(
        const std::string& url,
        const std::map<std::string, std::string>& headers = {}) = 0;
};

/**
 * @brief CURL-based HTTP client implementation
 */
class CurlHttpClient : public HttpClient {
private:
    void* curl_; // CURL handle (void* to avoid including curl.h)
    bool initialized_;

public:
    /**
     * @brief Constructor
     */
    CurlHttpClient();
    
    /**
     * @brief Destructor
     */
    ~CurlHttpClient() override;
    
    /**
     * @brief Initialize the HTTP client
     * 
     * @return true if initialization was successful, false otherwise
     */
    bool Initialize() override;
    
    /**
     * @brief Make a GET request
     * 
     * @param url The URL to request
     * @param headers The request headers
     * @return HttpResponse The response
     */
    HttpResponse Get(
        const std::string& url,
        const std::map<std::string, std::string>& headers = {}) override;
    
    /**
     * @brief Make a POST request
     * 
     * @param url The URL to request
     * @param body The request body
     * @param headers The request headers
     * @return HttpResponse The response
     */
    HttpResponse Post(
        const std::string& url,
        const std::string& body,
        const std::map<std::string, std::string>& headers = {}) override;
    
    /**
     * @brief Make a PUT request
     * 
     * @param url The URL to request
     * @param body The request body
     * @param headers The request headers
     * @return HttpResponse The response
     */
    HttpResponse Put(
        const std::string& url,
        const std::string& body,
        const std::map<std::string, std::string>& headers = {}) override;
    
    /**
     * @brief Make a DELETE request
     * 
     * @param url The URL to request
     * @param headers The request headers
     * @return HttpResponse The response
     */
    HttpResponse Delete(
        const std::string& url,
        const std::map<std::string, std::string>& headers = {}) override;

private:
    /**
     * @brief Perform a request
     * 
     * @param url The URL to request
     * @param method The HTTP method
     * @param body The request body (optional)
     * @param headers The request headers
     * @return HttpResponse The response
     */
    HttpResponse PerformRequest(
        const std::string& url,
        const std::string& method,
        const std::string& body = "",
        const std::map<std::string, std::string>& headers = {});
    
    /**
     * @brief Write callback for CURL
     * 
     * @param data The data received
     * @param size The size of each data element
     * @param nmemb The number of data elements
     * @param userdata User data pointer
     * @return size_t The number of bytes processed
     */
    static size_t WriteCallback(void* data, size_t size, size_t nmemb, void* userdata);
    
    /**
     * @brief Header callback for CURL
     * 
     * @param data The header data
     * @param size The size of each data element
     * @param nmemb The number of data elements
     * @param userdata User data pointer
     * @return size_t The number of bytes processed
     */
    static size_t HeaderCallback(void* data, size_t size, size_t nmemb, void* userdata);
};

/**
 * @brief Factory for creating HTTP clients
 */
class HttpClientFactory {
public:
    /**
     * @brief Create an HTTP client
     * 
     * @return std::unique_ptr<HttpClient> The created HTTP client
     */
    static std::unique_ptr<HttpClient> CreateHttpClient();
};

} // namespace rathena

#endif // HTTP_CLIENT_HPP