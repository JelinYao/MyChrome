# MyChrome
使用libCef+Duilib开发属于自己的浏览器，支持多标签、多进程、视频播放、开发者模式、保存网页为PDF文件、网页缩放等功能。
libcef官方下载地址：https://bitbucket.org/chromiumembedded/cef/downloads/

# 更新记录
    2019-09-08：
        新建分支chrome3809，升级到最新版的libCef，chrome内核版本号3809，使用VS2017开发，不支持XP系统（若要支持XP，请使用master分支代码）。

    2019-10-01
        更新chrome接口回调函数类，适配最版本接口，功能对其2623版本。

    2020-07-15  
        支持网页缩放，目前只能焦点主窗口上时滚动鼠标缩放，后续完善下

# 编译：  
    项目使用VS2017编译，打开sln工程文件编译即可，使用libCef最后一个支持XP的版本2623，可以稳定运行在xp、win7、win8、win10上。

# 扩展：  
    项目是2016年开发的，作为一个业余项目，一直没有时间精力去扩展，很多地方都有待完善。
    UI都是随便找的，所以看起来不太友好😂
    
    
# 运行截图：

![](https://raw.githubusercontent.com/JelinYao/MyChrome/master/img/1.png)

![](https://raw.githubusercontent.com/JelinYao/MyChrome/master/img/2.png)

![](https://raw.githubusercontent.com/JelinYao/MyChrome/master/img/3.png)

![](https://raw.githubusercontent.com/JelinYao/MyChrome/master/img/4.png)

![](https://raw.githubusercontent.com/JelinYao/MyChrome/master/img/5.png)
