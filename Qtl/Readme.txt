Rochus said:

The purpose of Qtl is to provide a reasonably complete Lua binding to Qt GUI.
Signals, Slots and Properties are already accessible to Lua through the Qt meta object system.

Unfortunately this is quite unfinished work, done by an off-shore contractor of mine. I did some 
debugging and clearing up, so some basic classes like QWidget are reasonably stable. But most of
the classes are unfinished and buggy (tons of warnings).
Since nobody complained so far, I assume the scientists only used LuaQt2 so far.
Maybe it's a good idea to kick Qtl out, unless someone finds time to debug and complete it.

