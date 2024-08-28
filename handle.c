#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

// Function to handle POST request
void handle_post_request() {
    char query[MAX_QUERY_LENGTH];
    size_t content_length;
    char *content_length_str = getenv("CONTENT_LENGTH");

    if (content_length_str == NULL) {
        printf("{\"error\":1,\"message\":\"Missing CONTENT_LENGTH\"}\n");
        return;
    }

    content_length = (size_t)atoi(content_length_str);
    if (content_length >= MAX_QUERY_LENGTH) {
        printf("{\"error\":1,\"message\":\"Request too large\"}\n");
        return;
    }

    // Read POST data
    fread(query, 1, content_length, stdin);
    query[content_length] = '\0'; // Null-terminate

//query = {"ACT":"Login","param":["admin":"admin","pwd":"123456789521456213254652"]}

    struct json_
}

//{"ACT":"Login","param":["admin":"admin","pwd":"123456789521456213254652"]}

int main() {
    // Check request method
    const char *method = getenv("REQUEST_METHOD");

    // Print HTTP header
    printf("Content-Type: application/json\n\n");

    if (method != NULL && strcmp(method, "POST") == 0) {
        handle_post_request();
    } else if (method != NULL && strcmp(method, "GET") == 0) {
        handle_get_request();
    } else {
        // Method not supported
        printf("{\"error\":1,\"message\":\"Method not supported\"}\n");
    }

    return 0;
}
