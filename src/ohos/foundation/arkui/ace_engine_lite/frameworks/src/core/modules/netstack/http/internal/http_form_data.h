/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HTTP_FORM_DATA
#define HTTP_FORM_DATA

#ifdef __cplusplus
extern "C" {
#endif

#define FORM_DATA_PATH_MAXLEN 256
#define CLIENT_FORM_DATA_NUM 1

#define FORM_DATA_BOUNDARY "----WebKitFormBoundarypXEfCkZnHjoOSnPc0"
#define FORM_DATA_BOUNDARY_START "\r\n--" FORM_DATA_BOUNDARY
#define FORM_DATA_BOUNDARY_END FORM_DATA_BOUNDARY_START "--\r\n"

typedef struct HttpFormDataNode HttpFormDataNode;
struct HttpFormDataNode {
    HttpFormDataNode *next;
    int isFile;
    char filePath[FORM_DATA_PATH_MAXLEN];
    char *formatContent;
    int formatLen;
};

typedef struct {
    int isUsed;
    HttpFormDataNode *formDataNode;
    HttpClientData *clientData;
} FormDataInfo;

void HttpClientClearFormData(HttpClientData *clientData);
int HttpClientFormdataLen(HttpClientData *clientData);
int HttpClientSendFormdata(HttpClient *client, HttpClientData *clientData);

#ifdef __cplusplus
}
#endif

#endif