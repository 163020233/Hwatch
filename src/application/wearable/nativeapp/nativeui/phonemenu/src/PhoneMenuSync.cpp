/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuSync
 * Create: 2025-07-12
 */

#include "stdlib.h"
#include "cJSON.h"
#include "wearable_log.h"
#include "phonemenu/PhoneMenuSync.h"

namespace OHOS {
std::list<Contacts> PhoneMenuSync::list_;
uint32_t PhoneMenuSync::WriteFileContact(const std::string &filePath, std::string &content)
{
    char resolved[PATH_MAX] = {0};
#ifdef _WIN32
    if (_fullpath(resolved, filePath.c_str(), PATH_MAX) == nullptr) {
#else
    if (realpath(filePath.c_str(), resolved) == nullptr) {
#endif
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuSync::WriteFileContact Error: failed to resolve path\n");
        return JSON_FILE_OPEN_FAIL;
    }
    std::ofstream output(resolved);
    if (!output.is_open()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "no file");
        return JSON_FILE_OPEN_FAIL;
    }
    output << content;
    output.close();
    return 0;
}

uint32_t PhoneMenuSync::ReadFileContact(const std::string &filePath)
{
#ifdef _WIN32
    char resolved[PATH_MAX] = {0};
    if (_fullpath(resolved, filePath.c_str(), PATH_MAX) == nullptr) {
#else
    char *resolved = realpath(filePath.c_str(), NULL);
    if (resolved == nullptr) {
#endif
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuSync::ReadFileContact Error: failed to resolve path\n");
        return JSON_FILE_OPEN_FAIL;
    }
#ifndef _WIN32
    UIFree(resolved);
    resolved = nullptr;
#endif
    std::ifstream input(filePath.c_str());
    if (!input.is_open()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "no file");
        return JSON_FILE_OPEN_FAIL;
    }
    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string content = buffer.str();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PhoneMenuSync::PhoneMenuSync [%s]", content.c_str());

    uint32_t ret = ParseContactsMsg(content);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ParseContactsMsg failed with error code: %u", ret);
        return ret;
    }

    PrintContacts();
    return 0;
}

uint32_t PhoneMenuSync::ParseContactsMsg(std::string &msgContent)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ParseContactsMsg %s\r\n", msgContent.c_str());
    cJSON *root = cJSON_Parse(msgContent.c_str());
    if (root == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "parse_contacts_msg cJSON_Parse failed: returned NULL pointer");
        return JSON_PARSE_FAIL;
    }
    if (!cJSON_IsArray(root)) {
        cJSON_Delete(root);
        return JSON_TYPE_FAIL;
    }
    list_.clear();
    int arraySize = cJSON_GetArraySize(root);
    for (int i = 0; i < arraySize; ++i) {
        cJSON *item = cJSON_GetArrayItem(root, i);
        if (cJSON_IsObject(item)) {
            cJSON *cidJson = cJSON_GetObjectItem(item, "cid");
            cJSON *nameJson = cJSON_GetObjectItem(item, "name");
            cJSON *phoneJson = cJSON_GetObjectItem(item, "phone");
            if (cidJson == nullptr || nameJson == nullptr || phoneJson == nullptr) {
                cJSON_Delete(root);
                return JSON_KEY_FAIL;
            }
            Contacts contacts;
            contacts.cid_ = cidJson->valuestring;
            contacts.name_ = nameJson->valuestring;
            contacts.phone_ = phoneJson->valuestring;
            list_.push_back(contacts);
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "cid [%s], name [%s], phone [%s]", contacts.cid_.c_str(),
                          contacts.name_.c_str(), contacts.phone_.c_str());
        }
    }
    cJSON_Delete(root);
    return 0;
}

std::list<Contacts> &PhoneMenuSync::GetContacts()
{
    return list_;
}

uint32_t PhoneMenuSync::PrintContacts()
{
    for (auto &x : list_) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "cid[%s] name[%s] phone[%s]", x.cid_.c_str(), x.name_.c_str(),
                      x.phone_.c_str());
    }
    return 0;
}

uint32_t PhoneMenuSync::LookContacts(std::string &contacts)
{
    cJSON *contactsArrayObj = cJSON_CreateArray();
    if (!contactsArrayObj) {
        return JSON_CREATE_FAIL;
    }
    for (auto &contacts : list_) {
        cJSON *contactsObj = cJSON_CreateObject();
        if (!contactsObj) {
            cJSON_Delete(contactsArrayObj);
            return JSON_CREATE_FAIL;
        }
        cJSON_AddItemToObject(contactsObj, "cid", cJSON_CreateString(contacts.cid_.c_str()));
        cJSON_AddItemToObject(contactsObj, "name", cJSON_CreateString(contacts.name_.c_str()));
        cJSON_AddItemToObject(contactsObj, "phone", cJSON_CreateString(contacts.phone_.c_str()));
        cJSON_AddItemToArray(contactsArrayObj, contactsObj);
    }
    char *jsonStr = cJSON_Print(contactsArrayObj);
    if (!jsonStr) {
        cJSON_Delete(contactsArrayObj);
        return JSON_TO_STRING_FAIL;
    }
    contacts = jsonStr;
    WEARABLE_LOGE(WEARABLE_LOG_MODULE_MSG_CENTER, "msg_center_lookup_contacts [%s]", jsonStr);
    cJSON_Delete(contactsArrayObj);
    free(jsonStr);
    jsonStr = nullptr;
    return 0;
}

uint32_t PhoneMenuSync::CleanFileContacts(const std::string &filePath)
{
#ifdef _WIN32
    char resolved[PATH_MAX] = {0};
    if (_fullpath(resolved, filePath.c_str(), PATH_MAX) == nullptr) {
#else
    char *resolved = realpath(filePath.c_str(), NULL);
    if (resolved == nullptr) {
#endif
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PhoneMenuSync::CleanFileContacts Error: failed to resolve path\n");
        return JSON_FILE_OPEN_FAIL;
    }

    std::ofstream output(resolved);
    if (!output.is_open()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "no file");
        return JSON_FILE_OPEN_FAIL;
    }
    output.close();

#ifndef _WIN32
    UIFree(resolved);
    resolved = nullptr;
#endif

    list_.clear();
    return 0;
}
}