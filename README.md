Lua4RS
======

A RetroShare plugin to automate and script things on RetroShare using the Lua language.

##What is RetroShare?##
RetroShare is a secure decentralised communication platform.
http://retroshare.sourceforge.net/

##What is Lua4RS?##
This plugin let you interact with RetroShare. It exposes most RetroShare functions to Lua and provides a trigger system. You can write scripts that react to events (from RetroShare) like _a friends comes online_ or _received a new mail_. With access to most RetroShare functions, you can do the same in the Lua language as you can do in C++.

##Requirements##
* RetroShare v0.5
* Lua 5.2 

##How to build##
Simply run "qmake" (or "qmake-qt4") and "make". Then place libLua4RS.so in your extension folder.
