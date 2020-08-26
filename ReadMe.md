### 一，概述

#### 1，SRProxy能做什么？

​	话不多说，直接上图更直观。

![image-20200825140210910](https://anyrtcboard.oss-cn-beijing.aliyuncs.com/image-20200825140210910.png)

​	从上图简单概括一下：SRProxy是实现RTC和SIP之间业务互通的桥梁，更是实现业务拓展的关键服务。



#### 	2，为何制定RTC与SIP互通的开源方案

​	RTC与SIP互通是基于市场的呼叫中心解决方案的需求，随着RTC技术的广泛应用，各个环节的应用需求层出不穷，传统的SIP呼叫中心也需要升级技术方案，我们在给贝壳找房、滴滴、浙江省公安厅等大型企业提供技术方案后进行项目分析发现：我们在提供服务时客户更关注的是自己的业务流程，如何让客户快速的集成，以及集成后灵活的改造方案来应对多变的需求，实现客户的价值，是项目能够顺利快速推进的重要因素。



#### 	3，RTC与SIP如何互通

​	要想让RTC与SIP互通，要解决两个层面的问题：信令层和媒体层。 两个网络使用的信令机制不同，所以要进行信令的转换，才能完成媒体的协商，建立会话。媒体层要完成编码的转换，以及rtp/srtp转换等功能。

​	虽然只是两段话描述了RTC与SIP互通的场景，但是实现的过程有N多的坑在等着你；[anyRTC](https://www.anyrtc.io)已在多个项目中验证了RTC与SIP互通的性能和稳定性。



#### 	4，开源说明

​	开源的RTC和SIP互通的网关代码，其中使用了三大模块：RTC，RTM，SIP，相关的模块以库的形式提供，支持Linux和Windows平台。

​	开源的网关代码支持与[ArCall](https://github.com/anyRTC-UseCase/ARCall)开源项目互通，支持呼叫流程制定和修改。

​	开源的网关支持三种模式：

 - IVR呼叫中心模块

   适用于呼叫中心，手机/网页/小程序客户端呼入IVR，通过语音导航实现相应的业务流程。

 - SIP主动呼RTC模块

   适用于客户坐席主动呼叫消费者，支持SIP呼叫客户端，支持SIP直呼手机号等。

 - RTC直呼SIP座机

   适用于企业内外网互通，通过APP客户端直接呼叫员工或指定坐席的工位座机。

​	

#### 	5，FAQ

 - 为何有的功能模块不开源？

   某些功能模块是经过项目验证的商业代码，相关的逻辑已经封装成接口，更加适合业务开发，这也是我们开源的初衷：为了更快更容易的实现RTC与SIP互通的技术方案；对于需要模块源码的公司或个人，可以申请商务对接以获取相应源码。

 - 能够实现哪些终端互通？

   结合[ArCall](https://github.com/anyRTC-UseCase/ARCall)项目，实现Web，Android，iOS，小程序，SIP座机，PSTN电话，手机互通。

 - 是否支持视频？

   本次开源的项目仅适合语音呼叫场景，对于SIP的视频互通场景，可申请商务对接探讨。

 - 开源是否收费？

   开源的代码及模块均可在公司规定的协议框架下免费使用；仅需要支付在使用过程中产生的服务费用。

 - [anyRTC](https://www.anyrtc.io)是否支持私有化？

   支持，anyRTC是国内RTC私有化先行者和领先者。

 - 网上很多RTC与SIP互通的开源方案，为何选择[anyRTC](https://www.anyrtc.io)？

   首先anyRTC的技术方案已经通过了很多大型项目的实际考验，属于商业版本，非一般开源的项目可比；

   其次anyRTC提供方案的同时，提供了专业的技术服务和保障，让您无任何后顾之忧；

   再次anyRTC还提供[ArCall](https://github.com/anyRTC-UseCase/ARCall)开源客户端项目，实现的是：Web，Android，iOS，小程序，SIP座机，PSTN电话，手机互通；绝非Demo级开源项目可比。

 - 为何采用C++ 作为开发语言

   C++是性能的保障，作为网关我们需要考虑大并发，性能压榨及资源消耗等等因素；

   核心功能使用C++语言，业务部分用户可自行选择相应语言进行开发。

   

### 二，呼叫流程

#### 1，状态流转图

​	呼叫邀请中，主叫可以通过 [LocalInvitation] 对象提供的 [getState] 方法查询当前呼叫邀请的有关状态；被叫可以通过 SDK 返回的 [RemoteInvitation]对象的 [getState]方法查询当前呼叫邀请的相关状态。

​	**LocalInvitationState**

​	下图描述了与主叫相关的呼叫邀请状态流转图：

![img](https://web-cdn.agora.io/docs-files/1582270646018)

​	**RemoteInvitationState**

​	下图描述了与被叫相关的呼叫邀请状态流转图：

![img](https://web-cdn.agora.io/docs-files/1582270656158)

#### 2，API 时序图

​	**取消已发送呼叫邀请**

![img](https://web-cdn.agora.io/docs-files/1565426396109)

​	**接受／拒绝呼叫邀请**

![img](https://web-cdn.agora.io/docs-files/1565427974586)

**注意事项及限制条件**

- 主叫设置的呼叫邀请 content 的字符串长度：8 KB，格式为 UTF-8。
- 被叫设置的呼叫邀请响应 response 的字符串长度：8 KB，格式为 UTF-8。
- 呼叫邀请的 channel ID 仅用于与老信令互通时设置。设置的 channel ID 必须与老信令 SDK 设置相同才能实现互通。字符串长度：64 字节，格式为 UTF-8。

### 三，如何运行

1，Windows 7 +

​	双击：SipRtcProxy.sln ，直接运行

​	项目是VS2017创建，VS2015，VS2019可自行验证。

2，Linux - Centos7.0 + 

​	下载代码到本地：Git clone https://github.com/anyRTC-UseCase/SipRtcProxy.git 

​	进入项目目录

​	执行：make

### 四，常见问题

详见[常见问题](https://docs.anyrtc.io/platforms/docs/platforms/FAQ/faq)。

**anyRTC创业扶持计划**

- 30万免费分钟数，助力初创企业快速发展。

> [anyRTC](https://www.anyrtc.io)初创企业扶持计划，只要通过企业审核，联系客服加入[anyRTC](https://www.anyrtc.io)创业扶持计划，即可享受30万免费分钟数。获得分钟数可降低在实时音视频技术服务所产生的成本费用，零成本快速启动项目。

- 专属技术指导支持

> [anyRTC](https://www.anyrtc.io)为初创企业提供一对一专属客服，为客户提供专业、认真的服务，及时解答您的疑惑。并为客户提供专属技术指导，更快上手，轻松上线！

### 五，联系我们

联系电话：021-65650071

QQ咨询群：580477436

ArCall技术交流群：597181019

咨询邮箱：[hi@dync.cc](mailto:hi@dync.cc)

技术问题：[开发者论坛](https://bbs.anyrtc.io/)

获取更多帮助前往：[www.anyrtc.io](http://www.anyrtc.io/)