## SRProxy介绍

目前 WebRTC 协议跟 SIP 协议互通场景主要运用在企业呼叫中心、企业内部通信、电话会议（PSTN）、智能门禁等场景，要想让 WebRTC 与 SIP 互通，要解决两个层面的问题：**信令层**和**媒体层**。两个网络使用的信令机制不同，所以要进行信令的转换，才能完成媒体的协商，建立会话。媒体层要完成编码的转换，以及 rtp/srtp 转换等功能。anyRTC 开源 SRProxy 网关，解决了WebRTC与SIP的协议转换，配合 anyRTC 开源的 ARCall 音视频呼叫 demo,演示如何通过 App/Web 端呼叫落地，下文就如何使用部署 SRProxy  网关，以及如何跟ARCall 互通进行展开，熟悉如何使用后，可集成SDK到自己的应用中，配合自身业务做对应的场景。

## 呼叫流程

### 一、ARCall呼叫逻辑

![20211130173754](https://mmbiz.qpic.cn/sz_mmbiz_jpg/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqR6IFwib3epEDVsyBcp1SrwrVhMgT21S9AqiaCAyPNYPvsIKQyLpaz1DYg/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

![20211130173807](https://mmbiz.qpic.cn/sz_mmbiz_jpg/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqRQSJ0cz2c0vRjb5qBxV3JcoAOkWaySCFnEjpIDF06zDwyuMqbt4pDZg/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

### 二、SRProxy转发逻辑

#### 1、SRProxy能做什么？

![image-20200825140210910](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqR676OBQgGTHNuOlUfnCgu9OEVsSRowVOyYiaYwg43b17es6IfhFbIJaw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

 从上图简单概括一下：SRProxy是实现RTC和SIP之间业务互通的桥梁，更是实现业务拓展的关键服务。

#### 2、呼叫流程

##### 1、状态流转图

 呼叫邀请中，主叫可以通过 [LocalInvitation] 对象提供的 [getState] 方法查询当前呼叫邀请的有关状态；被叫可以通过 SDK 返回的 [RemoteInvitation]对象的 [getState]方法查询当前呼叫邀请的相关状态。

 **LocalInvitationState**

 下图描述了与主叫相关的呼叫邀请状态流转图：

![img](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqRstgw3yxEIpHlbfktRgATLm97qugWiaRPPS6AF72IvlJPgJeMvMUrk9A/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

 **RemoteInvitationState**

 下图描述了与被叫相关的呼叫邀请状态流转图：

![img](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqRngic6ro09KNsXDVrvWoCteL8Wib5YrVPKVtptmXMkhXskzqpAskxw6hw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

##### 2、API 时序图

 **取消已发送呼叫邀请**

![img](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqR5uYE5bHqFzricEial2s1FFzLkfh4hnbJJ3AHYHAZxQ0bUUuPRcAvt3pg/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

 **接受／拒绝呼叫邀请**

![img](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqRdLibMjB6HkRicrYjv86dVrjkiaxq9ibQ8m7fzheu5ibFr2vdxHUaFbq0j7g/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

**注意事项及限制条件**

- 主叫设置的呼叫邀请 content 的字符串长度：8 KB，格式为 UTF-8。
- 被叫设置的呼叫邀请响应 response 的字符串长度：8 KB，格式为 UTF-8。
- 呼叫邀请的 channel ID 仅用于与老信令互通时设置。设置的 channel ID 必须与老信令 SDK 设置相同才能实现互通。字符串长度：64 字节，格式为 UTF-8。

## **创建一个应用**

### 一、注册账号

> 到[anyRTC官网](https://console.anyrtc.io/signin)注册一个开发者账号，并创建一个应用

![image-20211116101342701](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqRWkd1aQcyrjTGlicbUIiaVftv06PEfNwxciaVPYKenT80UskfDdNLHdepQ/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

### 二、创建应用获取AppId

![image-20211116102040742](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqRbf8rcjEXIuZeCopWXXVwKfLiaqL3B8bib3JoMhdR3ebE5pM2icIeHlrCw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)



## **部署freeswitch**

### 一、准备

1.1、系统

> Centos 7.9 最好是纯净服务器 不然可能会存在依赖装不上或冲突情况

1.2、防火墙

> 参考freeswitch防火墙: https://freeswitch.org/confluence/display/FREESWITCH/Firewall 

```
# 开放sip端口tcp协议
[root@localhost ~]# firewall-cmd --permanent --add-port=5060/tcp
# 开放sip端口udp协议
[root@localhost ~]# firewall-cmd --permanent --add-port=5060/udp
# 开放sip信令端口udp协议
[root@localhost ~]# firewall-cmd --permanent --add-port=5080/tcp
# 开放sip信令端口udp协议 
[root@localhost ~]# firewall-cmd --permanent --add-port=5080/udp
# 开放sip用于NAT穿越端口tcp协议
[root@localhost ~]# firewall-cmd --permanent --add-port=3478-3479/tcp
# 开放sip用于NAT穿越端口udp协议
[root@localhost ~]# firewall-cmd --permanent --add-port=3478-3479/udp
# 开放rtp端口（范围）
[root@localhost ~]# firewall-cmd --permanent --add-port=16384-32768/udp
# 让防火墙配置生效
[root@localhost ~]# firewall-cmd --reload

# 也可以直接关闭防火墙
[root@localhost ~]# systemctl disable --now firewalld
```
### 二、安装FreeSwitch
```
# 下载freeswitch
[root@localhost ~]# wget https://github.com/anyRTC-UseCase/SipRtcProxy/releases/download/freeswitch-V1.10.9/freeswitch.tar.gz

# 解压freeswitch
[root@localhost ~]# tar zxvf freeswitch.tar.gz
[root@localhost ~]# cd freeswitch
    
# 修改脚本配置
[root@localhost freeswitch]# vim freeswitch_install
TARGET_DIR=/usr/local						## 服务目录位置--一般默认
TARGET_WAN_IP="127.0.0.1"					## 公网环境写公网IP，内网环境写内网IP
TARGET_PASSWORD="1234"						## SIP账号密码

# 安装freeswitch
[root@localhost freeswitch]# ./freeswitch_install.sh
    
# 安装目录
[root@localhost freeswitch]# cd /usr/local/freeswitch/
```
### 三、FreeSwitch自动增加号码
>freeswitch是一个开源的呼叫中心服务，默认号码是1000-1019
>只有20个号码时,无法满足时,需要增加号码使用

> [root@localhost ~]# vim /usr/local/freeswitch/conf/dialplan/default.xml

```xml
 <extension name="Local_Extension">
   <!--<condition field="destination_number" expression="^(10[01][0-9])$">--> # 注释这一行 或者修改这一行
   <condition field="destination_number" expression="^(1[0-9][0-9][0-9]|20[0-9][0-9])$"> # 重新定义号码段 段为1000~2099
     <action application="export" data="dialed_extension=$1"/>
     <!-- bind_meta_app can have these args <key> [a|b|ab] [a|b|o|s] <app> -->
     <action application="bind_meta_app" data="1 b s execute_extension::dx XML features"/>
```

> [root@localhost ~]# freeswitch.sh

```shell
#!/bin/bash
# Author:  lzy
# data：  2021-11-16

# 设置变量
# freeswitch存放号码目录
TARGET_FREESWITCH_PREFIX=/usr/local/freeswitch/conf/directory/default/
# 因为默认已经有1000-1019 所以设置从1020开始,可以随时改
i=1020

# 设置循环,-le 小于等于2099结束
while [ $i -le 2599 ]
do
    # i=$i+1  $i=1020  逐步往上+1
    let i=$i+1
    # cp 复制1000.xml 重命名$i 逐步+1
    cp $TARGET_FREESWITCH_PREFIX/1000.xml  $TARGET_FREESWITCH_PREFIX/$i.xml
    # sed将1000.xml文件里面的1000 改为文件本身的数值
    sed -i "s/1000/$i/" $TARGET_FREESWITCH_PREFIX/$i.xml
done
```

```c
## 脚本授权 执行脚本
[root@localhost ~]# chmod +x freeswitch.sh
[root@localhost ~]# ./freeswitch.sh

## 保存后，在freeswitch客户端，输入reloadxml进行重新加载.xml文件
[root@localhost ~]# fs_cli
freeswitch@localhost>reloadxml
```

### 四、问题

1、公网部署没有声音及30秒自动挂断

```
## 修改配置文件
vim conf/sip_profiles/internal.xml
    <param name="ext-rtp-ip" value="autonat:127.0.0.1"/>		## 修改成公网IP
    <param name="ext-sip-ip" value="autonat:127.0.0.1"/>		## 修改成公网IP
    
vim conf/sip_profiles/external.xml
    <param name="ext-rtp-ip" value="127.0.0.1"/>				## 修改成公网IP
    <param name="ext-sip-ip" value="127.0.0.1"/>				## 修改成公网IP
```

## **部署SRProxy**

> 源码地址：https://github.com/anyRTC-UseCase/SipRtcProxy

### 一、Windows 7 +

 双击：SipRtcProxy.sln ，直接运行

 项目是VS2017创建，VS2015，VS2019可自行验证。

### 二、Linux - Centos7.0 +

` 下载代码到本地`

> [root@localhost ~]# git clone https://github.com/anyRTC-UseCase/SipRtcProxy.git
>
> [root@localhost ~]# cd   SipRtcProxy.git

`环境变量`

```shell
[root@localhost SipRtcProxy]# vim  /etc/profile
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/root/SipRtcProxy/so/
[root@localhost SipRtcProxy]# source /etc/profile
```

 `执行`

> [root@localhost SipRtcProxy]#  make

`配置文件`

>[root@localhost SipRtcProxy]#  vim  rtx.conf

```c
[global]
appid=XXXXX			             ## 创建应用时注册的Appid 
sip_svr=IP:5060      			 ## freeswitch_IP地址和端口，目前仅支持freeswitch默认端口

## 配置私有云RTC环境 注释默认连接公网环境
#[rtc]
#ip=RTC_IP
#port=6080

## 配置私有云RTM环境 注释默认连接公网环境
#[rtm]
#ip=RTM_IP
#port=7080

[rtm2sip]
on=1                              ## 是否开启RTM2SIP;0:不开启  1:开启
acc_rule=1005;10[06-19]           ## 号码范围根据freeswitch进行调整;号码情况:1005，1006-1019 共15个号码,同Appid下多个SRProxy 号码不能冲突

[proxy]
on=1                              ## 是否开启SIP代理;0:不开启 1:开启
sip_account=1000                  ## freeswitch代理账号:1000,客户端不可与此号码冲突；且一个SRProxy只能用一个号码,换个环境需换个号码,如:1001
sip_pwd=1234                      ## freeswitch密码
rtm_account=1086                  ## 登入RTM账号,客户端不可与此号码冲突,且一个appid只能用一个号码,换个环境需换个号码,如:1087

[log]
#* 0:SENSITIVE 1:VERBOSE 2:INFO 3:WARNING 4:ERROR 5:NONE
level=2
file=rtc_sip.log
max_file_size=100
```

` 前台启动`

> [root@localhost SipRtcProxy]#  ./SRProxy rtx.conf  

### 三、Linux - Centos7.0 + 已编译

> 链接：https://pan.baidu.com/s/1QhhIsO3NEf9olX19xVxBKg
> 提取码：l1f2

`创建目录`

>[root@localhost ~]# mkdir  /usr/local/ar4/
>
>[root@localhost ~]# tar zxvf SRProxy.tar.gz

`将srproxy.tar.gz放到/usr/local/ar4/解压`

> [root@localhost ~]# cd /usr/local/ar4/
>
> [root@localhost ar4]# tar zxvf srproxy.tar.gz

`将rtx.sh脚本放入/usr/bin下面并赋予权限`

> [root@localhost ~]#  chmod +x /usr/bin/rtx.sh

`进入目录`

> [root@localhost ~]#  cd /usr/local/ar4/srproxy/
> [root@localhost srproxy]#  vim conf/rtx.conf

```c
[global]
appid=XXXXX			             ## 创建应用时注册的Appid 
sip_svr=IP:5060      			 ## freeswitch_IP地址和端口，目前仅支持freeswitch默认端口

## 配置私有云RTC环境 注释默认连接公网环境
#[rtc]
#ip=RTC_IP
#port=6080

## 配置私有云RTM环境 注释默认连接公网环境
#[rtm]
#ip=RTM_IP
#port=7080

[rtm2sip]
on=1                              ## 是否开启RTM2SIP;0:不开启  1:开启
acc_rule=1005;10[06-19]           ## 号码范围根据freeswitch进行调整;号码情况:1005，1006-1019 共15个号码,同Appid下多个SRProxy 号码不能冲突

[proxy]
on=1                              ## 是否开启SIP代理;0:不开启 1:开启
sip_account=1000                  ## freeswitch代理账号:1000,客户端不可与此号码冲突；且一个SRProxy只能用一个号码,换个环境需换个号码,如:1001
sip_pwd=1234                      ## freeswitch密码
rtm_account=1086                  ## 登入RTM账号,客户端不可与此号码冲突,且一个appid只能用一个号码,换个环境需换个号码,如:1087

[log]
#* 0:SENSITIVE 1:VERBOSE 2:INFO 3:WARNING 4:ERROR 5:NONE
level=2
file=rtc_sip.log
max_file_size=100
```

`配置没有问题后启动SRProxy`

> 进入目录：
>
> [root@localhost ~]# cd /usr/local/ar4/srproxy/
>
> [root@localhost srproxy]#  rtx.sh start SRProxy 			## 启动
>
> [root@localhost srproxy]#  rtx.sh restart SRProxy 		## 重启
>
> [root@localhost srproxy]#  rtx.sh stop SRProxy 			## 停止

`添加任务计划 实现自启动`

> [root@localhost ~]# crontab -e

> */1 * * * * sh /usr/local/ar4/srproxy/moni_srp.sh >/dev/null 2>&1

## Demo演示

### 一、登入ARCall  （连接公网RTC、RTM）

> ARCall源码下载地址： https://github.com/anyRTC-UseCase/ARCall
>
> 配置AppId一定要和SRProxy 配置文件一致

![](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqR4fqsUj4aNuclvnrJicutrCrFD4cnzr1Hicmqr45k5bJTzrfKEYtdrCQw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

### 二、登入sip（模拟电话 连接FreeSwitch）

> 下载地址：https://www.microsip.org/downloads
>
> 连接的freeswitch一定要和SRProxy配置的一致
>
> 添加账户  点击Menu--->Add account

![image-20211129095550616](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqRVbQAW8JuhW1icCgL7qPnXjqB0icjgpYVHm4ug4yDc65D732OSQryiaHPQ/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

![image-20211116165838263](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqRZzjjja37JMlc658NCjfOcKIJg2dek3gRMvS7y2IYzB2Y8X1Lz7SCPQ/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)



### 三、ARCall拨打sip

> ARCall拨打sip   使用正常流程即可，号码多少就拨打多少
>
> 注意： 拨打设备 一定要有麦克风才可拨通



![image-20211116171743377](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqRiad6rHsgNBd5RjxI4U0ExaYEYpL53bHIvEwMXlRYyjupEgrPickuGeqw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

`呼叫成功`

![image-20211116171641368](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqRg7AJgbDPHmPnTaON69k5yf4nPmkN5Cdygp8ibbxZY3LGGibOQysibETnw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

### 四、sip拨打ARCall

> sip打ARCall需要前面加个0 ，根据配置Sip转发规则而定
>
> 注意： 拨打设备 一定要有麦克风才可拨通

![image-20211116170713481](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqRRFBIeb1H3ShwwdGdnib0tSfE5zVTnt2Q5M4mm25ibuqFmkaDHjLWYL7A/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

`呼叫成功`

![image-20211116170731114](https://mmbiz.qpic.cn/sz_mmbiz_png/lf9n56ou2IBIUJianOGe6Qg10lQgK5NqRib3Gt06zZEcjCa8hhJj14KdytibwDqG1AQicLqUxV4tT3Tar5JI9XAR8g/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)
