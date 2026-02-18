#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <microhttpd.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

#define PORT 3000
#define EXTERNAL_URL "https://icanhazdadjoke.com/search"

// Struct to handle curl response data
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback for curl to write data into memory
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) return 0;
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

// Function to fetch jokes from the external API
char* fetch_jokes() {
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk = {malloc(1), 0};

    curl_handle = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");

    curl_easy_setopt(curl_handle, CURLOPT_URL, EXTERNAL_URL);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        free(chunk.memory);
        return NULL;
    }

    curl_easy_cleanup(curl_handle);
    return chunk.memory;
}

// Handler for /v1/api/jokes
enum MHD_Result handle_jokes(struct MHD_Connection *connection) {
    char *raw_json = fetch_jokes();
    if (!raw_json) return MHD_NO;

    cJSON *root = cJSON_Parse(raw_json);
    cJSON *results = cJSON_GetObjectItem(root, "results");
    cJSON *output_root = cJSON_CreateObject();
    cJSON *jokes_array = cJSON_CreateArray();

    cJSON *item = NULL;
    cJSON_ArrayForEach(item, results) {
        cJSON *joke_text = cJSON_GetObjectItem(item, "joke");
        cJSON_AddItemToArray(jokes_array, cJSON_CreateString(joke_text->valuestring));
    }
    cJSON_AddItemToObject(output_root, "jokes", jokes_array);

    char *json_response = cJSON_PrintUnformatted(output_root);

    struct MHD_Response *response = MHD_create_response_from_buffer(
        strlen(json_response), (void*)json_response, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, "Content-Type", "application/json");

    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    cJSON_Delete(root);
    cJSON_Delete(output_root);
    free(raw_json);
    return ret;
}

// Global handler (Includes Middleware logic)
enum MHD_Result request_handler(void *cls, struct MHD_Connection *connection,
                               const char *url, const char *method,
                               const char *version, const char *upload_data,
                               size_t *upload_data_size, void **con_cls) {

    // Simple Middleware: Log start
    time_t start = time(NULL);
    printf("[START] %s %s\n", method, url);

    enum MHD_Result ret;
    if (strcmp(url, "/v1/api/jokes") == 0 && strcmp(method, "GET") == 0) {
        ret = handle_jokes(connection);
    } else {
        const char *not_found = "{\"detail\":\"Not Found\"}";
        struct MHD_Response *res = MHD_create_response_from_buffer(strlen(not_found), (void*)not_found, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, res);
        MHD_destroy_response(res);
    }

    // Simple Middleware: Log end
    printf("[END] %s %s - Done in %lds\n", method, url, time(NULL) - start);
    return ret;
}

int main() {
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                             &request_handler, NULL, MHD_OPTION_END);
    if (!daemon) return 1;

    printf("Server running on port %d...\n", PORT);
    getchar(); // Keep server running until enter is pressed

    MHD_stop_daemon(daemon);
    return 0;
}

// To Build -> gcc main.c -lmicrohttpd -lcurl -lcjson -o joke_server
// To run -> ./joke_server
