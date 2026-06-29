# sw21_sign_tool_cbg

## Usage

```
usage: fbb_sign_tool.py [-h] -t TYPE [-i INPUT] -o OUTPUT [-a]

optional arguments:
  -h, --help            show this help message and exit
  -t TYPE, --type TYPE  Image type: 'root_key', 'flashboot', 'recovery', 'provision', 'param_areas', 'tee_fw', 'app_image', 'bt_image', 'wifi0_image', 'wifi1_image', 'wifi2_image', 'wifi3_image', 'sensor_image', 'dsp_main',
                        'dsp_overlay', 'npu_image', 'gnss_image', 'debug_image', 'fota_package'.
  -i INPUT, --input INPUT
                        Input file path.
  -o OUTPUT, --output OUTPUT
                        Output file path.
  -a, --agent           Sign use agent server flag.
```

需要准备好环境变量：
- `domain_user`: 域账号，类似于c00123456
- `domain_pwd` : 域密码

签名权限：申请加入域群组：TC-MBB-Signature-SmartWear-SportsWatch-and-bracelet

> 使用-a选项时，可以不用配置上述domain_user, domain_pwd和域群组权限。