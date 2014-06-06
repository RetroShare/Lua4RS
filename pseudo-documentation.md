not really a documentation - more an overview

	Lua4RSPlugin  
	|- Lua4RSWidget  
	|	|- LuaCore   
	|  
	|- (RsPeers)  
	|- (RsNotify)  
	|- (RsPluginHandler)  
  
	LuaCore (singleton)  
	|- lua_state  
	|- LuaList  
	|	|- LuaContainer  
	|		|- LuaCode  
	|		|	|- Code  
	|		|	|- Description  
	|		|	|- Name  
	|		|  
	|		|- LuaConfig  
	|  
	|- Lua4RSNotify  
	|- Lua4RSTickThread  
	|- Lua4RSWidget  
	|- (RsPeers)  
