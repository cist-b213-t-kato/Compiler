#include <stdio.h>

int main() {
	//http://home.a00.itscom.net/hatada/asm/inline/inline01.html
	//http://ext-web.edu.sgu.ac.jp/koike/CA14/assembler_content.html
	__asm {
		mov rax, 42
		ret
	}

	return 0;
}
