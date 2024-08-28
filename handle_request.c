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

void execute_command(const char* command, char* output, size_t maxsize) {
    // Dummy implementation for executing UCI command
    // Replace with actual execution code
        FILE *fp;
        char buffer[MAX_BUFFER];
        size_t currentsize = 0;
    if((fp = popen(command, "r")) == NULL)
    {
        perror("popen failed");
        output[0] = '\0';
        return;
    }

    while(fgets(buffer, sizeof(buffer) - 1, fp) != NULL)
    {
        size_t len = strlen(buffer);
        if(currentsize + len < maxsize - 1)
        {
            strcpy(output + currentsize, buffer);
            currentsize += len;
        }
    }
    output[maxsize - 1] = '\0';

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
    if (myjson == NULL) {
        printf("{\"error\":1,\"message\":\"Invalid JSON\"}\n");
        return;
    }

    const char* action = json_get_string_value_by_field(myjson, "ACT");
    struct json_object* response = NULL;
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
    } else if (action != NULL && strcmp(action, "GetLAN") == 0) {
        char ifname[MAX_BUFFER] = {0};
        char force_link[MAX_BUFFER] = {0};
        char type[MAX_BUFFER] = {0};
        char proto[MAX_BUFFER] = {0};
        char ipaddr[MAX_BUFFER] = {0};
        char netmask[MAX_BUFFER] = {0};
        char ip6assign[MAX_BUFFER] = {0};
        char multicast_querier[MAX_BUFFER] = {0};
        char igmp_snooping[MAX_BUFFER] = {0};
        char ieee1905managed[MAX_BUFFER] = {0};
        char mtu[MAX_BUFFER] = {0};
       	execute_command("uci get network.lan.ifname", ifname, MAX_BUFFER);
        execute_command("uci get network.lan.force_link", force_link, MAX_BUFFER);
        execute_command("uci get network.lan.type", type, MAX_BUFFER);
       	execute_command("uci get network.lan.proto", proto, MAX_BUFFER);
        execute_command("uci get network.lan.ipaddr", ipaddr, MAX_BUFFER);
        execute_command("uci get network.lan.netmask", netmask, MAX_BUFFER);
        execute_command("uci get network.lan.ip6assign", ip6assign, MAX_BUFFER);
        execute_command("uci get network.lan.multicast_querier", multicast_querier, MAX_BUFFER);
        execute_command("uci get network.lan.igmp_snooping", igmp_snooping, MAX_BUFFER);
        execute_command("uci get network.lan.ieee1905managed", ieee1905managed, MAX_BUFFER);
        execute_command("uci get network.lan.mtu", mtu, MAX_BUFFER);
        response = json_object_new_object();
        json_object_object_add(response, "ifname", json_object_new_string(ifname));
        json_object_object_add(response, "force_link", json_object_new_string(force_link));
        json_object_object_add(response, "type", json_object_new_string(type));
        json_object_object_add(response, "proto", json_object_new_string(proto));
        json_object_object_add(response, "ipaddr", json_object_new_string(ipaddr));
        json_object_object_add(response, "netmask", json_object_new_string(netmask));
        json_object_object_add(response, "ip6assign", json_object_new_string(ip6assign));
        json_object_object_add(response, "multicast_querier", json_object_new_string(multicast_querier));
        json_object_object_add(response, "igmp_snooping", json_object_new_string(igmp_snooping));
        json_object_object_add(response, "ieee1905managed", json_object_new_string(ieee1905managed));
        json_object_object_add(response, "mtu", json_object_new_string(mtu));
        printf("%s", (char*)json_object_to_json_string(response));
    }else if(action != NULL && strcmp(action, "GetWAN") == 0) {
		char ifname[MAX_BUFFER] = {0};
        char proto[MAX_BUFFER] = {0};
        char type[MAX_BUFFER] = {0};
		response = json_object_new_object();
		execute_command("uci get network.wan.ifname", ifname, MAX_BUFFER);
        execute_command("uci get network.wan.proto", proto, MAX_BUFFER);
        execute_command("uci get network.wan.type", type, MAX_BUFFER);
		json_object_object_add(response, "ifname", json_object_new_string(ifname));
        json_object_object_add(response, "proto", json_object_new_string(proto));
        json_object_object_add(response, "type", json_object_new_string(type));
		printf("%s", (char*)json_object_to_json_string(response));
	}else if(action != NULL && strcmp(action, "GetVersion") == 0) {
		
	}else {
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

