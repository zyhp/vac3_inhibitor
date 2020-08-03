# VAC3 Inhibitor

Completely disables any verification routines.

- Compile the code using any `C++17` compiler and inject into **steam.exe** as soon as it loads up
- **steam.exe** needs to be open as admin
- To check the output/logs add `-console` as a command argument when starting **steam.exe**

***Some considerations...***
- Injection routine can be fully automated using the steam protocol (`steam://`), *at least put some effort and make an exercise loader...*
- This has been working for more than 2 years, signatures had to be updated a single time due some *asm* operators change...
- There are other ways to achieve the same effect, such as:
	- `jmp/ret` on module begin...
	- hook steam free_std to hook inside every module loaded and disable the function calls...
	- make steam unload their own modules after being loaded
	- you get the idea, get creative in case this ever gets patched