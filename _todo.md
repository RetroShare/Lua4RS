20140531, fc
  - complete replacement of Lua4RSMainWidget with Lua4RSWidget. plugin does compile but not run/load. when done: delete Lua4RSMainWidget + its files completely. check
  - note: off for a short vacation, back on monday or tuesday

20140605, fc 
  - lua5.2 include problem. check
  
20140608, sehraf
  - new/edit/save/delete front-end code is a mess (but working) - see next points
  - (DONE) need an API for LuaContainer to handle things (instead of handling things in the front-end code ....)
  - need a rename function that renames files on the hard disk (also need code that detects renames)
  - (DONE) hide LuaCode and LuaConfig completely and make necessary things available through LuaContainer API
  - move script description to config
  - (low priority) rename LuaContainerList to LuaContainerList_t
  - (low priority) rename LuaList to LuaContainerList (to be more clear)
