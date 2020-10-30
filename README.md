# VAC3 Inhibitor

Completely disables any verification routines.

Compile the code using any `C++17` compiler and inject into **steam.exe** as soon as it loads up, **steam.exe** needs to be open as admin or simple download it from the releases and run the .exe ...

***Some considerations...***
- There are other ways to achieve the same effect, such as:
	- `jmp/ret` on module begin...
	- hook steam free_std to hook inside every module loaded and disable the function calls...
	- make steam unload their own modules after being loaded
	- you get the idea, get creative in case this ever gets patched
