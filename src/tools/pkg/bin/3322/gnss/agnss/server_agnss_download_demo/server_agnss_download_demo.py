#!/usr/bin/env python
# coding:utf-8
# ============================================================================
# @brief    demo to download agnss data from hisilicon server
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
# ============================================================================
import requests
import time
import calendar
import hmac
import base64
import binascii


# 网络连接相关
HOST = r'geo-drcn.platform.dbankcloud.cn'  # 域名
PORT = 443  # 端口号
URI = r'/higeo/v1/xgnss/agnss'  # uri路径

# 鉴权认证相关
APP_ID = r''  # 账号，请联系销售代表获取
APP_SECRET = b''  # 密钥，请联系销售代表获取
UUID = r''  # 每台设备的唯一标识码，请根据产品进行适配

# 请求参数
CONSTELLATION = r''  # 掩码表示卫星模式，0x1=GPS，0x2=GLONASS，0x4=BEIDOU，0x8=GALILEO，默认15表示全模
DATATYPE = r''  # 掩码表示请求的数据类型，0x1=星历，0x2=UTC模型，0x4=电离层参数，0x8=实时完好性信息，0x10=频点对应关系，默认31表示全部数据


class AgnssRequest:
    def __init__(self):
        self.ts = 0
        self.qry_str = ''
        self.signature = ''
        self.url = ''
        self.session = requests.session()

    def format_req_str(self):
        qry_str = ''
        if CONSTELLATION and CONSTELLATION != '15':
            # constellation默认值15，则无需填写
            qry_str += f'constellation={CONSTELLATION}'
        if DATATYPE and DATATYPE != '31':
            # datatype默认值31，则无需填写
            qry_str += f'&datatype={DATATYPE}' if qry_str else f'datatype={DATATYPE}'
        self.qry_str = qry_str

    def gen_auth_str(self):
        # 构造待签名字符串
        str_to_sign = f'GET&{URI}&{self.qry_str}&&appid={APP_ID}&timestamp={self.ts}'
        # 计算签名HMAC值
        h1 = hmac.new(key=APP_SECRET, digestmod='sha256')
        h1.update(str_to_sign.encode('utf-8'))
        b1 = base64.b64encode(h1.digest())
        signature = b1.decode('utf-8')
        self.signature = signature

    def update_session(self):
        # 填充完整url
        self.url = r'https://{}:{}{}{}{}'.format(HOST, PORT, URI, '?' if self.qry_str else '', self.qry_str)
        # 组装请求消息头
        self.session.headers.update(
            {
                'X-Request-ID': f'{UUID}',
                'Authorization': f'HMAC-SHA256 appid={APP_ID},timestamp={self.ts},signature={self.signature}'
            }
        )

    def connect_session(self):
        try:
            print(f'正在连接服务器...')
            rsp = self.session.get(url=self.url, verify=False, timeout=(3, 27))
            print(f'Agnss请求结果：{rsp.status_code}，结果码：{rsp.reason}, 响应长度：{len(rsp.content)}')

            # 处理响应值
            if rsp.status_code == 200:
                # 响应200表示请求成功
                print(r'响应内容：')
                # 响应内容的Agnss辅助数据是二进制码流，转换成16进制打印
                print(binascii.hexlify(rsp.content))
                print(f'Agnss辅助数据请求成功')
                with open(r'.\AGNSS.dat', 'wb') as f:
                    f.write(rsp.content)
            elif rsp.status_code == 401:
                if rsp.reason == 'TimestampMoreEarlierOrLater':
                    print(r'unix时间戳异常，请检查服务器下发的响应消息头中“X-Auth-Timestamp”与请求消息头的'
                          r'“Authorization”中的“timestamp”是否相差很大')
                    print(r'请检查“timestamp”是否取用正确的UNIX时间戳，单位是毫秒')
                    # 如设备本身无时间系统，可在此处取用服务器下发的“X-Auth-Timestamp”进行重新请求
                    print(f'响应消息头：{rsp.headers}')
                    server_ts = rsp.headers.get('X-Auth-Timestamp', 0)
                    self.ts = server_ts
                    # 签名认证
                    self.gen_auth_str()
                    # 更新请求消息头
                    self.update_session()
                    # 重新请求
                    rsp2 = self.session.get(url=self.url, verify=False, timeout=(3, 27), proxies=PROXIES)
                    print(f'Agnss重新请求结果：{rsp2.status_code}，结果码：{rsp2.reason}, 响应长度：{len(rsp2.content)}')

                elif rsp.reason == 'authorization is missed':
                    print(r'“Authorization”字段异常，请检查请求消息头是否正确填写“Authorization”字段')
                elif rsp.reason == 'appid is missed':
                    print(r'“appid”字段异常，请检查请求消息头的“Authorization”中是否正确填写“appid”字段')
                elif rsp.reason == 'timestamp is invalid':
                    print(r'“timestamp”字段异常，请检查请求消息头的“Authorization”中是否正确填写“timestamp”字段')
                elif rsp.reason == 'signature is invalid':
                    print(r'“signature”字段异常，请检查请求消息头的“Authorization”中已填写“signature”字段')
                elif rsp.reason == "Can't get the keys of appid":
                    print(r'“appid”无效，请检查请求消息头的“Authorization”中的“appid”的合法性')
                elif rsp.reason == 'SignatureInvalid':
                    print(r'“signature”异常，请检查请求消息头的“Authorization”中的“signature”正确性，'
                          r'检查appid和app_secret是否匹配')
                elif rsp.reason == 'TimestampMoreEarlierOrLater':
                    print(r'异常，请检查')
                elif rsp.reason == 'TimestampMoreEarlierOrLater':
                    print(r'异常，请检查')
            self.session.close()
        except requests.exceptions.SSLError as e:
            print(e)
            print('请检查网络是否通畅')
        except requests.exceptions.ProxyError as e:
            print(e)
            print('请检查网络是否通畅')
        except Exception as e:
            print(e)

    def request(self, ts=0):
        self.ts = ts

        # step1. 构造规范化请求字符串
        self.format_req_str()

        # step2. 构造鉴权信息字符串
        self.gen_auth_str()

        # step3. 构造HTTP GET请求
        self.update_session()
        print(f'请求URL:{self.url}')

        # step4. 发送请求
        self.connect_session()


if __name__ == '__main__':
    # 获取当前UNIX时间，单位毫秒
    now = calendar.timegm(time.gmtime()) * 1000
    agnss = AgnssRequest()
    agnss.request(now)
