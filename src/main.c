#include <stdio.h>
#include "parson.h"

int main(void) {
    JSON_Value *root_value;
    JSON_Object *person;
    JSON_Array *emails;
    size_t i;

    const float BASE_POTION_SPEED = 180.0;
    const float BASE_POTENT_MULTIPLER = 6.0;

    /* Get access to the json and confirm the root value is a json type */ 
    root_value = json_parse_file("json/test.json");
    if (json_value_get_type(root_value) != JSONObject) {
        fprintf(stderr, "%s", "Incorrect JSON\n");
        return -1;
    }

    person = json_value_get_object(root_value);
    emails = json_object_dotget_array(person, "contact.emails");
    printf("%s\n", json_object_get_string(person, "name"));

    for (i = 0; i < json_array_get_count(emails); i++) {
        printf("%s\n", json_array_get_string(emails, i));
    }

    return 0;
}