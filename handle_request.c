#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

#define MAX_QUERY_LENGTH 1024
#define MAX_BUFFER 512
// Dummy implementations for missing functions
// Replace these with actual implementations
const char* json_get_string_value_by_field(struct json_object* obj, const char* field) {
    struct json_object* value;
    if (json_object_object_get_ex(obj, field, &value)) {
        return json_object_get_string(value);
    }
    return NULL;
}

struct json_object* json_get_json_object_by_field(struct json_object* obj, const char* field) {
    struct json_object* value;
    if (json_object_object_get_ex(obj, field, &value)) {
        return value;
    }
    return NULL;
}

void parse_get_query(const char* query, char* module, char* action) {
    // Dummy implementation for parsing GET query
    // Replace with actual parsing code
    strcpy(module, "default_module");
    strcpy(action, "default_action");
}

void execute_command(const char* action, char* output, size_t size) {
    // Dummy implementation for executing UCI command
    // Replace with actual execution code
	FILE *fp;
	char buf[MAX_BUFFER];
	size_t csize = 0;
	if((fp = popen(action, "r")) == NULL)
	{
		perror("popen failed");
		output[0] = '\0';
		return;
	}

	while(fgets(buf, sizeof(buf)-1, fp) != NULL)
	{
		size_t len = strlen(buf);
		if(csize+len < size-1)
		{
			strcpy(output + csize, buf);
			csize+=len;
		}
	}
	output[size-1] = '\0';
	
	if(pclose(fp) == -1)
	{
		perror("pclose failed");
	}
}

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

    struct json_object* myjson = json_tokener_parse(query);
    struct json_object* response = NULL;
    if (myjson == NULL) {
        printf("{\"error\":1,\"message\":\"Invalid JSON\"}\n");
        return;
    }

    const char* action = json_get_string_value_by_field(myjson, "ACT");

    if (action != NULL && strcmp(action, "Login") == 0) {
        struct json_object* param = json_get_json_object_by_field(myjson, "param");
        if (param != NULL) {
            const char* admin = json_get_string_value_by_field(param, "admin");
            const char* pwd = json_get_string_value_by_field(param, "pwd");
            if (admin != NULL && pwd != NULL && strcmp(admin, "admin") == 0 && strcmp(pwd, "12345678") == 0) {
                printf("{\"error\":0}\n");
            } else {
                printf("{\"error\":1,\"message\":\"admin or pwd error!\"}\n");
            }
        } else {
            printf("{\"error\":1,\"message\":\"Missing param field\"}\n");
        }
    } else if (action != NULL && strcmp(action, "GetDHCP") == 0) {
        char ipaddr[MAX_BUFFER] = {0};
		char network[MAX_BUFFER] = {0};
		char start[MAX_BUFFER] = {0};
		char limit[MAX_BUFFER] = {0};
		char leasetime[MAX_BUFFER] = {0};
		execute_command("uci get network.lan.ipaddr", ipaddr, MAX_BUFFER);
		execute_command("uci get network.lan.network", network, MAX_BUFFER);
		execute_command("uci get dhcp.lan.start", start, MAX_BUFFER);
		execute_command("uci get dhcp.lan.limit", limit, MAX_BUFFER);
		execute_command("uci get dhcp.lan.leasetime", leasetime, MAX_BUFFER);
		response = json_object_new_object();
		json_object_object_add(response, "ipaddr", json_object_new_string(ipaddr));
		json_object_object_add(response, "network", json_object_new_string(network));
		json_object_object_add(response, "start", json_object_new_string(start));
		json_object_object_add(response, "limit", json_object_new_string(limit));
		json_object_object_add(response, "leasetime", json_object_new_string(leasetime));
		printf("%s", json_object_to_json_string(response));
    } else {
        printf("{\"error\":1,\"message\":\"Unknown action\"}\n");
    }

    json_object_put(myjson); // Free the JSON object
	json_object_put(response);
}

int main() {
    // Check request method
    const char *method = getenv("REQUEST_METHOD");

    // Print HTTP header
    printf("Content-Type: application/json\n\n");

    if (method != NULL && strcmp(method, "POST") == 0) {
        handle_post_request();
    } else if (method != NULL && strcmp(method, "GET") == 0) {
        //handle_get_request(); 
    } else {
        // Method not supported
        printf("{\"error\":1,\"message\":\"Method not supported\"}\n");
    }

    return 0;
}
