# gfx-convert tool

This tool is used to convert SVG or PNG file into eraw.bin which can be loaded
by EGT code directly to make graphic render fast. Usually this tool is running
on your Linux host PC, because it has no meaning if you convert graphics on
embedded target.

## Usage Example

1. Before using this tool, please make sure that you've followed the egt build
instruction to build egt library ready.
```
	git clone --recursive https://github.com/linux4sam/egt.git
	cd egt
	./autogen.sh
	./configure
	make
```
2. Make the gfx-convert tool.
```
	cd egt/tool/gfx-convert
	make
```
3. Start the gfx-convert, of course, you can run ./gfx-convert -h to get help.

```
	./gfx-convert -s
	./gfx-convert -i png picture1.png
	./gfx-convert -i png picture2.png
		......
	./gfx-convert -i png pictureN.png
	./gfx-convert file.svg
	./gfx-convert -e
```
4. When conversion finished, copy ./eraw.h to source code to include in your
application cpp code, and copy ./eraw.bin to the target. At last your application
will read eraw.bin on target to get the graphics data.

5. The generated eraw.h has an array inidicating the graphics id with offset and
length, and it has an array index in the comment. So when you are coding, you could
use the array index directly without iterating the array to find the graphics you
want. There is an example in egt/examples/motorcycledash.cpp, please refer to that
to learn how to use the eraw.h and eraw.bin.

## License

EGT is released under the terms of the `Apache 2` license. See the [COPYING](../COPYING)
file for more information.
