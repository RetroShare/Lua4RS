Lua4RS
======

A RetroShare plugin to automate and script things on RetroShare using the Lua language.

What is RetroShare?
-------------------
RetroShare is a secure decentralised communication platform.
http://retroshare.sourceforge.net/

What is Lua4RS?
---------------
This plugin let you interact with RetroShare. It exposes most RetroShare functions to Lua and provides a trigger system. You can write scripts that react to events (from RetroShare) like _a friends comes online_ or _received a new mail_. With access to most RetroShare functions, you can do the same in the Lua language as you can do in C++.

Requirements
------------
* RetroShare v0.6 (use the 0.5 tag for RetroShare 0.5)
* at least Lua 5.2
  * This also includes developer packages and everything you need on your linux distro. 
  * For Arch Linux install [lua](https://www.archlinux.org/packages/extra/x86_64/lua/). 
  * For Ubuntu you need lua5.2_5.2.4-1ubuntu1_amd64.deb, liblua5.2-0_5.2.4-1ubuntu1_amd64.deb, liblua5.2-0-dbg_5.2.4-1ubuntu1_amd64.deb, liblua5.2-dev_5.2.4-1ubuntu1_amd64.deb, 
lua5.2-doc_5.2.4-1ubuntu1_all.deb (based on ASmith's report)

How to build
------------
Simply run "qmake" (or "qmake-qt4") and "make". Then place libLua4RS.so/Lua4RS.dll in your extension folder.
